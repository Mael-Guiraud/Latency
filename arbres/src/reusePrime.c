#include "structs.h"
#include "treatment.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include "test.h"
int id_col(Graph g,int route, int P,int offset, Period_kind kind)
{

	if(kind == FORWARD)
	{
		offset += g.routes[route][0]->length;
		if(g.routes[0][1]->period_f[offset%P] != 0)
		{
			
			return g.routes[0][1]->period_f[offset%P];

		}

	}
	else
	{
		offset += g.routes[route][3]->length;
		offset += g.routes[route][2]->length;
		if(g.routes[0][1]->period_b[offset%P] != 0)
		{
			return g.routes[0][1]->period_b[offset%P];
		}

	}
	printf("We shall not be in this situation.");
	exit(44);

	return 0;
}

int find_new_offset(Graph g, int P, int message_size, int forbidden_offset, int id_route)
{

	for(int offset =0 ; offset < P; offset++)
	{
		if(offset != forbidden_offset)
		{
			if((message_no_collisions( g, id_route, offset,message_size,FORWARD,P))&&(message_no_collisions( g, id_route, offset+route_length(g,id_route),message_size,BACKWARD,P)) )
				return offset;
		}

	}
	return -1;
}

void free_period(Graph g,int route,int offset,int message_size,Period_kind kind,int P)
{

	if(kind == FORWARD)
	{
		//For each arcs
		for(int i=0;i<g.size_routes[route];i++)
		{
			
			//This is a contention point
			if(g.routes[route][i]->period_f != NULL)
			{
				for(int j=0;j<message_size;j++)
				{
					g.routes[route][i]->period_f[(offset+j)%P]=0;
				}
					
				
			}
			offset += g.routes[route][i]->length;
		}
	}
	else
	{
		//For each arcs
		for(int i=g.size_routes[route]-1;i>=0;i--)
		{
			//This is a contention point
			if(g.routes[route][i]->period_f != NULL)
			{
				for(int j=0;j<message_size;j++)
				{
					g.routes[route][i]->period_b[(offset+j)%P]=0;
				}				
			}
			offset += g.routes[route][i]->length;
		}
	}
}

Assignment search_moove(Graph g, int P, int message_size, int id_pb, Assignment a)
{
	int id;
	int new_offset;
	printf("Forward : \n ");
	affiche_tab(g.routes[0][1]->period_f,P,stdout);
	printf("Backward : \n ");
	affiche_tab(g.routes[0][1]->period_b,P,stdout);
	for(int offset = 0;offset<P;offset++)
	{
		printf(" offset %d(%d) offset back %d(%d) \n",offset,offset+g.routes[id_pb][0]->length,(offset+route_length(g,id_pb))%P,(offset+route_length(g,id_pb)+g.routes[id_pb][3]->length)%P);
		if(message_no_collisions( g, id_pb, offset,message_size,FORWARD,P))
		{
			if(!message_no_collisions( g, id_pb, offset+route_length(g,id_pb),message_size,BACKWARD,P))
			{	
				printf("Collision backward (%d ) ",message_no_collisions( g, id_pb, offset+route_length(g,id_pb),message_size,BACKWARD,P));

				id = id_col(g,id_pb, P,offset+route_length(g,id_pb), BACKWARD);
				printf(" avec %d \n",id);
			}
			else
			{
				printf("Impossible (no cols forward and backward)\n");
			}
		}
		else
		{
			if(message_no_collisions( g, id_pb, offset+route_length(g,id_pb),message_size,BACKWARD,P))
			{
				printf("Collision forward");
				id = id_col(g,id_pb, P,offset, FORWARD);
				printf(" avec %d \n",id);

			}
			else
			{
				printf("Double collisions\n");
				continue;
			}
		}
		
		new_offset = find_new_offset(g,P,message_size,offset,id);
		if(new_offset != -1)
		{
			free_period(g,id,offset,message_size,FORWARD,P);
			free_period(g,id,offset+route_length(g,id),message_size,BACKWARD,P);
			fill_period(g,id,new_offset,message_size,FORWARD,P);
			fill_period(g,id,new_offset+route_length(g,id),message_size,BACKWARD,P);
			a->offset_forward[id]=new_offset;
			a->offset_backward[id]=new_offset+route_length(g,id);
			a->waiting_time[id]=0;

			fill_period(g,id_pb,offset,message_size,FORWARD,P);
			fill_period(g,id_pb,offset+route_length(g,id_pb),message_size,BACKWARD,P);
			a->offset_forward[id_pb]=new_offset;
			a->offset_backward[id_pb]=new_offset+route_length(g,id_pb);
			a->waiting_time[id_pb]=0;
			a->nb_routes_scheduled++;
			return a;
		}
		else
		{
			continue;
		}
	}
	return a;

}

Assignment PRIME_reuse(Graph g, int P, int message_size)
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
		else
		{
			printf("%d ne marche pas  \n",i);
			a = search_moove(g,  P,  message_size,  i,a);
			
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