#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <omp.h>
#define MESSAGE_SIZE 1000
#define PERIOD 100000
#define STEP 1
#define ROUTES_SIZE_MAX 10000
#define NB_SIMULS 100

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
int first_fit(int* graph,int nb_routes,int period,int message_size)
{
	int * aller = init_period(period);
	int * retour = init_period(period);
	int nb_routes_ok=0;
	for(int i=0;i<nb_routes;i++)
	{
		for(int j=0;j<period;j++)
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
	return nb_routes_ok;
}
int random_offset(int *graph,int nb_routes,int period,int message_size)
{
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
}
int meta_offset(int *graph,int nb_routes,int period,int message_size)
{
	if(period%message_size != 0)
	{
		printf("Warning, P/tau != 0, meta offset cannot run\n");
		return 0;
	}
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
	char * noms[] = {"FirstFit","RandomOffset","MetaOffset"};
	char buf[256];
	FILE * f[nb_algos];
	float success[nb_algos];
	for(int i=0;i<nb_algos;i++)
	{
		sprintf(buf,"%s.plot",noms[i]);
		f[i] = fopen(buf,"w");
		if(!f[i])perror("Error while opening file\n");
		success[i]=0.0;
	}



	for(int i=33;i<nb_routes;i++)
	{
		printf("Computing %d routes : \n",i);

		for(int algo=0;algo<nb_algos;algo++)
		{
			success[algo]= 0.0;
		}
		#pragma omp parallel for private(graph,tmp)  
		for(int j=0;j<nb_simuls;j++)
		{
			fprintf(stdout,"\r%d/%d",j+1,nb_simuls);fflush(stdout);
			graph = random_graph(i,size_route);
			for(int algo = 0;algo<nb_algos;algo++)
			{
				printf("Appel de l'algo %d \n",algo);
				switch(algo){
					case 0:
						tmp =  first_fit(graph,i,period,message_size);
						#pragma omp critical
							success[algo] += tmp;
						break;
					case 1:
						tmp = random_offset(graph,i,period,message_size);
						#pragma omp critical
							success[algo] += tmp;
						break;
					case 2:
						tmp = meta_offset(graph,i,period,message_size);
						#pragma omp critical
							success[algo] += tmp;
						break;
					}

			}
			
			free(graph);
			
		}
		for(int algo=0;algo<nb_algos;algo++)
		{
			fprintf(f[algo],"%d %f",i,(success[algo]/nb_simuls)*100/i);
		}
	}

	for(int i=0;i<nb_algos;i++)
	{
		fclose(f[i]);
	}
	return 0;
		
	
}