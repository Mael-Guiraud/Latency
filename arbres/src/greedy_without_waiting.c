#include "structs.h"
#include "treatment.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

int tic_won(int * period, int offset, int P,int message_size)
{
	int won = 0;
	int i;
	//left
	for(i=0;(i<message_size) && (period[(offset-i+P)%P]==0 ) ;i++);
	won+= (message_size-i);

	//right
	for(i=0; (i<message_size) && (period[(offset+i+message_size)%P]==0);i++);
	won+= (message_size-i);
	
	return won;
}


int sum_tics_won(Graph g,int route,int offset,int message_size,int P)
{
	int sum = 0;
	for(int i=0;i<g.size_routes[route];i++)
		{
			
			//This is a contention point
			if(g.routes[route][i]->period_f != NULL)
			{
				
				if(g.routes[route][i]->period_f[offset%P] || g.routes[route][i]->period_f[(offset+message_size-1)%P] )
				{
					return -1;
				}
				sum +=tic_won(g.routes[route][i]->period_f,offset,P,message_size);
			}
			offset += g.routes[route][i]->length; 
			
		}

		for(int i=g.size_routes[route]-1;i>=0;i--)
		{
			
			//This is a contention point
			if(g.routes[route][i]->period_b != NULL)
			{
				if(g.routes[route][i]->period_b[offset%P] || g.routes[route][i]->period_b[(offset+message_size-1)%P] )
				{
					return -1;
				}
				sum +=tic_won(g.routes[route][i]->period_b,offset,P,message_size);
			}
			offset += g.routes[route][i]->length; 
			
		}
		return sum;
}

Assignment greedy_tics_won(Graph g, int P, int message_size)
{

	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;
	int tics_won_tmp;
	int max_tics_won;
	int best_offset;


	//for each route
	for(int i=0;i<g.nb_routes;i++)
	{
	
		max_tics_won = -1;
		best_offset = -1;
		for(int offset=0;offset<P;offset++)
		{
			tics_won_tmp = sum_tics_won( g, i, offset, message_size, P);
			
			if(tics_won_tmp > max_tics_won)
			{
				max_tics_won = tics_won_tmp;
				best_offset = offset;
			
			}
		}
		if(best_offset == -1)
		{
			
			a->offset_forward[i]=-1;
			a->offset_backward[i]=-1;
			a->waiting_time[i]=-1;
		}
		else
		{
			fill_period(g,i,best_offset,message_size,FORWARD,P);
			a->offset_forward[i]=best_offset;
			fill_period(g,i,best_offset+route_length(g,i),message_size,BACKWARD,P);
			a->offset_backward[i]=0;
			a->waiting_time[i]=0;
			a->nb_routes_scheduled++;	
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

Assignment greedy_PRIME(Graph g, int P, int message_size)
{

	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;
 	int offset;
 	int bool_found = 1;
	

	//for each route
	for(int i=0;i<g.nb_routes;i++)
	{

		offset=0;
		bool_found = 1;
		while((!message_no_collisions( g, i, offset,message_size,FORWARD,P))||(!message_no_collisions( g, i, offset+route_length(g,i),message_size,BACKWARD,P)) )
		{

			offset++;
			if(offset == P)
			{
				a->offset_forward[i]=-1;
				a->offset_backward[i]=-1;
				a->waiting_time[i]=-1;
				bool_found = 0;
				break;
			}
		}
		if(bool_found)
		{
			
			fill_period(g,i,offset,message_size,FORWARD,P);
			a->offset_forward[i]=offset;
			fill_period(g,i,offset+route_length(g,i),message_size,BACKWARD,P);
			a->offset_backward[i]=offset+route_length(g,i);
			a->waiting_time[i]=0;
			a->nb_routes_scheduled++;
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
