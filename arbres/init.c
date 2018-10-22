#include "structs.h"
#include "config.h"
#include "biparti.h"
#include "test.h"
#include <stdlib.h>
#include <stdio.h>

void init_arc(Arc * a)
{
	a->length = rand()%MAX_LENGTH;
	a->nb_routes = 0;
	a->period_f = NULL;
	a->period_b = NULL;
}


Graph init_graph(int nb_routes, int P)
{
	//FAit a la main pour l'instant
	int nb_total_arcs = (nb_routes * NB_ARCS - (nb_routes-1)) ;

	Graph g;
	g.nb_routes = nb_routes;
	g.arc_pool = malloc(sizeof(Arc)*nb_total_arcs);
	g.routes = malloc(sizeof(Route*)*nb_routes);
	g.size_routes = malloc(sizeof(int)*nb_routes);

	for(int i=0;i<nb_total_arcs;i++)
	{
		init_arc(&g.arc_pool[i]);	
	}
	for(int i= 0 ;i<nb_routes;i++)
	{
		g.routes[i]=malloc(sizeof(Route)*NB_ARCS);
	}
	for(int i=0;i<nb_routes;i++)
	{
		g.size_routes[i]=NB_ARCS;
	}

	//Route 1
	for(int i=0;i<4;i++)
	{
		g.routes[0][i] = &g.arc_pool[i];
		g.arc_pool[i].nb_routes++;
	}
	//Route 2
	for(int i=4;i<7;i++)
	{
		g.routes[1][i-4] = &g.arc_pool[i];
		g.arc_pool[i].nb_routes++;
	}
	g.routes[1][3] = &g.arc_pool[3];
	g.arc_pool[3].nb_routes++;
	g.arc_pool[3].period_f = calloc(P,sizeof(int));
	g.arc_pool[3].period_b = calloc(P,sizeof(int));

	//Route 3
	
	g.routes[2][0] = &g.arc_pool[4];
	g.arc_pool[4].nb_routes++;
	g.arc_pool[4].period_f = calloc(P,sizeof(int));
	g.arc_pool[4].period_b = calloc(P,sizeof(int));
	for(int i=7;i<10;i++)
	{
		g.routes[2][i-6] = &g.arc_pool[i];
		g.arc_pool[i].nb_routes++;
	}



		
	return g;
}

Graph init_graph_tree(int nb_routes, int P)
{
	//FAit a la main pour l'instant
	int nb_total_arcs = 2 + nb_routes*(NB_ARCS-1) - nb_routes/2 + 1;//Datacenter + nb uniques + nb on collision+ collision arc
	int index_arc;
	Graph g;
	g.nb_routes = nb_routes;
	g.arc_pool = malloc(sizeof(Arc)*nb_total_arcs);
	g.routes = malloc(sizeof(Route*)*nb_routes);
	g.size_routes = malloc(sizeof(int)*nb_routes);

	for(int i=0;i<nb_total_arcs;i++)
	{
		init_arc(&g.arc_pool[i]);	
	}
	for(int i= 0 ;i<nb_routes;i++)
	{
		g.routes[i]=malloc(sizeof(Route)*NB_ARCS);
	}
	for(int i=0;i<nb_routes;i++)
	{
		g.size_routes[i]=NB_ARCS;
	}

	//Init des collisions

	//DC1
	g.arc_pool[0].period_f = calloc(P,sizeof(int));
	g.arc_pool[0].period_b = calloc(P,sizeof(int));

	//DC2
	g.arc_pool[1].period_f = calloc(P,sizeof(int));
	g.arc_pool[1].period_b = calloc(P,sizeof(int));

	//Collision
	g.arc_pool[2].period_f = calloc(P,sizeof(int));
	g.arc_pool[2].period_b = calloc(P,sizeof(int));
	index_arc = 3;
	for(int i=0;i<nb_routes/2;i++)
	{
		for(int j=0;j<NB_ARCS-1;j++)
		{
			g.routes[i][j] = &g.arc_pool[index_arc];
			g.arc_pool[index_arc].nb_routes++;
			index_arc++;
		}
		g.routes[i][NB_ARCS-1] = &g.arc_pool[i%2];//One route on two is assigned to DC1, other on DC2
		g.arc_pool[i%2].nb_routes++;

	}
	for(int i=nb_routes/2;i<nb_routes;i++)
	{

		g.routes[i][0] = &g.arc_pool[index_arc];
		g.routes[i][1] = &g.arc_pool[2];
		g.routes[i][NB_ARCS-1] = &g.arc_pool[i%2];//One route on two is assigned to DC1, other on DC2
		g.arc_pool[index_arc].nb_routes++;
		g.arc_pool[2].nb_routes++;
		g.arc_pool[i%2].nb_routes++;
		index_arc++;
	}

		
	return g;
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

int* random_number_routes(int nb_bbu,int nb_arcs_in_biparti)
{

	int * tab = malloc(sizeof(int)*(nb_bbu+nb_arcs_in_biparti));
	for(int i=0;i<(nb_bbu+nb_arcs_in_biparti);i++)
	{
		if(NB_ROUTES_PER_FLOW == 1)
		{
			tab[i] = 1;
		}
		else
			tab[i] = rand()%(NB_ROUTES_PER_FLOW-1) +1;
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
Graph init_graph_random_tree(int P)
{
	int nb_bbu = NB_BBU;
	int nb_collisions = NB_COLLISIONS;
	int ** graph = random_biparti(nb_bbu,nb_collisions);
	int nb_real_collisions = count_real_collisions(graph,nb_bbu,nb_collisions);
	int nb_arcs_in_biparti = count_routes(graph,nb_bbu,nb_collisions);
	int * nb_routes_per_flow = random_number_routes(nb_bbu,nb_arcs_in_biparti);
	
	
	int nb_routes = sum(nb_routes_per_flow,nb_arcs_in_biparti+nb_bbu);
	
	int nb_total_arcs = nb_bbu + nb_real_collisions + nb_arcs_in_biparti;
	int index_arc,index_route;
	int nb_fake_collisions;
	int index_middle_arc;
	 affiche_biparti(graph,nb_bbu,nb_collisions);
	//printf("%d %d %d %d %d\n",nb_bbu,nb_collisions,nb_real_collisions,nb_total_arcs,nb_routes);
	Graph g;
	g.nb_routes = nb_routes;
	g.arc_pool = malloc(sizeof(Arc)*nb_total_arcs);
	g.routes = malloc(sizeof(Route*)*nb_routes);
	g.size_routes = malloc(sizeof(int)*nb_routes);
	g.arc_pool_size = nb_total_arcs;
	for(int i=0;i<nb_total_arcs;i++)
	{
		init_arc(&g.arc_pool[i]);	
	}

	for(int i=0;i<nb_real_collisions+nb_bbu;i++)
	{
		g.arc_pool[i].period_f = calloc(P,sizeof(int));
		g.arc_pool[i].period_b = calloc(P,sizeof(int));
	}
	index_route = 0;
	//The antennas directly connected to the datacenters
	for(int i=0;i<nb_bbu;i++)
	{
		for(int j=0;j<nb_routes_per_flow[i];j++)
		{
			g.routes[index_route]=malloc(sizeof(Route));
			g.size_routes[index_route] = 1;
			g.routes[index_route][0] =  &g.arc_pool[i];
			g.arc_pool[i].routes_id[g.arc_pool[i].nb_routes] = index_route;
			g.arc_pool[i].nb_routes++;
			index_route++;
		}
	}
	//printf("1 %d \n",nb_real_collisions+nb_bbu);
	index_arc = nb_bbu+nb_real_collisions;
	nb_fake_collisions = 0;
	index_middle_arc = nb_bbu;
	for(int i=0;i<nb_bbu;i++)
	{
		nb_fake_collisions = 0;
		for(int j=0;j<nb_collisions;j++)
		{
			nb_fake_collisions += detect_fake(graph,nb_bbu,j);
			if(graph[i][j])//If there is an arc
			{
				for(int k=0;k<nb_routes_per_flow[index_middle_arc];k++)
				{
					//printf("%d %d %d %d \n",nb_bbu+j-nb_fake_collisions,index_arc,i,index_route);
					g.routes[index_route]=malloc(sizeof(Route)*3);
					g.size_routes[index_route] = 3;
					g.routes[index_route][0] = &g.arc_pool[nb_bbu+j-nb_fake_collisions];
					g.arc_pool[nb_bbu+j-nb_fake_collisions].routes_id[g.arc_pool[nb_bbu+j-nb_fake_collisions].nb_routes] = index_route;
					g.arc_pool[nb_bbu+j-nb_fake_collisions].nb_routes++;
					g.routes[index_route][1] = &g.arc_pool[index_arc];
					g.arc_pool[index_arc].routes_id[g.arc_pool[index_arc].nb_routes] = index_route;
					g.arc_pool[index_arc].nb_routes++;
					g.routes[index_route][2] = &g.arc_pool[i];
					g.arc_pool[i].routes_id[g.arc_pool[i].nb_routes] = index_route;
					g.arc_pool[i].nb_routes++;
					index_route++;
					
				}
				index_arc++;
				index_middle_arc++;

			}
		}
	}	
	free_biparti(graph,nb_bbu);	
	return g;
}