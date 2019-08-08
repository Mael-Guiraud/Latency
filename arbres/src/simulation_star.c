
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
#include "spall_waiting.h"
#include "reusePrime.h"
#include "fptPAZL.h"
#include "color.h"
#include <unistd.h>
#include "jsondump.h"
#include <string.h>
#include <math.h>
char * strcmpt(float nb_simuls)
{	
	
	char * str = (char*)malloc(sizeof(char)*64);
	sprintf(str,"\r%%%dd/%%d",(int)log10(nb_simuls));
	return str;
}

void star_search_random_routes()
{

	srand(time(NULL));

	int message_size = MESSAGE_SIZE;
	int P=PERIOD;
	float nb_simuls = NB_SIMULS;
	Assignment a,a2,a3 ;
	Graph g;
	char * str= strcmpt(nb_simuls);
	
	int cmpt_fail = 0;
	char nom [64];
	
	float moy = 0;
	float moy2 = 0;
	float moy3 = 0;
	float min = P;
	float min2 = P;
	float min3 = P;

	for( int j=0; j<(int)nb_simuls; j++ )
	{
		g = init_graph_etoile(P/MESSAGE_SIZE,P);
		a = greedy_PRIME(g, P, message_size);
		reset_periods(g,P);
		a2 = PRIME_reuse(g, P, message_size);
		a3 = linear_search(g, P/MESSAGE_SIZE,  P,  MESSAGE_SIZE);
	
			moy += a->nb_routes_scheduled;

			moy2 += a2->nb_routes_scheduled;
			moy3 += a3->nb_routes_scheduled;
		
		if(min > a->nb_routes_scheduled)
		{
			
				min = a->nb_routes_scheduled;
		}
				
		if(min2 > a2->nb_routes_scheduled)
		{
		
				min2 = a2->nb_routes_scheduled;
		}
		if(min3 > a3->nb_routes_scheduled)
		{
	
				min3 = a3->nb_routes_scheduled;
				print_graphvitz(g,"../view/graphminFPT");
		}

		if(a3->nb_routes_scheduled>a2->nb_routes_scheduled) 
		{
			cmpt_fail++;
			if(cmpt_fail< 5)
			{
				printf("\nCas N° %d :\n",cmpt_fail);
				affiche_graph_routes(g, stdout);
				sprintf(nom,"../view/graph%d",cmpt_fail);
				print_graphvitz(g,nom);
				printf("Solution pour Greedy SWAP :\n");
				affiche_assignment(a2,g.nb_routes,stdout);
				affiche_period_star(g,P,stdout);
				printf("Solution pour FPT :\n");
				affiche_assignment(a3,a3->nb_routes_scheduled,stdout);
				//affiche_graph(g,P,stdout);
				printf("\n-------------------------\n");
			}


		}
		if(a3->nb_routes_scheduled<a2->nb_routes_scheduled) 
		{
			printf("THIS IS IMPOSSIBLE : FPT does not found an existing solution.\n");
			exit(44);
		}

		free_assignment(a);
		free_assignment(a2);
		free_assignment(a3);
		
		free_graph(g);	
		
		fprintf(stdout,str,j+1,(int)nb_simuls);
	}
	printf("\nFPT ne passe au minimum que %f routes .\n",min3);
	printf("Instance dans ../view/graphminFPT.pdf");

	printf("\nP = %d (%d routes)  :   FPTMoy   SwapMoy   GreedyMoy   FPTMin   SwapMin   GreedyMin\n ",P,P/message_size);
	  printf("                     %f %f  %f  %f  %f  %f  ",(moy3/nb_simuls)/((float)P/(float)MESSAGE_SIZE),(moy2/nb_simuls)/((float)P/(float)MESSAGE_SIZE),(moy/nb_simuls)/((float)P/(float)MESSAGE_SIZE),min3/((float)P/(float)MESSAGE_SIZE),min2/((float)P/(float)MESSAGE_SIZE),min/((float)P/(float)MESSAGE_SIZE));

	printf("\n");

	
	
	
	free(str);


}

void star_all_routes_lenghts()
{

	srand(time(NULL));
	int message_size = MESSAGE_SIZE;
	int P=PERIOD;
	int nb_routes = P/MESSAGE_SIZE;
	float nb_simuls = pow((double)P,(double)nb_routes-1);
	int taille_max=nb_routes-1;
	int tab[taille_max+1];
	

	Assignment a,a2,a3 ;
	Graph g;

	int cmpt_fail = 0;
	float moy = 0;
	float moy2 = 0;
	float moy3 = 0;
	float min = P;
	float min2 = P;
	float min3 = P;
	char * str = strcmpt(nb_simuls);
	int nb_boucles = (int)pow((double)P,(double)nb_routes-1);
	//char nom [64];
	
	
	// Pour tout les nombres de 0 a P^(nbroutes -1)
	#pragma omp parallel for private(tab,a,a2,a3,g) schedule(dynamic,500) if(PARALLEL) 
	for( int j=0; j<nb_boucles; j++ )
	{

		//On ecrit le nombre en base P
		tab[0]=0;
		for(int i=0;i<taille_max;i++)
		{
			tab[i+1]=0;
		}
		chgt_base(P,j,tab+1);



		g =  init_graph_etoile_values_set(nb_routes,P,tab);
		a = greedy_PRIME(g, P, message_size);
		reset_periods(g,P);
		a2 = PRIME_reuse(g, P, message_size);
	

		#pragma omp atomic update
			moy += a->nb_routes_scheduled;
		#pragma omp atomic update			
			moy2 += a2->nb_routes_scheduled;
		
		
		#pragma omp critical
		{
			if(min > a->nb_routes_scheduled)
			{
					min = a->nb_routes_scheduled;
					print_graphvitz(g,"../view/graphminGreedy");
			}
		}
		#pragma omp critical
		{			
			if(min2 > a2->nb_routes_scheduled)
			{
					min2 = a2->nb_routes_scheduled;
					print_graphvitz(g,"../view/graphminSWAPT");
			}
		}

		if(a2->nb_routes_scheduled <= 6)
		{
			a3 = linear_search(g, P/MESSAGE_SIZE,  P,  MESSAGE_SIZE);
			#pragma omp atomic update
				moy3 += a3->nb_routes_scheduled;
			#pragma omp critical
			{
				if(min3 > a3->nb_routes_scheduled)
				{
				
						min3 = a3->nb_routes_scheduled;
						print_graphvitz(g,"../view/graphminFPT");
				}
	
			}
			if(a3->nb_routes_scheduled>a2->nb_routes_scheduled) 
			{
				cmpt_fail++;
			}
			free_assignment(a3);
		}
		else
		{
			#pragma omp atomic update
				moy3 += a2->nb_routes_scheduled;
		}
	

		/*if(a3->nb_routes_scheduled>a2->nb_routes_scheduled) 
		{
			cmpt_fail++;

			if( (cmpt_fail % SHOW_EVERY) == 1)
			{
				printf("\nCas N° %d :\n",cmpt_fail);
				affiche_graph_routes(g, stdout);
				sprintf(nom,"../view/graph%d",cmpt_fail);
				print_graphvitz(g,nom);
				printf("Solution pour Greedy SWAP :\n");
				affiche_assignment(a2,g.nb_routes,stdout);
				affiche_period_star(g,P,stdout);
				printf("Solution pour FPT :\n");
				affiche_assignment(a3,a3->nb_routes_scheduled,stdout);
				printf("\n-------------------------\n");
			}


		}
		if(a3->nb_routes_scheduled<a2->nb_routes_scheduled) 
		{
			printf("THIS IS IMPOSSIBLE : FPT does not found an existing solution.\n");
			exit(44);
		}*/
	

		

		free_assignment(a);
		free_assignment(a2);
		
		free_graph(g);	

		#pragma omp critical
			fprintf(stdout,str,j+1,(int)nb_simuls);
	}
	printf("\nIl y a eu %d cas dans lequel FPT est meilleur que swap .\n",cmpt_fail);
	printf("\nFPT ne passe au minimum que %f routes .\n",min3);
	printf("Instance dans ../view/graphminFPT.pdf");

	printf("\nP = %d (%d routes)  :   FPTMoy   SwapMoy   GreedyMoy   FPTMin   SwapMin   GreedyMin\n ",P,nb_routes);
	printf("                     %f %f  %f  %f  %f  %f  ",(moy3/nb_simuls)/((float)P/(float)MESSAGE_SIZE),(moy2/nb_simuls)/((float)P/(float)MESSAGE_SIZE),(moy/nb_simuls)/((float)P/(float)MESSAGE_SIZE),min3/((float)P/(float)MESSAGE_SIZE),min2/((float)P/(float)MESSAGE_SIZE),min/((float)P/(float)MESSAGE_SIZE));

	printf("\n");

	
	
	
	free(str);


}

