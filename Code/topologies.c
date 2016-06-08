// GENERATION DES TOPOLOGIES
#include "graph.h"

//mode 0 = tout random, 1 - 0 sur les leaves , 2 - 0 sur les sources
Graphe topologie1(int sources, int leaves,int mode)
{
	int x,y;
	if(mode == 0)
	{
		x = 700;
		y = 700;
	}
	else if(mode == 1)
	{
		x=0;
		y=700;
	}
	else
	{
		x=700;
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
	for(i;i<sources+leaves+2;i++)
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
