#include "structs.h"
#include "treatment.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

int tic_lost(int * period, int offset, int P)
{
	int lost = 0;
	for(int i=0;i<P;i++)
	{
		if(period[(offset-i+P)%P]==0)
			lost++;
		else
			return lost;
	}
	return 0;// If the period is empty, no tics losts
}


int sum_tics_lost(Graph g,int route,int offset,int message_size,int P)
{
	int sum = 0;
	for(int i=0;i<g.size_routes[route];i++)
		{
			offset += g.routes[route][i]->length; 
			//This is a contention point
			if(g.routes[route][i]->period_f != NULL)
			{
				
				if(g.routes[route][i]->period_f[offset%P] || g.routes[route][i]->period_f[(offset+message_size-1)%P] )
				{
					return -1;
				}
				sum +=tic_lost(g.routes[route][i]->period_f,offset,P);
			}
			
		}

		for(int i=g.size_routes[route]-1;i>=0;i--)
		{
			offset += g.routes[route][i]->length; 
			//This is a contention point
			if(g.routes[route][i]->period_b != NULL)
			{
				if(g.routes[route][i]->period_b[offset%P] || g.routes[route][i]->period_b[(offset+message_size-1)%P] )
				{
					return -1;
				}
				sum +=tic_lost(g.routes[route][i]->period_b,offset,P);
			}
			
		}
		return sum;
}

Assignment greedy_min_lost(Graph g, int P, int message_size)
{
	if(P < message_size)
		return NULL;
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);
	int tics_lost_tmp;
	int min_tics_lost;
	int min_id;
	int nb_meta_inter = P/message_size;

	//printf("Forward \n");
	//for each route
	for(int i=0;i<g.nb_routes;i++)
	{
		//printf("Route %d : \n",i);
		min_tics_lost = INT_MAX;
		min_id = -1;
		for(int j=0;j<nb_meta_inter;j++)
		{
			tics_lost_tmp = sum_tics_lost( g, i, j*message_size, message_size, P);
			if((tics_lost_tmp < min_tics_lost) && (tics_lost_tmp != -1))
			{
				min_tics_lost = tics_lost_tmp;
				min_id = j;
			}
		}
		if(min_id == -1)
		{
			free_assignment(a);
			return NULL;
		}
		fill_period(g,i,min_id*message_size,message_size,FORWARD,P);
		a->offset_forward[i]=min_id*message_size;
		fill_period(g,i,min_id*message_size+route_length(g,i),message_size,BACKWARD,P);
		a->offset_backward[i]=0;
		a->waiting_time[i]=0;

			
	}

	return a;

}

