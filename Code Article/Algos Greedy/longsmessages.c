#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <omp.h>
#define MESSAGE_SIZE 1000
#define PERIOD 100000
#define STEP 1
#define ROUTES_SIZE_MAX 100000
#define NB_SIMULS 400
#define PARALLEL 0

int * random_graph(int nb_routes,int size_route)
{
	int * graph;
	assert(graph = (int*)malloc(sizeof(int)*nb_routes));
	for(int i=0;i<nb_routes;i++)
	{
		graph[i]=rand()%size_route;
	}
	return graph;
}
int* init_period(int period)
{
	int * p;
	assert(p = (int*)malloc(sizeof(int)*period));
	memset(p,0,sizeof(int)*period);
	return p;
}
void fill_periods(int * a, int *r, int offset,int decalage, int message_size, int period)
{
	for(int i=offset;i<(offset+message_size)%period;i++)
	{
		a[i] = 1;
		r[(i+decalage)%period]=1;
	}
}
int offset_ok(int * p,int*p2, int offset, int decalage,int period, int message_size)
{
	for(int i=offset;i<(offset+message_size)%period;i++)
	{
		if(p[i])
			return 0;
		if(p2[(i+decalage) % period])
			return 0;
	}
	return 1;
}
//renvoie la distance en slot a laquelle est la fin du message avec lequel on colissione dans la periode, ou 0 si l'offset est ok
int cols_check(int *P, int offset, int message_size,int per, int nb_routes)
{
	for(int i=0;i<nb_routes;i++)
	{
		if( (P[i]+message_size)%per < P[i]%per  ) // le message pi est a cheval sur deux periodes 
		{
			if(offset%per < (P[i]+message_size)%per)
			{
				//printf("PI CHEVAL Collision debut avec %d retour %d",P[i],(P[i]+message_size)%per - offset%per );
				return (P[i]+message_size)%per - offset%per ;
			}	
			if(( (offset+message_size)%per > P[i]%per) ||( (offset)%per >= P[i]%per) ||( (offset+message_size)%per <= (P[i]+message_size)%per) )
			{
				//printf("PI CHEVAL Collision fin avec %d",P[i],per - (offset)%per + (P[i]+message_size)%per);
				return per - (offset)%per + (P[i]+message_size)%per ;
			}	
		}
		else
		{
			if((offset%per >= P[i]%per) && ( offset%per < (P[i]+message_size)%per)  )
			{
				//printf("OFFSET CHEVAL collistion avec %d  retour %d \n",P[i],(P[i]+message_size)%per - offset%per);
				return (P[i]+message_size)%per - offset%per ;
			}	
			if(( (offset+message_size)%per > P[i]%per) && ( (offset+message_size)%per <= (P[i]+message_size)%per) )
			{
				if(offset%per > (P[i]+message_size)%per)
				{
					//printf("Offset normal collisions avec%d retour %d\n",P[i],per - (offset)%per + (P[i]+message_size)%per);
					return per - (offset)%per + (P[i]+message_size)%per ; 
				}
				else
				{
					//printf("Collision avec %d retour %d \n",P[i],(P[i]+message_size)%per - offset%per );
					return (P[i]+message_size)%per - offset%per ;
				}
			}
		}
	}

	return 0;

}
int verifie_solution(int * aller, int * retour,int message_size,int nb_routes)
{
	for(int i=0;i<nb_routes;i++)
	{
		for(int j=0;j<nb_routes;j++)
		{
			if(i!=j)
			{
				if(abs(aller[i]-aller[j])<message_size)
				{
					printf(" pb aller i %d j %d val |%d - %d| = %d",i,j,aller[i],aller[j],abs(aller[i]-aller[j]));
					return 0;
				}
				if(abs(retour[i]-retour[j])<message_size)
				{
					printf(" pb retour i %d j %d val |%d - %d| = %d",i,j,retour[i],retour[j],abs(retour[i]-retour[j]));
					return 0;
				}
			}
		
		}
	}
	return 1;
}
int first_fit_core(int*aller,int*retour,int begin,int* graph,int nb_routes,int period,int message_size,int debug)
{
	int nb_routes_ok=0;
	int out;
	for(int i=begin;i<nb_routes;i++)
	{
		if(graph[i] != -1)
		{
			int check_value = 1;
			int offset = 0;
			out  = 0;
			while(check_value)
			{
				check_value = cols_check(aller,offset,message_size,period,nb_routes_ok+begin);
				int tmp= check_value;
				//printf("%d ",check_value);
				check_value += cols_check(retour,offset+check_value+graph[i],message_size,period,nb_routes_ok+begin);
				//printf ("%d \n",check_value-tmp);
				offset += check_value;
				if(offset >= period)
				{
					out = 1;
					break;
				}
			}
			//si on a pas trouvé d'offset pour cette route, on passe a la suivante
			if(out)
				continue;
			if(debug)
			{
				printf("rajout de offset %d, decalage = %d id = %d\n",offset,graph[i],i);
			
			}
			aller[nb_routes_ok+begin]=offset;
			retour[nb_routes_ok+begin]=(offset+graph[i])%period;
			nb_routes_ok++;
			if(debug)
			{
					printf("\n aller  \n");
					for(int k=0;k<nb_routes_ok+begin;k++)
						{printf("%d ",aller[k]);}printf("\n");
					printf("retour \n");
					for(int k=0;k<nb_routes_ok+begin;k++)
						{printf("%d ",retour[k]);}printf("\n");
			}
		}
		
		
	}
	


	return nb_routes_ok;
}
int first_fit(int* graph,int nb_routes,int period,int message_size)
{
	int aller[nb_routes] ;
	int retour[nb_routes] ;
	
	for(int i=0;i<nb_routes;i++)
	{
		aller[i]=0;
		retour[i]=0;
	}
	int nb_ok = first_fit_core(aller,retour,0,graph,nb_routes,period,message_size,0);

	if(!verifie_solution(aller,retour,message_size,nb_ok))
	{
		printf("Error verifie solution first fit \n") ;
		exit(3);
		}
	return nb_ok;
	
}
int min(int a, int b){
	return (a<b)?a:b;
}
int trouve_plus_proche_supp(int *t, int n, int size,int message_size,int period,int decalage, int type)
{
	int maxtmp=INT_MAX;
	int mini = INT_MAX;

	for(int i= 0;i<size;i++)
	{
		if(t[i]<mini)
			mini = t[i];
		if(t[i]-message_size>n)
		{
			if(maxtmp > t[i]-message_size)
				maxtmp = t[i]-message_size;
		}

		//CAS hyper particulier ou on à tout pile une place pour mettre le message
		if(t[i]-message_size==n)
		{
			return 1;
		}
	}
	
	if(maxtmp == INT_MAX)
	{
		if(type == 0)//A l'aller
		{
			return period-n+mini-message_size;
		}
		else
		{
			//toujours cas particulier mais modulo
			if(period-n+mini-message_size == 0)
			{
				return 1;
			}
			return min(period-n+mini-message_size,period-n+decalage);
		}
		
	}
	return maxtmp-n;
}


//renvoie le nombre de tics disponible avant le prochain tic occupé dans la periode
int prochain_occup(int * aller, int * retour, int offset, int nb_routes,int period,int message_size,int decalage)
{
	if(nb_routes == 0)
		return period;
	int next_aller = trouve_plus_proche_supp(aller,offset,nb_routes,message_size,period,decalage,0);
	int next_retour = trouve_plus_proche_supp(retour,(offset+decalage)%period,nb_routes,message_size,period,decalage,1);
	//printf("return min %d %d \n",next_retour,next_aller);
	return min(next_retour,next_aller);
}
int random_offset(int *graph,int nb_routes,int period,int message_size)
{
	//Version non opti
	/*
	int * aller = init_period(period);
	int * retour = init_period(period);
	int nb_routes_ok=0;
	int nb_eligible;
	int chosen_offset;
	int nb_seen;
	for(int i=0;i<nb_routes;i++)
	{
		nb_eligible = 0;
		for(int j=0;j<period;j++)
		{
			if(offset_ok(aller,retour,j,graph[i],period,message_size))
			{
				nb_eligible++;
			}
		}
		chosen_offset = rand()%nb_eligible;
		nb_seen = 0;
		for(int j=0;j<period;j++)
		{
			if(offset_ok(aller,retour,j,graph[i],period,message_size))
			{
				nb_seen++;
				if(nb_seen == chosen_offset){
					fill_periods(aller,retour,j,graph[i],message_size,period);
					nb_routes_ok++;
					break;
				}
					
			}
		}

	}
	free(aller);
	free(retour);
	return nb_routes_ok;
	*/

	//version opti
	int aller[nb_routes] ;
	int retour[nb_routes] ;
	int nb_eligible;
	int chosen_offset;
	int old_eligible;
	int old_offset;
	int out;
	int tmp,tmp2,tmp3;

	for(int i=0;i<nb_routes;i++)
	{
		aller[i]=0;
		retour[i]=0;
	}
	int nb_routes_ok=0;
	for(int i=0;i<nb_routes;i++)
	{
		nb_eligible = 0;
		int offset = 0;
		//printf("nnouvelle route %d decalage %d\n",i,graph[i]);
		//phase init
		tmp2=1;
		tmp3=1;
		out = 0;
		while(tmp2 || tmp3)
		{
			tmp2 = cols_check(aller,offset,message_size,period,nb_routes_ok);
			offset += tmp2;
			tmp3 =   cols_check(retour,offset+graph[i],message_size,period,nb_routes_ok);
			offset +=tmp3;

			if(offset >=period)
			{
				out =1;
				break;
			}
			
		}
		if(out)
		{
			//printf("0 places dispo apres scan debut\n");
			continue;
		}
		
	
		while(offset < period)
		{
			//On est deja placé à une position eligible si on est ici
			tmp = prochain_occup(aller,retour,offset,nb_routes_ok,period,message_size,graph[i]);
			nb_eligible += tmp;
			//printf("\noffset %d nb eligible %d\n",offset,nb_eligible);
		/*	for(int k=0;k<nb_routes_ok;k++)
				{printf("%d ",aller[k]);}printf("\n");
			printf("offset_retour %d \n",offset+graph[i]);
			for(int k=0;k<nb_routes_ok;k++)
				{printf("%d ",retour[k]);}printf("\n");
			printf("prochain occup : %d nb eligible is now %d\n",prochain_occup(aller,retour,offset,nb_routes_ok,period,message_size,graph[i]),nb_eligible);
			*/
			offset += tmp;
			//Ici l'offset est placé tau avant un message (a l'aller ou au retour, on fait en sorte qu'il collisione avec le message)
			offset += message_size;
			
			tmp2=1;
			tmp3=1;
			out = 0;
			while(tmp2 || tmp3)
			{
				tmp2 = cols_check(aller,offset,message_size,period,nb_routes_ok);
				offset += tmp2;
				tmp3 =   cols_check(retour,offset+graph[i],message_size,period,nb_routes_ok);
				offset +=tmp3;

				if(offset >=period)
				{
					out =1;
					break;
				}
				
			}
			if(out)
				break;
			
			//printf("offset apres cols check %d (%d %d %d %d %d)\n",offset,tmp,cols_check(aller,offset,message_size,period,nb_routes_ok),cols_check(retour,offset+graph[i],message_size,period,nb_routes_ok),tmp2,tmp3);
		}
		
		//printf("%d %d %d\n",nb_eligible,offset,nb_routes_ok);
		if(nb_eligible == 0)
		{
			//printf("NB eligible = %d \n",nb_eligible);
			continue;
		}
		chosen_offset = rand()%nb_eligible;
		nb_eligible = 0;
		offset = 0;
		
		//printf("%d choosen\n",chosen_offset);
		tmp2=1;
		tmp3=1;
		out = 0;
		while(tmp2 || tmp3)
		{
			tmp2 = cols_check(aller,offset,message_size,period,nb_routes_ok);
			offset += tmp2;
			tmp3 =   cols_check(retour,offset+graph[i],message_size,period,nb_routes_ok);
			offset +=tmp3;

			if(offset >=period)
			{
				out =1;
				break;
			}
			
		}
		if(out)
		{
			//printf("Scan second 0 places dispo \n");
			continue;
		}
		out = 0;
		while(offset < period)
		{
			old_offset = offset;
			old_eligible = nb_eligible;
			tmp = prochain_occup(aller,retour,offset,nb_routes_ok,period,message_size,graph[i]);
			nb_eligible += tmp;
			if(nb_eligible > chosen_offset)
			{
				out = 1;
				offset = old_offset + chosen_offset- old_eligible;
				break;
			}
			offset += tmp;
			offset += message_size;

			tmp2=1;
			tmp3=1;
			while(tmp2 || tmp3)
			{
				tmp2 = cols_check(aller,offset,message_size,period,nb_routes_ok);
				offset += tmp2;
				tmp3 =   cols_check(retour,offset+graph[i],message_size,period,nb_routes_ok);
				offset +=tmp3;
				if(offset >= period)
				{
					printf("Se poser la question si c'est normal que ca arrive\n");
					break;
				}
			}
			
		}
	//	printf("Apres sortie boucle 2, offset%d \n",offset);
		if(out)
		{	
			aller[nb_routes_ok]=offset;
			retour[nb_routes_ok]=(offset+graph[i])%period;
			nb_routes_ok++;
			graph[i]=-1;
		}

			
		else
		{
			printf("%d %d Ca ne devrait pas arriver, si on avait un offset eligible on devrait l'avoir retrouvé\n ",offset,nb_eligible);
			exit(2);
		}
		


		
	}
	if( first_fit_core(aller,retour,nb_routes_ok,graph,nb_routes,period,message_size,1))
	{
		printf("First fit arrive a rajouter une route, c'est étrange. %d %d\n",nb_eligible,verifie_solution(aller,retour,message_size,nb_routes_ok+1));
		
		exit(4);
	}
	if(!verifie_solution(aller,retour,message_size,nb_routes_ok))
	{
		printf("Error verifie solution RandomOffset\n") ;
		exit(3);
		}
	
	return nb_routes_ok;
}
int meta_offset(int *graph,int nb_routes,int period,int message_size)
{
	if(period%message_size != 0)
	{
		printf("Warning, P/tau != 0, meta offset cannot run\n");
		return 0;
	}

	//Version non opti
	/*
	int * aller = init_period(period);
	int * retour = init_period(period);
	int nb_routes_ok=0;
	
	for(int i=0;i<nb_routes;i++)
	{
		
		for(int j=0;j<period;j+=message_size)
		{
			if(offset_ok(aller,retour,j,graph[i],period,message_size))
			{
				fill_periods(aller,retour,j,graph[i],message_size,period);
				nb_routes_ok++;
				break;
			}
		}
		

	}
	free(aller);
	free(retour);
	return nb_routes_ok;*/

	//Version opti
	int aller[nb_routes] ;
	int retour[nb_routes] ;
	int out;
	int val;
	for(int i=0;i<nb_routes;i++)
	{
		aller[i]=0;
		retour[i]=0;
	}
	int nb_routes_ok=0;
	for(int i=0;i<nb_routes;i++)
	{
		int check_value = 1;
		int offset = 0;
		out  = 0;
		while(check_value)
		{
			check_value = cols_check(aller,offset,message_size,period,nb_routes_ok);
			val = cols_check(retour,offset+check_value+graph[i],message_size,period,nb_routes_ok);
	
			if(val)
				check_value += val + message_size - val%message_size;
			offset += check_value;

			if(offset >= period)
			{
				out = 1;
				break;
			}

		}

		//si on a pas trouvé d'offset pour cette route, on passe a la suivante
		if(out)
			continue;
		aller[nb_routes_ok]=offset;
		retour[nb_routes_ok]=(offset+graph[i])%period;
		nb_routes_ok++;
		
	}	
	if(!verifie_solution(aller,retour,message_size,nb_routes_ok))
	{
		printf("Error verifie solution MetaOffset\n") ;
		exit(4);
		}
	
	return nb_routes_ok;
}
int main()
{
	int* graph;
	int nb_simuls = NB_SIMULS;
	int message_size = MESSAGE_SIZE;
	int period = PERIOD;
	int nb_routes = PERIOD / message_size;
	int size_route = ROUTES_SIZE_MAX;
	srand(time(NULL));
	int tmp;
	int nb_algos = 3;
	char * noms[] = {"FirstFit","MetaOffset","RandomOffset"};
	char buf[256];
	FILE * f[nb_algos];
	float success[nb_algos];
	for(int i=0;i<nb_algos;i++)
	{
		sprintf(buf,"%s.plot",noms[i]);
		printf("Opening %s ...",buf);
		f[i] = fopen(buf,"w");
		if(!f[i])perror("Error while opening file\n");
		success[i]=0.0;
		printf("OK\n");
	}



	for(int i=33;i<=nb_routes;i++)
	{
		

		for(int algo=0;algo<nb_algos;algo++)
		{
			success[algo]= 0.0;
		}
		#pragma omp parallel for private(graph,tmp)  if (PARALLEL)
		for(int j=0;j<nb_simuls;j++)
		{
			fprintf(stdout,"\r Computing %d routes :  %d/%d",i,j+1,nb_simuls);fflush(stdout);
			graph = random_graph(i,size_route);
			for(int algo = 0;algo<nb_algos;algo++)
			{
				
				switch(algo){
					case 0:
						tmp =  first_fit(graph,i,period,message_size);
						
						break;
					case 1:
						
						tmp = meta_offset(graph,i,period,message_size);
						break;
					case 2:
						tmp = random_offset(graph,i,period,message_size);

						break;
					}
					if(tmp == i)
						#pragma omp atomic
							success[algo]++;

			}
			
			free(graph);
			
		}
		for(int algo=0;algo<nb_algos;algo++)
		{

			fprintf(f[algo],"%d %f\n",i,(success[algo]/nb_simuls)*100);
		}
		printf("\n");
	}

	for(int i=0;i<nb_algos;i++)
	{
		fclose(f[i]);
	}
	return 0;
		
	
}