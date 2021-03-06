#include "structs.h"
#include "treatment.h"

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include "test.h"

typedef struct{
	int nb_cols;
	int cols[128];
} Collisions;
Collisions id_col(Graph * g,int route, int P,int offset, Period_kind kind, int message_size)
{
	Collisions c;
	for(int i=0;i<128;i++)
	{
		c.cols[i] = -1;
	}
	c.nb_cols = 0;
	
	int last = 0;

	if(kind == FORWARD)
	{
		for(int j=0;j<g->size_routes[route];j++)
		{
			
			if(g->routes[route][j]->period_f != NULL)
			{
				for(int i=0;i<message_size;i++)
				{
		
					if(g->routes[route][j]->period_f[(offset+i)%P] != 0)
					{
						if(g->routes[route][j]->period_f[(offset+i)%P] != last)
						{
							
							if(g->routes[route][j]->period_f[(offset+i)%P] == -1)
							{
								c.cols[c.nb_cols] = 0;
								
							}
							else
							{
								c.cols[c.nb_cols] = g->routes[route][j]->period_f[(offset+i)%P];
							}
							last = g->routes[route][j]->period_f[(offset+i)%P];
							c.nb_cols++;
						}
						

					}
				}
			}
			offset += g->routes[route][j]->length;
		}

	}
	else
	{
		//For each arcs
		for(int j=g->size_routes[route]-1;j>=0;j--)
		{
			
			//This is a contention point
			if(g->routes[route][j]->period_b != NULL)
			{
				for(int i=0;i<message_size;i++)
				{
				
					if(g->routes[route][j]->period_b[(offset+i)%P] != 0)
					{
						
						if(g->routes[route][j]->period_b[(offset+i)%P] != last)
						{
							
							if(g->routes[route][j]->period_b[(offset+i)%P] == -1)
							{
								c.cols[c.nb_cols] = 0;
							}
							else
							{
								c.cols[c.nb_cols] = g->routes[route][j]->period_b[(offset+i)%P];
							}
							c.nb_cols++;
							last = g->routes[route][j]->period_b[(offset+i)%P];
						}
					}
				}
			}
			offset += g->routes[route][j]->length;
		}
		

	}

	return c;
}

int find_new_offset(Graph * g, int P, int message_size, int id_route)
{

	for(int offset =0 ; offset < P; offset++)
	{
		if((message_no_collisions( g, id_route, offset,message_size,FORWARD,P))&&(message_no_collisions( g, id_route, offset+route_length(g,id_route),message_size,BACKWARD,P)) )
			return offset;
	}
	return -1;
}

void free_period(Graph * g,int route,int offset,int message_size,Period_kind kind,int P)
{

	if(kind == FORWARD)
	{
		//For each arcs
		for(int i=0;i<g->size_routes[route];i++)
		{
			
			//This is a contention point
			if(g->routes[route][i]->period_f != NULL)
			{
				for(int j=0;j<message_size;j++)
				{
					g->routes[route][i]->period_f[(offset+j)%P]=0;
				}
					
				
			}
			offset += g->routes[route][i]->length;
		}
	}
	else
	{
		//For each arcs
		for(int i=g->size_routes[route]-1;i>=0;i--)
		{
			//This is a contention point
			if(g->routes[route][i]->period_f != NULL)
			{
				for(int j=0;j<message_size;j++)
				{
					g->routes[route][i]->period_b[(offset+j)%P]=0;
				}				
			}
			offset += g->routes[route][i]->length;
		}
	}
}

Assignment search_moove(Graph * g, int P, int message_size, int id_pb, Assignment a)
{
	Collisions c,c2;
	c.nb_cols = 0;
	c2.nb_cols = 0;
	int fail;
	int var_continue;
	
	//printf("Forward : \n ");
	//affiche_tab(g->routes[0][1]->period_f,P,stdout);
	//printf("Backward : \n ");
	//affiche_tab(g->routes[0][1]->period_b,P,stdout);
	for(int offset = 0;offset<P;offset++)
	{
		//printf(" offset %d(%d) offset back %d(%d) \n",offset,offset+g->routes[id_pb][0]->length,(offset+route_length(g,id_pb))%P,(offset+route_length(g,id_pb)+g->routes[id_pb][3]->length)%P);
		if(message_no_collisions( g, id_pb, offset,message_size,FORWARD,P))
		{
			if(!message_no_collisions( g, id_pb, offset+route_length(g,id_pb),message_size,BACKWARD,P))
			{	
				//printf("Collision backward %d %d\n ",!message_no_collisions( g, id_pb, offset+route_length(g,id_pb),message_size,BACKWARD,P), offset+route_length(g,id_pb));

				c = id_col(g,id_pb, P,offset+route_length(g,id_pb), BACKWARD,message_size);
				
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
				//printf("Collision forward\n");
				c = id_col(g,id_pb, P,offset, FORWARD,message_size);
				
				//printf(" avec %d \n",id);

			}
			else
			{
				c = id_col(g,id_pb, P,offset, FORWARD,message_size);
				c2 = id_col(g,id_pb, P,offset+route_length(g,id_pb), BACKWARD,message_size);
				for(int i=0;i<c2.nb_cols;i++)
				{
					var_continue = 0;
					for(int j=0;j<c.nb_cols;j++)
					{
						//la route dans C2 existe dans c
						if(c2.cols[i]==c.cols[j])
						{
							var_continue = 1;
							break;
						}
					}
					if(var_continue)
						continue;
					//la route n'existait pas
					c.cols[c.nb_cols] = c2.cols[i];
					c.nb_cols++;

				}
				//printf("Double collisions\n");
			
			}
		}
		
		//On a trouvé la ou les routes problématiques
		if(c.cols[0] == -1)
		{
			printf("Error 96\n");exit(96);
		}

		int old_forward[c.nb_cols];
		int old_backward[c.nb_cols];
		int new_offset[c.nb_cols];
		fail = -1;
		//On efface toutes les routes qui gennent
		for(int i=0;i<c.nb_cols;i++)
		{
			old_forward[i] = a->offset_forward[c.cols[i]];
			old_backward[i] = a->offset_backward[c.cols[i]];
			free_period(g,c.cols[i],a->offset_forward[c.cols[i]],message_size,FORWARD,P);
			free_period(g,c.cols[i],a->offset_backward[c.cols[i]],message_size,BACKWARD,P);
		}
		//On place notre message
		fill_period(g,id_pb,offset,message_size,FORWARD,P);
		fill_period(g,id_pb,offset+route_length(g,id_pb),message_size,BACKWARD,P);

		//On tente de mettre les routes qu'on a enlevé
		for(int i=0;i<c.nb_cols;i++)
		{
			new_offset[i] = find_new_offset(g,P,message_size,c.cols[i]);
			if(new_offset[i] != -1)//Si c'est ok
			{
				fill_period(g,c.cols[i],new_offset[i],message_size,FORWARD,P);
				fill_period(g,c.cols[i],new_offset[i]+route_length(g,c.cols[i]),message_size,BACKWARD,P);
				a->offset_forward[c.cols[i]]=new_offset[i];
				a->offset_backward[c.cols[i]]=(new_offset[i]+route_length(g,c.cols[i]))%P;
				a->waiting_time[c.cols[i]]=0;
			}
			else
			{
				fail = i;
				break;
			}
			
		}
		if(fail != -1)//On n'a pas réussi à replacer tout le monde, on remet tout comme avant
		{
			//D'abbord on efface la nouvelle route
			free_period(g,id_pb,offset,message_size,FORWARD,P);
			free_period(g,id_pb,offset+route_length(g,id_pb),message_size,BACKWARD,P);

			for(int i=0;i<fail;i++)//On efface toutes les routes que l'on avait replacé
			{
				free_period(g,c.cols[i],new_offset[i],message_size,FORWARD,P);
				free_period(g,c.cols[i],new_offset[i]+route_length(g,c.cols[i]),message_size,BACKWARD,P);
			
			}
			for(int i=0;i<c.nb_cols;i++)//Puis on remet toutes les routes comme avant
			{
				fill_period(g,c.cols[i],old_forward[i],message_size,FORWARD,P);
				fill_period(g,c.cols[i],old_backward[i],message_size,BACKWARD,P);
				a->offset_forward[c.cols[i]]= old_forward[i];
				a->offset_backward[c.cols[i]]= old_backward[i];			
				
			}

			
		}
		else //C'est ok, la nouvelle route est acceptée
		{
			a->offset_forward[id_pb]=offset;
			a->offset_backward[id_pb]=(offset+route_length(g,id_pb))%P;
			a->waiting_time[id_pb]=0;
			a->nb_routes_scheduled++;
			return a;
		}

	
	
	}
	return a;

}

Assignment PRIME_reuse(Graph * g, int P, int message_size)
{

	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = calloc(g->nb_routes,sizeof(int));
	a->offset_backward = calloc(g->nb_routes,sizeof(int));
	a->waiting_time = calloc(g->nb_routes,sizeof(int));
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;
 	int offset;
 	int bool_found = 1;
	

	//for each route
	for(int i=0;i<g->nb_routes;i++)
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
			a->offset_backward[i]=(offset+route_length(g,i))%P;
		
			a->waiting_time[i]=0;
			a->nb_routes_scheduled++;
		}
		else
		{
			//printf("%d ne marche pas  \n",i);
			
			a = search_moove(g,  P,  message_size,  i,a);
			
		}

		
	}
	if(a->nb_routes_scheduled == g->nb_routes)
	{
		a->all_routes_scheduled = 1;

	}
	else
	{
		a->all_routes_scheduled = 0;
	}
	return a;

}