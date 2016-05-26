// TOUTES LES PETITES FONCTIONS DE CALCULS DIVERSES
#include "graph.h"

//renvoie un entier aléatoire entre 0 et n
long int rand_entier(int n){
	
	return random()/(double) RAND_MAX *(n+1);
}
//renvoie la distance du début de la route jusqu'au sommet numero v
int distance(Route r, int v)
{
	if(v==-1)
		return -1;
	int i,j;
	int sum=0;
	for(i=v;i>0;i--)
	{
		for(j=0;j<r.vertices[i].edge_number;j++)
		{
			if(r.vertices[i-1].index==r.vertices[i].neighboors[j])
			{
				sum += r.vertices[i].weight[j];
			}
		}
	}
return sum;
}

//Retourne le numero du sommet sur la route, -1 si le sommet n'est pas sur la route
int vertex_index(Route r, Vertex v)
{
	int i;
	for(i=0;i<r.route_lenght;i++)
	{
		if(r.vertices[i].index == v.index)
			return i;
	}
	return -1;
}

//Retourne la route la plus longue dans une liste de route
int longest(Route * r,int taille)
{
	int i;
	int max = distance(r[0],r[0].route_lenght);
	int indice_max = 0;
	for(i=1;i<taille;i++)
	{
		if(distance(r[i],r[i].route_lenght) > max)
		{
			max = distance(r[i],r[i].route_lenght);
			indice_max = i;
		}
		
	}
	return indice_max;
}
//Retourne le max tableau d'int
int greater(int * tab,int taille)
{
	int i;
	int max = tab[0];
	int indice_max = 0;
	for(i=1;i<taille;i++)
	{
		if(tab[i] > max)
		{
			max = tab[i];
			indice_max = i;
		}
		
	}
	return indice_max;
}
//Retourne le min d'un tableau d'int
int lower(int * tab,int taille)
{
	int i;
	int min = tab[0];
	int indice_min = 0;
	for(i=1;i<taille;i++)
	{
		if(tab[i] < min)
		{
			min = tab[i];
			indice_min = i;
		}
		
	}
	return indice_min;
}
//test si un tableau de bool est rempli de 1 (retourne 1 si oui, 0 sinon)
int full(int * tab, int taille)
{
	int i;
	int ack=1;
	for(i=0;i<taille;i++)
	{
		if(tab[i] == 0)
			ack = 0;
	}
	return ack;
}
void affichetab(int* tab, int taille)
{
	printf("TABLEAU de taille %d\n",taille);
	int i;
	for(i=0;i<taille;i++)
	{
		printf("%d ",tab[i]);
	}
	printf("\n");
}
//additionne les valeurs de deux tableaux de meme taille
void ajoutetab(int * t1, int * t2, int taille)
{
	int i;
	for(i=0;i<taille;i++)
	{
		t1[i]+=t2[i];
	}
}
int tMax(Graphe g,TwoWayTrip t)
{
	int i;
	int max = 2*distance(g.routes[0],g.routes[0].route_lenght)+t.W[0];
	for(i=0;i<t.taille;i++)
	{
		if(2*distance(g.routes[i],g.routes[i].route_lenght)+t.W[i] > max)
			max = 2*distance(g.routes[i],g.routes[i].route_lenght)+t.W[i];
	}
	return max;
}

//retourne la route la plus longue parmis celles qui peuvent partir a un offset donnée
int longerOfeEligible(int * Dl, int * lambda, int * arrivee,int offset, int taille)
{
	int i;
	int max;
	int indice_max;

	//boucle pour trouver la premire valeure eligible
	for(i=0;i<taille;i++)
	{
		if(arrivee[i]+lambda[i] <= offset)
		{
			if(Dl[i] !=0)
			{
				max = Dl[i];
				indice_max = i;
				break;
			}
		}
		max = -1;
		indice_max = -1;
	}

	for(i=0;i<taille;i++)
	{
		if(arrivee[i]+lambda[i] < offset)
		{
			if(Dl[i]>max)
			{
				max = Dl[i];
				indice_max=i;
			}
		}
	}
	return indice_max;
}
