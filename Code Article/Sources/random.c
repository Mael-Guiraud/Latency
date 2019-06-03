/***
Copyright (c) 2018 Guiraud Maël
All rights reserved.
*///

#include <stdio.h>
#include <stdlib.h>

#include "struct.h"
#include "operations.h"
#include "tests.h"

struct elem{
	int numero_route;
	int depart;
	int arrivee;
	int deadline;
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


liste ajoute_elem_trie(liste l,int depart,int arrivee, int numero_route, int deadline)
{
	liste new = malloc(sizeof(struct elem));
	new->numero_route = numero_route;
	new->depart = depart;
	new->arrivee = arrivee;
	new->deadline = deadline;
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
//renvoie l'element d'indice i
liste search_elem(liste l, int i)
{
	if(est_vide(l))return l;
	//Premier element

	if(i == 0)
	{
		return l;
	}
	int compteur = 0;
	while(l->suiv->suiv)
	{
		if(compteur == i-1)
		{
			return l->suiv;

		}
		compteur++;
		l = l->suiv;
	}
	//cas du dernier element
	if(compteur == i-1)
	{
		return l->suiv;

	}
	return NULL;
	
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
//Cherche l'element qui a la plus petite deadline parmis tout les elements dont la date d'arrivee est < t
int search_eligible(liste l,int t)
{
	if(est_vide(l))return -1;
	//Premier element
	int id_min = 0;
	int i=0;
	int deadline_max = l->deadline;
	while(l->arrivee < t)
	{
		if(l->deadline > deadline_max)
		{
			deadline_max = l->deadline;
			id_min = i;
		}
		i++;
		l = l->suiv;
		if(!l) return id_min;
	}
	return id_min;
	
}
//moderes 0 moyenne 1 max
//modegestion 1 fifo 0 prior
int stochastic(Graphe g, int taille_paquets, int periode, int nombre_periodes, int mode_res,int mode_gestion)
{
	int nb_routes = g.N/2;

	//tableau qui contiens le tick de départs des messages dans les sources
	int departs[nb_routes];
	int deadline[nb_routes];
	if(!SYNCH)
		for(int i=0;i<nb_routes;i++)departs[i]=rand()%periode;
	else
		for(int i=0;i<nb_routes;i++)departs[i]=0;
	for(int i=0;i<nb_routes;i++)deadline[i]= 2* (g.matrice[nb_routes][i] + g.matrice[nb_routes][1+i+nb_routes]);
	

	Pile forward;
	forward.pile = NULL;
	forward.taille=0;
	int id;
	liste tmp = NULL;


	//Envoie les messages pour cs
	for(int i=0;i<nombre_periodes;i++)
	{
		for(int j=0;j<nb_routes;j++)
		{

			forward.pile = ajoute_elem_trie(forward.pile,departs[j]+i*periode,i*periode+departs[j]+g.matrice[nb_routes][j] ,j, deadline[j]-g.matrice[nb_routes][j]);

			forward.taille++;
		}
	}


	Pile backward;
	backward.pile = NULL;
	backward.taille=0;

	int t=0;//offset dans la forward

	// a partir de la liste, envoie les messages dans la backward periode
	if(mode_gestion)
	{
		for(int i=0;i<forward.taille;i++)
		{
			t = max(t,forward.pile->arrivee);
			backward.pile = ajoute_elem_trie(backward.pile,forward.pile->depart,t+2*g.matrice[nb_routes][1+forward.pile->numero_route+nb_routes] ,forward.pile->numero_route,forward.pile->deadline - 2*g.matrice[nb_routes][1+forward.pile->numero_route+nb_routes]);
			t+= taille_paquets;
			backward.taille++;
			forward.pile = supprime_elem(forward.pile,0);
		}
	}
	else
	{
		for(int i=0;i<forward.taille;i++)
		{
			if(t > forward.pile->arrivee)
			{
				id = search_eligible(forward.pile,t);
				tmp = search_elem(forward.pile,id);
			}
			else
			{
				id = 0;
				tmp = forward.pile;
				t = forward.pile->arrivee;
			}
			
			backward.pile = ajoute_elem_trie(backward.pile,tmp->depart,t+2*g.matrice[nb_routes][1+tmp->numero_route+nb_routes] ,tmp->numero_route,tmp->deadline - 2*g.matrice[nb_routes][1+tmp->numero_route+nb_routes]);
			t+= taille_paquets;
			backward.taille++;
			forward.pile = supprime_elem(forward.pile,id);
		}
	}

	int temps_trajet;
	int tmax = 0;
	long long int moyenne=0;
	t=0;
	//On depile les elements et on cherche le max dans les tmax
	if(mode_gestion)
	{
		for(int i=0;i<backward.taille;i++)
		{
			t = max(t,backward.pile->arrivee);
			temps_trajet= t+g.matrice[nb_routes][backward.pile->numero_route]-backward.pile->depart;
			tmax = max(tmax,temps_trajet);
			moyenne += temps_trajet;
			t+= taille_paquets;
			backward.pile = supprime_elem(backward.pile,0);
		}
	}
	else
	{
		for(int i=0;i<backward.taille;i++)
		{
			if(t > backward.pile->arrivee)
			{
				id = search_eligible(backward.pile,t);
				tmp = search_elem(backward.pile,id);
			}
			else
			{
				id = 0;
				tmp = backward.pile;
				t = backward.pile->arrivee;
			}
			temps_trajet= t+g.matrice[nb_routes][tmp->numero_route]-tmp->depart;
			tmax = max(tmax,temps_trajet);
			moyenne += temps_trajet;
			t+= taille_paquets;
			backward.pile = supprime_elem(backward.pile,id);
		}
	}

	if(mode_res)
		return tmax;
	return (int)moyenne/(nb_routes*nombre_periodes);
}
