#include "structs.h"
#include "treatment.h"

#include <stdlib.h>
#include <stdio.h>
Assignment loaded_greedy(Graph g, int P, int message_size)
{
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	int offset;
	int current_route;
	Arc current arc;
	int routes[g.nb_routes];
	for(int i=0;i<g.nb_routes;i++)
		routes[i]=0;

	int * load_order = load_links(g);


	for(int i=0;i<g.arc_pool_size;i++)
	{
		current_route
		for(int j=0;j<g.arc_pool[load_order[i]].nb_routes;j++)
		{
			offset = 0;
		}
		while( !message_collisions( g, g.arc_pool[load_order[i]].routes_id[j], offset,message_size,FORWARD,P) )
		{
			//fprintf(stdout,"\roffset %d",offset);fflush(stdout);
			offset++;
			if(offset == P)
			{
				free_assignment(a);
				//printf("\nNot possbile\n");
				return NULL;
			}
		}
		fill_period(g,i,offset,message_size,FORWARD,P);
		a->offset_forward[i]=offset;
	}
	//printf("Forward \n");
	//for each route
	for(int i=0;i<g.nb_routes;i++)
	{
		//printf("Route %d : \n",i);
		offset=0;
		
		while( !message_collisions( g, i, offset,message_size,FORWARD,P) )
		{
			//fprintf(stdout,"\roffset %d",offset);fflush(stdout);
			offset++;
			if(offset == P)
			{
				free_assignment(a);
				//printf("\nNot possbile\n");
				return NULL;
			}
		}
		//printf("\n Offset %d for route %d\n",offset,i);
		fill_period(g,i,offset,message_size,FORWARD,P);
		a->offset_forward[i]=offset;
	}

	int arrival_BBU[g.nb_routes];
	int order[g.nb_routes];
	for(int i=0;i<g.nb_routes;i++)
		order[i]=i;
	
	for(int i= 0;i<g.nb_routes;i++)
	{
		arrival_BBU[i] = route_length(g,i) + a->offset_forward[i];
	}
	tri_bulles(arrival_BBU,order, g.nb_routes);

	//printf("\nBackward\n");
		//for each route
	for(int i=0;i<g.nb_routes;i++)
	{
		//printf("Route %d : \n",order[i]);
		offset=0;
		
		while( !message_collisions( g, order[i], offset,message_size,BACKWARD,P) )
		{
			//fprintf(stdout,"\roffset %d",offset);fflush(stdout);
			offset++;
			if(offset == P)
			{
				free_assignment(a);
				//printf("\nNot possbile\n");
				return NULL;
			}
		}
		//printf("\nOffset %d for route %d\n",offset,order[i]);
		fill_period(g,order[i],offset,message_size,BACKWARD,P);
		a->offset_backward[order[i]]=offset;
	}
	return a;
	free(load_order);

}