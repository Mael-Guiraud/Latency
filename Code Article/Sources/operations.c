#include <stdio.h>
#include <stdlib.h>

#include "struct.h"
//trie le tableau "odre"
//Ordre contient les indices de routes, mais il est trié en fonction des tailles de "tab"
// De la plus longue a la plus courte

void tri_bulles(int* tab,int* ordre,int taille)
{
	int sorted;
	int tmp;
	int tmp_ordre;

	int tabcpy[taille];
	for(int i=0;i<taille;i++)tabcpy[i]=tab[i];

	for(int i=taille-1;i>=1;i--)
	{
		sorted = 1;
		for(int j = 0;j<=i-1;j++)
		{

			if(tabcpy[j+1]>tabcpy[j])
			{
				tmp_ordre = ordre[j+1];
				ordre[j+1]=ordre[j];
				ordre[j]=tmp_ordre;
				tmp = tabcpy[j+1];
				tabcpy[j+1]= tabcpy[j];
				tabcpy[j]= tmp;
				sorted = 0;
			}
		}
		if(sorted){return;}
	}

}

//Pareil mais trié de la plus courte a la plus longue
void tri_bulles_inverse(int* tab,int* ordre,int taille)
{
	int sorted;
	int tmp;
	int tmp_ordre;

	int tabcpy[taille];
	for(int i=0;i<taille;i++)tabcpy[i]=tab[i];

	for(int i=taille-1;i>=1;i--)
	{
		sorted = 1;
		for(int j = 0;j<=i-1;j++)
		{

			if(tabcpy[j+1]<tabcpy[j])
			{
				tmp_ordre = ordre[j+1];
				ordre[j+1]=ordre[j];
				ordre[j]=tmp_ordre;
				tmp = tabcpy[j+1];
				tabcpy[j+1]= tabcpy[j];
				tabcpy[j]= tmp;
				sorted = 0;
			}
		}
		if(sorted){return;}
	}

}



//Teste si une solution n'a pas de colisions
int is_ok(Graphe g, int taille_paquet, int * mi, int * wi,int p)
{
	int nbr_route = g.N /2;
	int a,b,c,d;
	//aller
	for(int i=1;i<nbr_route;i++)
	{
		a= (mi[i]+g.matrice[nbr_route][i])%p;
		b= (a+taille_paquet-1)%p;
		for(int j=i-1;j>=0;j--)
		{
			c= (mi[j]+g.matrice[nbr_route][j])%p;
			d= (c+taille_paquet-1)%p;
			if( ( (c>=a)&&(c<=b) ) || ( (d>=a)&&(d<=b) ) )
			{
				printf("PB(aller ) entre %d(%d-%d) et %d(%d-%d)\n",i,a,b,j,c,d);
				return 0;
			}
		}
	}

	//retour

	for(int i=0;i<nbr_route;i++)
	{
		a= (mi[i]+g.matrice[nbr_route][i]+2*g.matrice[nbr_route][nbr_route+1+i]+wi[i])%p;
		b= (a+taille_paquet-1)%p;
		for(int j=0;j<i;j++)
		{
			c= (mi[j]+g.matrice[nbr_route][j]+2*g.matrice[nbr_route][nbr_route+1+j]+wi[j])%p;
			d= (c+taille_paquet-1)%p;
			if( ( (c>=a)&&(c<=b) ) || ( (d>=a)&&(d<=b) ) )
			{
				printf("PB(retour ) entre %d(%d-%d) et %d(%d-%d)\n",i,a,b,j,c,d);
				return 0;
			}	
		}
	}
	return 1;
}
//Permute les elements d'un tableau
void fisher_yates(int * tab, int taille)
{
	int r;
	int tmp;
	for(int i=taille-1;i>0;i--)
	{
		r=rand()%(i+1);
		tmp = tab[i];
		tab[i]= tab[r];
		tab[r]=tmp;
	}
}



//Retourne l'indice du min d'un tableau d'int
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


//Retourne l'indice max tableau d'int
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


int max(int a, int b)
{
	if(a>b)
		return a;
	return b;
}

int min(int a,int b)
{
	if(a<b)
		return a;
	return b;
}



//Renvoie la taille de la route la plus longue
int longest_route(Graphe g)
{
	int nb_routes = g.N/2;
	int max = g.matrice[nb_routes][0]+g.matrice[nb_routes][nb_routes+1];
	for(int i=1;i<nb_routes;i++)
	{
		if( g.matrice[nb_routes][i]+g.matrice[nb_routes][nb_routes+1+i] > max)
			max = g.matrice[nb_routes][i]+g.matrice[nb_routes][nb_routes+1+i];
	}
	return 2*max;
}

