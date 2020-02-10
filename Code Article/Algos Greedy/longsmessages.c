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
#define ROUTES_SIZE_MAX 1000
#define NB_SIMULS 1000
#define PARALLEL 1

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
int min(int a, int b){
	return (a<b)?a:b;
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
				return (P[i]+message_size)%per - offset%per ;
			}	
			if(( (offset+message_size)%per > P[i]%per) ||( (offset)%per >= P[i]%per) ||( (offset+message_size)%per <= (P[i]+message_size)%per) )
			{
				return per - (offset)%per + (P[i]+message_size)%per ;
			}	
		}
		else
		{
			if((offset%per >= P[i]%per) && ( offset%per < (P[i]+message_size)%per)  )
			{
				return (P[i]+message_size)%per - offset%per ;
			}	
			if(( (offset+message_size)%per > P[i]%per) && ( (offset+message_size)%per <= (P[i]+message_size)%per) )
			{
				if(offset%per > (P[i]+message_size)%per)
				{
					return per - (offset)%per + (P[i]+message_size)%per ; 
				}
				else
				{
					return (P[i]+message_size)%per - offset%per ;
				}
			}
		}
	}

	return 0;

}

int verifie_solution(int * aller, int * retour,int message_size,int nb_routes,int period)
{
	for(int i=0;i<nb_routes;i++)
	{
		for(int j=0;j<nb_routes;j++)
		{
			if(i!=j)
			{
				if(min(abs(aller[i]-aller[j]),abs(aller[i]+period-aller[j]))<message_size)
				{
					printf(" pb aller i %d j %d val |%d - %d| = %d",i,j,aller[i],aller[j],abs(aller[i]-aller[j]));
					return 0;
				}
				if(min(abs(retour[i]-retour[j]),abs(retour[i]+period-retour[j]))<message_size)
				{
					printf(" pb retour i %d j %d val |%d - %d| = %d",i,j,retour[i],retour[j],abs(retour[i]-retour[j]));
					return 0;
				}
			}
		
		}
	}
	return 1;
}
int first_fit_core(int*aller,int*retour,int begin,int* graph,int nb_routes,int period,int message_size)
{
	int nb_routes_ok=0;
	int out;
	for(int i=0;i<nb_routes;i++)
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
			aller[nb_routes_ok+begin]=offset;
			retour[nb_routes_ok+begin]=(offset+graph[i])%period;
			nb_routes_ok++;
			
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
	int nb_ok = first_fit_core(aller,retour,0,graph,nb_routes,period,message_size);

	if(!verifie_solution(aller,retour,message_size,nb_ok,period))
	{
		printf("Error verifie solution first fit \n") ;
		exit(3);
	}
	return nb_ok;
	
}

int trouve_plus_proche_supp(int *t, int n, int size,int message_size,int period,int decalage, int type)
{
	int maxtmp=INT_MAX;
	int mini = INT_MAX;

	for(int i= 0;i<size;i++)
	{
		if(t[i]<mini)
			mini = t[i];
		if(t[i]-message_size>=n)
		{
			if(maxtmp > t[i]-message_size)
				maxtmp = t[i]-message_size;
		}

	}
	
	if(maxtmp == INT_MAX)
	{
		if(type == 0)//A l'aller
		{
			return period-n+mini-message_size+1;
		}
		else
		{
	
			return min(period-n+mini-message_size+1,period-n+decalage+1);
		}
		
	}

	return maxtmp-n+1;
}

int nb_lost_after(int *t, int n, int size,int message_size,int period)
{

	if(size == 0)
		return 0;
	int maxtmp=INT_MAX;
	int mini = INT_MAX;

	for(int i= 0;i<size;i++)
	{
		if(t[i]<mini)
			mini = t[i];
		if(t[i]-message_size>=n)
		{
			if(maxtmp > t[i]-message_size)
				maxtmp = t[i]-message_size;
		}

	}
	
	if(maxtmp == INT_MAX)
	{
		
		return period-n+mini-message_size;

		
	}
	return maxtmp-n;
}
int nb_lost_before(int *t, int n, int size,int message_size,int period)
{
	if(size == 0)
		return 0;
	int mintmp=-1;
	int maxi = -1;

	for(int i= 0;i<size;i++)
	{
		if(t[i]>maxi)
			maxi = t[i];
		if(t[i]+message_size<=n)
		{
			if(mintmp < t[i]+message_size)
				mintmp = t[i]+message_size;
		}


	}
	
	if(mintmp == -1)
	{
		return n+period-(maxi+message_size);
	}
	
	return n-mintmp;
}

//renvoie le nombre de tics disponible avant le prochain tic occupé dans la periode
int prochain_occup(int * aller, int * retour, int offset, int nb_routes,int period,int message_size,int decalage)
{
	if(nb_routes == 0)
		return period;
	int next_aller = trouve_plus_proche_supp(aller,offset,nb_routes,message_size,period,decalage,0);
	int next_retour = trouve_plus_proche_supp(retour,(offset+decalage)%period,nb_routes,message_size,period,decalage,1);
	return min(next_retour,next_aller);
}

int random_offset(int *graph,int nb_routes,int period,int message_size)
{

	//version opti
	int aller[nb_routes] ;
	int retour[nb_routes] ;
	int nb_eligible;
	int chosen_offset;
	int old_eligible;
	int old_offset;
	int out;
	int tmp,tmp2,tmp3;
	int cpygraph[nb_routes];
	memcpy(cpygraph,graph,sizeof(int)*nb_routes);

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
			continue;
		}
		while(offset < period)
		{
			//On est deja placé à une position eligible si on est ici
			tmp = prochain_occup(aller,retour,offset,nb_routes_ok,period,message_size,graph[i]);
			nb_eligible += tmp;
			offset += tmp;
			//Ici l'offset est placé tau avant un message (a l'aller ou au retour, on fait en sorte qu'il collisione avec le message)
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
		}
		if(nb_eligible == 0)
		{
			continue;
		}
		chosen_offset = rand()%nb_eligible;
		nb_eligible = 0;
		offset = 0;
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
		if(out)
		{	
			aller[nb_routes_ok]=offset%period;
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
	if( first_fit_core(aller,retour,nb_routes_ok,graph,nb_routes,period,message_size))
	{
		printf("First fit arrive a rajouter une route, c'est étrange (RandomOffset). \n");
		
		exit(4);
	}
	if(!verifie_solution(aller,retour,message_size,nb_routes_ok,period))
	{
		printf("Error verifie solution RandomOffset\n") ;
		exit(3);
	}
	memcpy(graph,cpygraph,sizeof(int)*nb_routes);
	return nb_routes_ok;
}
typedef struct{
	int offset;
	int lost;
} Losttics;
int min_tics_lost(Losttics * tab, int size)
{
	int min = INT_MAX;
	int offset = INT_MAX;
	for(int i=0;i<size;i++)
	{
		if(tab[i].lost < min)
		{
			min = tab[i].lost;
			offset = tab[i].offset;
		}
	}
	if(offset == INT_MAX)
	{
		printf("On à pas trouvé de valeur min dans le tableau de tics lost, ce n'est pas normal.\n");
		exit(7);
	}
	return offset;
}
int super_compact(int *graph,int nb_routes,int period,int message_size)
{

	//version opti
	int aller[nb_routes] ;
	int retour[nb_routes] ;
	int nb_eligible;
	int chosen_offset;
	int old_eligible;
	int old_offset;
	int out;
	int tmp,tmp2,tmp3;
	int idlost = 0;
	Losttics tablost[nb_routes*4];
	int cpygraph[nb_routes];
	memcpy(cpygraph,graph,sizeof(int)*nb_routes);
	for(int i=0;i<nb_routes;i++)
	{
		aller[i]=0;
		retour[i]=0;
	}
	int nb_routes_ok=0;
	for(int i=0;i<nb_routes;i++)
	{
		idlost = 0;
		nb_eligible = 0;
		int offset = 0;
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
			continue;
		}		
		while(offset < period)
		{
			
			tablost[idlost].offset = offset;
			tablost[idlost].lost = min(message_size,nb_lost_before(aller,offset,nb_routes_ok,message_size,period))
								  +min(message_size,nb_lost_after(aller,offset,nb_routes_ok,message_size,period))
								  +min(message_size,nb_lost_before(retour,(offset+graph[i])%period,nb_routes_ok,message_size,period))
								  +min(message_size,nb_lost_after(retour,(offset+graph[i])%period,nb_routes_ok,message_size,period));
			idlost++;
			//On est deja placé à une position eligible si on est ici
			tmp = prochain_occup(aller,retour,offset,nb_routes_ok,period,message_size,graph[i]);
			offset += tmp-1;
			tablost[idlost].offset = offset;
			tablost[idlost].lost = min(message_size,nb_lost_before(aller,offset,nb_routes_ok,message_size,period))
								  +min(message_size,nb_lost_after(aller,offset,nb_routes_ok,message_size,period))
								  +min(message_size,nb_lost_before(retour,(offset+graph[i])%period,nb_routes_ok,message_size,period))
								  +min(message_size,nb_lost_after(retour,(offset+graph[i])%period,nb_routes_ok,message_size,period));
			idlost++;
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
		}
		offset = min_tics_lost(tablost,idlost);
		aller[nb_routes_ok]=offset%period;
		retour[nb_routes_ok]=(offset+graph[i])%period;
		nb_routes_ok++;
		graph[i]=-1;	
	}
	if( first_fit_core(aller,retour,nb_routes_ok,graph,nb_routes,period,message_size))
	{
		printf("First fit arrive a rajouter une route, c'est étrange (SuperCompact). \n");
		
		exit(6);
	}
	if(!verifie_solution(aller,retour,message_size,nb_routes_ok,period))
	{
		printf("Error verifie solution SuperCompact\n") ;
		exit(5);
	}
		memcpy(graph,cpygraph,sizeof(int)*nb_routes);

	return nb_routes_ok;
}
int meta_offset(int *graph,int nb_routes,int period,int message_size)
{
	if(period%message_size != 0)
	{
		printf("Warning, P/tau != 0, meta offset cannot run\n");
		return 0;
	}
	int aller[nb_routes] ;
	int retour[nb_routes] ;
	int out;
	for(int i=0;i<nb_routes;i++)
	{
		aller[i]=0;
		retour[i]=0;
	}
	int nb_routes_ok=0;
	for(int i=0;i<nb_routes;i++)
	{
		out = 0;
		for(int offset=0;offset<period;offset+= message_size)
		{
			if(!cols_check(aller,offset,message_size,period,nb_routes_ok) && !cols_check(retour,offset+graph[i],message_size,period,nb_routes_ok) )
			{
				aller[nb_routes_ok]=offset;
				retour[nb_routes_ok]=(offset+graph[i])%period;
				nb_routes_ok++;
				out = 1;
			}
			if(out)
				break;
		}
	
	}	
	if(!verifie_solution(aller,retour,message_size,nb_routes_ok,period))
	{
		printf("Error verifie solution MetaOffset\n") ;
		exit(4);
		}
	
	return nb_routes_ok;
}
//trie le tableau decalage en fonction des releases
void tri_bulles(int* releases,int* decalage,int taille)
{
	int sorted;
	int tmp;
	int tmp_ordre;

	int tabcpy[taille];
	for(int i=0;i<taille;i++)tabcpy[i]=releases[i];

	for(int i=taille-1;i>=1;i--)
	{
		sorted = 1;
		for(int j = 0;j<=i-1;j++)
		{

			if(tabcpy[j+1]<tabcpy[j])
			{
				tmp_ordre = decalage[j+1];
				decalage[j+1]=decalage[j];
				decalage[j]=tmp_ordre;
				tmp = tabcpy[j+1];
				tabcpy[j+1]= tabcpy[j];
				tabcpy[j]= tmp;
				sorted = 0;
			}
		}
		if(sorted){return;}
	}

}

//check if two messages are a pair or not
int check_pair(int di, int dj,int message_size,int period)
{
	int a = di/message_size;
	int b = dj/message_size;
	int c = period/message_size;

	if((b-a==1) || (b-a==-(c-1)))
	{
		return 0;
	}
	return 1;
}
int mod(int a, int b)
{
	int res = a%b;
	if(res >= 0)
	{
		return res;
	}
	else
	{
		while(res < 0)
		{
			res += b;
		}
		return res;
	}
}
int pair_meta_offset(int *graph,int nb_routes,int period,int message_size)
{
	int cpygraph[nb_routes];
	memcpy(cpygraph,graph,sizeof(int)*nb_routes);
	int release[nb_routes];
	for(int i=0;i<nb_routes;i++)
	{
		release[i]= graph[i]%message_size;
	}
	tri_bulles(release,graph,nb_routes);
	int nb_routes_ok = 0;
	//Le graph est trié en fonction des releases
	int aller[nb_routes] ;
	int retour[nb_routes] ;
	int chkpair;
	int out;
	for(int i=0;i<nb_routes;i++)
	{
		aller[i]=0;
		retour[i]=0;
	}
	for(int i=0;i<nb_routes-1;i+=2)
	{
		chkpair = 0;
		while(!check_pair(graph[i],graph[i+1],message_size,period))
		{
			i++;
			if(i >nb_routes - 2)
			{
				chkpair = 1;
				break; 
			}
		}
		//Fin des routes.
		if(chkpair == 1)
			break;
		out = 0;
		for(int offset = 0 ;offset < period;offset+= message_size)
		{
			int offset2 = mod(offset+graph[i]+message_size + graph[i+1]%message_size-graph[i]%message_size-graph[i+1],period);
			if(!cols_check(aller,offset,message_size,period,nb_routes_ok) && !cols_check(aller,offset2,message_size,period,nb_routes_ok) &&
			   !cols_check(retour,offset+graph[i],message_size,period,nb_routes_ok) && !cols_check(retour,offset2+graph[i+1],message_size,period,nb_routes_ok))
			{
				aller[nb_routes_ok]=offset;
				retour[nb_routes_ok]=(offset+graph[i])%period;
				nb_routes_ok++;
				aller[nb_routes_ok]=offset2%period;
				retour[nb_routes_ok]=(offset2+graph[i+1])%period;
				nb_routes_ok++;
				graph[i]=-1;
				graph[i+1]=-1;
				out = 1;
			}
			if(out)
				break;
		}
	}

	for(int i=0;i<nb_routes;i++)
	{
		if(graph[i] != -1)
		{
			out = 0;
			for(int offset=0;offset<period;offset+= message_size)
			{
				if(!cols_check(aller,offset,message_size,period,nb_routes_ok) && !cols_check(retour,offset+graph[i],message_size,period,nb_routes_ok) )
				{
					aller[nb_routes_ok]=offset;
					retour[nb_routes_ok]=(offset+graph[i])%period;
					nb_routes_ok++;
					out = 1;
				}
				if(out)
					break;
			}
		}
		
	
	}	
	if(!verifie_solution(aller,retour,message_size,nb_routes_ok,period))
	{
		printf("Error verifie solution MetaOffsetPair\n") ;
		exit(8);
	}

	memcpy(graph,cpygraph,sizeof(int)*nb_routes);
	return nb_routes_ok;
}
int pair(int *graph,int nb_routes,int period,int message_size)
{
	int cpygraph[nb_routes];
	memcpy(cpygraph,graph,sizeof(int)*nb_routes);
	int release[nb_routes];
	for(int i=0;i<nb_routes;i++)
	{
		release[i]= graph[i]%message_size;
	}
	tri_bulles(release,graph,nb_routes);
	int nb_routes_ok = 0;
	//Le graph est trié en fonction des releases
	int aller[nb_routes] ;
	int retour[nb_routes] ;
	int chkpair;
	for(int i=0;i<nb_routes;i++)
	{
		aller[i]=0;
		retour[i]=0;
	}
	for(int i=0;i<nb_routes-1;i+=2)
	{
		chkpair = 0;
		while(!check_pair(graph[i],graph[i+1],message_size,period))
		{
			i++;
			if(i >nb_routes - 2)
			{
				chkpair = 1;
				break; 
			}
		}
		//Fin des routes.
		if(chkpair == 1)
			break;
		int check_value = 1;
		int check_value2 = 1;
		int offset = 0;
		int out  = 0;
		int offset2;
		while(check_value || check_value2)
		{
			offset2 = mod(offset+graph[i]+message_size + graph[i+1]%message_size-graph[i]%message_size-graph[i+1],period);
		
			check_value = cols_check(aller,offset,message_size,period,nb_routes_ok);
			check_value += cols_check(aller,offset2,message_size,period,nb_routes_ok);
			offset += check_value ;
			offset2 = mod(offset+graph[i]+message_size + graph[i+1]%message_size-graph[i]%message_size-graph[i+1],period);
	
			check_value2 = 1;
			while(check_value2)
			{
				check_value2 = cols_check(retour,offset+graph[i],message_size,period,nb_routes_ok);
				if(!check_value2)
				{
					check_value2 = cols_check(retour,offset2+graph[i+1],message_size,period,nb_routes_ok);
				}
				offset += check_value2;
				offset2 = mod(offset+graph[i]+message_size + graph[i+1]%message_size-graph[i]%message_size-graph[i+1],period);
				
				if(offset >= period)
				{
					out = 1;
					break;
				}
			}
			
		
			if(offset >= period)
			{
				out = 1;
				break;
			}
		}
		//si on a pas trouvé d'offset pour cette route, on passe a la suivante
		if(out)
			continue;

		offset2 = mod(offset+graph[i]+message_size + graph[i+1]%message_size-graph[i]%message_size-graph[i+1],period);
		aller[nb_routes_ok]=offset;
		retour[nb_routes_ok]=(offset+graph[i])%period;
		nb_routes_ok++;
		aller[nb_routes_ok]=offset2%period;
		retour[nb_routes_ok]=(offset2+graph[i+1])%period;
		nb_routes_ok++;

		graph[i]=-1;
		graph[i+1]=-1;


	}

	//Fin des paires, on lance le first fit
	nb_routes_ok += first_fit_core(aller,retour,nb_routes_ok,graph,nb_routes,period,message_size);
	
	if(!verifie_solution(aller,retour,message_size,nb_routes_ok,period))
	{
		printf("Error verifie solution pairs\n") ;
		exit(8);
	}

	memcpy(graph,cpygraph,sizeof(int)*nb_routes);
	return nb_routes_ok;
}

void print_gnuplot(char ** algos, int nb_algos,int period, int tau,int nb_messages)
{

	char buf[64];
	sprintf(buf,"success.gplt");
	FILE* f_GPLT = fopen(buf,"w");
	
	if(!f_GPLT){perror("Opening gplt file failure\n");exit(2);}

	for(int i=0;i<nb_algos;i++)
	{
		if(i>0)
		{
			fprintf(f_GPLT,"re");
		}	
		fprintf(f_GPLT,"plot '%s.plot' using 1:2 with lines title \"%s\" \n",algos[i],algos[i]);
	}

	
	fprintf(f_GPLT,"set term postscript color solid\n"

	"set title \"Performance of different algorithms for PAZL tau = %d , P = %d , nbroutes = %d\"\n"
	"set xlabel \"Nb routes\" \n"
	//"set xtics 10\n" 

	"set key bottom left \n"
	"set ylabel \"Success rate\"\n"
	"set output '| ps2pdf - success_period%d_tau%d_nb-mess_%d.pdf'\nreplot\n",tau,period,nb_messages,period,tau,nb_messages);
	fclose(f_GPLT);
	

}
int main(int argc,char * argv[])
{
	int* graph;

	int nb_simuls = NB_SIMULS;
	int message_size = MESSAGE_SIZE;
	int period = PERIOD;
	int nb_routes = PERIOD / message_size;
	int size_route = ROUTES_SIZE_MAX;
	srand(time(NULL));
	int tmp;
	int nb_algos = 6;
	char * noms[] = {"FirstFit","MetaOffset","RandomOffset","SuperCompact","Paires","MetaOffsetPaires"};
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

	for(int i=1;i<=nb_routes;i++)
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
			pair(graph,i,period,message_size);
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
					case 3:
						tmp = super_compact(graph,i,period,message_size);
						break;
					case 4:
						tmp = pair(graph,i,period,message_size);
						break;
					case 5:
						tmp = pair_meta_offset(graph,i,period,message_size);
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
	print_gnuplot( noms,  nb_algos,period, message_size,nb_routes);
	return 0;
		
	
}
