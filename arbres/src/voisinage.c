#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "structs.h"
#include "treatment.h"
typedef struct{
	int route;
	int level;
	int pos;
} Voisin;


Voisin nouveau_voisin(Voisin v,Graph g)
{
	Period_kind kind;
	int tmp, idtmp;
	int x = v.level;
	kind = FORWARD;
	if(x >= g.nb_levels[v.route]/2)
	{
		x=g.nb_levels[v.route]-v.level-1;
		kind = BACKWARD;
	}
	v.pos++;
	if(v.pos >= g.contention[v.route][x]->nb_routes)
	{
		v.pos = 0;
		v.level++;
	}
	if(v.level >= g.nb_levels[v.route])
	{
		v.level=0;
		v.route++;
	}
	if(v.route >= g.nb_routes)
	{
		v.route = -1;
		return v;
	}

	x=v.level;
	if(v.level >= g.nb_levels[v.route]/2)
	{
		x=g.nb_levels[v.route]-v.level-1;
		kind = BACKWARD;
	}

	
	for(int i=0;i<g.contention[v.route][x]->nb_routes;i++)
	{
		if(g.contention[v.route][x]->routes_id[i] == v.route)
		{
			idtmp = i;//contient l'indice de la position de v.route avant le voisinage
		}
	}
	//echange v.pos et v.route
	tmp = g.contention[v.route][x]->routes_order[v.pos];
	g.contention[v.route][x]->routes_order[v.pos] = v.route;
	g.contention[v.route][x]->routes_order[idtmp] = tmp;
	return v;
}

void random_ordre(int * tab, int size)
{
	int random,tmp;

	for(int i=0;i<size;i++)
	{
		random = rand()%(size-i);
		tmp = tab[random+i];
		tab[random+i] = tab[i];
		tab[i] = tmp; 
	}
}
Voisin init_voisinage(Graph g, Voisin v)
{
	
/*	for(int i=0;i<g.nb_routes;i++)
	{
		for(int j = 0 ; j<g.size_routes[i];j++)
		{
			for(int k = 0 ; k<g.routes[i][j]->nb_routes;k++)
			{
				g.routes[i][j]->routes_order[k] = g.routes[i][j]->routes_id[k];
			}		

			random_ordre(g.routes[i][j]->routes_order,g.routes[i][j]->nb_routes);
		}
	}
	*/
	for(int i=0;i<g.arc_pool_size;i++)
	{
		for(int j=0;j<g.arc_pool[i].nb_routes;j++)
		{
			g.arc_pool[i].routes_order[j] = g.arc_pool[i].routes_id[j] ;
		}
		random_ordre(g.arc_pool[i].routes_order,g.arc_pool[i].nb_routes);

	}
	v.route =0;
	v.pos =0;
	v.level =0;
	return v;
}

void fill_Per(int * P, int route, int offset, int message_size, int per)
{
	for(int i=0;i<message_size;i++)
	{
		P[(i+offset)%per] = route;
	}
}
void init_Per(int * P, int per)
{
	for(int i=0;i<per;i++)
	{
		P[i] = -1;
	}
}
int col_check(int *P, int offset, int message_size,int per)
{
	for(int i=0;i<message_size;i++)
	{
		if(P[(i+offset)%per] != -1)
		{
			return 0;
		}
	}
	return 1;
}
void cpy_per(int * t , int * t2 ,int p)
{
	for(int i=0;i<p;i++)
	{
		t2[i]= t[i];
	}
}
Assignment assignment_with_orders(Graph g, int P, int message_size)
{

	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = NULL;
	a->offset_backward = NULL;
	a->waiting_time = NULL;
	a->buffers = malloc(sizeof(int*)*g.nb_routes);
	for(int i=0;i<g.nb_routes;i++)
	{
		a->buffers[i] = malloc(sizeof(int)*g.contention_level);
	}
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;
	Period_kind kind;
	int CL;
	int offset ;
	int current_route;
	int dl;
	int Per[P];
	int ok;
 	//for each contention  level
 	for(int i=0;i<g.contention_level;i++)
 	{
 		offset = 0;
 		if(i<g.contention_level/2)
 		{	
 			CL = i;
 			kind = FORWARD;
 		}
 		else
 		{
 			CL = g.contention_level-i-1;
 			kind = BACKWARD;
 		}

 		for(int j=0;j<g.arc_pool_size;j++)
 		{
 			offset = 0;
 			init_Per(Per,P);
 			if(g.arc_pool[j].contention_level == CL)
 			{
 				for(int k=0;k<g.arc_pool[j].nb_routes;k++)
 				{
 					current_route = g.arc_pool[j].routes_order[k];
 					dl = route_length_untill_arc(g,current_route,&g.arc_pool[j],kind);
 					if(kind == BACKWARD)
 					{
 						dl += route_length_with_buffers_forward(g, current_route);
 					}
 					if(dl < offset )
 					{
 						g.arc_pool[j].routes_delay_b[current_route] =  offset - dl;
 						offset+=2500;
 					}
 					else
 					{
 						offset = dl;
 					}
 					ok = 0;
 					for(int a =0;a<P;a++)
 					{

 						if(col_check(Per,offset+a,message_size,P))
 						{
 							g.arc_pool[j].routes_delay_b[current_route] += a;
 							offset += a;
 							ok = 1;
 							break;
 						}

 					}
 					if(ok == 0)
 					{
 						return a;
 					}
 					fill_Per(Per, current_route, offset, message_size,P);
 					a->buffers[current_route][i] = g.arc_pool[j].routes_delay_b[current_route];
 				}
 				if(kind == FORWARD)
 					cpy_per(Per,g.arc_pool[j].period_f,P);
 				else
 					cpy_per(Per,g.arc_pool[j].period_b,P);
 			}
 		}

 	}
	a->all_routes_scheduled = 1;
	return a;

}
void reinit_delays(Graph g)
{
	for(int i=0;i<g.arc_pool_size;i++)
	{
		for(int j=0;j<128;j++)
		{
			g.arc_pool[i].routes_delay_b[j] = 0;
			g.arc_pool[i].routes_delay_f[j] = 0;
		}
	}
}
void cpy_orders(int*  cpy[], Graph g,int sens)
{
	if(sens)//on copie du graph a cpy
	{
		for(int i=0;i<g.arc_pool_size;i++)
		{
			for(int j=0;j<g.arc_pool[i].nb_routes;j++)
			{

				cpy[i][j] = g.arc_pool[i].routes_order[j];
			}
		}
	}
	else
	{
		for(int i=0;i<g.arc_pool_size;i++)
		{
			for(int j=0;j<g.arc_pool[i].nb_routes;j++)
			{
				g.arc_pool[i].routes_order[j] = cpy[i][j];
			}
		}
	}
}
int parcours_voisinage(Graph g,int P, int message_size,Voisin v, int mintime)
{

	Assignment a;
	
	int time;
	int **orders = malloc(sizeof(int*)*g.arc_pool_size);
	for(int i=0;i<g.arc_pool_size;i++)
	{
		orders[i] = malloc(sizeof(int)*128);
	}
	cpy_orders(orders,g,1);//on fait une sauvegarde dans l'état
	while(v.route != -1)
	{
		reset_periods(g,P);
		a = assignment_with_orders(g,P,message_size);
		time = travel_time_max_buffers(g);
		if(time < mintime)
		{

			cpy_orders(orders,g,1);
			mintime = time;
		}
		v= nouveau_voisin(v,g);
		reinit_delays(g);
		
	}
	if(mintime == INT_MAX)
	{
		for(int i=0;i<g.arc_pool_size;i++)
		{
			free(orders[i]);
		}
		free(orders);
		return -1;
	}
	cpy_orders(orders,g,0);//on remet le graph optimal
	for(int i=0;i<g.arc_pool_size;i++)
	{
		free(orders[i]);
	}
	free(orders);
	return mintime;
}

int descente(Graph g, int P, int message_size)
{
		Voisin v;

	int tmin;
	int min = INT_MAX;
	v= init_voisinage( g,  v);
	while( (tmin = parcours_voisinage(g,P,message_size,v,min)) != -1)
	{

		if(min > tmin)
			min = tmin;
		else
			return min;
	}
	if(min = INT_MAX)
		return -1;
	return tmin;
}