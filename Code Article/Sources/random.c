#include <stdio.h>
#include <stdlib.h>

#include "struct.h"
#include "operations.h"
#include "tests.h"

struct elem{
	int numero_route;
	int depart;
	int arrivee;
	struct elem *suiv;
};

typedef struct elem* liste;

typedef struct{
	liste pile;
	int taille;
}Pile;

int est_vide(liste l)
{
	if(l == NULL)
		return 1;
	return 0;
}


liste ajoute_elem_trie(liste l,int depart,int arrivee, int numero_route)
{
	liste new = malloc(sizeof(struct elem));
	new->numero_route = numero_route;
	new->depart = depart;
	new->arrivee = arrivee;
	if(est_vide(l))//la liste est vide
	{
		new->suiv = NULL;
		return new;
	}

	if(l->arrivee > arrivee)//insertion au debut
	{
		new->suiv = l;
		return new;
	}
	liste debut = l;
	//on sarrête soit quand on est au bout, soit quand on doit ajouter l'element
	while(l->suiv)
	{
		if(l->suiv->arrivee > arrivee)//insertion au milieu
		{
			new->suiv = l->suiv;
			l->suiv = new;
			return debut;
		}
		l = l->suiv;
	}
	//Si on arrive la, c'est qu'on est à la fin.
	l->suiv = new;
	new->suiv = NULL;
	return debut;
}
//supprime l'element d'indice i
liste supprime_elem(liste l, int i)
{
	if(est_vide(l))return l;
	//Premier element
	liste tmp;
	if(i == 0)
	{
		tmp = l->suiv;
		free(l);
		return tmp;
	}
	liste debut = l;
	int compteur = 0;
	while(l->suiv->suiv)
	{
		if(compteur == i-1)
		{
			tmp = l->suiv;
			l->suiv = l->suiv->suiv;
			free(tmp);
			return debut;
		}
		compteur++;
		l = l->suiv;
	}
	//cas du dernier element
	if(compteur == i-1)
	{
		tmp = l->suiv;
		l->suiv = NULL;
		free(tmp);
	}
	return debut;
	
}

liste libere_liste(liste l)
{
	liste tmp;
	while(l)
	{
		tmp = l->suiv;
		//printf("libération de %d\n",l->val);
		free(l);
		l = tmp;
	}
	return NULL;
}


void affiche_liste(liste l)
{
	
	if(est_vide(l))
	{
		printf("La liste est vide \n");
		return;
	}
	while(l)
	{
		printf("(%d,%d,%d) ",l->numero_route,l->depart,l->arrivee);
		l = l->suiv;
	}
	printf("\n");
}

int stochastic(Graphe g, int taille_paquets, int periode, int nombre_periodes, int mode)
{
	int nb_routes = g.N/2;

	//tableau qui contiens le tick de départs des messages dans les sources
	int departs[nb_routes];
	for(int i=0;i<nb_routes;i++)departs[i]=rand()%periode;
	

	Pile forward;
	forward.pile = NULL;
	forward.taille=0;


	//Envoie les messages pour cs
	for(int i=0;i<nombre_periodes;i++)
	{
		for(int j=0;j<nb_routes;j++)
		{

			forward.pile = ajoute_elem_trie(forward.pile,departs[j]+i*periode,i*periode+departs[j]+g.matrice[nb_routes][j] ,j);

			forward.taille++;
		}
	}


	Pile backward;
	backward.pile = NULL;
	backward.taille=0;

	int t=0;//offset dans la forward

	// a partir de la liste, envoie les messages dans la backward periode
	for(int i=0;i<forward.taille;i++)
	{
		t = max(t,forward.pile->arrivee);
		backward.pile = ajoute_elem_trie(backward.pile,forward.pile->depart,t+2*g.matrice[nb_routes][1+forward.pile->numero_route+nb_routes] ,forward.pile->numero_route);
		t+= taille_paquets;
		backward.taille++;
		forward.pile = supprime_elem(forward.pile,0);
	}

	int temps_trajet;
	int tmax = 0;
	long long int moyenne=0;
	t=0;
	//On depile les elements et on cherche le max dans les tmax
	for(int i=0;i<backward.taille;i++)
	{
		t = max(t,backward.pile->arrivee);
		temps_trajet= t+g.matrice[nb_routes][backward.pile->numero_route]-backward.pile->depart;
		tmax = max(tmax,temps_trajet);
		moyenne += temps_trajet;
		t+= taille_paquets;
		backward.pile = supprime_elem(backward.pile,0);
	}

	if(mode)
		return tmax;
	return (int)moyenne/(nb_routes*nombre_periodes);
}
