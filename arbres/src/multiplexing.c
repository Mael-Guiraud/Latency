#include "structs.h"
#include "config.h"
#include <stdlib.h>
void init_arcs_state(Graph g)
{
	for(int i=0;i<g.arc_pool_size;i++)
	{
		g.arc_pool[i].elems = NULL;
		g.arc_pool[i].state = 0;

	}
}

int est_vide(void * l)
{
	if(l == NULL)
		return 1;
	return 0;
}

Event * ajoute_event_trie(Event * l,Event_kind kind, int date,int route,int arc_id,int deadline)
{
	Event * new = (Event *)malloc(sizeof(Event));
	new->kind = kind;
	new->date = date;
	new->route = route;
	new->arc_id = arc_id;
	new->deadline = deadline;
	if(est_vide(l))//la liste est vide
	{
		new->suiv = NULL;
		return new;
	}

	if(l->date > date)//insertion au debut
	{
		new->suiv = l;
		return new;
	}
	Event * debut = l;
	//on sarrête soit quand on est au bout, soit quand on doit ajouter l'element
	while(l->suiv)
	{
		if(l->suiv->date > date)//insertion au milieu
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


Elem * ajoute_elem_fifo(Elem * l,int numero_route,int arrival_in_queue,int deadline)
{
	Event * new = (Event *)malloc(sizeof(Event));
	new->numero_route = numero_route;
	new->arrival_in_queue = arrival_in_queue;
	new->deadline = deadline;
	if(est_vide(l))//la liste est vide
	{
		new->suiv = NULL;
		return new;
	}

	if(l->arrival_in_queue > arrival_in_queue)//insertion au debut
	{
		new->suiv = l;
		return new;
	}
	Event * debut = l;
	//on sarrête soit quand on est au bout, soit quand on doit ajouter l'element
	while(l->suiv)
	{
		if(l->suiv->arrival_in_queue > arrival_in_queue)//insertion au milieu
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


Elem * ajoute_elem_deadline(Elem * l,int numero_route,int arrival_in_queue,int deadline)
{
	Event * new = (Event *)malloc(sizeof(Event));
	new->numero_route = numero_route;
	new->arrival_in_queue = arrival_in_queue;
	new->deadline = deadline;
	if(est_vide(l))//la liste est vide
	{
		new->suiv = NULL;
		return new;
	}

	if(l->deadline > deadline)//insertion au debut
	{
		new->suiv = l;
		return new;
	}
	Event * debut = l;
	//on sarrête soit quand on est au bout, soit quand on doit ajouter l'element
	while(l->suiv)
	{
		if(l->suiv->deadline > deadline)//insertion au milieu
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

void init_events(Graph g, Event * liste_evt,int period, int nb_periods)
{
	int date;
	for(int i=0;i<g.nb_routes;i++)
	{
		if(SYNCH)
		{
			date =0;
		}
		else
		{
			date = rand()%period;
		}
		for(int j=0;j<nb_periods;j++)
		{
			ajoute_event_trie(liste_evt,MESSAGE,date,i,0,tmax);
		}
	}
}


int multiplexing(Graph g, int period, int message_size, int nb_periods)
{
	Event * liste_evt = NULL;
	init_events(g,liste_evt,period,nb_periods);
}