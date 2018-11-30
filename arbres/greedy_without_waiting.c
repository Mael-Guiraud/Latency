#include "structs.h"
#include "treatment.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

int tic_won(int * period, int offset, int P,int message_size)
{
	int won = 0;

	//left
	for(int i=0;i<message_size;i++)
	{
		if(period[(offset-i+P)%P]!=0)
		{
			won+= (message_size-i);
			break;
		}
	}

	//right
	for(int i=0;i<message_size;i++)
	{
		if(period[(offset+i+message_size)%P]!=0)
		{
			won+= (message_size-i);
			break;
		}
	}
	return won;// If the period is empty, no tics losts
}


int sum_tics_won(Graph g,int route,int offset,int message_size,int P)
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
				sum +=tic_won(g.routes[route][i]->period_f,offset,P,message_size);
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
				sum +=tic_won(g.routes[route][i]->period_b,offset,P,message_size);
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
	int tics_won_tmp;
	int max_tics_won;
	int id;
	int nb_meta_inter = P/message_size;

	//printf("Forward \n");
	//for each route
	for(int i=0;i<g.nb_routes;i++)
	{
		//printf("Route %d : \n",i);
		max_tics_won = -1;
		id = -1;
		for(int j=0;j<nb_meta_inter;j++)
		{
			tics_won_tmp = sum_tics_won( g, i, j*message_size, message_size, P);
			if((tics_won_tmp > max_tics_won) && (tics_won_tmp != -1))
			{
				max_tics_won = tics_won_tmp;
				id = j;
			}
		}
		if(id == -1)
		{
			free_assignment(a);
			return NULL;
		}
		fill_period(g,i,id*message_size,message_size,FORWARD,P);
		a->offset_forward[i]=id*message_size;
		fill_period(g,i,id*message_size+route_length(g,i),message_size,BACKWARD,P);
		a->offset_backward[i]=0;
		a->waiting_time[i]=0;

			
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