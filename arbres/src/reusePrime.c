#include "structs.h"
#include "treatment.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include "test.h"

typedef struct{
	int cols[2];
} Collisions;
Collisions id_col(Graph g,int route, int P,int offset, Period_kind kind, int message_size)
{
	Collisions c;
	c.cols[0] = -1;
	c.cols[1] = -1;
	int first = -1;
	if(kind == FORWARD)
	{
		offset += g.routes[route][0]->length;
		for(int i=0;i<message_size;i++)
		{
			if(g.routes[0][1]->period_f[(offset+i)%P] != 0)
			{
				
				if(first == -1)
				{
					if(g.routes[0][1]->period_f[(offset+i)%P] == -1)
					{
						first = 0;
						c.cols[0] = 0;
					}
					else
					{
						first = g.routes[0][1]->period_f[(offset+i)%P];
						c.cols[0] = g.routes[0][1]->period_f[(offset+i)%P];
					}
				}
				else
				{
					if(g.routes[0][1]->period_f[(offset+i)%P] != first)
					{
						if(g.routes[0][1]->period_f[(offset+i)%P] == -1)
						{
							c.cols[1] = 0;
						}
						else
						{
							c.cols[1] = g.routes[0][1]->period_f[(offset+i)%P];
						}
					
						return c;
					}
				}

			}
		}

	}
	else
	{
		offset += g.routes[route][3]->length;
		offset += g.routes[route][2]->length;
		for(int i=0;i<message_size;i++)
		{
			if(g.routes[0][1]->period_b[(offset+i)%P] != 0)
			{
				if(first == -1)
				{
					if(g.routes[0][1]->period_b[(offset+i)%P] == -1)
					{
						first = 0;
						c.cols[0] = 0;
					}
					else
					{
						first = g.routes[0][1]->period_b[(offset+i)%P];
						c.cols[0] = g.routes[0][1]->period_b[(offset+i)%P];
					}
				}
				else
				{
					if(g.routes[0][1]->period_b[(offset+i)%P] != first)
					{
						if(g.routes[0][1]->period_b[(offset+i)%P] == -1)
						{
							c.cols[1] = 0;
						}
						else
						{
							c.cols[1] = g.routes[0][1]->period_b[(offset+i)%P];
						}
			
						return c;
					}
				}

			}
		}

	}

	return c;
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
		else
			offset+= message_size;

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
	int id=0;//for the warning
	int id2=0;//for the warning
	int new_offset;
	int new_offset2;
	Collisions c;
	int old_forward, old_backward, old_waiting;
	//printf("Forward : \n ");
	//affiche_tab(g.routes[0][1]->period_f,P,stdout);
	//printf("Backward : \n ");
	//affiche_tab(g.routes[0][1]->period_b,P,stdout);
	for(int offset = 0;offset<P;offset++)
	{
		//printf(" offset %d(%d) offset back %d(%d) \n",offset,offset+g.routes[id_pb][0]->length,(offset+route_length(g,id_pb))%P,(offset+route_length(g,id_pb)+g.routes[id_pb][3]->length)%P);
		if(message_no_collisions( g, id_pb, offset,message_size,FORWARD,P))
		{
			if(!message_no_collisions( g, id_pb, offset+route_length(g,id_pb),message_size,BACKWARD,P))
			{	
				//printf("Collision backward (%d ) ",message_no_collisions( g, id_pb, offset+route_length(g,id_pb),message_size,BACKWARD,P));

				c = id_col(g,id_pb, P,offset+route_length(g,id_pb), BACKWARD,message_size);
				id = c.cols[0];
				id2 = c.cols[1];
				//printf(" avec %d \n",id);
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
				//printf("Collision forward");
				c = id_col(g,id_pb, P,offset, FORWARD,message_size);
				id = c.cols[0];
				id2 = c.cols[1];
				//printf(" avec %d \n",id);

			}
			else
			{
				//printf("Double collisions\n");
				continue;
			}
		}
		
		//On a trouvé la ou les routes problématiques
		if(id == -1)
		{
			printf("Error 95\n");exit(95);
		}

		//on fait la premiere route
		new_offset = find_new_offset(g,P,message_size,offset,id);
		//Si pas possible, next
		if(new_offset == -1)
				continue;
		free_period(g,id,a->offset_forward[id],message_size,FORWARD,P);
		free_period(g,id,a->offset_backward[id],message_size,BACKWARD,P);
		fill_period(g,id,new_offset,message_size,FORWARD,P);
		fill_period(g,id,new_offset+route_length(g,id),message_size,BACKWARD,P);
		old_forward = a->offset_forward[id];
		old_backward = a->offset_backward[id];
		old_waiting = a->waiting_time[id];
		a->offset_forward[id]=new_offset;
		a->offset_backward[id]=new_offset+route_length(g,id);
		a->waiting_time[id]=0;


		

		//Si il y avait une deuxiemme collision
		if(id2 != -1)
		{
			
			
			//Sinon, on cherche a faire la deuxieme route
			new_offset2 = find_new_offset(g,P,message_size,offset,id2);
			//si pas possible, next
			if(new_offset2 == -1)
			{
				//On annule le 1
				free_period(g,id,a->offset_forward[id],message_size,FORWARD,P);
				free_period(g,id,a->offset_backward[id],message_size,BACKWARD,P);
				fill_period(g,id,old_forward,message_size,FORWARD,P);
				fill_period(g,id,old_backward,message_size,BACKWARD,P);
				a->offset_forward[id]=old_forward;
				a->offset_backward[id]=old_backward;
				a->waiting_time[id]=old_waiting;
				continue;
			}
			//Si possible, on la décale 
			free_period(g,id2,a->offset_forward[id2],message_size,FORWARD,P);
			free_period(g,id2,a->offset_backward[id2],message_size,BACKWARD,P);
			fill_period(g,id2,new_offset,message_size,FORWARD,P);
			fill_period(g,id2,new_offset+route_length(g,id2),message_size,BACKWARD,P);
			a->offset_forward[id2]=new_offset;
			a->offset_backward[id2]=new_offset+route_length(g,id2);
			a->waiting_time[id2]=0;

		}
		//Si on est ici, on a réussi a tout décaler
		//insertion de la route
		fill_period(g,id_pb,offset,message_size,FORWARD,P);
		fill_period(g,id_pb,offset+route_length(g,id_pb),message_size,BACKWARD,P);
		a->offset_forward[id_pb]=new_offset;
		a->offset_backward[id_pb]=new_offset+route_length(g,id_pb);
		a->waiting_time[id_pb]=0;
		a->nb_routes_scheduled++;
		return a;


	
	
	}
	return a;

}

Assignment PRIME_reuse(Graph g, int P, int message_size,int swap)
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
			//printf("%d ne marche pas  \n",i);
			if(swap)
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