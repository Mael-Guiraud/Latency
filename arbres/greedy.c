#include "structs.h"
#include "treatment.h"

#include <stdlib.h>
#include <stdio.h>
Assignment greedy(Graph g, int P, int message_size)
{
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);
	int offset;
	int begin_offset;

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
		begin_offset = route_length(g,i) + a->offset_forward[i];
		offset=begin_offset;
		//printf("%d begin offset \n",begin_offset);
		while( !message_collisions( g, order[i], offset,message_size,BACKWARD,P) )
		{
			//fprintf(stdout,"\roffset %d",offset);fflush(stdout);
			offset++;
			if(offset == (P+begin_offset))
			{
				free_assignment(a);
				//printf("\nNot possbile\n");
				return NULL;
			}
		}
		//printf("\nOffset %d for route %d\n",offset,order[i]);
		fill_period(g,order[i],offset,message_size,BACKWARD,P);
		a->offset_backward[order[i]]=offset;
		a->waiting_time[order[i]]=offset-begin_offset;
	}
	return a;

}

Assignment greedy_PRIME(Graph g, int P, int message_size)
{
	if(P < message_size)
		return NULL;
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);
	int offset;
	

	//printf("Forward \n");
	//for each route
	for(int i=0;i<g.nb_routes;i++)
	{
		//printf("Route %d : \n",i);
		offset=0;
		
		while((!message_collisions( g, i, offset,message_size,FORWARD,P))||(!message_collisions( g, i, offset+route_length(g,i),message_size,BACKWARD,P)) )
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
		fill_period(g,i,offset+route_length(g,i),message_size,BACKWARD,P);
		a->offset_backward[i]=offset+route_length(g,i);
		a->waiting_time[i]=0;
	}

	return a;

}

Assignment loaded_greedy(Graph g, int P, int message_size)
{
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);
	int offset,begin_offset;
	
	int routes[g.nb_routes];
	for(int i=0;i<g.nb_routes;i++)
		routes[i]=0;

	int * load_order = load_links(g);


	for(int i=0;i<g.arc_pool_size;i++)
	{
		
		for(int j=0;j<g.arc_pool[load_order[i]].nb_routes;j++)
		{
			if(!routes[g.arc_pool[load_order[i]].routes_id[j]])
			{
				offset = 0;
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
				fill_period(g,g.arc_pool[load_order[i]].routes_id[j],offset,message_size,FORWARD,P);
				a->offset_forward[g.arc_pool[load_order[i]].routes_id[j]]=offset;

				begin_offset = route_length(g,g.arc_pool[load_order[i]].routes_id[j]) + a->offset_forward[g.arc_pool[load_order[i]].routes_id[j]];
				while( !message_collisions( g, g.arc_pool[load_order[i]].routes_id[j], offset,message_size,BACKWARD,P) )
				{
					//fprintf(stdout,"\roffset %d",offset);fflush(stdout);
					offset++;
					if(offset == (P+begin_offset))
					{
						free_assignment(a);
						//printf("\nNot possbile\n");
						return NULL;
					}
				}
				//printf("\nOffset %d for route %d\n",offset,order[i]);
				fill_period(g,g.arc_pool[load_order[i]].routes_id[j],offset,message_size,BACKWARD,P);
				a->offset_backward[g.arc_pool[load_order[i]].routes_id[j]]=offset;
				a->waiting_time[g.arc_pool[load_order[i]].routes_id[j]]=offset-begin_offset;
				routes[g.arc_pool[load_order[i]].routes_id[j]] = 1;
			}
		}
		
	}
	free(load_order);
	return a;
	

}