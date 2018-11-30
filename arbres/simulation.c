#include "structs.h"
#include "test.h"
#include "init.h"
#include "treatment.h"
#include "config.h"
#include "greedy_without_waiting.h"
#include "greedy_waiting.h"

#include <stdio.h>
void test()
{
	
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

void simul(Assignment (*ptrfonction)(Graph,int,int,int),char * nom)
{

	int message_size = MESSAGE_SIZE;
	int nb_success;
	Graph g;
	int P ;
	double loadmax = 0;
	double tmpl;
	FILE * f = fopen(nom,"w");
	if(!f)perror("Error while opening file\n");
	for(int tmax=TMAX_MIN;tmax<=TMAX_MAX;tmax+=TMAX_GAP)
	{
		nb_success=0;
		loadmax = 0;
		for(int i=0;i<NB_SIMULS;i++)
		{

			g= init_graph_random_tree(STANDARD_LOAD);
			P= (load_max(g)*MESSAGE_SIZE)/STANDARD_LOAD;
			Assignment a = ptrfonction( g, P, message_size,tmax);
			if(a)
			{
				if(travel_time_max( g, tmax, a))
				{

					nb_success++;
					free_assignment(a);
				}
			}
			tmpl= (load_max(g)*MESSAGE_SIZE)/(float)P;
			if(loadmax < tmpl)loadmax = tmpl;
			free_graph(g);
			fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
			fflush(stdout);
		}	
		printf("\n TMAX : %d success : %d/%d (Loadmax = %f).\n",tmax,nb_success,NB_SIMULS,loadmax);
		fprintf(f,"%d %f\n",tmax,nb_success/(float)NB_SIMULS);
	}
	
		
}


void simul_period(Assignment (*ptrfonction)(Graph,int,int),char * nom)
{

	int message_size = MESSAGE_SIZE;
	int nb_success;
	Graph g;
	int P;

	double loadmax = 0;
	double tmpl;
	FILE * f = fopen(nom,"w");
	if(!f)perror("Error while opening file\n");
	for(double load=LOAD_MIN;load<=LOAD_MAX;load+=LOAD_GAP)
	{
		loadmax = 0;
		nb_success=0;
		for(int i=0;i<NB_SIMULS;i++)
		{
			
			//printf("%f %d\n",load, P);

			g= init_graph_random_tree(load);
			P= (load_max(g)*MESSAGE_SIZE)/load;
			tmpl = load_max(g)*MESSAGE_SIZE /(float)P;
			
			Assignment a = ptrfonction( g, P, message_size);
			if(a)
			{
				//if(travel_time_max( g, tmax, a))
				//{

					nb_success++;
					free_assignment(a);
				//}
			}
			
			if(loadmax < tmpl)loadmax = tmpl;
			free_graph(g);
			fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
			fflush(stdout);
		}	
		printf("\n load : %f success : %d/%d (%f)\n",load,nb_success,NB_SIMULS,loadmax);
		fprintf(f,"%f %f\n",load,nb_success/(float)NB_SIMULS);
	}
	
		
}