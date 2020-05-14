#include "structs.h"
#include "treatment.h"
#include "voisinage.h"

#include "config.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#include "test.h"

Assignment greedy(Graph * g, int P, int message_size, int tmax)
{
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g->nb_routes);
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;
	a->offset_backward = malloc(sizeof(int)*g->nb_routes);
	a->waiting_time = malloc(sizeof(int)*g->nb_routes);
	int offset;
	int begin_offset;
	int bool_fail = 0;
	int bool_fail_tmax = 0;
	

	//for each route
	for(int i=0;i<g->nb_routes;i++)
	{
		
		offset=0;

	
		while( !message_no_collisions( g, i, offset,message_size,FORWARD,P) )
		{
			
			offset++;
			if(offset == P)
			{
				
				bool_fail = 1;
				break;
			}			
		}
		if(!bool_fail)
		{
			fill_period(g,i,offset,message_size,FORWARD,P);
			a->offset_forward[i]=offset;
			g->routes[i][0]->routes_delay_f[i] = offset;
		}
		else
		{
			a->offset_forward[i]=-1;
			bool_fail = 0;
		}
		
	}

	int deadline[g->nb_routes];
	int order[g->nb_routes];
	for(int i=0;i<g->nb_routes;i++)
		order[i]=i;
	
	for(int i= 0;i<g->nb_routes;i++)
	{
		deadline[i] = tmax - (2 * route_length(g,i) + a->offset_forward[i]);
	}

	tri_bulles_inverse(deadline,order, g->nb_routes);

	
		//for each route
	
	for(int i=0;i<g->nb_routes;i++)
	{
		bool_fail = 0;
		bool_fail_tmax = 0;
		//For the route we succeced to schedule in the way forward
		if(a->offset_forward[order[i]] != -1)
		{
			
			begin_offset = route_length(g,order[i]) + a->offset_forward[order[i]];
			offset=begin_offset;
			
			while( !message_no_collisions( g, order[i], offset,message_size,BACKWARD,P) )
			{

				offset++;
				if(offset == (P+begin_offset))
				{
					bool_fail = 1;
				}
			}
			if(!bool_fail)
			{
				
				if( (2*route_length( g,order[i]) + offset-begin_offset ) > tmax )
				{
				
					bool_fail_tmax = 1;
				}
				if(!bool_fail_tmax)
				{
				
					fill_period(g,order[i],offset,message_size,BACKWARD,P);
					a->offset_backward[order[i]]=offset;
					g->routes[order[i]][0]->routes_delay_b[order[i]] = offset-begin_offset;
					a->waiting_time[order[i]]=offset-begin_offset;
					a->nb_routes_scheduled++;
				}
				else
				{
					a->offset_backward[order[i]]=-1;
					a->waiting_time[order[i]]=-1;
				}
				
			}
			else
			{
				a->offset_backward[order[i]]=-1;
				a->waiting_time[order[i]]=-1;
			}
		}
		else
		{
			a->offset_backward[order[i]]=-1;
			a->waiting_time[order[i]]=-1;
		}
	}
	if(a->nb_routes_scheduled == g->nb_routes)
	{
		a->all_routes_scheduled = 1;

	}
	else
	{
		a->all_routes_scheduled = 0;
	}



	return a;

}


/*
ARG mode :
	- 0 : sort the routes on arcs by id
	- 1 : uses the longest route first
	- 2 : use the route with the most collisions first
*/
Assignment greedy_by_arcs(Graph * g, int P, int message_size,int mode)
{
	
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g->nb_routes);
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;
	a->offset_backward = malloc(sizeof(int)*g->nb_routes);
	a->waiting_time = malloc(sizeof(int)*g->nb_routes);
	int offset,begin_offset,offset_back;
	int best_offset=0;
	int best_begin = 0;
	int best_back = INT_MAX;
	int back_found;
	int routes[g->nb_routes];
	for(int i=0;i<g->nb_routes;i++)
		routes[i]=0;

	int * load_order = load_links(g);//store the id of the arcs, by the most loaded to the less loaded
	int * id_routes;

	for(int i=0;i<g->arc_pool_size;i++)
	{
		switch (mode) {// id_routes  stores the id of the routes to use, considering the sort chosen

		case 0 :
			id_routes = routes_by_id(g->arc_pool[load_order[i]]); 
		break;

		case 1 :
			id_routes = sort_longest_routes_on_arc(g, g->arc_pool[load_order[i]]);
		break;
		case 2:
			id_routes = sort_routes_by_collisions(g,g->arc_pool[load_order[i]]);
		break;
		default:
			printf("Mode non connu(greedy by arcs).\n");exit(97);
		break;
		}
		for(int j=0;j<g->arc_pool[load_order[i]].nb_routes;j++)
		{
			
			if(!routes[id_routes[j]])
			{
				for(offset = 0;offset<P;offset++)
				{

					if(message_no_collisions( g, id_routes[j], offset,message_size,FORWARD,P))
					{
					
						begin_offset = route_length(g,id_routes[j]) + offset;
						offset_back = begin_offset;
						back_found = 1;
						best_begin = 0;
						best_back = INT_MAX;
						while( !message_no_collisions( g, id_routes[j], offset_back,message_size,BACKWARD,P) )
						{

							offset_back++;
							if(offset_back == (P+begin_offset))
							{
								back_found = 0;
								break;
							}

							
						}
						if(back_found)
						{
							if((offset_back - begin_offset) < (best_back-best_begin))
							{
								best_offset = offset;
								best_begin = begin_offset;
								best_back = offset_back;
								break;
							}
							
						}

					}
						
				}
				if(best_back == INT_MAX)
				{

					a->offset_forward[id_routes[j]]=-1;
					a->offset_backward[id_routes[j]]=-1;
					a->waiting_time[id_routes[j]]=-1;
					routes[id_routes[j]] = 1;
					
				}
				else
				{
					/*if( (2*route_length( g,id_routes[j]) + best_back-best_begin ) > tmax )
					{
						a->offset_forward[id_routes[j]]=-1;
						a->offset_backward[id_routes[j]]=-1;
						a->waiting_time[id_routes[j]]=-1;
						routes[id_routes[j]] = 1;
					}
					else
					{*/
						fill_period(g,id_routes[j],best_offset,message_size,FORWARD,P);
						a->offset_forward[id_routes[j]]=best_offset;
						g->routes[id_routes[j]][0]->routes_delay_f[id_routes[j]] = best_offset;
						
						fill_period(g,id_routes[j],best_back,message_size,BACKWARD,P);
						a->offset_backward[id_routes[j]]=best_back;
						a->waiting_time[id_routes[j]]=best_back-best_begin;
						g->routes[id_routes[j]][0]->routes_delay_b[id_routes[j]] = best_back-best_begin;
						routes[id_routes[j]] = 1;	
						a->nb_routes_scheduled++;
					//}
					
				}
				
				
			}
		}
		free(id_routes);
	}
	free(load_order);
	if(a->nb_routes_scheduled == g->nb_routes)
	{
		a->all_routes_scheduled = 1;

	}
	else
	{
		a->all_routes_scheduled = 0;
	}
	if(verifie_solution( g,message_size))
	{
		printf("La solution n'est pas correcte Loaded greedy %d (error %d) ",mode,verifie_solution( g,message_size));
		affiche_graph(g,P,stdout);
		exit(82);
	}
	return a;
	

}

Assignment loaded_greedy(Graph * g, int P, int message_size)
{
	return greedy_by_arcs(g,P,message_size,0);
}

Assignment loaded_greedy_longest(Graph * g, int P, int message_size)
{
	return greedy_by_arcs(g,P,message_size,1);
}

Assignment loaded_greedy_collisions(Graph * g, int P, int message_size)
{
	return greedy_by_arcs(g,P,message_size,2);
}



int oderinarc(int* release, int * budget, int  P , int size,int message_size,int * order, int * delay,int * id,int * period)
{
	/*printf("ARc :");
	for(int i=0;i<size;i++)printf(" %d ",id[i]);printf("\n");
		printf("Release :");
	for(int i=0;i<size;i++)printf(" %d ",release[i]);printf("\n");
		printf("Budget :");
	for(int i=0;i<size;i++)printf(" %d ",budget[i]);printf("\n");*/

	int offset = 0;
	//Recherche de la premiere route à arriver.
	int min = release[0];
	//int max;
	int current_route = 0;
	for(int i=1;i<size;i++)
	{
		if(release[i] < min)
		{
			min = release[i];
			current_route = i;
		}
	}
	//int order = calloc(size,sizeof(int));
	order[0]=id[current_route];
	delay[id[current_route]] = 0;
	int Per[size];
	Per[0]=release[current_route];
	int firstorder = order[0];

	offset = release[current_route]+message_size;
	int begin_offset = release[current_route];
	fill_Per(period, id[current_route], release[current_route], message_size,P);
	//printf("Premiere route : %d(pos %d dans le tableau) release %d\n",id[current_route],current_route,release[current_route]);
	
	release[current_route]= INT_MAX;

	for(int i=1;i<size;i++)
	{
		//Choix de la route éligible
		current_route = -1;
		min = INT_MAX;
		for(int k=0;k<size;k++)
		{
			if(release[k]<offset)//route dispo
			{
				if(budget[k]<min)//le plus de reste a parcourir
				{
					min = budget[k];
					current_route = k;
				}
			}
		}

		//Si on ne trouve pas de routes eligible
		if(current_route == -1)
		{
			min = release[0];
			current_route = 0;
			for(int k=1;k<size;k++)
			{
				if(release[k] < min)
				{
					min = release[k];
					current_route = k;
				}
			}
		}
		//printf("route eligible : %d(pos %d dans le tableau) release %d\n",id[current_route],current_route,release[current_route]);
		//Detection de bug
		if(release[current_route]==INT_MAX){printf("PAS NORMAL;\n");exit(93);}
		order[i]=id[current_route];
		if(release[current_route] < offset )
		{
			
			delay[id[current_route]] =  offset - release[current_route];
			//printf(" buffer de %d sur la route %d (offset etait à %d, release à %d)\n",delay[id[current_route]],id[current_route],offset,release[current_route]);
		}
		else
		{
			offset = release[current_route];
			delay[id[current_route]] = 0;
			//printf("Offset est à %d delay route %d a 0\n",offset,id[current_route]);
		}
		/*
		int ok = 0;
		for(int add =0;add<P;add++)
		{

			if(cols_check(Per,offset+add,message_size,P,i))
			{
				
				delay[id[current_route]] += add;
				//printf("Délai additionel de %d sur la route %d \n",add,id[current_route]);
				offset += add;
				ok = 1;
				break;
			}

		}
		
		if(ok == 0)
		{
			
			return 0;
		}*/
		int check_value = 1;
		int total_check = 0;
		while(check_value)
		{
			//printf("Check value for route %d offset %d \n",current_route,offset+total_check);
			check_value = cols_check(Per,offset+total_check,message_size,P,i);
			//printf("check = %d \n ",check_value);
			total_check += check_value;
			if(total_check >= P)
			{
				return 0;
			}
		}
		offset += total_check;

		//La calcul suivant cherche a determiner si une route est placée dans la period ou elle esst dispo ou non
		//il faut d'abbord chercher le debut de la periode courrante
		//begin offset est forcement le plus petit release, donc on cherche a decaler begin offset de P* le nombre de period qu'il faut
		int begin2 = begin_offset;
	
		while( release[current_route] > begin2 + P )
		{
			if(release[current_route]>(begin2 + P))
			{
				begin2 +=P;
			}
		}

		if(VOISINAGE)
		{
			//une fois qu'on est la, le release est inferieur a la fin de la permiere periode, donc si offset dépasse, alors on met ordre a -ordre
			if((offset >= P+begin2))
			{
				if(id[current_route] == 0)
					order[i]= INT_MAX;
				else
					order[i] = -order[i];
			}	
		}
		//printf("i = %d, oder = %d , offset %d \n",i,order[i],offset);
		
		delay[id[current_route]] += total_check;


		Per[i]=offset;
		fill_Per(period, id[current_route], offset, message_size,P);
		offset+=message_size;
		release[current_route]=INT_MAX;
		budget[current_route]=INT_MAX;
		//printf("nouveau tour de boucle, offset = %d \n",offset);
	}
	for(int i=0;i<size;i++)
	{
		Per[i] = Per[i] - begin_offset;
		Per[i] = Per[i] % P;
	}
	tri_bulles_inverse(Per,order,size);

	if(order[0] != firstorder)
	{
		printf("impossible, le tri a mélangé les ordres (greedy) \n");
		exit(56);
	}
	return 1;
}


int greedy_deadline(Graph * g, int P, int message_size)
{
	Period_kind kind;
	int CL;

 	//for each contention  level
 	for(int i=0;i<g->contention_level;i++)
 	{

 		if(i<g->contention_level/2)
 		{	
 			CL = i;
 			kind = FORWARD;
 		}
 		else
 		{
 			CL = g->contention_level-i-1;
 			kind = BACKWARD;
 		}

 		for(int j=0;j<g->arc_pool_size;j++)
 		{

 			if(g->arc_pool[j].contention_level == CL)
 			{
 				int release[g->arc_pool[j].nb_routes];
 				int budget[g->arc_pool[j].nb_routes];
 				int ids[g->arc_pool[j].nb_routes];
 				if(kind == FORWARD)
 				{
 					for(int l=0;l<g->arc_pool[j].nb_routes;l++)
 					{
 						ids[l] = g->arc_pool[j].routes_id[l];
 						release[l] = route_length_untill_arc(g,g->arc_pool[j].routes_id[l], &g->arc_pool[j],FORWARD);
 					//	printf("retourlenghtuntillarc %d \n",route_length_untill_arc(g,g->arc_pool[j].routes_id[l], &g->arc_pool[j],FORWARD));
 						budget[l]= route_length(g,g->arc_pool[j].routes_id[l])*2 - route_length_untill_arc(g,g->arc_pool[j].routes_id[l], &g->arc_pool[j],FORWARD);
 						//printf("route %d(%d) : %d = %d - %d\n ",ids[l], release[l],budget[l], route_length(g,g->arc_pool[j].routes_id[l])*2,route_length_untill_arc_without_delay(g,g->arc_pool[j].routes_id[l], &g->arc_pool[j],FORWARD));
 					}
 					//printf("\n");
 					if(!oderinarc(release,  budget,P ,  g->arc_pool[j].nb_routes,message_size,g->arc_pool[j].routes_order_f, g->arc_pool[j].routes_delay_f,ids,g->arc_pool[j].period_f))
 						return 0;
 				}
 					
 				else
 				{
 					for(int l=0;l<g->arc_pool[j].nb_routes;l++)
 					{
 						ids[l] = g->arc_pool[j].routes_id[l];
 						release[l] = route_length_with_buffers_forward(g,g->arc_pool[j].routes_id[l]) + route_length_untill_arc(g,g->arc_pool[j].routes_id[l], &g->arc_pool[j],BACKWARD);
 						budget[l]= route_length(g,g->arc_pool[j].routes_id[l])*2 -route_length_with_buffers_forward(g,g->arc_pool[j].routes_id[l]) -route_length_untill_arc(g,g->arc_pool[j].routes_id[l], &g->arc_pool[j],BACKWARD);
 					}
 					if(!oderinarc( release, budget,P ,  g->arc_pool[j].nb_routes,message_size,g->arc_pool[j].routes_order_b, g->arc_pool[j].routes_delay_b,ids,g->arc_pool[j].period_b))
 						return 0;
 				}
 					
 			
 			}
 				

 		}
 	}

 	
	return 1;

}
int greedy_deadline_assignment(Graph * g, int P, int message_size)
{
	
//printf("\n\n\n\nnew greedy \n\n\n");
	reset_periods( g, P);

	if(!greedy_deadline(g, P, message_size))
	{
		printf("Error, L'algo d'initialisation greedy n'a pas pu trouver de solutions\n");
		return 0;
	}

	int t = travel_time_max_buffers(g);
	if(verifie_solution( g,message_size))
	{
		printf("La solution trouvée par l'algo greedy de base n'est pas correcte GD (error %d) ",verifie_solution( g,message_size));
		affiche_graph(g,P,stdout);
		exit(84);
	}
	if(VOISINAGE)
	{
		if( assignment_with_orders_vois1(g,P,message_size,1)==0)
			{
				printf("Assignmentwithordervois1 ne trouve pas de solution alors que l'algo d'init si ?? \n");
				exit(87);
			}
	}
	else
	{
		if( assignment_with_orders(g,P,message_size,1)==0)
		{
			printf("Assignmentwithorder ne trouve pas de solution alors que l'algo d'init si ?? \n");
			exit(87);
		}
	}


	int t2 = travel_time_max_buffers(g);

	if(t!= t2)
	{
		printf("Les deux algos sont pas pareils, impossible (%d %d).\n",t,t2);
		exit(85);
	}
	if(verifie_solution( g,message_size))
	{
		printf("La solution de assignment with order n'est pas correcte (error %d) ",verifie_solution( g,message_size));
		exit(86);
	}

	return t2;
}