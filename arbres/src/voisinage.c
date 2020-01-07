#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "structs.h"
#include "treatment.h"
#include "greedy_waiting.h"
#include "multiplexing.h"
#include "hash.h"
#include <string.h>
#include "test.h"
#include <math.h>
typedef struct{
	int route;
	int* pos;
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

void next(int * tab, int id)
{
	if(tab[id] == 1)
	{
		if(id==0)
		{
			printf("Error, the number max is reached.\n");
			exit(94);
		}
		tab[id]=0;
		next(tab,id-1);
	}
	else
	{
		tab[id]++;
	}
}

int fin_vois(int * tab, int size)
{
	for(int i=0;i<size;i++)
	{
		if(tab[i] != 1)
		{
		
			return 0;
		}
	}
	return 1;

}
void init_vois(int * tab, int size)
{
	for(int i=0;i<size;i++)
	{
		tab[i] =0;

	}

}
void echanger_gauche(int * tab, int size, int id)
{
	int tmp;
	if(size <= 1)
		return;
	if(id ==0)
	{
		tmp = tab[0];
		tab[0] = tab[size-1];
		tab[size-1]=tmp;
	}
	else
	{
		tmp = tab[id];
		tab[id] = tab[id-1];
		tab[id-1]=tmp;
	}
}
void echanger_droite(int * tab, int size, int id)
{
	int tmp;
	if(size <= 1)
		return;
	if(id ==size-1)
	{
		tmp = tab[0];
		tab[0] = tab[size-1];
		tab[size-1]=tmp;
	}
	else
	{
		tmp = tab[id];
		tab[id] = tab[id+1];
		tab[id+1]=tmp;
	}
}

Voisin nouveau_voisin(Voisin v,Graph g)
{

	int idtmp;
	Period_kind kind;
	int x;
	//On remet comme avant
	for(int level=0;level<g.nb_levels[v.route];level++)
	{
		x=level;
		kind = FORWARD;
		if(level >= g.nb_levels[v.route]/2)
		{
			x=g.nb_levels[v.route]-level-1;
			kind = BACKWARD;
		}
		//x contient le level de contention de l'arc, et kind le sens


		//On cherche l'indice de au quel v.route est placée dans l'arc
		idtmp = -1;
		for(int i=0;i<g.contention[v.route][x]->nb_routes;i++)
		{
			if(kind == FORWARD)
			{
				if(g.contention[v.route][x]->routes_order_f[i] == v.route)
				{
					idtmp = i;
					break;
				}
			}
			else
			{
				if(g.contention[v.route][x]->routes_order_b[i] == v.route)
				{
					idtmp = i;
					break;
				}
			}

		}
		if(idtmp ==-1)
		{
			printf("Error, indice not found.\n");exit(35);
		}


		if(v.pos[level] == 2)//permutation a gauche(car c'était droite avant)
		{
			if(kind == FORWARD)
			{
				echanger_gauche(g.contention[v.route][x]->routes_order_f,g.contention[v.route][x]->nb_routes , idtmp);
			}
			else
			{
				echanger_gauche(g.contention[v.route][x]->routes_order_b,g.contention[v.route][x]->nb_routes , idtmp);
			}
		}
		if(v.pos[level] == 1)//permutation a droite
		{
			if(kind == FORWARD)
			{
				echanger_droite(g.contention[v.route][x]->routes_order_f,g.contention[v.route][x]->nb_routes , idtmp);
			}
			else
			{
				echanger_droite(g.contention[v.route][x]->routes_order_b,g.contention[v.route][x]->nb_routes , idtmp);
			}
		}
	}
	if(fin_vois(v.pos,g.nb_levels[v.route]))//Nouvelle route
	{
		v.route++;
		free(v.pos);
		if(v.route >= g.nb_routes)
		{
			v.route = -1;

			return v;
		}
		v.pos = malloc(sizeof(int)* g.nb_levels[v.route]);
		init_vois(v.pos,g.nb_levels[v.route]);
	}
	else
	{
		next(v.pos,g.nb_levels[v.route]-1);
	}
	

	//On echange dans la periode
	for(int level=0;level<g.nb_levels[v.route];level++)
	{
		x=level;
		kind = FORWARD;
		if(level >= g.nb_levels[v.route]/2)
		{
			x=g.nb_levels[v.route]-level-1;
			kind = BACKWARD;
		}
		//x contient le level de contention de l'arc, et kind le sens


		//On cherche l'indice de au quel v.route est placée dans l'arc
		idtmp = -1;
		
		for(int i=0;i<g.contention[v.route][x]->nb_routes;i++)
		{
			if(kind == FORWARD)
			{
				if(g.contention[v.route][x]->routes_order_f[i] == v.route)
				{
					idtmp = i;
					break;
				}
			}
			else
			{
				if(g.contention[v.route][x]->routes_order_b[i] == v.route)
				{
					idtmp = i;
					break;
				}
			}

		}
		if(idtmp ==-1)
		{
			printf("Error, indice not found.\n");exit(35);
		}


		if(v.pos[level] == 1)//permutation a gauche
		{
			if(kind == FORWARD)
			{
				echanger_gauche(g.contention[v.route][x]->routes_order_f,g.contention[v.route][x]->nb_routes , idtmp);
			}
			else
			{
				echanger_gauche(g.contention[v.route][x]->routes_order_b,g.contention[v.route][x]->nb_routes , idtmp);
			}
		}
		if(v.pos[level] == 2)//permutation a droite
		{
			if(kind == FORWARD)
			{
				echanger_droite(g.contention[v.route][x]->routes_order_f,g.contention[v.route][x]->nb_routes , idtmp);
			}
			else
			{
				echanger_droite(g.contention[v.route][x]->routes_order_b,g.contention[v.route][x]->nb_routes , idtmp);
			}
		}
	}
	
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
	v.pos = malloc(sizeof(int)* g.nb_levels[v.route]);
	init_vois(v.pos,g.nb_levels[v.route]);

	return v;
}
Voisin reinit_voins(Graph g, Voisin v)
{
	v.route =0;
	v.pos= malloc(sizeof(int)* g.nb_levels[v.route]);
	init_vois(v.pos,g.nb_levels[v.route]);

	return v;
}

void fill_Per(int * P, int route, int offset, int message_size, int per)
{
	for(int i=0;i<message_size;i++)
	{
		if(route == 0)
			P[(i+offset)%per] = -1;
		else
			P[(i+offset)%per] = route;
	}
}
void init_Per(int * P, int per)
{
	memset(P,-1,per*sizeof(int));
	/*for(int i=0;i<per;i++)
	{
		P[i] = -1;
	}*/
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
int cols_check(int *P, int offset, int message_size,int per, int nb_routes)
{
	for(int i=0;i<nb_routes;i++)
	{
		if( (P[i]+message_size)%per < P[i]%per  ) // le message pi est a cheval sur deux periodes 
		{
			//if( (offset%per <= (P[i]+message_size)%per)||( (offset+message_size)%per >= P[i]%per) ||( (offset)%per >= P[i]%per) ||( (offset+message_size)%per <= (P[i]+message_size)%per) )
			if(offset%per < (P[i]+message_size)%per)
			{
				//printf("1 %d\n",(P[i]+message_size)%per - offset%per);
				return (P[i]+message_size)%per - offset%per ;
			}	
			if(( (offset+message_size)%per > P[i]%per) ||( (offset)%per >= P[i]%per) ||( (offset+message_size)%per <= (P[i]+message_size)%per) )
			{
				//printf("2 %d\n",per - (offset)%per + (P[i]+message_size)%per );
				return per - (offset)%per + (P[i]+message_size)%per ;
			}	
		}
		else
		{
			if((offset%per >= P[i]%per) && ( offset%per < (P[i]+message_size)%per)  )
			{
				//printf("3 %d\n",(P[i]+message_size)%per - offset%per);
				return (P[i]+message_size)%per - offset%per ;
			}	
			if(( (offset+message_size)%per > P[i]%per) && ( (offset+message_size)%per <= (P[i]+message_size)%per) )
			{
				if(offset%per > (P[i]+message_size)%per)
				{
					//printf("4 %d\n",per - (offset)%per + (P[i]+message_size)%per );
					return per - (offset)%per + (P[i]+message_size)%per ; 
				}
				else
				{
					//printf("5 %d\n",(P[i]+message_size)%per - offset%per );
					return (P[i]+message_size)%per - offset%per ;
				}
			}
		}
	}

	return 0;



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
Assignment assignment_with_orders(Graph g, int P, int message_size, int print)
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

	//int ok;

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
 			int Per[g.arc_pool[j].nb_routes];
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
 					//printf("Route %d release %d \n",current_route,dl);
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
 						if(kind == FORWARD)
 							g.arc_pool[j].routes_delay_f[current_route] =  0;
 						else
 							g.arc_pool[j].routes_delay_b[current_route] =  0;
 						
 					}
 					//ok = 0;
 					/*
 					for(int add =0;add<P;add++)
 					{

 						if(cols_check(Per,offset+add,message_size,P,k))
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
 					}*/
 					int check_value = 1;
 					int total_check = 0;
 					while(check_value)
 					{
 						check_value = cols_check(Per,offset+total_check,message_size,P,k);

 						total_check += check_value;
 						if(total_check >= P)
 						{
 							return a;
 						}
 					}
 					offset += total_check;
 					if(kind == FORWARD)
 						g.arc_pool[j].routes_delay_f[current_route] += total_check;
 					else
 						g.arc_pool[j].routes_delay_b[current_route] += total_check;

 					Per[k]=offset;
 					if(print)
 					{
 						if(kind == FORWARD)
 							fill_Per(g.arc_pool[j].period_f, current_route, offset, message_size,P);
 						else
 							fill_Per(g.arc_pool[j].period_b, current_route, offset, message_size,P);
 					}
 					
 					offset+=message_size;

 					//printf("%d ",current_route);printf("\n");
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
		memset(g.arc_pool[i].routes_delay_f,0,sizeof(int)*128);
		memset(g.arc_pool[i].routes_delay_b,0,sizeof(int)*128);
		/*printf("[%d]",g.arc_pool[i].nb_routes);
		for(int j=0;j<128;j++)
		{
			printf("%d",g.arc_pool[i].routes_delay_b[j]);
			//g.arc_pool[i].routes_delay_f[j] = 0;
		}
		printf("\n\n");*/
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
			if(g.arc_pool[i].contention_level != -1)
			{
				for(int j=0;j<g.arc_pool[i].nb_routes;j++)
				{

					printf("(%d | %d) ",cpy[i][j],g.arc_pool[i].routes_order_f[j]);
				}
				
				printf("\n");
			}
			
		}
		printf("\n backward : \n");
	
		for(int i=0;i<g.arc_pool_size;i++)
		{
			if(g.arc_pool[i].contention_level != -1)
			{
				for(int j=0;j<g.arc_pool[i].nb_routes;j++)
				{
					printf("(%d | %d) ",cpy[i+g.arc_pool_size][j],g.arc_pool[i].routes_order_b[j]);
					
				}
				
				printf("\n");
			}
			
		}
		/*for(int i=0;i<g.arc_pool_size;i++)
			if(g.arc_pool[i].contention_level != -1)
				if(indice_identique(cpy[i+g.arc_pool_size],g.arc_pool[i].nb_routes) || indice_identique(cpy[i],g.arc_pool[i].nb_routes)|| indice_identique(g.arc_pool[i].routes_order_f,g.arc_pool[i].nb_routes) || indice_identique(g.arc_pool[i].routes_order_b,g.arc_pool[i].nb_routes))
					exit(67);*/
	
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
		//reset_periods(g,P);
		a = assignment_with_orders(g,P,message_size,0);
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
	//printf("%d %d\n",mintime,begin);
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
	if(!greedy_deadline(g, P, message_size))
	{
		printf("Error, greedystatdeadline didnt find an order(voisinage.c)\n");
		v.route = -1;
		return v;
	}
	
	v.pos = malloc(sizeof(int)* g.nb_levels[v.route]);
	init_vois(v.pos,g.nb_levels[v.route]);
	v.route=0;
	return v;
}
Assignment descente(Graph g, int P, int message_size,int tmax)
{
	Voisin v;
	int nb_d =0;
	if(tmax)
		v= init_voisinage( g,  v);
	else
		v=init_voisinage_greedy(v,g,P,message_size,tmax);

	if(v.route == -1)
	{
		return NULL;
	}
	reinit_delays(g);
	int ** orders = parcours_voisinage(g,P,message_size,v,INT_MAX);
	reinit_delays(g);
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
		//printf("descente\n");
		nb_d++;
		if(a)
			free_assignment(a);
		v=reinit_voins(g,v);
		cpy_orders(orders,g,0);
		//printf("On remet le dernier graph renvoyé\n");aff_orders(orders,g);
		reinit_delays(g);
		//reset_periods(g,P);
		a = assignment_with_orders(g,P,message_size,0);
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
	a = assignment_with_orders(g,P,message_size,1);
	if(verifie_solution( g,message_size))
	{
		printf("La solution n'est pas correcte descente (error %d) ",verifie_solution( g,message_size));
		exit(82);
	}
	a->time = travel_time_max_buffers(g);

	a->nb_routes_scheduled = nb_d;
	return a;

}
Assignment best_of_x(Graph g, int P, int message_size,int tmax)
{
	Assignment a;
	Assignment best = NULL;
	int prev = INT_MAX;
	
	for(int i=0;i<tmax;i++)
	{

		a = descente(g,P,message_size,1);
		if(a->time < prev )
			best = a;
		else
			free_assignment(a);
	}
	return best;
}


//------------------------------------------------------
typedef struct trace{
	int ** order;
	int time;
	struct trace * suiv;
}trace_s;
typedef trace_s * Trace;

Trace ajouter_orders(Trace t,int ** order, int time)
{
	Trace new = malloc(sizeof(trace_s));
	new->order = order;
	new->time = time;
	new->suiv = t;
	return new;
}

int ordersequal(int**  cpy, Graph g)
{

	for(int i=0;i<g.arc_pool_size;i++)
	{
		for(int j=0;j<g.arc_pool[i].nb_routes;j++)
		{

			if((cpy[i][j] != g.arc_pool[i].routes_order_f[j]) || (cpy[i+g.arc_pool_size][j] != g.arc_pool[i].routes_order_b[j]))
				return 0;
		}

		
	}
	return 1;
}

int jamais_vu(Trace t, Graph g)
{
	while(t)
	{
		if(ordersequal(t->order,g))
		{
			//aff_orders(t->order,g);
			return 0;
		}
			
		t= t->suiv;
	
	}
	return 1;
}
int nb_elems(Trace t)
{
	int nb = 0;
	while(t)
	{
		nb++;
		t= t->suiv;
	}
	return nb;
}

void free_trace(Trace t,int arcpoolsize)
{
	Trace tmp;
	while(t)
	{
		for(int i=0;i<arcpoolsize*2;i++)
			free(t->order[i]);
		free(t->order);
		tmp = t;
		t = t->suiv;
		free(tmp);
	}
}


Trace parcours_voisinage_tabou(Graph g,int P, int message_size,Voisin v,Trace * hash_table,int sizehash)
{

	Assignment a=NULL;
	
	int min = INT_MAX;
	int key;
	int **orders = malloc(sizeof(int*)*g.arc_pool_size*2);
	for(int i=0;i<g.arc_pool_size;i++)
	{
		orders[i] = malloc(sizeof(int)*g.arc_pool[i].nb_routes);
		orders[i+g.arc_pool_size] = malloc(sizeof(int)*g.arc_pool[i].nb_routes);
	}
	//printf("Nouveau parcours\n");
	while(v.route != -1)
	{
		//printf("nouveau voisin \n");
		key = hash_graph(g,sizehash);
		while(!jamais_vu(hash_table[key],g))
		{
			//printf("Voisin interdit \n");

			v= nouveau_voisin(v,g);

			if(v.route == -1)
			{
				cpy_orders(orders,g,0);//on remet le graph optimal
				hash_table[key] = ajouter_orders(hash_table[key],orders,min);
				return hash_table[key];
			}
			key = hash_graph(g,sizehash);
		}
		a = assignment_with_orders(g,P,message_size,0);
		if(a->all_routes_scheduled)
		{

			a->time = travel_time_max_buffers(g);
			
			if(a->time <= min)
			{
				//printf("Nouveau meileur temmps %d \n",a->time);
				cpy_orders(orders,g,1);// de g vers orders
				//aff_orders(orders,g);
				min = a->time;
				
			}
		}
		reinit_delays(g);
		free_assignment(a);
		v= nouveau_voisin(v,g);
	}
	cpy_orders(orders,g,0);//on remet le graph optimal
	key = hash_graph(g,sizehash);
	hash_table[key] = ajouter_orders(hash_table[key],orders,min);
	return hash_table[key];
}
int size(Trace t)
{
	int cmpt =0;
	while(t)
	{
		cmpt ++;
		t = t->suiv;
	}
	return cmpt;
}
Assignment taboo(Graph g, int P, int message_size,int nb_steps)
{
	Voisin v;
	Assignment a=NULL; 
	int sizehash = nb_steps*2;
	Trace * hash_table = malloc(sizeof(Trace) *sizehash);
	FILE * f = fopen("../data/trace","w");
	Trace t;
	for(int i=0;i<sizehash;i++)hash_table[i]=NULL;
	
	if(!greedy_deadline(g, P, message_size))
	{
		printf("Error, greedystatdeadline didnt find an order(voisinage.c)\n");
		return a;
	}

	a = assignment_with_orders(g,P,message_size,0);
	a->time = travel_time_max_buffers(g);

	int min = a->time;
	reinit_delays(g);
	int **orders = malloc(sizeof(int*)*g.arc_pool_size*2);

	for(int i=0;i<g.arc_pool_size;i++)
	{
		orders[i] = malloc(sizeof(int)*g.arc_pool[i].nb_routes);
		orders[i+g.arc_pool_size] = malloc(sizeof(int)*g.arc_pool[i].nb_routes);
	}
	//aff_orders(orders,g);
	cpy_orders(orders,g,1);

	int key = hash_graph(g,sizehash);
	hash_table[key] = ajouter_orders(hash_table[key],orders,a->time);
	fprintf(f,"%d %d \n",0,hash_table[key]->time);
	free_assignment(a);
	

	v.pos = malloc(sizeof(int)* g.nb_levels[v.route]);
	init_vois(v.pos,g.nb_levels[v.route]);
	v.route=0;
	
	int cmpt = 0;
	
	int nb_steps_better=1;
	int ** best_order = orders;
	//printf("Taboo \n");
	
	while(cmpt < nb_steps)
	{
		cmpt ++;
		t= parcours_voisinage_tabou( g, P,  message_size, v,hash_table,sizehash);
		//fprintf(f,"%d %d \n",cmpt,t->time);
		//printf("step :%d best = %d \n",cmpt, t->time);
		if(t->time<min)
		{
			best_order = t->order;
			min = t->time;
			nb_steps_better = cmpt;
		}
		v=reinit_voins(g,v);
		//fprintf(stdout,"\r step %d/%d",cmpt,nb_steps);
		fflush(stdout);
		
	}



	//Fin du parcours, on remet le meilleur ordre dans le graph, on calcul et on renvoie
	cpy_orders(best_order,g,0);
	reinit_delays(g);
	reset_periods(g,P);
	a = assignment_with_orders(g,P,message_size,1);
	a->nb_routes_scheduled = nb_steps_better;
	a->time = travel_time_max_buffers(g);
	if(verifie_solution( g,message_size))
	{
		printf("La solution n'est pas correcte TABOO (error %d) ",verifie_solution( g,message_size));
		exit(83);
	}
	//printf("%d nb step \n",nb_steps_better);
	
	
	for(int i=0;i<sizehash;i++)
	{
		//printf("%d , \n",size(hash_table[i]));
		free_trace(hash_table[i],g.arc_pool_size);
	}
	free(hash_table);
	//printf("\n");
	fclose(f);
	return a;

}
//-----------------------------------------------


Voisin Voisin_alea(Graph g)
{
	Voisin v;
	int idtmp;
	Period_kind kind;
	int x;

	//Generation du voisin aleatoire
	v.route = rand()%g.nb_routes;
	v.pos = malloc(sizeof(int)* g.nb_levels[v.route]);
	for(int i=0;i<g.nb_levels[v.route];i++)
	{
		v.pos[i]=rand()%2;
	}

	//On echange dans la periode
	for(int level=0;level<g.nb_levels[v.route];level++)
	{
		x=level;
		kind = FORWARD;
		if(level >= g.nb_levels[v.route]/2)
		{
			x=g.nb_levels[v.route]-level-1;
			kind = BACKWARD;
		}
		//x contient le level de contention de l'arc, et kind le sens


		//On cherche l'indice de au quel v.route est placée dans l'arc
		idtmp = -1;
		
		for(int i=0;i<g.contention[v.route][x]->nb_routes;i++)
		{
			if(kind == FORWARD)
			{
				if(g.contention[v.route][x]->routes_order_f[i] == v.route)
				{
					idtmp = i;
					break;
				}
			}
			else
			{
				if(g.contention[v.route][x]->routes_order_b[i] == v.route)
				{
					idtmp = i;
					break;
				}
			}

		}
		if(idtmp ==-1)
		{
			printf("Error, indice not found (recuit, voisin alea).\n");exit(35);
		}


		if(v.pos[level] == 1)//permutation a gauche
		{
			if(kind == FORWARD)
			{
				echanger_gauche(g.contention[v.route][x]->routes_order_f,g.contention[v.route][x]->nb_routes , idtmp);
			}
			else
			{
				echanger_gauche(g.contention[v.route][x]->routes_order_b,g.contention[v.route][x]->nb_routes , idtmp);
			}
		}
		
	}
	
	return v;
}


void remet_voisin(Graph g,Voisin v)
{
	
	int idtmp;
	Period_kind kind;
	int x;

	//On echange dans la periode
	for(int level=0;level<g.nb_levels[v.route];level++)
	{
		x=level;
		kind = FORWARD;
		if(level >= g.nb_levels[v.route]/2)
		{
			x=g.nb_levels[v.route]-level-1;
			kind = BACKWARD;
		}
		//x contient le level de contention de l'arc, et kind le sens


		//On cherche l'indice de au quel v.route est placée dans l'arc
		idtmp = -1;
		
		for(int i=0;i<g.contention[v.route][x]->nb_routes;i++)
		{
			if(kind == FORWARD)
			{
				if(g.contention[v.route][x]->routes_order_f[i] == v.route)
				{
					idtmp = i;
					break;
				}
			}
			else
			{
				if(g.contention[v.route][x]->routes_order_b[i] == v.route)
				{
					idtmp = i;
					break;
				}
			}

		}
		if(idtmp ==-1)
		{
			printf("Error, indice not found (recuit, reinit voisin ).\n");exit(35);
		}


		if(v.pos[level] == 1)//permutation a gauche
		{
			if(kind == FORWARD)
			{
				echanger_droite(g.contention[v.route][x]->routes_order_f,g.contention[v.route][x]->nb_routes , idtmp);
			}
			else
			{
				echanger_droite(g.contention[v.route][x]->routes_order_b,g.contention[v.route][x]->nb_routes , idtmp);
			}
		}
		
	}
	free(v.pos);
}
int CritMetropolis(int delta, int t)
{
	if(t<=0)
	{
		return 0;
	}
	if(delta >= 0)
	{
		return 1;
	}
	else
	{
		float proba = (float)delta/(float)t;
		float random = (float)rand() / (float)RAND_MAX;
		if(random < expf(-proba))
		{
			return 1;
		}
	}
	return 0;
}
Assignment recuit(Graph g, int P, int message_size, int param)
{
	//Parametres du recuit
	int nb_paliers = param;
	float temperature = 5000;
	float coeff= 0.95;
	int seuil_arret = 10;
	float seuil_incr_cmpt = 0.10;

	Assignment a=NULL;
	if(!greedy_deadline(g, P, message_size))
	{
		printf("Error, greedystatdeadline didnt find an order(voisinage.c)\n");
		return a;
	}
	
	a = assignment_with_orders(g,P,message_size,0);
	a->time = travel_time_max_buffers(g);
	
	int min = a->time;
	int time_actuel = min;
	reinit_delays(g);
	free_assignment(a);
	int nb_moves;
	float acceptance_rate;
	Voisin v;
	
	int **orders = malloc(sizeof(int*)*g.arc_pool_size*2);

	for(int i=0;i<g.arc_pool_size;i++)
	{
		orders[i] = malloc(sizeof(int)*g.arc_pool[i].nb_routes);
		orders[i+g.arc_pool_size] = malloc(sizeof(int)*g.arc_pool[i].nb_routes);
	}
	int cmpt = 0;
	int nb_step = 0;
	while(cmpt < seuil_arret) //Condition d'arret à définir
	{
		nb_moves = 0;
		for(int i=0;i<nb_paliers;i++)
		{
			v = Voisin_alea(g);
			nb_step++;

			a = assignment_with_orders(g,P,message_size,0);
			if(a->all_routes_scheduled)
			{
				a->time = travel_time_max_buffers(g);
				if(CritMetropolis(a->time - time_actuel,temperature))//On swap sur le nouveau voisin.
				{
					time_actuel = a->time;
					//Le graph a ete changé dans voisin alea, on a donc rien a faire

					//Sauvegarde du mec le plus opti
					if(a->time < min)
					{
						cpy_orders(orders,g,1);// de g vers orders
						min = a->time;
						cmpt = 0;
					}
					nb_moves++;
					
				}
				else //Echec, on ne prend pas ce voisin, on remet le graph d'avant avant de tirer un nouveau voisin aleatoire
				{
					remet_voisin(g,v);
				}
			}//On a pas reussi a construire une solution, on remet le voisin d'avant
			else
			{
				remet_voisin(g,v);
			}
			reinit_delays(g);
			free_assignment(a);
		}
		acceptance_rate = (float)nb_moves/nb_paliers;
		if(acceptance_rate < seuil_incr_cmpt)
		{
			cmpt++;
		}
			
		temperature *=  coeff;
	}
	printf("Temperature %f, nb_step %d \n",temperature,nb_step);


	//On prend la meilleure solution vue, et bye
	cpy_orders(orders,g,0);
	reinit_delays(g);
	reset_periods(g,P);
	a = assignment_with_orders(g,P,message_size,1);

	a->time = travel_time_max_buffers(g);
	if(verifie_solution( g,message_size))
	{
		printf("La solution n'est pas correcte recuit (error %d) ",verifie_solution( g,message_size));
		exit(83);
	}
	return a;

}