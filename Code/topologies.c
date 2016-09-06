// GENERATION DES TOPOLOGIES
#include "graph.h"

Graphe topologie1_variation(int taille, int taille_SS)
{
	int x,y;
	x = TAILLE_ROUTE/3;
	y = taille_SS;
	int sources = taille;
	int leaves = taille;
	Graphe g;
	g.sources = sources;
	g.vertex_number = sources+leaves+2;
	g.vertices = malloc((g.vertex_number)*sizeof(Vertex));
	int i;
	//sources
	for(i=0;i<sources;i++)
	{
		g.vertices[i].index = i;
		g.vertices[i].edge_number = 0;
	}
	
	//sources switch
	g.vertices[i].index = i;
	g.vertices[i].edge_number = sources;
	g.vertices[i].neighboors= malloc(sources*sizeof(int));
    g.vertices[i].weight= calloc(sources,sizeof(int));
	int j;
	for(j=0;j<sources;j++)
	{
		
		g.vertices[i].neighboors[j] = j;
		g.vertices[i].weight[j] = rand_entier(x);
	}
	
	//leaves switch
	i++;
	g.vertices[i].index = i;
	g.vertices[i].edge_number = 1;
	g.vertices[i].neighboors= malloc(1*sizeof(int));
    g.vertices[i].weight= calloc(1,sizeof(int));
    g.vertices[i].neighboors[0] = sources;
	g.vertices[i].weight[0] = rand_entier(700);
	i++;
	//leaves
	for(;i<sources+leaves+2;i++)
	{
		g.vertices[i].index = i;
		g.vertices[i].edge_number = 1;
		g.vertices[i].neighboors= malloc(1*sizeof(int));
		g.vertices[i].weight= calloc(1,sizeof(int));
		g.vertices[i].neighboors[0] = sources+1;
		g.vertices[i].weight[0] = rand_entier(y);
	}
	g.routes = malloc(sizeof(Route)*sources);
	for(i=0;i<sources;i++)
	{

		g.routes[i].route_lenght = 4;

		g.routes[i].vertices= malloc(sizeof(Vertex)*4);

		for(j=0;j<4;j++)
		{
			if(j==0)
				g.routes[i].vertices[j] = g.vertices[i];
			else 
				if(j==1)
					g.routes[i].vertices[j] = g.vertices[sources];
				else 
					if(j==2)
						g.routes[i].vertices[j] = g.vertices[sources +1];
					else 
						g.routes[i].vertices[j] = g.vertices[sources+1+i+1];
			
		}
	
	}

	return g;
}
//Genere un Graphe de la topologie 1
//mode 0 = tout random, 1 - 0 sur les leaves , 2 - 0 sur les sources
Graphe topologie1(int sources, int leaves,int mode)
{
	int x,y;
	if(mode == 0)
	{
		x = TAILLE_ROUTE/3;
		y = 4000;
	}
	else if(mode == 1)
	{
		x=0;
		y=TAILLE_ROUTE/3;
	}
	else
	{
		x=TAILLE_ROUTE/3;
		y=0;
	}
	Graphe g;
	g.sources = sources;
	g.vertex_number = sources+leaves+2;
	g.vertices = malloc((g.vertex_number)*sizeof(Vertex));
	int i;
	//sources
	for(i=0;i<sources;i++)
	{
		g.vertices[i].index = i;
		g.vertices[i].edge_number = 0;
	}
	
	//sources switch
	g.vertices[i].index = i;
	g.vertices[i].edge_number = sources;
	g.vertices[i].neighboors= malloc(sources*sizeof(int));
    g.vertices[i].weight= calloc(sources,sizeof(int));
	int j;
	for(j=0;j<sources;j++)
	{
		
		g.vertices[i].neighboors[j] = j;
		g.vertices[i].weight[j] = rand_entier(x);
	}
	
	//leaves switch
	i++;
	g.vertices[i].index = i;
	g.vertices[i].edge_number = 1;
	g.vertices[i].neighboors= malloc(1*sizeof(int));
    g.vertices[i].weight= calloc(1,sizeof(int));
    g.vertices[i].neighboors[0] = sources;
	g.vertices[i].weight[0] = rand_entier(700);
	i++;
	//leaves
	for(;i<sources+leaves+2;i++)
	{
		g.vertices[i].index = i;
		g.vertices[i].edge_number = 1;
		g.vertices[i].neighboors= malloc(1*sizeof(int));
		g.vertices[i].weight= calloc(1,sizeof(int));
		g.vertices[i].neighboors[0] = sources+1;
		g.vertices[i].weight[0] = rand_entier(y);
	}
	g.routes = malloc(sizeof(Route)*sources);
	for(i=0;i<sources;i++)
	{

		g.routes[i].route_lenght = 4;

		g.routes[i].vertices= malloc(sizeof(Vertex)*4);

		for(j=0;j<4;j++)
		{
			if(j==0)
				g.routes[i].vertices[j] = g.vertices[i];
			else 
				if(j==1)
					g.routes[i].vertices[j] = g.vertices[sources];
				else 
					if(j==2)
						g.routes[i].vertices[j] = g.vertices[sources +1];
					else 
						g.routes[i].vertices[j] = g.vertices[sources+1+i+1];
			
		}
	
	}

	return g;
}


//donne g renversé (topologie 1)
Graphe renverse(Graphe g)
{
	Graphe gr;
	gr.sources = g.sources;
	gr.vertex_number = g.vertex_number;
	gr.vertices = malloc((gr.vertex_number)*sizeof(Vertex));
	
	int i,j,k;
	int count = 0;
	for(i=0;i<gr.vertex_number;i++)
	{
		count =0;
		gr.vertices[i].index=i;
		for(j=0;j<g.vertex_number;j++)
		{
			for(k=0;k<g.vertices[j].edge_number;k++)
			{
				if(g.vertices[j].neighboors[k] == i)
				{
					count++;
				}
			}
		}
		gr.vertices[i].edge_number = count;
		gr.vertices[i].neighboors= malloc(count*sizeof(int));
		gr.vertices[i].weight= calloc(count,sizeof(int));
		count = 0;
		for(j=0;j<gr.vertex_number;j++)
		{
			for(k=0;k<g.vertices[j].edge_number;k++)
			{
				if(g.vertices[j].neighboors[k] == i)
				{
					gr.vertices[i].neighboors[count] = g.vertices[j].index;
					gr.vertices[i].weight[count] = g.vertices[j].weight[k];
					count++;
				}
			}
		}
	}
	
	gr.routes = malloc(sizeof(Route)*gr.sources);
	for(i=0;i<gr.sources;i++)
	{
		gr.routes[i].route_lenght = g.routes[i].route_lenght ;
		gr.routes[i].vertices= malloc(sizeof(Vertex)*gr.routes[i].route_lenght);
		for(j=0;j<gr.routes[i].route_lenght;j++)
		{
			gr.routes[i].vertices[j] = gr.vertices[g.routes[i].vertices[g.routes[i].route_lenght-1-j].index];
		}
	}
	return gr;
}


//Genere un Graphe de la topologie avec les poids donnés dans le tableau

Graphe topologie1_manuelle(int sources, int leaves,int * tab)
{

	Graphe g;
	g.sources = sources;
	g.vertex_number = sources+leaves+2;
	g.vertices = malloc((g.vertex_number)*sizeof(Vertex));
	int i;
	//sources
	for(i=0;i<sources;i++)
	{
		g.vertices[i].index = i;
		g.vertices[i].edge_number = 0;
	}
	
	//sources switch
	g.vertices[i].index = i;
	g.vertices[i].edge_number = sources;
	g.vertices[i].neighboors= malloc(sources*sizeof(int));
    g.vertices[i].weight= calloc(sources,sizeof(int));
	int j;
	for(j=0;j<sources;j++)
	{
		
		g.vertices[i].neighboors[j] = j;
		g.vertices[i].weight[j] = tab[j];
	}
	
	//leaves switch
	i++;
	g.vertices[i].index = i;
	g.vertices[i].edge_number = 1;
	g.vertices[i].neighboors= malloc(1*sizeof(int));
    g.vertices[i].weight= calloc(1,sizeof(int));
    g.vertices[i].neighboors[0] = sources;
	g.vertices[i].weight[0] = tab[sources];
	i++;
	//leaves
	for(;i<sources+leaves+2;i++)
	{
		g.vertices[i].index = i;
		g.vertices[i].edge_number = 1;
		g.vertices[i].neighboors= malloc(1*sizeof(int));
		g.vertices[i].weight= calloc(1,sizeof(int));
		g.vertices[i].neighboors[0] = sources+1;
		g.vertices[i].weight[0] = tab[i-1];
	}
	g.routes = malloc(sizeof(Route)*sources);
	for(i=0;i<sources;i++)
	{

		g.routes[i].route_lenght = 4;

		g.routes[i].vertices= malloc(sizeof(Vertex)*4);

		for(j=0;j<4;j++)
		{
			if(j==0)
				g.routes[i].vertices[j] = g.vertices[i];
			else if(j==1)
				g.routes[i].vertices[j] = g.vertices[sources];
			else if(j==2)
				g.routes[i].vertices[j] = g.vertices[sources +1];
			else 
				g.routes[i].vertices[j] = g.vertices[sources+1+i+1];
			
		}
	
	}

	return g;
}
