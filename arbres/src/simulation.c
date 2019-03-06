#include "structs.h"
#include "test.h"
#include "init.h"
#include "treatment.h"
#include "config.h"
#include "greedy_without_waiting.h"
#include "greedy_waiting.h"

#include <time.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
void test()
{
	srand(time(NULL));
	int P ;
	int message_size = MESSAGE_SIZE;
	Graph g = init_graph_random_tree(STANDARD_LOAD);
	P= (load_max(g)*MESSAGE_SIZE)/STANDARD_LOAD;
	affiche_graph(g,P);
	printf("Liens load : ");
	affiche_tab(load_links(g),g.arc_pool_size);

	Assignment a = greedy_min_lost( g, P, message_size);
	if(a)
	{
		affiche_assignment( a,g.nb_routes);
		free_assignment(a);
	}
	else
		printf("No assignment found\n");
	affiche_graph(g,P);
	free_graph(g);

}

void simul(int seed,Assignment (*ptrfonction)(Graph,int,int,int),char * nom)
{
	srand(seed);
	int message_size = MESSAGE_SIZE;
	int nb_success;
	double moy_routes_scheduled ;
	Graph g;
	int P ;
	char buf[64];
	sprintf(buf,"../data/%s",nom);
	FILE * f = fopen(buf,"w");
	if(!f)perror("Error while opening file\n");
	for(int tmax=TMAX_MIN;tmax<=TMAX_MAX;tmax+=TMAX_GAP)
	{
		nb_success=0;
	
		moy_routes_scheduled = 0;
		
		for(int i=0;i<NB_SIMULS;i++)
		{
			g= init_graph_random_tree(STANDARD_LOAD);
			P= (load_max(g)*MESSAGE_SIZE)/STANDARD_LOAD;
			Assignment a = ptrfonction( g, P, message_size,tmax);
			if(a->all_routes_scheduled)
			{
				if(travel_time_max( g, tmax, a))
				{
					nb_success++;
				}
				
			}
			
			moy_routes_scheduled += a->nb_routes_scheduled;
			//printf("\n%d %d %d\n",g.nb_routes,a->nb_routes_scheduled,a->all_routes_scheduled);
			free_assignment(a);
	
			free_graph(g);
			fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
			fflush(stdout);
		}	
		printf("\n TMAX : %d success : %d/%d .\n",tmax,nb_success,NB_SIMULS);
		fprintf(f,"%d %f %f\n",tmax,nb_success/(float)NB_SIMULS,moy_routes_scheduled/(float)NB_SIMULS);
	}
	fclose(f);
	
		
}


void simul_period(int seed,Assignment (*ptrfonction)(Graph,int,int),char * nom)
{
	srand(seed);
	int message_size = MESSAGE_SIZE;
	int nb_success;
	Graph g;
	int P;
	double moy_routes_scheduled ;

	Assignment a;
	char buf[64];
	sprintf(buf,"../data/%s",nom);
	FILE * f = fopen(buf,"w");
	if(!f)perror("Error while opening file\n");
	for(double load=LOAD_MIN;load<=LOAD_MAX;load+=LOAD_GAP)
	{
		
		nb_success=0;
		moy_routes_scheduled = 0;
		#pragma omp parallel for private(g,P,a) schedule (static,1)
		for(int i=0;i<NB_SIMULS;i++)
		{
			
			//printf("%f %d\n",load, P);

			g= init_graph_random_tree(load);
			P= (load_max(g)*MESSAGE_SIZE)/load;

			
			a = ptrfonction( g, P, message_size);
			if(a->all_routes_scheduled)
			{
				//if(travel_time_max( g, tmax, a))
				//{
					#pragma omp atomic update
					nb_success++;
					
				//}
			}

			#pragma omp atomic update
			moy_routes_scheduled += a->nb_routes_scheduled;
			free_assignment(a);
			free_graph(g);
			fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
			fflush(stdout);
		}	
		printf("\n load : %f success : %d/%d \n",load,nb_success,NB_SIMULS);
		fprintf(f,"%f %f %f\n",load,nb_success/(float)NB_SIMULS,moy_routes_scheduled/(float)NB_SIMULS);
	}
	fclose(f);
	
		
}