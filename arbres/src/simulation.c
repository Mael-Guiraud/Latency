#include <time.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "test.h"
#include "init.h"
#include "treatment.h"
#include "config.h"
#include "greedy_without_waiting.h"
#include "greedy_waiting.h"
#include "data_treatment.h"
#include "multiplexing.h"


void test()
{
	FILE * f = fopen("logs.txt","w");
	if(!f){printf("ERROR oppening file logs.txt\n");exit(36);}
	printf("\n\n ----------- TEST ON ONE TOPOLOGY ---------- \n\n");
	fprintf(f,"\n\n ----------- TEST ON ONE TOPOLOGY ---------- \n\n");
	srand(time(NULL));
	int P ;
	int message_size = MESSAGE_SIZE;
	int tmax;
	int margin = rand()%MARGIN_MAX;
	Assignment a;
	

	Graph g = init_graph_random_tree(STANDARD_LOAD);
	if(FIXED_PERIOD_MOD)
			P = PERIOD;
		else
			P= (load_max(g)*MESSAGE_SIZE)/STANDARD_LOAD;
		if(TMAX_MOD)
			tmax = TMAX;
		else
			tmax = longest_route(g) + margin;
			
	printf("Parameters : \n");
	printf("	Fixed period   : ");if(FIXED_PERIOD_MOD){printf("ON ");}else{printf("OFF ");}printf("| P = %d .\n",P);
	printf("	Fixed tmax     : ");if(TMAX_MOD){printf("ON\n");}else{printf("OFF");}printf("| TMAX = %d .\n",tmax);
	printf("	Message size   : %d .\n",message_size);
	printf("	Margin(random) : %d (note : if TMAX is fixed, this parameter is not efficient).\n",margin);
	fprintf(f,"Parameters : \n");
	fprintf(f,"	Fixed period   : ");if(FIXED_PERIOD_MOD){fprintf(f,"ON ");}else{fprintf(f,"OFF ");}fprintf(f,"| P = %d .\n",P);
	fprintf(f,"	Fixed tmax     : ");if(TMAX_MOD){fprintf(f,"ON\n");}else{fprintf(f,"OFF");}fprintf(f,"| TMAX = %d .\n",tmax);
	fprintf(f,"	Message size   : %d .\n",message_size);
	fprintf(f,"	Margin(random) : %d (note : if TMAX is fixed, this parameter is not efficient).\n",margin);


	printf("\n ---- \n Graph Generated ...\n");
	fprintf(f,"\n ---- \n Graph Generated ...\n");
	affiche_graph(g,P,f);
	printf("Number of routes on the loadest arc : %d .\n",load_max(g));
	fprintf(f,"Number of routes on the loadest arc : %d .\n",load_max(g));
	fprintf(f,"Load of each links : ");
	affiche_tab(load_links(g),g.arc_pool_size,f);

	printf("\n ------- \n Policies : \n");
	fprintf(f,"\n ------- \n Policies : \n");

	printf(" WITHOUT WAITING TIME : \n\n");
	fprintf(f," WITHOUT WAITING TIME : \n\n");
	printf("\n Greedy min lost : \n");
	fprintf(f,"\n Greedy min lost : \n");
	a = greedy_min_lost( g, P, message_size);
	if(a)
	{
		printf("Assignment found ! \n");
		fprintf(f,"Assignment found ! \n");
		affiche_assignment( a,g.nb_routes,f);
		free_assignment(a);
	}
	else
	{
		printf("No assignment found\n");
		fprintf(f,"No assignment found\n");
	}
	fprintf(f,"Graph after : \n");affiche_graph(g,P,f);
	printf("Reseting periods ...\n");
	fprintf(f,"Reseting periods ...\n");
	reset_periods(g,P);

	printf("\n Greedy prime: \n");
	fprintf(f,"\n Greedy prime: \n");
	a = greedy_PRIME( g, P, message_size);
	if(a)
	{
		printf("Assignment found ! \n");
		fprintf(f,"Assignment found ! \n");
		affiche_assignment( a,g.nb_routes,f);
		free_assignment(a);
	}
	else
	{
		printf("No assignment found\n");
		fprintf(f,"No assignment found\n");
	}
	fprintf(f,"Graph after : \n");affiche_graph(g,P,f);
	printf("Reseting periods ...\n");
	fprintf(f,"Reseting periods ...\n");
	reset_periods(g,P);
	

	printf("\n\n --------- \n\n WITH WAITING TIME \n");
	fprintf(f,"\n\n --------- \n\n WITH WAITING TIME \n");
	
	printf("\n  Greedy : \n");
	fprintf(f,"\n  Greedy : \n");
	a = greedy( g, P, message_size,tmax);
	if(a)
	{
		printf("Assignment found !\n");
		fprintf(f,"Assignment found !\n");
		affiche_assignment( a,g.nb_routes,f);
		printf("Travel time max = %d \n",travel_time_max( g, tmax, a));
		fprintf(f,"Travel time max = %d \n",travel_time_max( g, tmax, a));
		free_assignment(a);
	}
	else
	{
		printf("No assignment found\n");
		fprintf(f,"No assignment found\n");
	}
	fprintf(f,"Graph after : \n");affiche_graph(g,P,f);
	printf("Reseting periods ...\n");
	fprintf(f,"Reseting periods ...\n");
	reset_periods(g,P);
	
	printf("\n Loaded Greedy: \n");
	fprintf(f,"\n Loaded Greedy: \n");
	a = loaded_greedy( g, P, message_size,tmax);
	if(a)
	{
		printf("Assignment found !\n");
		fprintf(f,"Assignment found !\n");
		affiche_assignment( a,g.nb_routes,f);
		printf("Travel time max = %d \n",travel_time_max( g, tmax, a));
		fprintf(f,"Travel time max = %d \n",travel_time_max( g, tmax, a));
		free_assignment(a);
	}
	else
	{
		printf("No assignment found\n");
		fprintf(f,"No assignment found\n");
	}
	fprintf(f,"Graph after : \n");affiche_graph(g,P,f);
	printf("Reseting periods ...\n");
	fprintf(f,"Reseting periods ...\n");
	reset_periods(g,P);
	
	printf("\n Loaded Greedy longest: \n");
	fprintf(f,"\n Loaded Greedy longest: \n");
	a = loaded_greedy_longest( g, P, message_size,tmax);
	if(a)
	{
		printf("Assignment found !\n");
		fprintf(f,"Assignment found !\n");
		affiche_assignment( a,g.nb_routes,f);
		printf("Travel time max = %d \n",travel_time_max( g, tmax, a));
		fprintf(f,"Travel time max = %d \n",travel_time_max( g, tmax, a));
		free_assignment(a);
	}
	else
	{
		printf("No assignment found\n");
		fprintf(f,"No assignment found\n");
	}
	fprintf(f,"Graph after : \n");affiche_graph(g,P,f);
	printf("Reseting periods ...\n");
	fprintf(f,"Reseting periods ...\n");
	reset_periods(g,P);
	
	printf("\n Loaded Greedy collisions: \n");
	fprintf(f,"\n Loaded Greedy collisions: \n");
	a = loaded_greedy_collisions( g, P, message_size,tmax);
	if(a)
	{
		printf("Assignment found !\n");
		fprintf(f,"Assignment found !\n");
		affiche_assignment( a,g.nb_routes,f);
		printf("Travel time max = %d \n",travel_time_max( g, tmax, a));
		fprintf(f,"Travel time max = %d \n",travel_time_max( g, tmax, a));
		free_assignment(a);
	}
	else
	{
		printf("No assignment found\n");
		fprintf(f,"No assignment found\n");
	}
	fprintf(f,"Graph after : \n");affiche_graph(g,P,f);
	printf("Reseting periods ...\n");
	fprintf(f,"Reseting periods ...\n");
	reset_periods(g,P);

	fprintf(f,"\n --------- \n Statistical Multiplexing . \n\n");
	printf("\n --------- \n Statistical Multiplexing . \n\n");
	fprintf(f,"Fifo \n Testing the chain reaction of multiplexing ... \n");
	printf("Fifo \n Testing the chain reaction of multiplexing ... \n");
	int last_time_ellapsed =0;
	int time_ellapsed = 0;
	int nb_periods=1;
	while(1)
	{
		time_ellapsed = multiplexing(g, P, message_size, nb_periods, FIFO);
		printf("For %d period(s), the max time ellapsed is %d \n",nb_periods,time_ellapsed);
		fprintf(f,"For %d period(s), the max time ellapsed is %d \n",nb_periods,time_ellapsed);
		if(time_ellapsed > (last_time_ellapsed+last_time_ellapsed/10) )
		{
			nb_periods *= 10;
			printf("This time is more than 10%% higher than the previous. \n We increase the number of periods to %d.\n",nb_periods);
			fprintf(f,"This time is more than 10%% higher than the previous. \n We increase the number of periods to %d.\n",nb_periods);
			last_time_ellapsed = time_ellapsed;
		}
		else
			break;
	}

	printf("\n ------- \nTime ellapsed first \n Testing the chain reaction of multiplexing ... \n");
	fprintf(f,"\n ------- \nTime ellapsed first \n Testing the chain reaction of multiplexing ... \n");
	last_time_ellapsed =0;
	time_ellapsed = 0;
	nb_periods=1;
	while(1)
	{
		time_ellapsed = multiplexing(g, P, message_size, nb_periods, DEADLINE);
		printf("For %d period(s), the max time ellapsed is %d \n",nb_periods,time_ellapsed);
		fprintf(f,"For %d period(s), the max time ellapsed is %d \n",nb_periods,time_ellapsed);
		if(time_ellapsed > (last_time_ellapsed+last_time_ellapsed/10) )
		{
			nb_periods *= 10;
			printf("This time is more than 10%% higher than the previous. \n We increase the number of periods to %d.\n",nb_periods);
			fprintf(f,"This time is more than 10%% higher than the previous. \n We increase the number of periods to %d.\n",nb_periods);
			last_time_ellapsed = time_ellapsed;
		}
		else
			break;
	}



	printf("\n printing graphvitz ...");print_graphvitz(g);printf("Ok.\n");
	printf("\n printing python ...");print_python(g);printf("Ok.\n");
	printf("Logs in logs.txt\n");
	free_graph(g);
	fclose(f);

}


void simul(int seed,Assignment (*ptrfonction)(Graph,int,int,int),char * nom)
{
	srand(seed);
	int message_size = MESSAGE_SIZE;
	int nb_success;
	int tmax;
	double moy_routes_scheduled ;
	Graph g;
	int P ;
	char buf[64];
	sprintf(buf,"../data/%s",nom);
	FILE * f = fopen(buf,"w");
	Assignment a;
	if(!f)perror("Error while opening file\n");
	if(TMAX_MOD)
	{
		printf("\n The margin displayed on screen are not used in source code, tmax is %d .\n",TMAX);
	}
	for(int margin=MARGIN_MIN;margin<=MARGIN_MAX;margin+=MARGIN_GAP)
	{
		nb_success=0;
	
		moy_routes_scheduled = 0;
		
		for(int i=0;i<NB_SIMULS;i++)
		{
			g= init_graph_random_tree(STANDARD_LOAD);
			if(FIXED_PERIOD_MOD)
				P = PERIOD;
			else
				P= (load_max(g)*MESSAGE_SIZE)/STANDARD_LOAD;
			if(TMAX_MOD)
			{
				if(longest_route(g)*2 > TMAX)
					printf("WARNING! TMAX is higher than the longest route. \n");
				tmax = TMAX;
			}
			else
				tmax = longest_route(g) + margin;
			
			a = ptrfonction( g, P, message_size,tmax);
			if(a->all_routes_scheduled)
			{
				if(travel_time_max( g, tmax, a) != -1)
				{
					nb_success++;
				}
				
			}
			
			moy_routes_scheduled += a->nb_routes_scheduled;
			//printf("\n%d %d %d\n",g.nb_routes,a->nb_routes_scheduled,a->all_routes_scheduled);
			free_assignment(a);
	
			free_graph(g);
			fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
			fflush(stdout);
		}	
		printf("\n margin : %d success : %d/%d .\n",margin,nb_success,NB_SIMULS);
		fprintf(f,"%d %f %f\n",margin,nb_success/(float)NB_SIMULS,moy_routes_scheduled/(float)NB_SIMULS);
	}
	fclose(f);
	
		
}


void simul_period(int seed,Assignment (*ptrfonction)(Graph,int,int),char * nom)
{
	srand(seed);
	int message_size = MESSAGE_SIZE;
	int nb_success;
	Graph g;
	int P;
	double moy_routes_scheduled ;

	Assignment a;
	char buf[64];
	sprintf(buf,"../data/%s",nom);
	FILE * f = fopen(buf,"w");
	if(!f)perror("Error while opening file\n");
	for(double load=LOAD_MIN;load<=LOAD_MAX;load+=LOAD_GAP)
	{
		
		nb_success=0;
		moy_routes_scheduled = 0;
		#pragma omp parallel for private(g,P,a) schedule (static,1)
		for(int i=0;i<NB_SIMULS;i++)
		{
			
			//printf("%f %d\n",load, P);

			g= init_graph_random_tree(load);
			P= (load_max(g)*MESSAGE_SIZE)/load;

			
			a = ptrfonction( g, P, message_size);
			if(a->all_routes_scheduled)
			{
				
				#pragma omp atomic update
				nb_success++;
					
				
			}

			#pragma omp atomic update
			moy_routes_scheduled += a->nb_routes_scheduled;
			free_assignment(a);
			free_graph(g);
			fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
			fflush(stdout);
		}	
		printf("\n load : %f success : %d/%d \n",load,nb_success,NB_SIMULS);
		fprintf(f,"%f %f %f\n",load,nb_success/(float)NB_SIMULS,moy_routes_scheduled/(float)NB_SIMULS);
	}
	fclose(f);
	
		
}