#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "structs.h"
#include "treatment.h"
#include "greedy_waiting.h"
typedef struct{
	int route;
	int level;
	int pos;
} Voisin;

int indice_identique(int * t, int taille)
{
	for(int i=0;i<taille;i++)
	{
		for(int j=taille-1;j>i;j--)
		{
			//printf("%d %d \n",t[i],t[j]);
			if(t[i]==t[j])
			{
				//printf("%d %d \n",t[i],taille);
				return 1;

			}
			
		}
	}
	return 0;
}
Voisin nouveau_voisin(Voisin v,Graph g)
{
	
	int tmp, idtmp;
	int x = v.level;
	Period_kind kind;

	if(x >= g.nb_levels[v.route]/2)
	{
		x=g.nb_levels[v.route]-v.level-1;
		
	}
	
	if(v.pos >= g.contention[v.route][x]->nb_routes)
	{
		v.pos = 0;
		v.level++;
		if(v.level == g.nb_levels[v.route]/2)//Si on est au retour sur l'arc de la bbu, pas besoin de changer les ordres.
		{
			v.level++;
		}
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
	kind = FORWARD;
	if(v.level >= g.nb_levels[v.route]/2)
	{
		x=g.nb_levels[v.route]-v.level-1;
		kind = BACKWARD;
	}

	

	//echange v.pos et v.route
	if(kind == FORWARD)
	{

		
		if(v.pos ==0)
		{
			for(int i=0;i<g.contention[v.route][x]->nb_routes;i++)
			{
				if(g.contention[v.route][x]->routes_order_f[i] == v.route)
				{
					g.contention[v.route][x]->pos_swap = i;//contient l'indice de la position de v.route avant le voisinage
					//printf("swapf = %d -----------------------------------\n",i);
				}

			}
			tmp = g.contention[v.route][x]->routes_order_f[v.pos];
			g.contention[v.route][x]->routes_order_f[v.pos] = v.route;
			g.contention[v.route][x]->routes_order_f[g.contention[v.route][x]->pos_swap] = tmp;
			if(v.level == (g.nb_levels[v.route]/2)-1)
			{
				tmp = g.contention[v.route][x]->routes_order_b[v.pos];
				g.contention[v.route][x]->routes_order_b[v.pos] = v.route;
				g.contention[v.route][x]->routes_order_b[g.contention[v.route][x]->pos_swap] = tmp;
			}
			
		}
		else
		{
			g.contention[v.route][x]->routes_order_f[v.pos-1] = g.contention[v.route][x]->routes_order_f[g.contention[v.route][x]->pos_swap];
			g.contention[v.route][x]->routes_order_f[g.contention[v.route][x]->pos_swap] = g.contention[v.route][x]->routes_order_f[v.pos];
			g.contention[v.route][x]->routes_order_f[v.pos] = v.route;
			if(v.level == (g.nb_levels[v.route]/2)-1)
			{
				g.contention[v.route][x]->routes_order_b[v.pos-1] = g.contention[v.route][x]->routes_order_b[g.contention[v.route][x]->pos_swap];
				g.contention[v.route][x]->routes_order_b[g.contention[v.route][x]->pos_swap] = g.contention[v.route][x]->routes_order_b[v.pos];
				g.contention[v.route][x]->routes_order_b[v.pos] = v.route;
			}
		}
		
	}
	else
	{
		if(v.pos ==0)
		{
			for(int i=0;i<g.contention[v.route][x]->nb_routes;i++)
			{
				if(g.contention[v.route][x]->routes_order_b[i] == v.route)
				{
					g.contention[v.route][x]->pos_swap = i;//contient l'indice de la position de v.route avant le voisinage
					//printf("swapb = %d ---------------------------------\n",i);
				}
			}
			tmp = g.contention[v.route][x]->routes_order_b[v.pos];
			g.contention[v.route][x]->routes_order_b[v.pos] = v.route;
			g.contention[v.route][x]->routes_order_b[g.contention[v.route][x]->pos_swap] = tmp;
			if(v.level == (g.nb_levels[v.route]/2))
			{
				printf("Ce cas de figure ne dois pas arriver, normalement si on est au backward, cet arc n'est pas traité\n");
				exit(54);
			}
			
		}
		else
		{
			g.contention[v.route][x]->routes_order_b[v.pos-1] = g.contention[v.route][x]->routes_order_b[g.contention[v.route][x]->pos_swap];
			g.contention[v.route][x]->routes_order_b[g.contention[v.route][x]->pos_swap] = g.contention[v.route][x]->routes_order_b[v.pos];
			g.contention[v.route][x]->routes_order_b[v.pos] = v.route;
			if(v.level == (g.nb_levels[v.route]/2))
			{
				printf("Ce cas de figure ne dois pas arriver, normalement si on est au backward, cet arc n'est pas traité\n");
				exit(54);
			}
		}
	}
	v.pos++;
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
			g.arc_pool[i].routes_order_f[j] = g.arc_pool[i].routes_id[j] ;
			g.arc_pool[i].routes_order_b[j] = g.arc_pool[i].routes_id[j] ;
		}
		
		if(i<g.nb_bbu)
		{
			random_ordre(g.arc_pool[i].routes_order_f,g.arc_pool[i].nb_routes);
			for(int j=0;j<g.arc_pool[i].nb_routes;j++)
			{
				g.arc_pool[i].routes_order_b[j] = g.arc_pool[i].routes_order_f[j];
			}
		}
		else
		{
			random_ordre(g.arc_pool[i].routes_order_f,g.arc_pool[i].nb_routes);
			random_ordre(g.arc_pool[i].routes_order_b,g.arc_pool[i].nb_routes);
		}
	}
	v.route =0;
	v.pos =0;
	v.level =0;
	return v;
}
Voisin reinit_voins(Voisin v)
{
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
		if(t[i] == 0)
			t2[i] = -1;
		else
		{
			if(t[i]==-1)
				t2[i] = 0;
			else
				t2[i]= t[i];
		}
		
	}
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
 				/*printf("Routes sur l'arc : %d\n",kind);
 				for(int k=0;k<g.arc_pool[j].nb_routes;k++)
 				{
 					if(kind == FORWARD)
 						printf("%d ",g.arc_pool[j].routes_order_f[k]);
 					else
 						printf("%d ",g.arc_pool[j].routes_order_f[k]);
 				}
 				printf("\n");*/
 				for(int k=0;k<g.arc_pool[j].nb_routes;k++)
 				{


 					if(kind == FORWARD)
 						current_route = g.arc_pool[j].routes_order_f[k];
 					else
 						current_route = g.arc_pool[j].routes_order_b[k];
 					dl = route_length_untill_arc(g,current_route,&g.arc_pool[j],kind);
 					if(kind == BACKWARD)
 					{
 						dl += route_length_with_buffers_forward(g, current_route);
 					}
 				
 					if(dl < offset )
 					{
 						
 						if(kind == FORWARD)
 							g.arc_pool[j].routes_delay_f[current_route] =  offset - dl;
 						else
 							g.arc_pool[j].routes_delay_b[current_route] =  offset - dl;
 					}
 					else
 					{
 						offset = dl;
 					}
 					ok = 0;
 					for(int add =0;add<P;add++)
 					{

 						if(col_check(Per,offset+add,message_size,P))
 						{
 							
 							if(kind == FORWARD)
 								g.arc_pool[j].routes_delay_f[current_route] += add;
 							else
 								g.arc_pool[j].routes_delay_b[current_route] += add;
 							offset += add;
 							ok = 1;
 							break;
 						}

 					}
 					
 					if(ok == 0)
 					
 					{
 					
 						return a;
 					}
 					fill_Per(Per, current_route, offset, message_size,P);
 					offset+=message_size;
 					//printf("%d ",current_route);printf("\n");
 				}
 				if(kind == FORWARD)
 				{
 					cpy_per(Per,g.arc_pool[j].period_f,P);
 					//affiche_periode(Per, P, stdout);
 				}
 				else
 				{
 					cpy_per(Per,g.arc_pool[j].period_b,P);
 					//affiche_periode(Per, P, stdout);
 				}

 				

 			}
 		}

 	}
 	for(int i=0;i<g.nb_routes;i++)
 	{
 		a->offset_forward[i] = 0;
 		a->offset_backward[i] = route_length_with_buffers_forward(g, i);
 		a->waiting_time[i]=0;
 	}
	a->all_routes_scheduled = 1;
	a->nb_routes_scheduled = g.nb_routes;


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

				cpy[i][j] = g.arc_pool[i].routes_order_f[j];
				cpy[i+g.arc_pool_size][j] = g.arc_pool[i].routes_order_b[j];
			}
	
			
		}
	
	}
	else
	{
	
		for(int i=0;i<g.arc_pool_size;i++)
		{
			for(int j=0;j<g.arc_pool[i].nb_routes;j++)
			{
				g.arc_pool[i].routes_order_f[j] = cpy[i][j];
				g.arc_pool[i].routes_order_b[j] = cpy[i+g.arc_pool_size][j];
				
			}
		
			
		}

	}
}
void aff_orders(int*  cpy[], Graph g)
{
	
		printf("forward : \n");
		for(int i=0;i<g.arc_pool_size;i++)
		{

			for(int j=0;j<g.arc_pool[i].nb_routes;j++)
			{

				printf("(%d | %d) ",cpy[i][j],g.arc_pool[i].routes_order_f[j]);
			}
	
			printf("\n");
		}
		printf("\n backward : \n");
	
		for(int i=0;i<g.arc_pool_size;i++)
		{
			for(int j=0;j<g.arc_pool[i].nb_routes;j++)
			{
				printf("(%d | %d) ",cpy[i+g.arc_pool_size][j],g.arc_pool[i].routes_order_b[j]);
				
			}
		
			printf("\n");
		}
		for(int i=0;i<g.arc_pool_size;i++)
		if(indice_identique(cpy[i+g.arc_pool_size],g.arc_pool[i].nb_routes) || indice_identique(cpy[i],g.arc_pool[i].nb_routes)|| indice_identique(g.arc_pool[i].routes_order_f,g.arc_pool[i].nb_routes) || indice_identique(g.arc_pool[i].routes_order_b,g.arc_pool[i].nb_routes))
			exit(67);
	
}


int ** parcours_voisinage(Graph g,int P, int message_size,Voisin v, int mintime)
{

	Assignment a=NULL;
	
	int begin = mintime;

	int **orders = malloc(sizeof(int*)*g.arc_pool_size*2);
	for(int i=0;i<g.arc_pool_size*2;i++)
	{
		orders[i] = malloc(sizeof(int)*128);
	}
	cpy_orders(orders,g,1);//on fait une sauvegarde dans l'état
	//printf("Sauvegarde de l'état \n");aff_orders(orders,g);
	while(v.route != -1)
	{
		reset_periods(g,P);
		a = assignment_with_orders(g,P,message_size);
		if(a->all_routes_scheduled)
		{
			a->time = travel_time_max_buffers(g);
			if(a->time < mintime)
			{
		
				cpy_orders(orders,g,1);
				//printf("nouveau etat optimal\n");aff_orders(orders,g);
				mintime = a->time;
				
			}
		}
		//printf("\n\navant nouveau voisin\n");aff_orders(orders,g);
		v= nouveau_voisin(v,g);
		//printf("%d %d %d \n",v.pos,v.level,v.route);
		//printf("Apres noveau voising \n");aff_orders(orders,g);
		//printf("\n\n");
		reinit_delays(g);
	
		free_assignment(a);

	}
	cpy_orders(orders,g,0);//on remet le graph optimal
	//printf("On remet l'état optimal\n");aff_orders(orders,g);
	if(mintime == begin) //on a pas amélioré
	{
		for(int i=0;i<g.arc_pool_size*2;i++)
		{
			free(orders[i]);
		}
		free(orders);
		return NULL;
	}


	return orders;
}
Voisin init_voisinage_greedy(Voisin v, Graph g, int P, int message_size, int tmax)
{
	Assignment a= loaded_greedy_collisions( g, P, message_size,tmax);
	
	convert_graph_order(g,P);
	reset_periods(g,P);
	
	free_assignment(a);

	a = assignment_with_orders(g,P,message_size);
	
	reset_periods(g,P);
	free_assignment(a);
	v.pos=0;
	v.level=0;
	v.route=0;
	return v;
}
Assignment descente(Graph g, int P, int message_size,int tmax)
{
	Voisin v;
	int nb_d =0;
	v= init_voisinage( g,  v);
	//v=init_voisinage_greedy(v,g,P,message_size,tmax);

	int ** orders = parcours_voisinage(g,P,message_size,v,INT_MAX);

	Assignment a=NULL;
	if(!orders)
	{

		Assignment a = malloc(sizeof(struct assignment));
		a->offset_forward = malloc(sizeof(int)*g.nb_routes);
		a->offset_backward = malloc(sizeof(int)*g.nb_routes);
		a->waiting_time = malloc(sizeof(int)*g.nb_routes);
		a->nb_routes_scheduled = nb_d;
		a->all_routes_scheduled = 0;
		return a;
	}
	

	while(orders != NULL)
	{
		nb_d++;
		if(a)
			free_assignment(a);
		v=reinit_voins(v);
		cpy_orders(orders,g,0);
		//printf("On remet le dernier graph renvoyé\n");aff_orders(orders,g);
		reinit_delays(g);
		reset_periods(g,P);
		a = assignment_with_orders(g,P,message_size);
		a->time = travel_time_max_buffers(g);
		
		for(int i=0;i<g.arc_pool_size*2;i++)
		{
			free(orders[i]);
		}
		free(orders);
		orders = parcours_voisinage(g,P,message_size,v,a->time);

		//printf("%p \n",orders);
	}
	//printf("Recalcul avec le meilleur ordre ---------------------\n\n\n");aff_orders(orders,g);
	//ici quand on quitte, le graph est normalement remis au meilleur de avant
	if(a)
		free_assignment(a);
	reinit_delays(g);
	reset_periods(g,P);
	a = assignment_with_orders(g,P,message_size);
	a->time = travel_time_max_buffers(g);

	a->nb_routes_scheduled = nb_d;
	return a;

}
Assignment best_of_x(Graph g, int P, int message_size,int tmax)
{
	Assignment a;
	Assignment best = NULL;
	int prev = INT_MAX;
	for(int i=0;i<10;i++)
	{
		a = descente(g,P,message_size,tmax);
		if(a->time < prev )
			best = a;
		else
			free_assignment(a);
	}
	return best;
}