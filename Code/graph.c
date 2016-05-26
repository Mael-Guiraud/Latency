// FONCTIONS SUR LES GRAPHES
#include "graph.h"

void ecrire_fichier(Graphe g)
{
	Graphe gr = renverse(g);
	creationfichier(g,"g.dot");
	creationfichier(gr,"gr.dot");
	
}
void creationfichier(Graphe g,char * nom){

	FILE *f;
	f=fopen(nom,"w+");
	fprintf(f,"graph noeud_%d {\n", g.vertex_number);
	fprintf(f,"node [fixedsize=true width=0.4 height=0.25];\n");
	int i,j;
	for(i=0; i <g.vertex_number;i++){
		for(j=0; j < g.vertices[i].edge_number;j++){
			fprintf(f,"%d -- %d [dir=back, label=\"%d\"]; \n",g.vertices[i].index,g.vertices[i].neighboors[j],g.vertices[i].weight[j]);
		}
	}
	fprintf(f,"}");
	fclose(f);
	
}

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

TwoWayTrip heuristique_top_1(Graphe g, int P)
{
	TwoWayTrip t ;
	t.taille=g.sources;
	//offsets
	t.M = malloc(sizeof(int)*g.sources);
	t.W = malloc(sizeof(int)*g.sources);
	int * Dl = malloc(sizeof(int)*g.sources);
	int * lambdaV = malloc(sizeof(int)*g.sources);
	//booleans 1= route affecté 0=route non traitée
	int aff[g.sources];
	int i;
	int offset = 0;
	
	//init
	for(i=0;i<g.sources;i++)
	{
		aff[i] = 0;
		Dl[i] = distance(g.routes[i],g.routes[i].route_lenght);
		lambdaV[i] = distance(g.routes[i],1);
	}
	
	//tant qu'on n'a pas affecté toutes les routes
	while(!full(aff,g.sources))
	{
		i=greater(Dl,g.sources);
		//Si on est le premier
		if(offset==0)
		{
			t.M[i] = 0;
			offset=2500+lambdaV[i];
		}
		else
		{
			t.M[i] = offset-lambdaV[i];
			offset += 2500;
		}
		aff[i] = 1;
		Dl[i]=0;
	}
	
	//init second offset : waiting times
	Graphe gr = renverse(g);
	int arrivee[gr.sources];
	int arriveeandlambda[gr.sources];
	offset = 0;
	for(i=0;i<gr.sources;i++)
	{
		aff[i] = 0;
		Dl[i] = distance(gr.routes[i],gr.routes[i].route_lenght);
		lambdaV[i] = distance(gr.routes[i],1);
		arrivee[i] = Dl[i]+t.M[i];
		arriveeandlambda[i] = arrivee[i];
	}
	ajoutetab(arriveeandlambda,lambdaV,g.sources);
	
	//tant qu'on n'a pas affecté toutes les routes
	while(!full(aff,gr.sources))
	{
		//Si on est le premier
		if(offset==0)
		{
			i=lower(arriveeandlambda,gr.sources);
			t.W[i] = 0;
			offset=2500+lambdaV[i]+arrivee[i];
		}
		else
		{
			
			i=longerOfeEligible(Dl, lambdaV,arrivee,offset,gr.sources);
			//dans le cas ou i est deja traité, on ajoute 100 a l'offset et on retrouve un autre i.
			//printf("i=%d offset = %d , lambdaV = %d arrive = %d\n",i, offset, lambdaV[i],arrivee[i]);
			
			if(Dl[i] ==0)
			{
				offset+=100;
			}
			else
			{
			t.W[i] = offset-lambdaV[i]-arrivee[i];
			offset += 2500;
			}
		}
		aff[i] = 1;
		Dl[i]=0;
	}
	
return t;
	
}











