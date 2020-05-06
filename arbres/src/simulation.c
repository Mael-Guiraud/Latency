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
#include "voisinage.h"
#include <string.h>
#include <math.h>
#include <limits.h>
#include "borneInf.h"
#include "fpt.h"


void test()
{
	//unsigned int seed = 1588782688;
	unsigned int seed = time(NULL);
	FILE * f = fopen("logs.txt","w");
	if(!f){printf("ERROR oppening file logs.txt\n");exit(36);}
	printf("\n\n ----------- TEST ON ONE TOPOLOGY ---------- \n");
	fprintf(f,"\n\n ----------- TEST ON ONE TOPOLOGY ---------- \n");
	srand(seed);
	printf("%d %u %x seed \n",seed,seed,seed);
	int P ;
	int message_size = MESSAGE_SIZE;
	int tmax;
	int margin = rand()%MARGIN_MAX;
	char buf[128];
	char buf_dot[128];
	int * tmp;

	sprintf(buf,"mkdir -p ../view/assignments");
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
				
	

	Graph  g = init_graph_random_tree(STANDARD_LOAD);
	if(FIXED_PERIOD_MOD)
	{
		P = PERIOD;
	}
	else
		P= (load_max(&g)*MESSAGE_SIZE)/STANDARD_LOAD;
	if(TMAX_MOD)
	{
		tmax = TMAX;
	}
	else
		tmax = longest_route(&g)*2 + margin;

			
	printf("Parameters : \n");
	printf("	Fixed period   : ");if(FIXED_PERIOD_MOD){printf("ON ");}else{printf("OFF ");}printf("| P = %d .\n",P);
	if(P < load_max(&g)*MESSAGE_SIZE)
			printf(YEL "WARNING: not enought space to schedule all the messages on the loadest link.\n" RESET);
	printf("	Margin(random) : %d (note : if TMAX is fixed, this parameter is not efficient).\n",margin);
	printf("	Fixed tmax     : ");if(TMAX_MOD){printf("ON ");}else{printf("OFF");}printf("| TMAX = %d .\n",tmax);
	if(longest_route(&g)*2 > TMAX)
			printf(YEL "WARNING! TMAX is lower than the longest route. \n" RESET);
	printf("	Message size   : %d .\n",message_size);
	printf("	Routes Synch   : ");if(SYNCH){printf("ON ");}else{printf("OFF ");}printf("\n");

	fprintf(f,"Parameters : \n");
	fprintf(f,"	Fixed period   : ");if(FIXED_PERIOD_MOD){fprintf(f,"ON ");}else{fprintf(f,"OFF ");}fprintf(f,"| P = %d .\n",P);
	if(PERIOD < load_max(&g)*MESSAGE_SIZE)
			fprintf(f,YEL "WARDNING: not enought space to schedule all the messages on the loadest link.\n" RESET);
	fprintf(f,"	Margin(random) : %d (note : if TMAX is fixed, this parameter is not efficient).\n",margin);
	fprintf(f,"	Fixed tmax     : ");if(TMAX_MOD){fprintf(f,"ON ");}else{fprintf(f,"OFF");}fprintf(f,"| TMAX = %d .\n",tmax);
	if(longest_route(&g)*2 > TMAX)
			fprintf(f,YEL "WARNING! TMAX is higher than the longest route. \n" RESET);
	fprintf(f,"	Message size   : %d .\n",message_size);
	fprintf(f,"	Routes Synch   : ");if(SYNCH){fprintf(f,"ON ");}else{fprintf(f,"OFF ");}fprintf(f,"\n");


	printf(" ---- \n Graph * generated ...\n");
	fprintf(f,"\n ---- \n Graph * generated ...\n");
	affiche_graph(&g,P,f);
	printf("Number of routes on the graph : %d .\n",g.nb_routes);
	printf("Number of routes on the loadest arc : %d .\n",load_max(&g));
	printf("Length of the longest route (%d)(%d) (length)(*2).\n",longest_route(&g),longest_route(&g)*2);
	fprintf(f,"Length of the longest route (%d)(%d) (length)(*2).\n",longest_route(&g),longest_route(&g)*2);
	fprintf(f,"Number of routes on the loadest arc : %d .\n",load_max(&g));
	fprintf(f,"Load of each links : ");
	tmp = load_links(&g);
	affiche_tab(tmp,g.arc_pool_size,f);
	free(tmp);


	printf("------- \n TESTING ALGORITHMS : \n");
	fprintf(f,"\n ------- \n TESTING ALGORITHMS : \n");


	char nom[32];
	printf("Borneinf %d \n",borneInf(&g,P,message_size));
	
	sprintf(nom,"borneinf");
	
	reset_periods(&g,P);reinit_delays(&g);

	int coupes[NB_COUPES];
	for(int i=0;i<NB_COUPES;i++)coupes[i]=1; 
		double coupes_m[NB_COUPES];
	int fpt = branchbound( &g, P,  message_size,coupes,coupes_m);
	
	printf("FPT = %d \n",fpt);
	
	reset_periods(&g,P);reinit_delays(&g);
		
	float nb_pas;	
	int recuits = recuit( &g, P, message_size,1000,&nb_pas);
	printf("Recuit %d, %f pas \n",recuits,nb_pas);	
	sprintf(nom,"recuit");
	printf("Valeur de verifie_solution = %d \n",verifie_solution(&g,message_size));
	sprintf(buf_dot,"../view/assignments/%sf.dot",nom);
	print_assignment(&g,P,buf_dot);
	sprintf(buf,"dot -Tpdf %s -o ../view/assignments/%sf.pdf",buf_dot,nom);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	sprintf(buf,"rm -rf %s",buf_dot);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	sprintf(buf_dot,"../view/assignments/%sb.dot",nom);
	print_assignment_backward(&g,P,buf_dot);
	sprintf(buf,"dot -Tpdf %s -o ../view/assignments/%sb.pdf",buf_dot,nom);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	sprintf(buf,"rm -rf %s",buf_dot);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	//free_assignment(a);
	fprintf(f,"Graph after : \n");affiche_graph(&g,P,f);
	fprintf(f,"Reseting periods ...\n");
	reset_periods(&g,P);reinit_delays(&g);
	if(recuits<fpt)
	{
		printf(RED "!!!!!!!!!!FPT trouve moins bien que le recuit!!!!!!!!!!!! \n" RESET);
		exit(44);
	}
	else
	{
		printf(GRN "FPT trouve au moins aussi bien que le recuit ! \n" RESET);
	}
	/*
	int descent = descente( g, P, message_size,0);
	printf("descente %d \n",descent);	
	sprintf(nom,"descente");
	printf("Valeur de verifie_solution = %d \n",verifie_solution(g,message_size));
	sprintf(buf_dot,"../view/assignments/%sf.dot",nom);
	print_assignment(g,P,buf_dot);
	sprintf(buf,"dot -Tpdf %s -o ../view/assignments/%sf.pdf",buf_dot,nom);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	sprintf(buf,"rm -rf %s",buf_dot);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	sprintf(buf_dot,"../view/assignments/%sb.dot",nom);
	print_assignment_backward(g,P,buf_dot);
	sprintf(buf,"dot -Tpdf %s -o ../view/assignments/%sb.pdf",buf_dot,nom);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	sprintf(buf,"rm -rf %s",buf_dot);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	//free_assignment(a);
	fprintf(f,"Graph after : \n");affiche_graph(g,P,f);
	fprintf(f,"Reseting periods ...\n");
	reset_periods(g,P);reinit_delays(g);
	if(descent<fpt)
	{
		printf(RED "!!!!!!!!!!FPT trouve moins bien que la descente!!!!!!!!!!!! \n" RESET);
		exit(44);
	}
	else
	{
		printf(GRN "FPT trouve au moins aussi bien que la descente ! \n" RESET);
	}
	int tabo = taboo( g, P, message_size,100);
	printf("taboo %d \n",tabo);	
	sprintf(nom,"taboo");
	printf("Valeur de verifie_solution = %d \n",verifie_solution(g,message_size));
	sprintf(buf_dot,"../view/assignments/%sf.dot",nom);
	print_assignment(g,NULL,P,buf_dot);
	sprintf(buf,"dot -Tpdf %s -o ../view/assignments/%sf.pdf",buf_dot,nom);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	sprintf(buf,"rm -rf %s",buf_dot);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	sprintf(buf_dot,"../view/assignments/%sb.dot",nom);
	print_assignment_backward(g,NULL,P,buf_dot);
	sprintf(buf,"dot -Tpdf %s -o ../view/assignments/%sb.pdf",buf_dot,nom);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	sprintf(buf,"rm -rf %s",buf_dot);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	//free_assignment(a);
	fprintf(f,"Graph after : \n");affiche_graph(g,P,f);
	fprintf(f,"Reseting periods ...\n");
	reset_periods(g,P);reinit_delays(g);
	if(tabo<fpt)
	{
		printf(RED "!!!!!!!!!!FPT trouve moins bien que le taboo!!!!!!!!!!!! \n" RESET);
		exit(44);
	}
	else
	{
		printf(GRN "FPT trouve au moins aussi bien que le taboo ! \n" RESET);
	}*/
	

	printf("\n printing graphvitz ...");print_graphvitz(&g,"../view/view.dot");printf("Ok.\n");
	printf("\n printing python ...");print_python(&g);printf("Ok.\n");
	printf("\n printing json arcs ..."); print_json_arcs(&g);
	printf("Logs in logs.txt\n");
	free_graph(&g);
	fclose(f);


}


void simuldistrib(int seed)
{
	
	int nb_algos = 8 ;
	char * noms[] = {"GreedyDeadline","BorneInfSort","Descente","Taboo","DescenteX","BorneInfSimons","Recuit","FPT"};

	srand(seed);
	int message_size = MESSAGE_SIZE;
	Graph  g;
	int P ;
	int coupes[NB_COUPES];
	double coupes_m[NB_COUPES];
	for(int i=0;i<NB_COUPES;i++)
	{
		coupes[i]=1;
	}

	char buf[256];
	FILE * f[nb_algos];
	for(int i=0;i<nb_algos;i++)
	{
		sprintf(buf,"../data/%s.plot",noms[i]);
		f[i] = fopen(buf,"w");
		if(!f[i])perror("Error while opening file\n");
	}


	int a=0;
	

	
	
	int time[nb_algos];
	int res[nb_algos][NB_SIMULS];
	float nb_pas[4];
	
	for(int i=0;i<3;i++)nb_pas[i] = 0;
	#pragma omp parallel for private(g,P,a,time)  if(PARALLEL)
	for(int i=0;i<NB_SIMULS;i++)
	{
		saut:
		a = 0;

		g= init_graph_random_tree(STANDARD_LOAD);
		int l = 2*longest_route(&g);
		
		//printf("%d \n",longest_route);
		if(FIXED_PERIOD_MOD)
		{
			if(PERIOD < load_max(&g)*MESSAGE_SIZE)
				printf("			WARNING, not enought space to schedule all the message on the loadest link.\n");
			P = PERIOD;
		}
		else
			P= (load_max(&g)*MESSAGE_SIZE)/STANDARD_LOAD;
		
		for(int algo = 0;algo<nb_algos;algo++)
		{
			a= 0;
			//printf("thread %d Starting algo %d :\n",omp_get_thread_num(),algo);
			switch(algo){
				case 5:
					time[algo] = borneInf( &g, P, message_size)-l;	
					//printf("%d longest_route\n",l);
				break;
				case 1:
					time[algo] = borneInf2( &g, message_size)-l;	
				break;
				case 2:
					//printf("DESCENTE \n\n\n\n\n\n\n\n\n");
					a = descente( &g, P, message_size,0,&nb_pas[0]);
					/*if(a)
						nb_pas[0] += a->nb_routes_scheduled;*/
				break;
				case 3:
					a = taboo( &g, P, message_size,100);
					if(a)
						nb_pas[2] += 100;
				break;
				case 4:
					a = best_of_x( &g, P, message_size,10,&nb_pas[1]);
					/*if(a)
						nb_pas[2] += a->nb_routes_scheduled;*/
				break;
				case 0:
					a =  greedy_deadline_assignment( &g, P, message_size);
					if(!a)
					{
						free_graph(&g);
						goto saut;
					}
				break;
				case 6:
					a = recuit( &g, P, message_size,1000,&nb_pas[3]);
					break;
				case 7:
					a = branchbound( &g, P, message_size,coupes,coupes_m);
					break;
				}


				if(a)
				{
					time[algo] = a-l;
					//printf("time %d = %d \n",algo, time[algo]);
					/*if(algo == 4)
					{
						time[algo] = a;//a->time-l;
					}
					else
						time[algo] = travel_time_max_buffers(g)-l;	*/

					//free_assignment(a);
				}
					
				else
				{
					if((algo != 1) && (algo != 5))
					{
						time[algo] = time[0];
					}
				}
					
					/*printf("algo %d \n",algo);
					int lenght=0;
					for(int j=1;j<g->nb_routes;j++)
					{
						lenght = route_length_with_buffers(g,j);

						printf("taille de la route %d = %d , %d\n",j,lenght,2*route_length(g,j));
						
						
					}*/
				
				//printf("Algo %d a = %p \n",algo,a);
				reset_periods(&g,P);
				
				reinit_delays(&g);
			
		}
		for(int algo = 0;algo<nb_algos;algo++)
		{
				#pragma omp critical
					res[algo][i]=time[algo];
		}
		if((time[2] > time[0]) )
			printf("La descente est moins bonne que l'algo greedy d'init  %d %d\n",time[0],time[2]);
		if((time[3]>time[0]) )
			printf("Le taboo est moins bon que l'algo greedy d'init \n");
		if((time[1]>time[5]) )
		{
			printf("Pb de born inf  simons %d calcul %d lenght %d\n",time[5]+l,time[1]+l,l);
			print_graphvitz(&g,"../view/view.dot");
			affiche_graph(&g,P,stdout);
			exit(45);
		}
		if(time[7]<time[6])
			{
				printf(GRN "FPT meilleur que recuit (%d %d).\n" RESET,time[7],time[6]);
			}
		for(int k=2;k<nb_algos;k++)
		{
			if((time[7]>time[k])&&k!=5)
			{
				printf(RED "Algo %d meilleur que fpt (%d %d).\n" RESET,k,time[k],time[7]);
				printf("\n printing graphvitz ...");print_graphvitz(&g,"../view/view.dot");printf("Ok.\n");
				exit(24);
			}
			
			if((time[k]<time[5]) || (time[k]<time[1]))
			{
				printf("On dépasse la borne inf, c'est chelou algo %d tps algo %d tmps borne 1 %d tmps borne 2 %d lenght %d\n",k,time[k],time[5],time[1],l);
				
				exit(4);

			}
		}
		
		free_graph(&g);
		fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
		fflush(stdout);
	}	

	//int max=0;
	for(int i=0;i<nb_algos;i++)
	{
		tri_bulles_classique_croissant(res[i],NB_SIMULS);
		/*if(res[i][NB_SIMULS-1] > max)
			max = res[i][NB_SIMULS-1];*/
		
	}
	//int interval_size = max / NB_POINTS;
	for(int i=0;i<nb_algos;i++)
	{
		for(int j=0;j<NB_SIMULS;j++)
		{
			fprintf(f[i],"%d \n",res[i][j]);
			//fprintf(f[i],"%d \n",(res[i][j]/interval_size)  * interval_size);
		}
		fclose(f[i]);
	}
	
	char * ylabels2[] = {"Nombre d'instances"};
	print_gnuplot_distrib("waiting",noms, nb_algos, "Cumulative distribution of the Latency", "Latency", ylabels2);
	
	printf("Nombre de pas moyen : Descente %f | DescenteX %f | Taboo %f | Recuit %f \n",nb_pas[0]/NB_SIMULS,nb_pas[1]/NB_SIMULS,nb_pas[2]/NB_SIMULS,nb_pas[3]/NB_SIMULS);
	
		
}
void testcoupefpt(int seed)
{
	   struct timeval tv1, tv2;
	 	
	
	char* noms[]= {"seconde dans premiere","i+1 pas collé","i Collé","routes suivantes avant i","BorneInf","plus petit id","NotFillInArc"};
	int message_size = MESSAGE_SIZE;
	Graph  g;
	int P ;
	int coupes[NB_COUPES];
	double coupes_m[NB_COUPES];
	int cuts[4];
	long long moy;
	long double hauteur_moy;
	long double calcul_moy;
	for(int i=0;i<4;i++)
	{
		cuts[i]=0;
	}
	for(int i=0;i<4;i++)
	{
		
		printf("Coupe %s :\n",noms[i]);
		cuts[i]=1;
		for(int j=0;j<4;j++)
		{
			moy =0;
			hauteur_moy = 0.0;
			calcul_moy = 0.0;
			cuts[j]=1;
			srand(seed);
			#pragma omp parallel for private(g,P,coupes,coupes_m)  if(PARALLEL)
			for(int k=0;k<NB_SIMULS;k++)
			{

				for(int l=0;l<NB_COUPES;l++)
				{
					if(l<4)
						coupes[l]=cuts[l];
					else
						coupes[l]=0;
				}
				
				g= init_graph_random_tree(STANDARD_LOAD);
				
				//printf("%d \n",longest_route);
				if(FIXED_PERIOD_MOD)
				{
					if(PERIOD < load_max(&g)*MESSAGE_SIZE)
						printf("			WARNING, not enought space to schedule all the message on the loadest link.\n");
					P = PERIOD;
				}
				else
					P= (load_max(&g)*MESSAGE_SIZE)/STANDARD_LOAD;
				
				gettimeofday (&tv1, NULL);
				branchbound( &g, P, message_size,coupes,coupes_m);
				gettimeofday (&tv2, NULL);	
		
				
				
				moy += coupes[i];

				hauteur_moy += coupes_m[i];
			//	printf("%d %f\n",coupes[i],coupes_m[i]);
				calcul_moy += time_diff(tv1,tv2);
				free_graph(&g);
				fprintf(stdout,"\r	%d/%d",k+1,NB_SIMULS);
				fflush(stdout);
			}
			if(i == j)
			{
				printf(".... - Seul [nombre moyen = %lld | Hauteur moyenne = %Lf | Temps moyen = %Lf] \n",moy/NB_SIMULS,hauteur_moy/NB_SIMULS,calcul_moy/NB_SIMULS);
			}
			else
			{
				printf(".... - Seul Avec %s = [nombre moyen = %lld | Hauteur moyenne = %Lf | Temps moyen = %Lf] \n",noms[j],moy/NB_SIMULS,hauteur_moy/NB_SIMULS,calcul_moy/NB_SIMULS);
				cuts[j]=0;
			}
		
			
		}
		printf("\n");

		cuts[i]=0;
		

	}	



	
		
}
