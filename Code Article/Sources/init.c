
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>


#include "struct.h"
#include "tests.h"

//Déclare une matrice remplie de 0 de taille N*N
int ** declare_matrice(int N)
{
	int ** matrice;
	assert(matrice = malloc(sizeof(int*)*N));
	for(int i = 0;i<N;i++)
	{
		assert(matrice[i]= calloc(N,sizeof(int)));
	}
	return matrice;
}


Graphe init_graphe(int N)
{
	Graphe g;
	g.N = N;
	g.matrice = declare_matrice(N);
	return g;
}
void libere_matrice(Graphe g)
{
	for(int i=0;i<g.N;i++)
	{
		free(g.matrice[i]);
	}
	free(g.matrice);
}

//prends un graphe vide en argument et le transforme en etoile
void graphe_etoile(Graphe g,int taille_liens)
{
	if(!(g.N%2)){printf("Impossible de générer une étoile avec un nombre pair de sommets\n");exit(5);}
	int pivot = g.N/2;
	int alea;

	for(int i=0;i<g.N;i++)
	{
		alea = rand()%taille_liens;

		g.matrice[pivot][i] = alea;
		g.matrice[i][pivot] = alea;
	}
	g.matrice[pivot][pivot] = 0;
}
//prends un graphe vide en argument et le transforme en etoile Dans laquelle les arcs Ct-ti sont nuls
void graphe_etoile_opti(Graphe g,int taille_liens)
{
	if(!(g.N%2)){printf("Impossible de générer une étoile avec un nombre pair de sommets\n");exit(5);}
	int pivot = g.N/2;
	int alea;
	for(int i=0;i<pivot;i++)
	{
		alea = rand()%taille_liens;
		g.matrice[pivot][i] = alea;
		g.matrice[i][pivot] = alea;
	}
	g.matrice[pivot][pivot] = 0;
}

//prends un graphe vide en argument et le transforme en etoile dure a resoudre
void graphe_etoile_dur(Graphe g,int taille_liens,int diff)
{
	if(!(g.N%2)){printf("Impossible de générer une étoile avec un nombre pair de sommets\n");exit(5);}
	int pivot = g.N/2;
	int alea;

	for(int i=0;i<pivot/2;i++)
	{
		alea = (taille_liens/2)-rand()%(diff);
		g.matrice[pivot][i] = alea;
		g.matrice[i][pivot] = alea;
		alea = (taille_liens/2)-rand()%(diff);
		g.matrice[pivot][1+pivot+i] = alea;
		g.matrice[1+pivot+i][pivot] = alea;
	}
	for(int i=pivot/2;i<pivot;i++)
	{
		alea = taille_liens-rand()%(diff);
		g.matrice[pivot][i] = alea;
		g.matrice[i][pivot] = alea;
		alea = taille_liens-rand()%(diff);
		g.matrice[pivot][1+pivot+i] = alea;
		g.matrice[1+pivot+i][pivot] = alea;
	}
	g.matrice[pivot][pivot] = 0;
}


