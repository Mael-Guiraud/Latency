#include "structs.h"
#include "treatment.h"

#include <stdlib.h>
#include <stdio.h>
Assignment greedy(Graph g, int P, int message_size)
{
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	int offset;

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
		//printf("\nOffset %d for route %d\n",offset,i);
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

}