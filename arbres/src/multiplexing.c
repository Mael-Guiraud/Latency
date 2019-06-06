#include "structs.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

FILE * logs;
void init_arcs_state(Graph g)
{
	for(int i=0;i<g.arc_pool_size;i++)
	{
		g.arc_pool[i].elems_f = NULL;
		g.arc_pool[i].elems_b = NULL;
		g.arc_pool[i].state_f = 0;
		g.arc_pool[i].state_b = 0;

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
	
	fprintf(logs,"new event arc at date %d",liste_evt->date+message_size);

	current_route_size = g.size_routes[liste_evt->route];
	if(liste_evt->kind_p == FORWARD)
	{
		g.routes[liste_evt->route][liste_evt->arc_id]->state_f = 1;
		liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,FORWARD);
		fprintf(logs,"Way forward (arc %d, length %d), new event message at date %d\n",liste_evt->arc_id,g.routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->date+g.routes[liste_evt->route][liste_evt->arc_id]->length);
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
		g.routes[liste_evt->route][liste_evt->arc_id]->state_b = 1;
		liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,BACKWARD);
		if(liste_evt->kind_p == BACKWARD)
		{
			fprintf(logs,"Way backward (arc %d, length %d)",liste_evt->arc_id,g.routes[liste_evt->route][liste_evt->arc_id]->length);
			if(liste_evt->arc_id == 0)
			{
				fprintf(logs,"end of the message, update \n");
				update_time_elapsed(g,liste_evt,p_time);
			}
			else
			{
				fprintf(logs,"new event message at date %d\n",liste_evt->date+g.routes[liste_evt->route][liste_evt->arc_id]->length);
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
	int time_waited;
	Elem * first_elem;
	Arc * current_arc;
	
	current_arc = g.routes[liste_evt->route][liste_evt->arc_id];
	current_route_size = g.size_routes[liste_evt->route];
	
	if(liste_evt->kind_p == FORWARD)
	{
		first_elem = g.routes[liste_evt->route][liste_evt->arc_id]->elems_f;
		if(first_elem == NULL)
		{
			printf("ERROR, THIS SHOULD NOT HAPPEND, function arc_free_fct is called only if first elem is not null, multiplexing.c\n");exit(46);
		}
		time_waited = liste_evt->date - first_elem->arrival_in_queue;
		fprintf(logs,"The elem at the top of the list (route %d arc %d) has waited %d slots (arrival %d, date %d).",first_elem->numero_route,first_elem->arc_id,time_waited,first_elem->arrival_in_queue,liste_evt->date);
	
		fprintf(logs,"New arc event at date %d\n",liste_evt->date+message_size);
		g.routes[liste_evt->route][liste_evt->arc_id]->state_f = 1;
		liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,FORWARD);
		fprintf(logs,"Way forward, new event message (arc %d length %d) at date %d \n",first_elem->arc_id,g.routes[first_elem->numero_route][first_elem->arc_id]->length,liste_evt->date);
		if(liste_evt->arc_id != current_route_size-1) // not the last arc
		{
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g.routes[first_elem->numero_route][first_elem->arc_id]->length,first_elem->numero_route,first_elem->arc_id+1,first_elem->deadline + g.routes[first_elem->numero_route][first_elem->arc_id]->length+time_waited,FORWARD);
		}
		else
		{
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g.routes[first_elem->numero_route][first_elem->arc_id]->length,first_elem->numero_route,first_elem->arc_id,first_elem->deadline + g.routes[first_elem->numero_route][first_elem->arc_id]->length+time_waited,BACKWARD);
		}
		current_arc->elems_f = first_elem->suiv;
	}
	else //backward
	{
		if(liste_evt->kind_p == BACKWARD)
		{
			first_elem = g.routes[liste_evt->route][liste_evt->arc_id]->elems_b;
			if(first_elem == NULL)
			{
				printf("ERROR, THIS SHOULD NOT HAPPEND, function arc_free_fct is called only if first elem is not null, multiplexing.c\n");exit(46);
			}
			time_waited = liste_evt->date - first_elem->arrival_in_queue;
			fprintf(logs,"The elem at the top of the list (route %d arc %d) has waited %d slots (arrival %d, date %d).",first_elem->numero_route,first_elem->arc_id,time_waited,first_elem->arrival_in_queue,liste_evt->date);
			
			fprintf(logs,"New arc event at date %d\n",liste_evt->date+message_size);
			g.routes[liste_evt->route][liste_evt->arc_id]->state_b = 1;
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,BACKWARD);
			fprintf(logs,"way backward (arc %d length %d) ",first_elem->arc_id,g.routes[first_elem->numero_route][first_elem->arc_id]->length);
			if(liste_evt->arc_id == 0)
			{
				fprintf(logs,"time updated\n");
				update_time_elapsed(g,liste_evt,p_time);
			}
			else
			{
				fprintf(logs,"new event message at date %d .\n",liste_evt->date+g.routes[first_elem->numero_route][first_elem->arc_id]->length);
				liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g.routes[first_elem->numero_route][first_elem->arc_id]->length,first_elem->numero_route,first_elem->arc_id-1,first_elem->deadline + g.routes[first_elem->numero_route][first_elem->arc_id]->length+time_waited,BACKWARD);
			}
			current_arc->elems_b = first_elem->suiv;
			
		}
		else
		{
			printf("ERROR, an event is a message and has no kind_p (multiplexing.c)\n");exit(87);
		}
	}
	free(first_elem);
	
	return liste_evt;
}
int multiplexing(Graph g, int period, int message_size, int nb_periods,Policy pol)
{
	logs = fopen("logs_multiplexing.txt","w");
	if(!logs){perror("Error opening multiplexing.txt");}
	Event * current;
	Event * liste_evt = NULL;
	liste_evt = init_events(g,liste_evt,period,nb_periods);
	init_arcs_state(g);
	int longest_time_elapsed = 0;
	while(liste_evt)
	{
		current = liste_evt;
		fprintf(logs,"\n Date %d: ",liste_evt->date);
		if(liste_evt->kind == MESSAGE)
		{
			fprintf(logs,"A message on the route %d has arrived.\n",liste_evt->route);
			
			if(((liste_evt->kind_p == FORWARD) && g.routes[liste_evt->route][liste_evt->arc_id]->state_f) || ((liste_evt->kind_p == BACKWARD) && g.routes[liste_evt->route][liste_evt->arc_id]->state_b)) // arc used
			{
				fprintf(logs,"The arc is used (pol = %d).\n",pol);
				fprintf(logs,"Adding elem (%d %d %d %d %d) - ",liste_evt->route,liste_evt->arc_id,liste_evt->date,liste_evt->deadline,liste_evt->kind_p);
				if(pol == FIFO)
				{
					if(liste_evt->kind_p == FORWARD)
						g.routes[liste_evt->route][liste_evt->arc_id]->elems_f = ajoute_elem_fifo(g.routes[liste_evt->route][liste_evt->arc_id]->elems_f,liste_evt->route,liste_evt->arc_id,liste_evt->date,liste_evt->deadline,liste_evt->kind_p);
					else
						g.routes[liste_evt->route][liste_evt->arc_id]->elems_b = ajoute_elem_fifo(g.routes[liste_evt->route][liste_evt->arc_id]->elems_b,liste_evt->route,liste_evt->arc_id,liste_evt->date,liste_evt->deadline,liste_evt->kind_p);

				}
				else
				{
					if(liste_evt->kind_p == FORWARD)
						g.routes[liste_evt->route][liste_evt->arc_id]->elems_f = ajoute_elem_deadline(g.routes[liste_evt->route][liste_evt->arc_id]->elems_f,liste_evt->route,liste_evt->arc_id,liste_evt->date,liste_evt->deadline,liste_evt->kind_p);
					else
						g.routes[liste_evt->route][liste_evt->arc_id]->elems_b = ajoute_elem_deadline(g.routes[liste_evt->route][liste_evt->arc_id]->elems_b,liste_evt->route,liste_evt->arc_id,liste_evt->date,liste_evt->deadline,liste_evt->kind_p);
				}
				
			}
			else // arc free
			{
				fprintf(logs,"The arc is free.");
				liste_evt = message_on_arc_free_fct(g,liste_evt,message_size,&longest_time_elapsed);
			}
		}
		else // arc
		{
			fprintf(logs,"An arc is available .\n");
			current = liste_evt;
			if( ( (liste_evt->kind_p == FORWARD) && g.routes[liste_evt->route][liste_evt->arc_id]->elems_f) || ( (liste_evt->kind_p == BACKWARD) && g.routes[liste_evt->route][liste_evt->arc_id]->elems_b) ) // if there is some messages to manage
			{
				fprintf(logs,"there is some messages in queue : \n");
				liste_evt = arc_free_fct(g,liste_evt,message_size,&longest_time_elapsed);
			}
			else
			{
				fprintf(logs,"arc no messages in queue, we set the arc to available");
				if(liste_evt->kind_p == FORWARD)
					g.routes[liste_evt->route][liste_evt->arc_id]->state_f = 0;
				else
					g.routes[liste_evt->route][liste_evt->arc_id]->state_b = 0;
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