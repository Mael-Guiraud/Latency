#include "structs.h"
#include "treatment.h"
#include "simons.h"
#include <stdlib.h>
#include <stdio.h>


Assignment loaded_simons(Graph g, int P, int message_size, int tmax)
{
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);
	int offset;
	
	int routes[g.nb_routes];
	for(int i=0;i<g.nb_routes;i++)
		routes[i]=0;

	int * load_order = load_links(g);
	int * res_waiting;
	PreElems elems;


	for(int i=0;i<g.arc_pool_size;i++)
	{
		elems.nb_elems = g.arc_pool[load_order[i]].nb_routes;
		elems.release = malloc(sizeof(int)*elems.nb_elems);
		elems.deadline = malloc(sizeof(int)*elems.nb_elems);
		
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
						free(elems.release);
						free(elems.deadline);
						free(load_order);
						//printf("\nNot possbile\n");
						return NULL;
					}
				}
				fill_period(g,g.arc_pool[load_order[i]].routes_id[j],offset,message_size,FORWARD,P);
				a->offset_forward[g.arc_pool[load_order[i]].routes_id[j]]=offset;

				elems.release[j]= offset + route_length(g,g.arc_pool[load_order[i]].routes_id[j]) + route_length_untill_arc(g,g.arc_pool[load_order[i]].routes_id[j], &g.arc_pool[load_order[i]],BACKWARD);
				elems.deadline[j]= tmax - elems.release[i] - ( route_length(g,g.arc_pool[load_order[i]].routes_id[j])  - route_length_untill_arc(g,g.arc_pool[load_order[i]].routes_id[j], &g.arc_pool[load_order[i]],BACKWARD));
				routes[g.arc_pool[load_order[i]].routes_id[j]] = 1;
			}
			else
			{
				elems.release[j]= a->offset_forward[g.arc_pool[load_order[i]].routes_id[j]] + route_length(g,g.arc_pool[load_order[i]].routes_id[j]) + route_length_untill_arc(g,g.arc_pool[load_order[i]].routes_id[j], &g.arc_pool[load_order[i]],BACKWARD);
				elems.deadline[j]=  elems.release[i] + message_size;
			}
		}
		res_waiting = FPT_PALL(elems, message_size, P);
		if(res_waiting == NULL)
		{
			free_assignment(a);
			free(elems.release);
			free(elems.deadline);
			free(load_order);
			//printf("\nNot possbile\n");
			return NULL;
		}
		for(int j=0;j<g.arc_pool[load_order[i]].nb_routes;j++)
		{
			a->waiting_time[g.arc_pool[load_order[i]].routes_id[j]]=res_waiting[j];
			a->offset_backward[g.arc_pool[load_order[i]].routes_id[j]]=res_waiting[j] + a->offset_forward[g.arc_pool[load_order[i]].routes_id[j]] + route_length(g,g.arc_pool[load_order[i]].routes_id[j]) ;
		}

		free(elems.release);
		free(elems.deadline);
	}
	free(load_order);
	return a;
	

}