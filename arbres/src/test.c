#include "structs.h"
#include "init.h"
#include "treatment.h"
#include "data_treatment.h"
#include <stdio.h>

void affiche_tab(int * tab, int taille, FILE * f)
{
	for(int i=0;i<taille;i++)
	{
		fprintf(f,"%d,",tab[i]);
	}
	fprintf(f,"\n");
}
void affiche_periode(int * p, int size, FILE * f)
{
	int old, current;
	old = p[0];
	fprintf(f,"%d[%d-",old,0);
	for(int i=1;i<size;i++)
	{
		current = p[i];
		if(old != current)
		{
			fprintf(f,"%d]  %d[%d-",i-1,current,i);
		}
		old = current;
	}
	fprintf(f,"%d]\n",size-1);
}


void affiche_graph(Graph g,int p, FILE * f)
{
	for(int i=0;i<g.nb_routes; i++)
	{
		fprintf(f,"Route %d total length = %d : \n",i,route_length( g,i));
		for(int j=0 ; j< g.size_routes[i];j++)
		{
			fprintf(f,"[%d], partagé avec %d routes\n",g.routes[i][j]->length,g.routes[i][j]->nb_routes);
			fprintf(f,"Routes sur l'arc : ");
			for(int k=0;k<g.routes[i][j]->nb_routes;k++)
			{
				fprintf(f,"%d ",g.routes[i][j]->routes_id[k]);
			}
			fprintf(f,"\n");
			if( g.routes[i][j]->period_f != NULL)
			{
				fprintf(f," Forward  \n");
				affiche_periode(g.routes[i][j]->period_f,p,f);
				fprintf(f,"Backward \n");
				affiche_periode(g.routes[i][j]->period_b,p,f);
			}
		}
		fprintf(f,"\n");
	}
	fprintf(f,"Fin\n");
	return;
}

void affiche_graph_routes(Graph g, FILE * f)
{
	for(int i=0;i<g.nb_routes; i++)
	{
		fprintf(f,"Route %d total length = %d : [ ",i,route_length( g,i));
		for(int j=0 ; j< g.size_routes[i];j++)
		{
			fprintf(f,"%d, ",g.routes[i][j]->length);
			
		}
		fprintf(f,"]\n");
	}
	return;
}
void affiche_period_star(Graph g,int p, FILE * f)
{

	fprintf(f," Forward  \n");
	affiche_periode(g.arc_pool[g.nb_routes].period_f,p,f);
	fprintf(f,"Backward \n");
	affiche_periode(g.arc_pool[g.nb_routes].period_b,p,f);
			

	return;
}
void affiche_biparti(int ** g, int a, int b, FILE * f)
{
	for(int i=0;i<a;i++)
	{
		for(int j=0;j<b;j++)
		{
			fprintf(f,"(%d)",g[i][j]);
		}
		fprintf(f,"\n");
	}
}
void affiche_assignment(Assignment a,int nb_routes, FILE * f)
{

	for( int i=0;i<nb_routes;i++)
	{
		fprintf(f,"Route %d : f :%d- b :%d / W : %d\n",i,a->offset_forward[i],a->offset_backward[i],a->waiting_time[i]);
	}
}