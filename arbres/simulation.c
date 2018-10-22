#include "structs.h"
#include "test.h"
#include "init.h"
#include "treatment.h"
#include "config.h"
#include "greedy.h"

#include <stdio.h>
void test()
{
	
	int P = PERIOD_MIN;
	int message_size = MESSAGE_SIZE;
	Graph g = init_graph_random_tree(P);
	affiche_graph(g,P);
	printf("Liens load : ");
	affiche_tab(load_links(g),g.arc_pool_size);

	Assignment a = greedy( g, P, message_size);
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
void simul()
{

	int message_size = MESSAGE_SIZE;
	int nb_success;
	Graph g;
	for(int P=PERIOD_MIN;P<=PERIOD_MAX;P+=PERIOD_GAP)
	{
		nb_success=0;
		for(int i=0;i<NB_SIMULS;i++)
		{
			g= init_graph_tree(NB_ROUTES,P);
			Assignment a = greedy( g, P, message_size);
			if(a)
			{
				nb_success++;
				free_assignment(a);
			}
			free_graph(g);
			fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
		}	
		printf("\n Period : %d success : %d/%d.\n",P,nb_success,NB_SIMULS);
	}
	
		
}