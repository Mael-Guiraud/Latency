#include <stdlib.h>
#include <stdio.h>
#include "structs.h"
#include "config.h"
#include "biparti.h"
#include "test.h"
#include "treatment.h"
#include "connexe.h"


void init_arc(Arc * a)
{
	a->length = rand()%MAX_LENGTH;
	a->nb_routes = 0;
	a->period_f = NULL;
	a->period_b = NULL;

	a->first = -1;
	a->bounded = 0;
	a->last = -1;
	a->bbu_dest = -1;
	a->seen = 0;
	for(int i=0;i<128;i++)
	{
		a->routes_id[i]=0;
		a->routes_order_f[i]=0;
		a->routes_order_b[i]=0;
		a->routes_delay_f[i]=0;
		a->routes_delay_b[i]=0;
	}
}

int count_real_collisions(int ** graph, int nb_bbu, int nb_collisions)
{
	int nb_real = 0;
	for(int i=0;i<nb_collisions;i++)
	{
		for(int j=0;j<nb_bbu;j++)
		{
			if(graph[j][i])
			{
				nb_real++;
				break;
			}
		}
	}	
	return nb_real;
}

int detect_fake(int ** graph, int nb_bbu, int col_id)
{
	for(int j=0;j<nb_bbu;j++)
	{
		if(graph[j][col_id])
		{
			return 0;
		}
		
	}	
	return 1;
}
int count_routes(int ** graph, int nb_bbu, int nb_collisions)
{
	int nb_routes = 0;
	for(int i=0;i<nb_bbu;i++)
	{
		for(int j=0;j<nb_collisions;j++)
		{
			if(graph[i][j])
			{
				nb_routes++;
			}
		}
	}	
	return nb_routes;
}

int* random_number_routes(int l)
{

	int * tab = malloc(sizeof(int)*(l));
	for(int i=0;i<(l);i++)
	{
		if(NB_ROUTES_PER_FLOW == 1)
		{
			tab[i] = 1;
		}
		else
			tab[i] = rand()%(NB_ROUTES_PER_FLOW) +1;
	}
	return tab;
}
int sum(int * tab, int l)
{

	int tmp=0;
	for(int i=0;i<l;i++)
	{
		tmp+= tab[i];
	}
	return tmp;
}
Graph init_graph_random_tree(double load)
{
	int nb_bbu = NB_BBU;
	int nb_collisions = NB_COLLISIONS;
	int ** graph = NULL;
	do
	{
		if(graph)
			free_biparti(graph,nb_bbu);
		graph = random_biparti(nb_bbu,nb_collisions);
		
	}while(!is_connexe(graph, nb_bbu, nb_collisions));
	int nb_real_collisions = count_real_collisions(graph,nb_bbu,nb_collisions);
	int nb_arcs_in_biparti = count_routes(graph,nb_bbu,nb_collisions);
	int * nb_routes_per_flow_bbu = random_number_routes(nb_bbu);
	int * nb_routes_per_flow_collisions = random_number_routes(nb_arcs_in_biparti);
	
	
	int nb_routes_bbu = sum(nb_routes_per_flow_bbu,nb_bbu);
	int nb_routes_collisions = sum(nb_routes_per_flow_collisions,nb_arcs_in_biparti);
	int nb_routes = nb_routes_bbu + nb_routes_collisions;

	int nb_total_arcs = nb_bbu + nb_routes + nb_arcs_in_biparti + nb_real_collisions;
	int index_arc,index_route;
	int nb_fake_collisions;
	int index_middle_arc;
	int index_bip;
	int real_period;

	//printf("%d %d %d %d %d\n",nb_bbu,nb_collisions,nb_real_collisions,nb_total_arcs,nb_routes);
	Graph g;
	g.kind = TREE;
	g.nb_routes = nb_routes;
	g.contention_level = 4;
	g.nb_bbu = nb_bbu;
	g.nb_collisions = nb_real_collisions;
	g.arc_pool = malloc(sizeof(Arc)*nb_total_arcs);
	g.routes = malloc(sizeof(Route*)*nb_routes);
	g.contention = malloc(sizeof(Route*)*nb_routes);
	g.size_routes = malloc(sizeof(int)*nb_routes);
	g.nb_levels = malloc(sizeof(int)*nb_routes);
	g.arc_pool_size = nb_total_arcs;
	for(int i=0;i<nb_total_arcs;i++)
	{
		init_arc(&g.arc_pool[i]);	
	}

	
	index_route = 0;
	index_arc = nb_bbu+nb_real_collisions+nb_arcs_in_biparti;
	//The antennas directly connected to the datacenters
	for(int i=0;i<nb_bbu;i++)
	{
		for(int j=0;j<nb_routes_per_flow_bbu[i];j++)
		{
			g.routes[index_route]=malloc(sizeof(Route)*2);
			g.contention[index_route]=malloc(sizeof(Route));
			g.size_routes[index_route] = 2;
			g.nb_levels[index_route] = 2;

			g.routes[index_route][0] =  &g.arc_pool[index_arc];
			g.arc_pool[index_arc].routes_id[g.arc_pool[index_arc].nb_routes] = index_route;
			g.arc_pool[index_arc].nb_routes++;
			g.arc_pool[index_arc].bbu_dest = i;
			g.arc_pool[index_arc].contention_level = -1;


			index_arc++;
			g.routes[index_route][1] =  &g.arc_pool[i];
			g.contention[index_route][0] =  &g.arc_pool[i];
			g.arc_pool[i].routes_id[g.arc_pool[i].nb_routes] = index_route;
			g.arc_pool[i].nb_routes++;
			g.arc_pool[i].contention_level = 1;
			

			index_route++;
		}

	}
	//printf("1 %d \n",nb_real_collisions+nb_bbu);
	
	nb_fake_collisions = 0;
	index_middle_arc = nb_bbu+nb_real_collisions;// pour identifier l'arc du graph dans le tableau d'arcs
	index_bip = 0; // pour identifier l'arc du graph dans le tableau nb_routes_per_flow_collisions
	for(int i=0;i<nb_bbu;i++)
	{
		nb_fake_collisions = 0;
		for(int j=0;j<nb_collisions;j++)
		{
			// Si cette colision est reliée avec aucune bbu, elle est inutile mais prise en compte quand même dans les calculs.
			nb_fake_collisions += detect_fake(graph,nb_bbu,j);
			if(graph[i][j])//If there is an arc
			{
				for(int k=0;k<nb_routes_per_flow_collisions[index_bip];k++)
				{
					//printf("%d %d %d %d \n",nb_bbu+j-nb_fake_collisions,index_arc,i,index_route);

					g.routes[index_route]=malloc(sizeof(Route)*4);
					g.contention[index_route]=malloc(sizeof(Route)*2);
					g.size_routes[index_route] = 4;
					g.nb_levels[index_route] = 4;

					//Arcs vers les antennes
					g.routes[index_route][0] = &g.arc_pool[index_arc];
					g.arc_pool[index_arc].routes_id[g.arc_pool[index_arc].nb_routes] = index_route;
					g.arc_pool[index_arc].nb_routes++;
					g.arc_pool[index_arc].bbu_dest = i;
					g.arc_pool[index_arc].contention_level = -1;
					index_arc++;

					//ARcs des points de collisions
					g.routes[index_route][1] = &g.arc_pool[nb_bbu+j-nb_fake_collisions];
					g.contention[index_route][0] = &g.arc_pool[nb_bbu+j-nb_fake_collisions];
					g.arc_pool[nb_bbu+j-nb_fake_collisions].routes_id[g.arc_pool[nb_bbu+j-nb_fake_collisions].nb_routes] = index_route;
					g.arc_pool[nb_bbu+j-nb_fake_collisions].nb_routes++;
					g.arc_pool[nb_bbu+j-nb_fake_collisions].bbu_dest = i;
					g.arc_pool[nb_bbu+j-nb_fake_collisions].contention_level = 0;

					//ARcs du graph biparti
					g.routes[index_route][2] = &g.arc_pool[index_middle_arc];
					g.arc_pool[index_middle_arc].routes_id[g.arc_pool[index_middle_arc].nb_routes] = index_route;
					g.arc_pool[index_middle_arc].nb_routes++;
					g.arc_pool[index_middle_arc].bbu_dest = i;
					g.arc_pool[index_middle_arc].contention_level = -1;
					

					//Arcs vers la BBU
					g.routes[index_route][3] = &g.arc_pool[i];
					g.contention[index_route][1] = &g.arc_pool[i];
					g.arc_pool[i].routes_id[g.arc_pool[i].nb_routes] = index_route;
					g.arc_pool[i].nb_routes++;
					g.arc_pool[i].bbu_dest = i;
					g.arc_pool[i].contention_level = 1;
					index_route++;
					
				}
				
				index_middle_arc++;
				index_bip ++;

			}
		}
	}	

	for(int i=0;i<nb_total_arcs;i++)
	{
		for(int j=0;j<128;j++)
		{
			g.arc_pool[i].routes_delay_f[j] =0;		
			g.arc_pool[i].routes_delay_b[j] =0;		
		}
		
	}

	
	if(FIXED_PERIOD_MOD)
		real_period = PERIOD;
	else
		real_period = (load_max(g) * MESSAGE_SIZE) / load;
	g.period = real_period;
	//for(int i=0;i<g.arc_pool_size;i++)
	for(int i=0;i<nb_real_collisions+nb_bbu;i++)
	{
		g.arc_pool[i].period_f = calloc(real_period,sizeof(int));
		g.arc_pool[i].period_b = calloc(real_period,sizeof(int));
	}
	
	free_biparti(graph,nb_bbu);	
	free(nb_routes_per_flow_bbu);
	free(nb_routes_per_flow_collisions);
	return g;
}

Graph init_graph_etoile(int nb, int P)
{
	
	int nb_routes = nb ;
	int nb_bbu = nb;
	int nb_total_arcs = nb_routes*3;


	//printf("%d %d %d %d %d\n",nb_bbu,nb_collisions,nb_real_collisions,nb_total_arcs,nb_routes);
	Graph g;
	g.nb_routes = nb_routes;
	g.kind = STAR;
	g.nb_bbu = nb_bbu;
	g.nb_collisions = 1;
	g.arc_pool = malloc(sizeof(Arc)*nb_total_arcs);
	g.routes = malloc(sizeof(Route*)*nb_routes);
	g.size_routes = malloc(sizeof(int)*nb_routes);
	g.arc_pool_size = nb_total_arcs;
	for(int i=0;i<nb_total_arcs;i++)
	{
		init_arc(&g.arc_pool[i]);	
	}

	
	int index_route = 0;
	int index_arc = nb_routes+1;

	for(int j=0;j<nb_routes;j++)
	{
		g.routes[index_route]=malloc(sizeof(Route)*3);
		g.size_routes[index_route] = 3;

		g.routes[index_route][0] =  &g.arc_pool[index_arc];
		g.arc_pool[index_arc].routes_id[g.arc_pool[index_arc].nb_routes] = index_route;
		g.arc_pool[index_arc].nb_routes++;
		g.arc_pool[index_arc].bbu_dest = index_route;
		index_arc++;
		//Arc partagé
		
		g.routes[index_route][1] =  &g.arc_pool[nb_routes];
		g.arc_pool[nb_routes].routes_id[g.arc_pool[nb_routes].nb_routes] = index_route;
		g.arc_pool[nb_routes].nb_routes++;
		

		g.routes[index_route][2] =  &g.arc_pool[j];
		g.arc_pool[j].routes_id[g.arc_pool[j].nb_routes] = index_route;
		g.arc_pool[j].nb_routes++;
		g.arc_pool[j].bbu_dest = index_route;
		index_route++;
	}

	



	g.arc_pool[nb_routes].period_f = calloc(P,sizeof(int));
	g.arc_pool[nb_routes].period_b = calloc(P,sizeof(int));
	
	
	return g;
}


Graph init_graph_etoile_values_set(int nb, int P,int * tab)
{
	
	int nb_routes = nb ;
	int nb_bbu = nb;
	int nb_total_arcs = nb_routes*3;


	//printf("%d %d %d %d %d\n",nb_bbu,nb_collisions,nb_real_collisions,nb_total_arcs,nb_routes);
	Graph g;
	g.kind = STAR;
	g.nb_routes = nb_routes;
	g.nb_bbu = nb_bbu;
	g.nb_collisions = 1;
	g.arc_pool = malloc(sizeof(Arc)*nb_total_arcs);
	g.routes = malloc(sizeof(Route*)*nb_routes);
	g.size_routes = malloc(sizeof(int)*nb_routes);
	g.arc_pool_size = nb_total_arcs;
	for(int i=0;i<nb_total_arcs;i++)
	{
		if(i<g.nb_bbu)
		{
			g.arc_pool[i].length = tab[i];
		}
		else
		{
			g.arc_pool[i].length = rand()%MAX_LENGTH;
		}
		
		g.arc_pool[i].nb_routes = 0;
		g.arc_pool[i].period_f = NULL;
		g.arc_pool[i].period_b = NULL;

		g.arc_pool[i].first = -1;
		g.arc_pool[i].last = -1;
		g.arc_pool[i].bbu_dest = -1;
		g.arc_pool[i].seen = 0;
	}

	
	int index_route = 0;
	int index_arc = nb_routes+1;

	for(int j=0;j<nb_routes;j++)
	{
		g.routes[index_route]=malloc(sizeof(Route)*3);
		g.size_routes[index_route] = 3;

		g.routes[index_route][0] =  &g.arc_pool[index_arc];
		g.arc_pool[index_arc].routes_id[g.arc_pool[index_arc].nb_routes] = index_route;
		g.arc_pool[index_arc].nb_routes++;
		g.arc_pool[index_arc].bbu_dest = index_route;
		index_arc++;
		//Arc partagé
		
		g.routes[index_route][1] =  &g.arc_pool[nb_routes];
		g.arc_pool[nb_routes].routes_id[g.arc_pool[nb_routes].nb_routes] = index_route;
		g.arc_pool[nb_routes].nb_routes++;
		

		g.routes[index_route][2] =  &g.arc_pool[j];
		g.arc_pool[j].routes_id[g.arc_pool[j].nb_routes] = index_route;
		g.arc_pool[j].nb_routes++;
		g.arc_pool[j].bbu_dest = index_route;
		index_route++;
	}

	



	g.arc_pool[nb_routes].period_f = calloc(P,sizeof(int));
	g.arc_pool[nb_routes].period_b = calloc(P,sizeof(int));
	
	
	return g;
}
