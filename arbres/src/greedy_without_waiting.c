#include "structs.h"
#include "treatment.h"
#include "starSPALL.h"
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
			g.routes[i][0]->routes_delay_f[i] = best_offset;
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
			g.routes[i][0]->routes_delay_f[i] = offset;
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
Assignment greedy_PRIME_allroutes(Graph g, int P, int message_size, element_sjt * tab)
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
		while((!message_no_collisions( g, tab[i].val, offset,message_size,FORWARD,P))||(!message_no_collisions( g, tab[i].val, offset+route_length(g,tab[i].val),message_size,BACKWARD,P)) )
		{

			offset++;
			if(offset == P)
			{
				a->offset_forward[tab[i].val]=-1;
				a->offset_backward[tab[i].val]=-1;
				a->waiting_time[tab[i].val]=-1;
				bool_found = 0;
				break;
			}
		}
		if(bool_found)
		{
			
			fill_period(g,tab[i].val,offset,message_size,FORWARD,P);
			a->offset_forward[tab[i].val]=offset;
			g.routes[i][0]->routes_delay_f[i] = offset;
			fill_period(g,tab[i].val,offset+route_length(g,tab[i].val),message_size,BACKWARD,P);
			a->offset_backward[tab[i].val]=offset+route_length(g,tab[i].val);
			a->waiting_time[tab[i].val]=0;
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
Assignment Prime_all_routes(Graph g, int P, int message_size,int tmax)
{
	Assignment a;
	element_sjt * tab = init_sjt(g.nb_routes);
	long long facto=fact(g.nb_routes);
	int travel_time;

	//Pour tout les ordres de routes :
	for(int i=0;i<facto;i++)
	{
	
		a = greedy_PRIME_allroutes(g,P,message_size,tab);
		if(a->all_routes_scheduled)
		{	
	
			travel_time = travel_time_max(g, tmax,a);
			if(travel_time != -1)
			{
				free(tab);
				return a;
			}
		}
		if(i!=facto-1)
			algo_sjt(tab,g.nb_routes);
		reset_periods(g,P);
		
	}
	free(tab);
	return a;
}