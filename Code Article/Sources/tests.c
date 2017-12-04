
#include <stdio.h>
#include <stdlib.h>

#include "struct.h"
#include "operations.h"



void affiche_solution(Graphe g, int taille_paquet, int * mi, int * wi)
{
	int nbr_route = g.N /2;
	int a,b;

	int aller[nbr_route];
	int retour[nbr_route];
	int ordrea[nbr_route];
	int ordrer[nbr_route];
	for(int i=0;i<nbr_route;i++)
	{
		ordrea[i]=i;
		ordrer[i]=i;
		aller[i]=mi[i]+g.matrice[nbr_route][i];
		retour[i]=mi[i]+g.matrice[nbr_route][i]+2*g.matrice[nbr_route][nbr_route+1+i]+wi[i];
	}
	tri_bulles_inverse(aller,ordrea,nbr_route);
	tri_bulles_inverse(retour,ordrer,nbr_route);
	//aller
	printf("\n----ALLER----\n");
	for(int i=0;i<nbr_route;i++)
	{
		a= aller[ordrea[i]];
		b= a+taille_paquet-1;
		printf("(%d[%d-%d]),",ordrea[i],a,b);
	}
	printf("\n------------\n");

	//retour
	printf("\n----Retour----\n");
	for(int i=0;i<nbr_route;i++)
	{
		a= retour[ordrer[i]];
		b= a+taille_paquet-1;
		printf("(%d[%d-%d]),",ordrer[i],a,b);
	}
	printf("\n------------\n\n\n");
}

void affiche_etoile(Graphe g)
{
	int nbr_route = g.N /2;
	printf("Graphe :\n");

	for( int i=0;i<nbr_route;i++)
		printf("route %d, taille %5d %5d - %5d \n",i,g.matrice[nbr_route][i]+g.matrice[nbr_route][i+nbr_route+1],g.matrice[nbr_route][i],g.matrice[nbr_route][i+nbr_route+1]);

	printf("\n");
}

//Affiche les intervales dans une periode (0 vide 1 plein)
void affiche_periode(int *p, int periode)
{
	int actuel = 0;
	for(int i=0;i<periode;i++)
	{
		if(p[i]!=actuel)
		{
			if(actuel)printf("-%d] ",i-1);
			else	  printf("[%d",i);
			actuel = 1-actuel;
		}
	}
	printf("\n");
}


void affiche_tab(int * tab, int taille)
{
	for(int i=0;i<taille;i++)
	{
		printf("%d ",tab[i]);
	}
	printf("\n");
}


void affiche_matrice(Graphe g)
{
	for(int i=0;i<g.N;i++)
	{
		for(int j=0;j<g.N;j++)
		{
			printf("%3d ",g.matrice[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}


