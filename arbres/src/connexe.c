
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef struct { //graphe représenté par une matrice d'adjacence
  	int nombre_sommet;
  	int ** matrice_adjacence;
} graphe;

float rand01()
{
	return (float)rand() / (float)RAND_MAX;
}

graphe init_graphe(int n){//créé un graphe dont tous les sommets sont isolés
	graphe G; 
	G.nombre_sommet = n;
	G.matrice_adjacence = malloc(sizeof(int*)*n);
	for(int i=0;i<G.nombre_sommet;i++)
	{
		G.matrice_adjacence[i] = calloc(n,sizeof(int));
	}
	return G;
}

void libere_graphe(graphe G){
	for(int i=0;i<G.nombre_sommet;i++)
	{
		free(G.matrice_adjacence[i]);
		
	}

	free(G.matrice_adjacence);
}




graphe graphe_arbre(int *pere, int n){//créer un arbre couvrant à partir du tableau des pères représenté comme un graphe
	graphe G=init_graphe(n);

	for(int i=0;i<G.nombre_sommet;i++)
	{
		if(pere[i] != i)
		{
			G.matrice_adjacence[i][pere[i]] = 1;
			G.matrice_adjacence[pere[i]][i] = 1;
		}
	}

	return G;
}

void parcours_sommet(graphe G, int s, int *couleur, int *pere){
	couleur[s] = 1;
	for(int i=0;i<G.nombre_sommet;i++)//Pour tout les sommets 
	{
		if(G.matrice_adjacence[s][i])//Si l'arrete existe (c'est mon voisin)
		{
			if(couleur[i] == 0)//Si le sommet n'a jamais été visité
			{
				pere[i] = s;
				
				parcours_sommet(G,i,couleur,pere);
				
			}
		}
	}
	couleur[s]=2; //on remonte

	
}

int parcours_graphe(graphe G){
	int *couleur = calloc(G.nombre_sommet,sizeof(int)); // 0 est blanc, 1 gris et 2 noir
	int *pere = malloc(sizeof(int)*G.nombre_sommet);
	int composante = 0;

	for(int i=0;i<G.nombre_sommet;i++){ //init
		couleur[i]=0;
		pere[i]=i;
	}

	for(int i=0;i<G.nombre_sommet;i++){
		if(couleur[i] == 0)
		{
			composante++;
			parcours_sommet(G,i,couleur,pere);
			
		}
	}


	free(couleur);
	free(pere);

	return composante;
}

int is_connexe(int ** tab, int nb_bbu, int nb_collisions)
{
	graphe G = init_graphe(nb_bbu+nb_collisions);

	//transforme le graphe biparti en un graph avec une matrice d'ajacence
	for(int i=0;i<nb_bbu;i++)
	{
		for(int j=0;j<nb_collisions;j++)
		{
			if(tab[i][j])
			{
				G.matrice_adjacence[i][nb_bbu+j] = 1;
				G.matrice_adjacence[nb_bbu+j][i] = 1;
			}
		}
	}
	printf("%d parcours\n",parcours_graphe(G));
	return (parcours_graphe(G)>1)?0:1;

}


