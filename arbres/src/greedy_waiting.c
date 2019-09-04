#include "structs.h"
#include "treatment.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>



Assignment greedy(Graph g, int P, int message_size, int tmax)
{
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);
	int offset;
	int begin_offset;
	int bool_fail = 0;
	int bool_fail_tmax = 0;
	

	//for each route
	for(int i=0;i<g.nb_routes;i++)
	{
		
		offset=0;

	
		while( !message_no_collisions( g, i, offset,message_size,FORWARD,P) )
		{
			
			offset++;
			if(offset == P)
			{
				
				bool_fail = 1;
				break;
			}			
		}
		if(!bool_fail)
		{
			fill_period(g,i,offset,message_size,FORWARD,P);
			a->offset_forward[i]=offset;
		}
		else
		{
			a->offset_forward[i]=-1;
			bool_fail = 0;
		}
		
	}

	int deadline[g.nb_routes];
	int order[g.nb_routes];
	for(int i=0;i<g.nb_routes;i++)
		order[i]=i;
	
	for(int i= 0;i<g.nb_routes;i++)
	{
		deadline[i] = tmax - (2 * route_length(g,i) + a->offset_forward[i]);
	}

	tri_bulles_inverse(deadline,order, g.nb_routes);

	
		//for each route
	
	for(int i=0;i<g.nb_routes;i++)
	{
		bool_fail = 0;
		bool_fail_tmax = 0;
		//For the route we succeced to schedule in the way forward
		if(a->offset_forward[order[i]] != -1)
		{
			
			begin_offset = route_length(g,order[i]) + a->offset_forward[order[i]];
			offset=begin_offset;
			
			while( !message_no_collisions( g, order[i], offset,message_size,BACKWARD,P) )
			{

				offset++;
				if(offset == (P+begin_offset))
				{
					bool_fail = 1;
				}
			}
			if(!bool_fail)
			{
				
				if( (2*route_length( g,order[i]) + offset-begin_offset ) > tmax )
				{
				
					bool_fail_tmax = 1;
				}
				if(!bool_fail_tmax)
				{
				
					fill_period(g,order[i],offset,message_size,BACKWARD,P);
					a->offset_backward[order[i]]=offset;
					a->waiting_time[order[i]]=offset-begin_offset;
					a->nb_routes_scheduled++;
				}
				else
				{
					a->offset_backward[order[i]]=-1;
					a->waiting_time[order[i]]=-1;
				}
				
			}
			else
			{
				a->offset_backward[order[i]]=-1;
				a->waiting_time[order[i]]=-1;
			}
		}
		else
		{
			a->offset_backward[order[i]]=-1;
			a->waiting_time[order[i]]=-1;
		}
	}
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


/*
ARG mode :
	- 0 : sort the routes on arcs by id
	- 1 : uses the longest route first
	- 2 : use the route with the most collisions first
*/
Assignment greedy_by_arcs(Graph g, int P, int message_size, int tmax,int mode)
{
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);
	int offset,begin_offset,offset_back;
	int best_offset=0;
	int best_begin = 0;
	int best_back = INT_MAX;
	int back_found;
	int routes[g.nb_routes];
	for(int i=0;i<g.nb_routes;i++)
		routes[i]=0;

	int * load_order = load_links(g);//store the id of the arcs, by the most loaded to the less loaded
	int * id_routes;

	for(int i=0;i<g.arc_pool_size;i++)
	{
		switch (mode) {// id_routes  stores the id of the routes to use, considering the sort chosen

		case 0 :
			id_routes = routes_by_id(g.arc_pool[load_order[i]]); 
		break;

		case 1 :
			id_routes = sort_longest_routes_on_arc(g, g.arc_pool[load_order[i]]);
		break;
		case 2:
			id_routes = sort_routes_by_collisions(g,g.arc_pool[load_order[i]]);
		break;
		default:
			printf("Mode non connu(greedy by arcs).\n");exit(97);
		break;
		}
		for(int j=0;j<g.arc_pool[load_order[i]].nb_routes;j++)
		{
			
			if(!routes[id_routes[j]])
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
					routes[id_routes[j]] = 1;
					
				}
				else
				{
					if( (2*route_length( g,id_routes[j]) + best_back-best_begin ) > tmax )
					{
						a->offset_forward[id_routes[j]]=-1;
						a->offset_backward[id_routes[j]]=-1;
						a->waiting_time[id_routes[j]]=-1;
						routes[id_routes[j]] = 1;
					}
					else
					{
						fill_period(g,id_routes[j],best_offset,message_size,FORWARD,P);
						a->offset_forward[id_routes[j]]=best_offset;

						
						fill_period(g,id_routes[j],best_back,message_size,BACKWARD,P);
						a->offset_backward[id_routes[j]]=best_back;
						a->waiting_time[id_routes[j]]=best_back-best_begin;
						routes[id_routes[j]] = 1;	
						a->nb_routes_scheduled++;
					}
					
				}
				
				
			}
		}
		free(id_routes);
	}
	free(load_order);
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

Assignment loaded_greedy(Graph g, int P, int message_size, int tmax)
{
	return greedy_by_arcs(g,P,message_size,tmax,0);
}

Assignment loaded_greedy_longest(Graph g, int P, int message_size, int tmax)
{
	return greedy_by_arcs(g,P,message_size,tmax,1);
}

Assignment loaded_greedy_collisions(Graph g, int P, int message_size, int tmax)
{
	return greedy_by_arcs(g,P,message_size,tmax,2);
}
