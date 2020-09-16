#include "structs.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
FILE * logs;
int computed_assignment;
#pragma omp threadprivate(computed_assignment)

void init_arcs_state(Graph * g)
{
	for(int i=0;i<g->arc_pool_size;i++)
	{
		g->arc_pool[i].elems_f = NULL;
		g->arc_pool[i].elems_b = NULL;
		g->arc_pool[i].state_f = 0;
		g->arc_pool[i].state_b = 0;

	}
}

int est_vide(void * l)
{
	if(l == NULL)
		return 1;
	return 0;
}

Event * ajoute_event_trie(Event * l,Event_kind kind, int date,int route,int arc_id,int time_elapsed,int deadline,Period_kind kind_p, int kind_message)
{
	Event * new = (Event *)malloc(sizeof(Event));
	new->kind = kind;
	new->kind_message = kind_message;
	new->date = date;
	new->route = route;
	new->arc_id = arc_id;
	new->deadline = deadline;
	new->time_elapsed = time_elapsed;
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


Elem * ajoute_elem_fifo(Elem * l,int numero_route,int arc_id,int arrival_in_queue,int time_elapsed,int deadline,Period_kind kind_p, int kind_message)
{
	Elem * new = (Elem *)malloc(sizeof(Elem));
	new->numero_route = numero_route;
	new->arrival_in_queue = arrival_in_queue;
	new->time_elapsed = time_elapsed;
	new->kind_p = kind_p;
	new->kind_message = kind_message;
	new->arc_id = arc_id;
	new->deadline = deadline;
	if(est_vide(l))//la liste est vide
	{
		new->suiv = NULL;
		return new;
	}

	if(l->arrival_in_queue < arrival_in_queue)//insertion au debut
	{
		new->suiv = l;
		return new;
	}
	Elem * debut = l;
	//on sarrête soit quand on est au bout, soit quand on doit ajouter l'element
	while(l->suiv)
	{
		if(l->suiv->arrival_in_queue < arrival_in_queue)//insertion au milieu
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


Elem * ajoute_elem_deadline(Elem * l,int numero_route,int arc_id,int arrival_in_queue,int time_elapsed,int deadline,Period_kind kind_p, int kind_message)
{
	Elem * new = (Elem *)malloc(sizeof(Elem));
	new->numero_route = numero_route;
	new->arrival_in_queue = arrival_in_queue;
	new->time_elapsed = time_elapsed;
	new->kind_p = kind_p;
	new->arc_id = arc_id;
	new->kind_message = kind_message;
	new->deadline = deadline;
	

	if(est_vide(l))//la liste est vide
	{
		new->suiv = NULL;
		return new;
	}
	/*if(kind_message == 0)
	{
		while(l->suiv)
		{
			l = l->suiv;
			//printf("%p \n",l->kind_message);
		}
		l->suiv = new;
		new->suiv = NULL;
	}*/
	if(l->deadline > deadline)//insertion au debut
	{
		new->suiv = l;
		return new;
	}
	Elem * debut = l;
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

Event * init_computed(Graph * g, Event * liste_evt,int period, int nb_periods)
{
	int tmax[g->nb_routes];
	int begin[g->nb_routes];
	
	int date;
	for(int i=0;i<g->nb_routes;i++)
	{
		for(int k = 0;k<g->size_routes[i];k++)
		{
			date = route_length_untill_arc(g,i,g->routes[i][k],FORWARD)+g->routes[i][k]->routes_delay_f[i];
			for(int j=0;j<nb_periods;j++)
			{
				liste_evt = ajoute_event_trie(liste_evt,MESSAGE,date+period*j,i,k,0,0,FORWARD,2);
			}
		//	printf("route %d arc %d message forward %d %d",i,k,date,g->routes[i][k]->routes_delay_f[i]);
			date = route_length_with_buffers_forward( g,i)+route_length_untill_arc(g,i,g->routes[i][k],BACKWARD)+g->routes[i][k]->routes_delay_b[i];

			for(int j=0;j<nb_periods;j++)
			{
				liste_evt = ajoute_event_trie(liste_evt,MESSAGE,date+period*j,i,k,0,0,BACKWARD,2);
			}
			//printf("message backward %d \n",date);
		}
			
		
	}
	return liste_evt;
}

Event * init_events(Graph * g, Event * liste_evt,int period, int nb_periods)
{
	int date;
	for(int i=0;i<g->nb_routes;i++)
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
		//	printf(" init event date %d , %d\n",date+period*j, MESSAGE);
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,date+period*j,i,0,0,INT_MAX,FORWARD,1);
		}
	
		

		
	}
	return liste_evt;
}
void read_param_file(float * tab, int size)
{
	FILE* file = fopen("beparameters","r");
	if(!file){perror("Opening \"beparameters\" failure\n");exit(2);}
	float sumn = 0.0;
	int trash;
	for(int i = 0;i<size;i++)
	{

		fscanf(file,"%d %f ",&trash, &tab[i]);
		sumn += tab[i];
	//	printf("%f \n",tab[i]);
	}
	//printf("%f somme\n",sumn);
}

int inverse_transform(float * tab, int size)
{
	float random = ((float)rand() / RAND_MAX );
	int id = 0;
	float sum = tab[0];
	while(random >= sum)
	{
		id++;
		sum += tab[id];
	}
	return id;
}


	
	
	
Event * init_BE(Graph * g, Event * liste_evt,int period, int nb_periods)
{

	float tab[20];
	read_param_file(tab, 20);


	int nb_paquets ;
	
	for(int i=0;i<period*nb_periods;i+=period)
	{
		//printf("i = %d \n",i);
		//nb_paquets = 2;
		nb_paquets = inverse_transform(tab,20);
		for(int j=0;j<nb_paquets;j++)
		{
			
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,i,j%g->nb_routes,0,0,INT_MAX,FORWARD,0);
		}
		nb_paquets = inverse_transform(tab,20);
		
		for(int j=0;j<nb_paquets;j++)
		{
			//printf("ajouter elem back\n");
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,i,j%g->nb_routes,0,0,INT_MAX,BACKWARD,0);
		}
		
		
	}
	return liste_evt;
}

void update_time_elapsed(int a,int * p_time)
{
	
	if(*p_time < a)
		*p_time = a;
	//printf(" new = %d \n",*p_time);
	return;
}
Event * message_on_arc_free_fct(Graph * g, Event * liste_evt,int message_size,int * p_time,int * be_time)
{
	int current_route_size;
	
	fprintf(logs,"new event arc at date %d",liste_evt->date+message_size);

	current_route_size = g->size_routes[liste_evt->route];
	if(liste_evt->kind_p == FORWARD)
	{
		
		g->routes[liste_evt->route][liste_evt->arc_id]->state_f = 1;
		if(liste_evt->kind_message)//Si c'est du CRAN
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,0,FORWARD,1);
		else
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+SIZE_BE_MESSAGE,liste_evt->route,liste_evt->arc_id,0,0,FORWARD,1);
		
		fprintf(logs,"Way forward (arc %d, length %d), new event message at date %d\n",liste_evt->arc_id,g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->date+g->routes[liste_evt->route][liste_evt->arc_id]->length);
		if(liste_evt->arc_id != current_route_size-1) // not the last arc
		{
			if(!computed_assignment || !liste_evt->kind_message )	
				liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->route,liste_evt->arc_id+1,liste_evt->time_elapsed + g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->deadline - g->routes[liste_evt->route][liste_evt->arc_id]->length,FORWARD,liste_evt->kind_message);

		}
		else
		{
			if(liste_evt->kind_message)//Si c'est du CRAN
			{
				if(!computed_assignment)
					liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->route,liste_evt->arc_id,liste_evt->time_elapsed + g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->deadline - g->routes[liste_evt->route][liste_evt->arc_id]->length,BACKWARD,1);
			}
			else
			{
				//printf("be time update in message 1\n");
				update_time_elapsed(liste_evt->time_elapsed+g->routes[liste_evt->route][liste_evt->arc_id]->length,be_time);
			}
		}
	}
	else //backward
	{
		
		g->routes[liste_evt->route][liste_evt->arc_id]->state_b = 1;
		if(liste_evt->kind_message)//Si c'est du CRAN
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,0,BACKWARD,1);
		else
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+SIZE_BE_MESSAGE,liste_evt->route,liste_evt->arc_id,0,0,BACKWARD,1);
		
		if(liste_evt->kind_p == BACKWARD)
		{
			fprintf(logs,"Way backward (arc %d, length %d)",liste_evt->arc_id,g->routes[liste_evt->route][liste_evt->arc_id]->length);
			if(liste_evt->arc_id == 0)
			{
				fprintf(logs,"end of the message, update \n");
				if(liste_evt->kind_message)//Si c'est du CRAN
				{
					if(!computed_assignment)
						update_time_elapsed(liste_evt->time_elapsed+g->routes[liste_evt->route][liste_evt->arc_id]->length,p_time);
						
				}
				else
				{
					//printf("be time update in message 2\n");
					update_time_elapsed(liste_evt->time_elapsed+g->routes[liste_evt->route][liste_evt->arc_id]->length,be_time);
				}
			}
			else
			{

				fprintf(logs,"new event message at date %d\n",liste_evt->date+g->routes[liste_evt->route][liste_evt->arc_id]->length);
				if(!computed_assignment || !liste_evt->kind_message )	
					liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->route,liste_evt->arc_id -1,liste_evt->time_elapsed+ g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->deadline - g->routes[liste_evt->route][liste_evt->arc_id]->length, BACKWARD,liste_evt->kind_message);
			}
		}
		else
		{
			printf("ERROR, an event is a message and has no kind_p (multiplexing->c)\n");exit(87);
		}
	}
	return liste_evt;
}

Event * arc_free_fct(Graph * g, Event * liste_evt,int message_size, int * p_time,int * be_time)
{
	int current_route_size;
	int time_waited;
	Elem * first_elem;
	Arc * current_arc;
	
	current_arc = g->routes[liste_evt->route][liste_evt->arc_id];
	current_route_size = g->size_routes[liste_evt->route];
	
	if(liste_evt->kind_p == FORWARD)
	{
		first_elem = g->routes[liste_evt->route][liste_evt->arc_id]->elems_f;
		if(first_elem == NULL)
		{
			printf("ERROR, THIS SHOULD NOT HAPPEND, function arc_free_fct is called only if first elem is not null, multiplexing->c\n");exit(46);
		}
		time_waited = liste_evt->date - first_elem->arrival_in_queue;
		//printf(" on a attendu %d \n",time_waited);
		fprintf(logs,"The elem at the top of the list (route %d arc %d) has waited %d slots (arrival %d, date %d).",first_elem->numero_route,first_elem->arc_id,time_waited,first_elem->arrival_in_queue,liste_evt->date);
		
		fprintf(logs,"New arc event at date %d+ (%d ou %d ) %d\n",liste_evt->date,message_size,SIZE_BE_MESSAGE,liste_evt->kind_message);
		g->routes[liste_evt->route][liste_evt->arc_id]->state_f = 1;
	
		if(first_elem->kind_message)//Si c'est du CRAN
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,0,FORWARD,1);
		else
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+SIZE_BE_MESSAGE,liste_evt->route,liste_evt->arc_id,0,0,FORWARD,1);

		fprintf(logs,"Way forward, new event message (arc %d length %d) at date %d \n",first_elem->arc_id,g->routes[first_elem->numero_route][first_elem->arc_id]->length,liste_evt->date);
		if(liste_evt->arc_id != current_route_size-1) // not the last arc
		{
			if(!computed_assignment || !first_elem->kind_message )	
			{
				fprintf(logs,"\n on cree le message time %d %d\n",first_elem->time_elapsed + g->routes[first_elem->numero_route][first_elem->arc_id]->length+time_waited,time_waited);
				liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g->routes[first_elem->numero_route][first_elem->arc_id]->length,first_elem->numero_route,first_elem->arc_id+1,first_elem->time_elapsed + g->routes[first_elem->numero_route][first_elem->arc_id]->length+time_waited,first_elem->deadline -g->routes[first_elem->numero_route][first_elem->arc_id]->length-time_waited,FORWARD,first_elem->kind_message);
			}
		}
		else
		{
			if(first_elem->kind_message)//Si c'est du CRAN
			{
				if(!computed_assignment)	
					liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g->routes[first_elem->numero_route][first_elem->arc_id]->length,first_elem->numero_route,first_elem->arc_id,first_elem->time_elapsed + g->routes[first_elem->numero_route][first_elem->arc_id]->length+time_waited,first_elem->deadline -g->routes[first_elem->numero_route][first_elem->arc_id]->length-time_waited,BACKWARD,first_elem->kind_message);
			}
			else
			{
				//printf("be time update in arc free 1\n");
				update_time_elapsed(first_elem->time_elapsed+g->routes[first_elem->numero_route][first_elem->arc_id]->length,be_time);
			}
		}

		current_arc->elems_f = first_elem->suiv;
	}
	else //backward
	{
		if(liste_evt->kind_p == BACKWARD)
		{
			first_elem = g->routes[liste_evt->route][liste_evt->arc_id]->elems_b;
			if(first_elem == NULL)
			{
				printf("ERROR, THIS SHOULD NOT HAPPEND, function arc_free_fct is called only if first elem is not null, multiplexing->c\n");exit(46);
			}
			time_waited = liste_evt->date - first_elem->arrival_in_queue;
			
			fprintf(logs,"The elem at the top of the list (route %d arc %d) has waited %d slots (arrival %d, date %d).",first_elem->numero_route,first_elem->arc_id,time_waited,first_elem->arrival_in_queue,liste_evt->date);
		
			fprintf(logs,"New arc event at date %d\n",liste_evt->date+message_size);
			g->routes[liste_evt->route][liste_evt->arc_id]->state_b = 1;

			if(first_elem->kind_message)//Si c'est du CRAN
				liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,0,BACKWARD,1);
			else
				liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+SIZE_BE_MESSAGE,liste_evt->route,liste_evt->arc_id,0,0,BACKWARD,1);

			fprintf(logs,"way backward (arc %d length %d) ",first_elem->arc_id,g->routes[first_elem->numero_route][first_elem->arc_id]->length);
			if(liste_evt->arc_id == 0)
			{
				fprintf(logs,"time updated\n");
				if(first_elem->kind_message)//Si c'est du CRAN
				{
					if(!computed_assignment)
						update_time_elapsed(first_elem->time_elapsed+g->routes[first_elem->numero_route][first_elem->arc_id]->length,p_time);
				}
				else
				{
					//printf("be time update in arc free 2\n");
					update_time_elapsed(first_elem->time_elapsed+g->routes[first_elem->numero_route][first_elem->arc_id]->length,be_time);
				}
			}
			else
			{

				fprintf(logs,"new event message at date %d .\n",liste_evt->date+g->routes[first_elem->numero_route][first_elem->arc_id]->length);
				if(!computed_assignment || !first_elem->kind_message )	
					liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g->routes[first_elem->numero_route][first_elem->arc_id]->length,first_elem->numero_route,first_elem->arc_id-1,first_elem->time_elapsed +g->routes[first_elem->numero_route][first_elem->arc_id]->length+time_waited,first_elem->deadline -g->routes[first_elem->numero_route][first_elem->arc_id]->length-time_waited,BACKWARD,first_elem->kind_message);
			}
			current_arc->elems_b = first_elem->suiv;
			
		}
		else
		{
			printf("ERROR, an event is a message and has no kind_p (multiplexing->c)\n");exit(87);
		}
	}
	free(first_elem);
	
	return liste_evt;
}
int multiplexing(Graph * g, int period, int message_size, int nb_periods,Policy pol,int computed)
{
	logs = fopen("logs_multiplexing.txt","w");
	computed_assignment = computed;
	//logs = stdout;

	if(!logs){perror("Error opening multiplexing->txt");}
	Event * current;
	Event * liste_evt = NULL;

	if(!computed_assignment)
		liste_evt = init_events(g,liste_evt,period,nb_periods);
	else
		liste_evt = init_computed(g,liste_evt,period,nb_periods);

	//liste_evt = init_BE(g,liste_evt,period,nb_periods);
	init_arcs_state(g);
	int longest_time_elapsed = 0;
	int time_be = 0;
	while(liste_evt)
	{
		current = liste_evt;
		fprintf(logs,"\n Date %d ",liste_evt->date);
		if(liste_evt->kind == MESSAGE)
		{
			fprintf(logs,"A message on the route %d has arrived.\n",liste_evt->route);
			
			if(((liste_evt->kind_p == FORWARD) && g->routes[liste_evt->route][liste_evt->arc_id]->state_f) || ((liste_evt->kind_p == BACKWARD) && g->routes[liste_evt->route][liste_evt->arc_id]->state_b)) // arc used
			{
				fprintf(logs,"The arc is used (pol = %d).\n",pol);

				fprintf(logs,"Adding elem (%d %d %d %d %d) - ",liste_evt->route,liste_evt->arc_id,liste_evt->date,liste_evt->time_elapsed,liste_evt->kind_p);
				if(pol == FIFO)
				{
					if(liste_evt->kind_p == FORWARD)
						g->routes[liste_evt->route][liste_evt->arc_id]->elems_f = ajoute_elem_fifo(g->routes[liste_evt->route][liste_evt->arc_id]->elems_f,liste_evt->route,liste_evt->arc_id,liste_evt->date,liste_evt->time_elapsed,liste_evt->deadline,liste_evt->kind_p,liste_evt->kind_message);
					else
						g->routes[liste_evt->route][liste_evt->arc_id]->elems_b = ajoute_elem_fifo(g->routes[liste_evt->route][liste_evt->arc_id]->elems_b,liste_evt->route,liste_evt->arc_id,liste_evt->date,liste_evt->time_elapsed,liste_evt->deadline,liste_evt->kind_p,liste_evt->kind_message);

				}
				else
				{
					if(liste_evt->kind_p == FORWARD)
						g->routes[liste_evt->route][liste_evt->arc_id]->elems_f = ajoute_elem_deadline(g->routes[liste_evt->route][liste_evt->arc_id]->elems_f,liste_evt->route,liste_evt->arc_id,liste_evt->date,liste_evt->time_elapsed,liste_evt->deadline,liste_evt->kind_p,liste_evt->kind_message);
					else
						g->routes[liste_evt->route][liste_evt->arc_id]->elems_b = ajoute_elem_deadline(g->routes[liste_evt->route][liste_evt->arc_id]->elems_b,liste_evt->route,liste_evt->arc_id,liste_evt->date,liste_evt->time_elapsed,liste_evt->deadline,liste_evt->kind_p,liste_evt->kind_message);
				}
				
			}
			else // arc free
			{
				fprintf(logs,"The arc is free.");
				liste_evt = message_on_arc_free_fct(g,liste_evt,message_size,&longest_time_elapsed, &time_be);
			}
		}
		else // arc
		{
			fprintf(logs,"An arc is available .\n");
			current = liste_evt;
			if( ( (liste_evt->kind_p == FORWARD) && g->routes[liste_evt->route][liste_evt->arc_id]->elems_f) || ( (liste_evt->kind_p == BACKWARD) && g->routes[liste_evt->route][liste_evt->arc_id]->elems_b) ) // if there is some messages to manage
			{
				fprintf(logs,"there is some messages in queue : \n");
				liste_evt = arc_free_fct(g,liste_evt,message_size,&longest_time_elapsed, &time_be);
			}
			else
			{
				fprintf(logs,"arc no messages in queue, we set the arc to available");
				if(liste_evt->kind_p == FORWARD)
					g->routes[liste_evt->route][liste_evt->arc_id]->state_f = 0;
				else
					g->routes[liste_evt->route][liste_evt->arc_id]->state_b = 0;
			}

		}
		if(liste_evt != current){
			printf("Error, this is not possible (multiplexing->c)\n");exit(73);
		}
		liste_evt = liste_evt->suiv;
		free(current);
	
	}

	fclose(logs);
	//printf("%d %d \n",longest_time_elapsed,time_be);
	return longest_time_elapsed;
}


