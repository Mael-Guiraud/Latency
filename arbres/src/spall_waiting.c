#include "structs.h"
#include "treatment.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#include "test.h"

Assignment RRH_first_spall(Graph g, int P, int message_size, int tmax)
{
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);


	int * id_routes = routes_sorted_lenght_arcs_bbu(g);

	int offset,begin_offset,offset_back;
	int best_offset=0;
	int best_begin = 0;
	int best_back = INT_MAX;
	int back_found;

	for(int j=0;j<g.nb_routes;j++)
	{
		
		for(offset = 0;offset<P;offset++)
		{

			if(message_no_collisions( g, id_routes[j], offset,message_size,FORWARD,P))
			{
			
				begin_offset = route_length(g,id_routes[j]) + offset;
				offset_back = begin_offset;
				back_found = 1;
				best_begin = 0;
				best_back = INT_MAX;
				while( !message_no_collisions( g, id_routes[j], offset_back,message_size,BACKWARD,P) )
				{

					offset_back++;
					if(offset_back == (P+begin_offset))
					{
						back_found = 0;
						break;
					}

					
				}
				if(back_found)
				{
					if((offset_back - begin_offset) < (best_back-best_begin))
					{
						best_offset = offset;
						best_begin = begin_offset;
						best_back = offset_back;
						break;
					}
					
				}

			}
				
		}
		if(best_back == INT_MAX)
		{

			a->offset_forward[id_routes[j]]=-1;
			a->offset_backward[id_routes[j]]=-1;
			a->waiting_time[id_routes[j]]=-1;
		
			
		}
		else
		{
			if((2*route_length( g,id_routes[j]) + best_back-best_begin ) > tmax )
			{
				a->offset_forward[id_routes[j]]=-1;
				a->offset_backward[id_routes[j]]=-1;
				a->waiting_time[id_routes[j]]=-1;
			
			}
			else
			{
				fill_period(g,id_routes[j],best_offset,message_size,FORWARD,P);
				a->offset_forward[id_routes[j]]=best_offset;

				
				fill_period(g,id_routes[j],best_back,message_size,BACKWARD,P);
				a->offset_backward[id_routes[j]]=best_back;
				a->waiting_time[id_routes[j]]=best_back-best_begin;	
				a->nb_routes_scheduled++;
			}
			
		}
				
				
			
		
		
	}
	free(id_routes);
	if(a->nb_routes_scheduled == g.nb_routes)
	{
		a->all_routes_scheduled = 1;

	}
	else
	{
		a->all_routes_scheduled = 0;
	}

	return a;
	

}
