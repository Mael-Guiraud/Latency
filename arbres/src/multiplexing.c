#include "structs.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
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

Event * ajoute_event_trie(Event * l,Event_kind kind, int date,int route,int arc_id,int deadline,Period_kind kind_p)
{
	Event * new = (Event *)malloc(sizeof(Event));
	new->kind = kind;
	new->date = date;
	new->route = route;
	new->arc_id = arc_id;
	new->deadline = deadline;
	new->kind_p = kind_p;
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


Elem * ajoute_elem_fifo(Elem * l,int numero_route,int arc_id,int arrival_in_queue,int deadline,Period_kind kind_p)
{
	Elem * new = (Elem *)malloc(sizeof(Elem));
	new->numero_route = numero_route;
	new->arrival_in_queue = arrival_in_queue;
	new->deadline = deadline;
	new->kind_p = kind_p;
	new->arc_id = arc_id;
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
	Elem * debut = l;
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


Elem * ajoute_elem_deadline(Elem * l,int numero_route,int arc_id,int arrival_in_queue,int deadline,Period_kind kind_p)
{
	Elem * new = (Elem *)malloc(sizeof(Elem));
	new->numero_route = numero_route;
	new->arrival_in_queue = arrival_in_queue;
	new->deadline = deadline;
	new->kind_p = kind_p;
	new->arc_id = arc_id;
	if(est_vide(l))//la liste est vide
	{
		new->suiv = NULL;
		return new;
	}

	if(l->deadline < deadline)//insertion au debut
	{
		new->suiv = l;
		return new;
	}
	Elem * debut = l;
	//on sarrête soit quand on est au bout, soit quand on doit ajouter l'element
	while(l->suiv)
	{
		if(l->suiv->deadline < deadline)//insertion au milieu
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

Event * init_events(Graph g, Event * liste_evt,int period, int nb_periods)
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
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,date,i,0,0,FORWARD);
		}
	}
	return liste_evt;
}

void update_time_elapsed(Graph g, Event* liste_evt,int * p_time)
{
	if(*p_time < liste_evt->deadline+g.routes[liste_evt->route][liste_evt->arc_id]->length)
		*p_time = liste_evt->deadline+g.routes[liste_evt->route][liste_evt->arc_id]->length;
	return;
}
Event * message_on_arc_free_fct(Graph g, Event * liste_evt,int message_size,int * p_time)
{
	int current_route_size;
	g.routes[liste_evt->route][liste_evt->arc_id]->state = 1;
	liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,NONE);
	current_route_size = g.size_routes[liste_evt->route];
	if(liste_evt->kind_p == FORWARD)
	{
		if(liste_evt->arc_id != current_route_size-1) // not the last arc
		{
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g.routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->route,liste_evt->arc_id+1,liste_evt->deadline + g.routes[liste_evt->route][liste_evt->arc_id]->length,FORWARD);
		}
		else
		{
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g.routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->route,liste_evt->arc_id,liste_evt->deadline + g.routes[liste_evt->route][liste_evt->arc_id]->length,BACKWARD);
		}
	}
	else //backward
	{
		if(liste_evt->kind_p == BACKWARD)
		{
			if(liste_evt->arc_id == 0)
			{
				update_time_elapsed(g,liste_evt,p_time);
			}
			else
			{
				liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g.routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->route,liste_evt->arc_id -1,liste_evt->deadline+ g.routes[liste_evt->route][liste_evt->arc_id]->length, BACKWARD);
			}
		}
		else
		{
			printf("ERROR, an event is a message and has no kind_p (multiplexing.c)\n");exit(87);
		}
	}
	return liste_evt;
}

Event * arc_free_fct(Graph g, Event * liste_evt,int message_size, int * p_time)
{
	int current_route_size;
	Elem * first_elem = g.routes[liste_evt->route][liste_evt->arc_id]->elems;
	if(first_elem == NULL)
	{
		printf("ERROR, THIS SHOULD NOT HAPPEND, function arc_free_fct is called only if first elem is not null, multiplexing.c\n");exit(46);
	}
	Arc * current_arc = g.routes[liste_evt->route][liste_evt->arc_id];
	g.routes[liste_evt->route][liste_evt->arc_id]->state = 1;
	liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,NONE);
	current_route_size = g.size_routes[liste_evt->route];
	if(first_elem->kind_p == FORWARD)
	{
		if(liste_evt->arc_id != current_route_size-1) // not the last arc
		{
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g.routes[first_elem->numero_route][first_elem->arc_id]->length,first_elem->numero_route,first_elem->arc_id+1,first_elem->deadline + g.routes[first_elem->numero_route][first_elem->arc_id]->length,FORWARD);
		}
		else
		{
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g.routes[first_elem->numero_route][first_elem->arc_id]->length,first_elem->numero_route,first_elem->arc_id,first_elem->deadline + g.routes[first_elem->numero_route][first_elem->arc_id]->length,BACKWARD);
		}
	}
	else //backward
	{
		if(first_elem->kind_p == BACKWARD)
		{
			if(liste_evt->arc_id == 0)
			{
				update_time_elapsed(g,liste_evt,p_time);
			}
			else
			{
				liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g.routes[first_elem->numero_route][first_elem->arc_id]->length,first_elem->numero_route,first_elem->arc_id-1,first_elem->deadline + g.routes[first_elem->numero_route][first_elem->arc_id]->length,BACKWARD);
			}
		}
		else
		{
			printf("ERROR, an event is a message and has no kind_p (multiplexing.c)\n");exit(87);
		}
	}

	current_arc->elems = first_elem->suiv;
	free(first_elem);
	return liste_evt;
}
int multiplexing(Graph g, int period, int message_size, int nb_periods,Policy pol)
{
	Event * current;
	Event * liste_evt = NULL;
	liste_evt = init_events(g,liste_evt,period,nb_periods);
	init_arcs_state(g);
	int longest_time_elapsed = 0;
	while(liste_evt)
	{
		current = liste_evt;
		if(liste_evt->kind == MESSAGE)
		{
			if(g.routes[liste_evt->route][liste_evt->arc_id]->state) // arc used
			{
				if(pol == 0)
				{
					g.routes[liste_evt->route][liste_evt->arc_id]->elems = ajoute_elem_fifo(g.routes[liste_evt->route][liste_evt->arc_id]->elems,liste_evt->route,liste_evt->arc_id,liste_evt->date,liste_evt->deadline,liste_evt->kind_p);
				}
				else
				{
					g.routes[liste_evt->route][liste_evt->arc_id]->elems = ajoute_elem_deadline(g.routes[liste_evt->route][liste_evt->arc_id]->elems,liste_evt->route,liste_evt->arc_id,liste_evt->date,liste_evt->deadline,liste_evt->kind_p);
				}
			}
			else // arc free
			{
				liste_evt = message_on_arc_free_fct(g,liste_evt,message_size,&longest_time_elapsed);
			}
		}
		else // arc
		{
			current = liste_evt;
			if(g.routes[liste_evt->route][liste_evt->arc_id]->elems ) // if there is some messages to manage
			{
				liste_evt = arc_free_fct(g,liste_evt,message_size,&longest_time_elapsed);
			}
			else
			{
				g.routes[liste_evt->route][liste_evt->arc_id]->state = 0;
			}

		}
		if(liste_evt != current){
			printf("Error, this is not possible (multiplexing.c)\n");exit(73);
		}
		liste_evt = liste_evt->suiv;
		free(current);
	
	}

	return longest_time_elapsed;
}