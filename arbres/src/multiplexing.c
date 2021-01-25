#include "structs.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
FILE * logs;
int computed_assignment;
#pragma omp threadprivate(computed_assignment)
FILE * f;
int PER;
int tabbef[10000];
int tabberoutef[10000];
int tabbeb[10000];
int tabberouteb[10000];
int cmptbef;
int cmptbeb;
int genbe;
long long int moyenne;
int nb_elem_moy ;
int fifomode;
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
		if(l->suiv->date >= date)//insertion au milieu
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
	Elem * debut = l;
	if(fifomode)
	{

		if(est_vide(l))//la liste est vide
		{
			new->suiv = NULL;
			return new;
		}
			
		//messageBE
		if(kind_message == 0)
		{
			while(l->suiv)
			{
				if(l->suiv->kind_message)
					l = l->suiv;
				else
					break;
					
			//printf("%d  %p\n",l->kind_message,l);
			}
			fifomode = 0;
			l->suiv = ajoute_elem_fifo(l->suiv,numero_route,arc_id,arrival_in_queue,time_elapsed,deadline,kind_p,kind_message);
			fifomode = 1;
			free(new);
			return debut;
		}
		if((l->arrival_in_queue < arrival_in_queue)||(l->kind_message == 0))//insertion au debut
		{
			new->suiv = l;
			return new;
		}

		//on sarrête soit quand on est au bout, soit quand on doit ajouter l'element
		while(l->suiv)
		{
			if((l->suiv->arrival_in_queue < arrival_in_queue)||(l->kind_message == 0))//insertion au milieu
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
	Elem * debut = l;
	//messageBE
	if(kind_message == 0)
	{
		while(l->suiv)
		{
			if(l->suiv->kind_message)
				l = l->suiv;
			else
				break;
				
		//printf("%d  %p\n",l->kind_message,l);
		}
		l->suiv = ajoute_elem_fifo(l->suiv,numero_route,arc_id,arrival_in_queue,time_elapsed,deadline,kind_p,kind_message);
		free(new);
		return debut;
	}
	if((l->deadline > deadline)||(l->kind_message == 0))//insertion au debut
	{
		new->suiv = l;
		return new;
	}
	
	//on sarrête soit quand on est au bout, soit quand on doit ajouter l'element
	while(l->suiv)
	{
		if((l->suiv->deadline > deadline)||(l->kind_message == 0))//insertion au milieu
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
				
				liste_evt = ajoute_event_trie(liste_evt,MESSAGE,date+period*j-SIZE_BE_MESSAGE,i,k,0,0,FORWARD,2);
				liste_evt = ajoute_event_trie(liste_evt,MESSAGE,date+period*j,i,k,0,0,FORWARD,1);
			}
			//if(k==1)printf("FPaquet %d date %d \n",i,date);
			//printf("route %d arc %d message forward %d %d",i,k,date,g->routes[i][k]->routes_delay_f[i]);
			date = route_length_with_buffers_forward( g,i)+route_length_untill_arc(g,i,g->routes[i][k],BACKWARD)+g->routes[i][k]->routes_delay_b[i];
			//printf("Calcul = mi %d routforward %d routeback %d",g->routes[i][0]->routes_delay_f[i],route_length_with_buffers_forward( g,i),route_length_untill_arc(g,i,g->routes[i][k],BACKWARD)+g->routes[i][k]->routes_delay_b[i]);
			//if(k==1)printf("BPaquet %d date %d \n",i,date);
			for(int j=0;j<nb_periods;j++)
			{
				
				liste_evt = ajoute_event_trie(liste_evt,MESSAGE,date+period*j-SIZE_BE_MESSAGE,i,k,0,0,BACKWARD,2);
				liste_evt = ajoute_event_trie(liste_evt,MESSAGE,date+period*j,i,k,0,0,BACKWARD,1);
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
			//printf(" init event date %d , %d\n",date+period*j, MESSAGE);
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
	//printf("%f somme moy  = %f \n",sumn,moy/size);
	fclose(file);
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


double Exponentielle(float lbda) // Mèthode de la tranformation inverse
{
double r = (double)random()/RAND_MAX; // un réel dans  [0...1]
    while (r==0 || r==1) {              // tant que r=0 ou 1  on refait un random
            r = (double)random()/RAND_MAX;
            }

       return -log(r)/(lbda*1.0); // -ln(U)/lambda   , avec U = unif[0,1]
}
	
	
Event * init_BE(Graph * g, Event * liste_evt,int period, int nb_periods)
{

	/*float tab[20];
	read_param_file(tab, 20);


	int nb_paquets ;
	
	for(int i=0;i<period*nb_periods;i+=period)
	{
		//printf("i = %d \n",i);
		//nb_paquets = 2;
		nb_paquets = inverse_transform(tab,20)*20;
		//nb_paquets = 1;
		int interval = (!nb_paquets)?0:period/nb_paquets;
		for(int j=0;j<nb_paquets;j++)
		{
			
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,i+interval*j,j%g->nb_routes,0,0,INT_MAX,FORWARD,0);
		}
		nb_paquets = inverse_transform(tab,20)*20;
		//nb_paquets = 1;
		interval = (!nb_paquets)?0:period/nb_paquets;
		for(int j=0;j<nb_paquets;j++)
		{
			//printf("ajouter elem back\n");
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,i+interval*j,j%g->nb_routes,g->size_routes[j%g->nb_routes]-1,0,INT_MAX,BACKWARD,0);
		}
		
		
	}*/
	int i=0;
	int routerand;

	if(!genbe)
	{
		while(i<period*nb_periods)
		{
			routerand = random()%g->nb_routes;
			tabberoutef[cmptbef] = routerand;
			i+= (int)Exponentielle(0.003);
			tabbef[cmptbef]= i;
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,i,routerand,0,0,INT_MAX,FORWARD,0);
			cmptbef++;
			if(cmptbef == 10000)
			{
				printf("Le nombre de message BE généré est trop grand\n");
				exit(21);
			}
			//printf("Message forward date %d route %d \n",i,routerand);
		}		
		i=0;
		while(i<period*nb_periods)
		{
			routerand = random()%g->nb_routes;
			tabberoutef[cmptbef] = routerand;
			i+= (int)Exponentielle(0.0001);
			tabbef[cmptbef]= i;
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,i,routerand,0,0,INT_MAX,FORWARD,0);
			cmptbef++;
			if(cmptbef == 10000)
			{
				printf("Le nombre de message BE généré est trop grand\n");
				exit(21);
			}
			//printf("Message forward date %d route %d \n",i,routerand);
		}		
		i=0;
		while(i<period*nb_periods)
		{
			routerand = random()%g->nb_routes;
			tabberouteb[cmptbeb] = routerand;
			i+= (int)Exponentielle(0.003);
			tabbeb[cmptbeb]= i;
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,i,routerand,g->size_routes[routerand]-1,0,INT_MAX,BACKWARD,0);
			cmptbeb++;
			if(cmptbeb == 10000)
			{
				printf("Le nombre de message BE généré est trop grand\n");
				exit(21);
			}
			//printf("Message backward date %d route %d \n",i,routerand);
		}
		i=0;
		while(i<period*nb_periods)
		{
			routerand = random()%g->nb_routes;
			tabberouteb[cmptbeb] = routerand;
			i+= (int)Exponentielle(0.0001);
			tabbeb[cmptbeb]= i;
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,i,routerand,g->size_routes[routerand]-1,0,INT_MAX,BACKWARD,0);
			cmptbeb++;
			if(cmptbeb == 10000)
			{
				printf("Le nombre de message BE généré est trop grand\n");
				exit(21);
			}
			//printf("Message backward date %d route %d \n",i,routerand);
		}
		//printf("first cmptbef %d cmptbeb %d \n ",cmptbef,cmptbeb);
		genbe = 1;
	}
	else
	{
		//printf("Les autres fois %d %d \n",cmptbef,cmptbeb);
		for(int j=0;j<cmptbef;j++)
		{
			routerand = tabberoutef[j];
			i= tabbef[j];
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,i,routerand,0,0,INT_MAX,FORWARD,0);
		//	printf("Message forward date %d route %d \n",i,routerand);
		}		
		i=0;
		for(int j=0;j<cmptbeb;j++)
		{
			routerand = tabberouteb[j];
			i= tabbeb[j];
			liste_evt = ajoute_event_trie(liste_evt,MESSAGE,i,routerand,g->size_routes[routerand]-1,0,INT_MAX,BACKWARD,0);
			//printf("Message backward date %d route %d \n",i,routerand);
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
	
	fprintf(logs,"new event arc at date %d kindmess = %d ",liste_evt->date+message_size,liste_evt->kind_message);
	//printf("message on arc free fct kindmesse %d\n",liste_evt->kind_message);
	current_route_size = g->size_routes[liste_evt->route];
	if(liste_evt->kind_p == FORWARD)
	{
		if(liste_evt->kind_message == 2)
		{
			//printf("on reserve forward route %d id %d ",liste_evt->route,liste_evt->arc_id);
			g->routes[liste_evt->route][liste_evt->arc_id]->state_f = 2;
			return liste_evt;
		}
		else	
			g->routes[liste_evt->route][liste_evt->arc_id]->state_f = 1;
		if(liste_evt->kind_message)//Si c'est du CRAN
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,0,FORWARD,0);
		else
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+SIZE_BE_MESSAGE,liste_evt->route,liste_evt->arc_id,0,0,FORWARD,1);
		
		fprintf(logs,"Way forward (arc %d, length %d), new event message at date %d\n",liste_evt->arc_id,g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->date+g->routes[liste_evt->route][liste_evt->arc_id]->length);
		if(liste_evt->arc_id != current_route_size-1) // not the last arc
		{
			if(!computed_assignment || !liste_evt->kind_message )
			{
				//printf("création message kind %d date %d route %d arc %d \n",liste_evt->kind_message,liste_evt->date+g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->route,liste_evt->arc_id+1);
				liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->route,liste_evt->arc_id+1,liste_evt->time_elapsed + g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->deadline - g->routes[liste_evt->route][liste_evt->arc_id]->length,FORWARD,liste_evt->kind_message);

			}	
				
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
			
				if(liste_evt->date > PER * 5)
				{
					fprintf(f,"%d \n",liste_evt->time_elapsed+g->routes[liste_evt->route][liste_evt->arc_id]->length- route_length(  g,liste_evt->route));
					moyenne += liste_evt->time_elapsed+g->routes[liste_evt->route][liste_evt->arc_id]->length- route_length(  g,liste_evt->route);
					nb_elem_moy++;
					update_time_elapsed(liste_evt->time_elapsed+g->routes[liste_evt->route][liste_evt->arc_id]->length- route_length(  g,liste_evt->route),be_time);
				}
			}
		}
	}
	else //backward
	{
		if(liste_evt->kind_message == 2)
		{
			//printf("on reserve backward route %d id %d ",liste_evt->route,liste_evt->arc_id);
			g->routes[liste_evt->route][liste_evt->arc_id]->state_b = 2;
			return liste_evt;
		}
		else
			g->routes[liste_evt->route][liste_evt->arc_id]->state_b = 1;
		if(liste_evt->kind_message)//Si c'est du CRAN
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,0,BACKWARD,2);
		else
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+SIZE_BE_MESSAGE,liste_evt->route,liste_evt->arc_id,0,0,BACKWARD,3);
		
		if(liste_evt->kind_p == BACKWARD)
		{
			fprintf(logs,"Way backward (arc %d, length %d)",liste_evt->arc_id,g->routes[liste_evt->route][liste_evt->arc_id]->length);
			if(liste_evt->arc_id == 0)
			{
				fprintf(logs,"end of the message, update \n");
				if(liste_evt->kind_message)//Si c'est du CRAN
				{
					if(!computed_assignment)
					{
						if(liste_evt->date > PER * 5)
						{
							update_time_elapsed(liste_evt->time_elapsed+g->routes[liste_evt->route][liste_evt->arc_id]->length,p_time);
						
						}
					}
						
				}
				else
				{
					//printf("be time update in message 2\n");
					
					if(liste_evt->date > PER * 5)
					{
						fprintf(f,"%d \n",liste_evt->time_elapsed+g->routes[liste_evt->route][liste_evt->arc_id]->length- route_length(  g,liste_evt->route));
						moyenne += liste_evt->time_elapsed+g->routes[liste_evt->route][liste_evt->arc_id]->length- route_length(  g,liste_evt->route);
						nb_elem_moy++;
						update_time_elapsed(liste_evt->time_elapsed+g->routes[liste_evt->route][liste_evt->arc_id]->length- route_length(  g,liste_evt->route),be_time);
					}
				}
			}
			else
			{

				fprintf(logs,"new event message at date %d\n",liste_evt->date+g->routes[liste_evt->route][liste_evt->arc_id]->length);
				if(!computed_assignment || !liste_evt->kind_message )
				{
					//printf("Event backwar\n");
					liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->route,liste_evt->arc_id -1,liste_evt->time_elapsed+ g->routes[liste_evt->route][liste_evt->arc_id]->length,liste_evt->deadline - g->routes[liste_evt->route][liste_evt->arc_id]->length, BACKWARD,liste_evt->kind_message);
		
				}	
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
		if(current_arc->state_f == 2)
			return liste_evt;
		first_elem = g->routes[liste_evt->route][liste_evt->arc_id]->elems_f;
		//printf("First elem kind = %d \n", first_elem->kind_message);
		if(first_elem == NULL)
		{
			printf("ERROR, THIS SHOULD NOT HAPPEND, function arc_free_fct is called only if first elem is not null, multiplexing->c\n");exit(46);
		}
		time_waited = liste_evt->date - first_elem->arrival_in_queue;
		//if(liste_evt->arc_id == 1)printf("Time f = %d first elem kind %d arrival %d date%d \n",time_waited,first_elem->kind_message,first_elem->arrival_in_queue,liste_evt->date);
		if(computed_assignment && first_elem->kind_message && time_waited)
					printf(" FORWARDWAIT %d date %d arrival %d  \n",time_waited, liste_evt->date , first_elem->arrival_in_queue);
		//printf(" on a attendu %d \n",time_waited);
		fprintf(logs,"The elem at the top of the list (route %d arc %d) has waited %d slots (arrival %d, date %d).",first_elem->numero_route,first_elem->arc_id,time_waited,first_elem->arrival_in_queue,liste_evt->date);
		
		fprintf(logs,"New arc event at date %d+ (%d ou %d ) %d\n",liste_evt->date,message_size,SIZE_BE_MESSAGE,liste_evt->kind_message);
	
		g->routes[liste_evt->route][liste_evt->arc_id]->state_f = 1;
		if(first_elem->kind_message)//Si c'est du CRAN
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,0,FORWARD,4);
		else
			liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+SIZE_BE_MESSAGE,liste_evt->route,liste_evt->arc_id,0,0,FORWARD,5);

		fprintf(logs,"Way forward, new event message (arc %d length %d) at date %d \n",first_elem->arc_id,g->routes[first_elem->numero_route][first_elem->arc_id]->length,liste_evt->date);
		if(liste_evt->arc_id != current_route_size-1) // not the last arc
		{

			if(!computed_assignment || !first_elem->kind_message )	
			{
					//printf("Time waited = %d kind elem = %d \n",time_waited,first_elem->kind_message);
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
				if(liste_evt->date > PER * 5)	
				{
					fprintf(f,"%d \n",first_elem->time_elapsed+g->routes[first_elem->numero_route][first_elem->arc_id]->length- route_length(  g,first_elem->numero_route)+time_waited);
					moyenne += first_elem->time_elapsed+g->routes[first_elem->numero_route][first_elem->arc_id]->length- route_length(  g,first_elem->numero_route)+time_waited;
					nb_elem_moy++;
					update_time_elapsed(first_elem->time_elapsed+g->routes[first_elem->numero_route][first_elem->arc_id]->length- route_length(  g,first_elem->numero_route)+time_waited,be_time);
				}
			}
		}

		current_arc->elems_f = first_elem->suiv;
	}
	else //backward
	{
		if(current_arc->state_b == 2)
			return liste_evt;
		if(liste_evt->kind_p == BACKWARD)
		{
			first_elem = g->routes[liste_evt->route][liste_evt->arc_id]->elems_b;
			if(first_elem == NULL)
			{
				printf("ERROR, THIS SHOULD NOT HAPPEND, function arc_free_fct is called only if first elem is not null, multiplexing->c\n");exit(46);
			}
			//printf("First elem kind = %d \n", first_elem->kind_message);
			time_waited = liste_evt->date - first_elem->arrival_in_queue;
			if(computed_assignment && first_elem->kind_message&& time_waited)
				printf(" BACKWAIT %d date %d arrival %d  \n",time_waited, liste_evt->date , first_elem->arrival_in_queue);
			fprintf(logs,"The elem at the top of the list (route %d arc %d) has waited %d slots (arrival %d, date %d).",first_elem->numero_route,first_elem->arc_id,time_waited,first_elem->arrival_in_queue,liste_evt->date);
		
			fprintf(logs,"New arc event at date %d\n",liste_evt->date+message_size);
		
			g->routes[liste_evt->route][liste_evt->arc_id]->state_b = 1;

			if(first_elem->kind_message)//Si c'est du CRAN
				liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+message_size,liste_evt->route,liste_evt->arc_id,0,0,BACKWARD,6);
			else
				liste_evt = ajoute_event_trie(liste_evt,ARC,liste_evt->date+SIZE_BE_MESSAGE,liste_evt->route,liste_evt->arc_id,0,0,BACKWARD,7);

			fprintf(logs,"way backward (arc %d length %d) ",first_elem->arc_id,g->routes[first_elem->numero_route][first_elem->arc_id]->length);
			if(liste_evt->arc_id == 0)
			{
				fprintf(logs,"time updated\n");
				if(first_elem->kind_message)//Si c'est du CRAN
				{
					if(!computed_assignment)
					{
						if(liste_evt->date > PER * 5)
							update_time_elapsed(first_elem->time_elapsed+g->routes[first_elem->numero_route][first_elem->arc_id]->length+time_waited,p_time);
					}
				}
				else
				{
					//printf("be time update in arc free 2\n");
					if(liste_evt->date > PER * 5){
						fprintf(f,"%d \n",first_elem->time_elapsed+g->routes[first_elem->numero_route][first_elem->arc_id]->length- route_length(  g,first_elem->numero_route)+time_waited);
						moyenne += first_elem->time_elapsed+g->routes[first_elem->numero_route][first_elem->arc_id]->length- route_length(  g,first_elem->numero_route)+time_waited;
						nb_elem_moy++;
						update_time_elapsed(first_elem->time_elapsed+time_waited+g->routes[first_elem->numero_route][first_elem->arc_id]->length- route_length(  g,first_elem->numero_route),be_time);
					}
				}
			}
			else
			{

				fprintf(logs,"new event message at date %d .\n",liste_evt->date+g->routes[first_elem->numero_route][first_elem->arc_id]->length);
				if(!computed_assignment || !first_elem->kind_message )	
				{
					//printf("Time waited = %d kind elem = %d \n",time_waited,first_elem->kind_message);
					liste_evt = ajoute_event_trie(liste_evt,MESSAGE,liste_evt->date+g->routes[first_elem->numero_route][first_elem->arc_id]->length,first_elem->numero_route,first_elem->arc_id-1,first_elem->time_elapsed +g->routes[first_elem->numero_route][first_elem->arc_id]->length+time_waited,first_elem->deadline -g->routes[first_elem->numero_route][first_elem->arc_id]->length-time_waited,BACKWARD,first_elem->kind_message);
				}
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
int multiplexing(Graph * g, int period, int message_size, int nb_periods,Policy pol,int computed,int * time_be,FILE * fichier,int reinit_be,int * moy,int fifom)
{
	fifomode = fifom;
	moyenne = 0;
	nb_elem_moy=0;
	if(reinit_be)
	{
		cmptbeb =0;
		cmptbef = 0;
		genbe = 0;
	}
	PER = period;
	f = fichier;
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
	
	while(liste_evt)
	{
		current = liste_evt;
		//printf(" Date %d\n ",liste_evt->date);
		if(liste_evt->kind == MESSAGE)
		{
			// printf("Arrive message kind %d route %d arc %d \n",liste_evt->kind_message,liste_evt->route,liste_evt->arc_id);
			if(liste_evt->kind_message == 2){
				liste_evt = message_on_arc_free_fct(g,liste_evt,message_size,&longest_time_elapsed, time_be);//on passe l'arc en mode reservé
				//printf("Reservation \n");			
			}
			else
			{
				
				fprintf(logs,"A message on the route %d has arrived.\n",liste_evt->route);
				//printf("%d %d %d %d  \n",computed_assignment,liste_evt->kind_p,g->routes[liste_evt->route][liste_evt->arc_id]->state_f,g->routes[liste_evt->route][liste_evt->arc_id]->state_b);
				if(computed_assignment && (( (liste_evt->kind_p == FORWARD) && (g->routes[liste_evt->route][liste_evt->arc_id]->state_f == 2)) || ((liste_evt->kind_p == BACKWARD) && g->routes[liste_evt->route][liste_evt->arc_id]->state_b==2))){
					if(liste_evt->kind_message==1)
					{
						//printf("on passe direct message free fct car c reservé \n");
						liste_evt = message_on_arc_free_fct(g,liste_evt,message_size,&longest_time_elapsed, time_be);
					}
				}
				else
				{
				
					//printf("On fait qqchose selon si l'arc est :\n");
					if(((liste_evt->kind_p == FORWARD) && g->routes[liste_evt->route][liste_evt->arc_id]->state_f) || ((liste_evt->kind_p == BACKWARD) && g->routes[liste_evt->route][liste_evt->arc_id]->state_b)) // arc used
					{
						//printf("utilisé, filde d'attente.\n");
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
						//printf("vide, ca y va\n");
						fprintf(logs,"The arc is free.");
						liste_evt = message_on_arc_free_fct(g,liste_evt,message_size,&longest_time_elapsed, time_be);
					}
					
				}
			}
			
		}
		else // arc
		{
			//printf("L'arc est libre\n");
			fprintf(logs,"An arc is available .\n");
			current = liste_evt;
			if( ( (liste_evt->kind_p == FORWARD) && g->routes[liste_evt->route][liste_evt->arc_id]->elems_f) || ( (liste_evt->kind_p == BACKWARD) && g->routes[liste_evt->route][liste_evt->arc_id]->elems_b) ) // if there is some messages to manage
			{
				//printf("On a des trucs a faire \n");
				fprintf(logs,"there is some messages in queue : \n");
				//printf("L'arc free est a cause de %d \n",liste_evt->kind_message);
				liste_evt = arc_free_fct(g,liste_evt,message_size,&longest_time_elapsed, time_be);
			}
			else
			{
				fprintf(logs,"arc no messages in queue, we set the arc to available");

				if(liste_evt->kind_p == FORWARD)
				{
					if(g->routes[liste_evt->route][liste_evt->arc_id]->state_f != 2)
						g->routes[liste_evt->route][liste_evt->arc_id]->state_f = 0;
				}
				else
				{
					if(g->routes[liste_evt->route][liste_evt->arc_id]->state_b != 2)
						g->routes[liste_evt->route][liste_evt->arc_id]->state_b = 0;
				}
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
	if(nb_elem_moy)
		*moy = moyenne/nb_elem_moy;
	return longest_time_elapsed;
}


