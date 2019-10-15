#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
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
		random = rand()%size-i;
		tmp = tab[random+i];
		tab[random+i] = tab[i];
		tab[i] = tmp; 
	}
}
Voisin init_voisinage(Graph g, Voisin v)
{
	
	for(int i=0;i<g.nb_routes;i++)
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
	v.route =0;
	v.pos =0;
	v.level =0;
	return v;
}


Assignment assignment_with_orders(Graph g, int P, int message_size)
{

	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;
	Period_kind kind;
	int CL;
 	//for each contention  level
 	for(int i=0;i<g.contention_level;i++)
 	{
 		if(i>=g.contention_level/2)
 		{	
 			CL = i;
 			kind = BACKWARD;
 		}
 		else
 		{
 			CL = g.contention_level-i-1;
 			kind = FORWARD;
 		}

 		for(int j=0;j<g.arc_pool_size;j++)
 		{
 			if(CL)
 		}
 	}
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

int parcours_voisinage()