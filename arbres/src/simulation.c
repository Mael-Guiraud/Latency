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



void test_one_algo(Graph g,int P, int message_size, int tmax, Assignment (*ptrfonctionnowaiting)(Graph,int,int),Assignment (*ptrfonctionwaiting)(Graph,int,int,int),char * nom,FILE * f)
{
	Assignment a;
	char buf[128];
	char buf_dot[128];
	printf(" %s: ",nom);
	fprintf(f,"\n %s: \n",nom);
	if(ptrfonctionnowaiting)
		a = ptrfonctionnowaiting( g, P, message_size);
	else
		a = ptrfonctionwaiting( g, P, message_size,tmax);
	if((a->all_routes_scheduled) && (travel_time_max( g, tmax, a) != -1) )
	{
		printf(GRN "OK | " RESET);
		fprintf(f,"Assignment found !\n");
		affiche_assignment( a,g.nb_routes,f);
		printf("Travel time max = %d | buffers %d \n",travel_time_max( g, tmax, a),travel_time_max_buffers(g) );
		fprintf(f,"Travel time max = %d \n",travel_time_max( g, tmax, a));
	}
	else
	{	
		
		printf(RED "Not OK -- " RESET);
		if(a->all_routes_scheduled)
		{
			printf("Travel time max = %d |buffers %d \n",travel_time_max( g, tmax, a), travel_time_max_buffers( g));
			fprintf(f,"Travel time max = %d \n",travel_time_max( g, tmax, a));
		}
		else
		{
			printf("No assignment found\n");
			fprintf(f,"No assignment found\n");
		}
			
	}
	printf("Valeur de verifie_solution = %d \n",verifie_solution(g,message_size));
	sprintf(buf_dot,"../view/assignments/%sf.dot",nom);
	print_assignment(g,a,P,buf_dot);
	sprintf(buf,"dot -Tpdf %s -o ../view/assignments/%sf.pdf",buf_dot,nom);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	sprintf(buf,"rm -rf %s",buf_dot);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	sprintf(buf_dot,"../view/assignments/%sb.dot",nom);
	print_assignment_backward(g,a,P,buf_dot);
	sprintf(buf,"dot -Tpdf %s -o ../view/assignments/%sb.pdf",buf_dot,nom);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	sprintf(buf,"rm -rf %s",buf_dot);
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	free_assignment(a);
	fprintf(f,"Graph after : \n");affiche_graph(g,P,f);
	fprintf(f,"Reseting periods ...\n");

		int borninf =borneInf(g,P,message_size);
	printf("La borne inf pour ce graph est :%d \n",borninf);
	printf("%d %d -------------------------\n",borninf,travel_time_max_buffers(g) );
	if(borninf >travel_time_max_buffers(g))
	{
		exit(49);
	}
	reset_periods(g,P);
}
void test()
{
	int seed;
	FILE * f = fopen("logs.txt","w");
	if(!f){printf("ERROR oppening file logs.txt\n");exit(36);}
	printf("\n\n ----------- TEST ON ONE TOPOLOGY ---------- \n");
	fprintf(f,"\n\n ----------- TEST ON ONE TOPOLOGY ---------- \n");
	srand(time(NULL));
	int P ;
	int message_size = MESSAGE_SIZE;
	int tmax;
	int margin = rand()%MARGIN_MAX;
	char buf[128];
	int * tmp;

	sprintf(buf,"mkdir -p ../view/assignments");
	if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
				
	

	Graph g = init_graph_random_tree(STANDARD_LOAD);
	if(FIXED_PERIOD_MOD)
	{
		P = PERIOD;
	}
	else
		P= (load_max(g)*MESSAGE_SIZE)/STANDARD_LOAD;
	if(TMAX_MOD)
	{
		tmax = TMAX;
	}
	else
		tmax = longest_route(g)*2 + margin;

			
	printf("Parameters : \n");
	printf("	Fixed period   : ");if(FIXED_PERIOD_MOD){printf("ON ");}else{printf("OFF ");}printf("| P = %d .\n",P);
	if(P < load_max(g)*MESSAGE_SIZE)
			printf(YEL "WARNING: not enought space to schedule all the messages on the loadest link.\n" RESET);
	printf("	Margin(random) : %d (note : if TMAX is fixed, this parameter is not efficient).\n",margin);
	printf("	Fixed tmax     : ");if(TMAX_MOD){printf("ON ");}else{printf("OFF");}printf("| TMAX = %d .\n",tmax);
	if(longest_route(g)*2 > TMAX)
			printf(YEL "WARNING! TMAX is lower than the longest route. \n" RESET);
	printf("	Message size   : %d .\n",message_size);
	printf("	Routes Synch   : ");if(SYNCH){printf("ON ");}else{printf("OFF ");}printf("\n");

	fprintf(f,"Parameters : \n");
	fprintf(f,"	Fixed period   : ");if(FIXED_PERIOD_MOD){fprintf(f,"ON ");}else{fprintf(f,"OFF ");}fprintf(f,"| P = %d .\n",P);
	if(PERIOD < load_max(g)*MESSAGE_SIZE)
			fprintf(f,YEL "WARDNING: not enought space to schedule all the messages on the loadest link.\n" RESET);
	fprintf(f,"	Margin(random) : %d (note : if TMAX is fixed, this parameter is not efficient).\n",margin);
	fprintf(f,"	Fixed tmax     : ");if(TMAX_MOD){fprintf(f,"ON ");}else{fprintf(f,"OFF");}fprintf(f,"| TMAX = %d .\n",tmax);
	if(longest_route(g)*2 > TMAX)
			fprintf(f,YEL "WARNING! TMAX is higher than the longest route. \n" RESET);
	fprintf(f,"	Message size   : %d .\n",message_size);
	fprintf(f,"	Routes Synch   : ");if(SYNCH){fprintf(f,"ON ");}else{fprintf(f,"OFF ");}fprintf(f,"\n");


	printf(" ---- \n Graph Generated ...\n");
	fprintf(f,"\n ---- \n Graph Generated ...\n");
	affiche_graph(g,P,f);
	printf("Number of routes on the graph : %d .\n",g.nb_routes);
	printf("Number of routes on the loadest arc : %d .\n",load_max(g));
	printf("Length of the longest route (%d)(%d) (length)(*2).\n",longest_route(g),longest_route(g)*2);
	fprintf(f,"Length of the longest route (%d)(%d) (length)(*2).\n",longest_route(g),longest_route(g)*2);
	fprintf(f,"Number of routes on the loadest arc : %d .\n",load_max(g));
	fprintf(f,"Load of each links : ");
	tmp = load_links(g);
	affiche_tab(tmp,g.arc_pool_size,f);
	free(tmp);


	printf("------- \n TESTING ALGORITHMS : \n");
	fprintf(f,"\n ------- \n TESTING ALGORITHMS : \n");

	//printf("Le voisinage donne un temps aller retour de %d.\n",descente(g,P,message_size));

	printf("- WITHOUT WAITING TIME : \n");
	fprintf(f," WITHOUT WAITING TIME : \n\n");
	
	//THE NAME MUST NOT CONTAIN SPACES
	//test_one_algo(g,P,message_size,tmax,&greedy_PRIME,NULL,"GreedyPrime",f);
	//test_one_algo(g,P,message_size,tmax,&PRIME_reuse,NULL,"PrimeReuse",f);
	//test_one_algo(g,P,message_size,tmax,&greedy_tics_won,NULL,"GreedyMinLost",f);
	
	

	printf("\n --------- \n- WITH WAITING TIME : \n");
	fprintf(f,"\n --------- \n WITH WAITING TIME \n");
	/*test_one_algo(g,P,message_size,tmax,NULL,&greedy,"Greedy",f);
	test_one_algo(g,P,message_size,tmax,NULL,&loaded_greedy,"LoadedGreedy",f);
	test_one_algo(g,P,message_size,tmax,NULL,&loaded_greedy_longest,"LoadedGreedyLongest",f);
	test_one_algo(g,P,message_size,tmax,NULL,&loaded_greedy_collisions,"LoadedGreedyCollisions",f);
	test_one_algo(g,P,message_size,tmax,NULL,&RRH_first_spall,"RRHFirst",f);*/
	test_one_algo(g,P,message_size,0,NULL,&descente,"Descente",f);
	//test_one_algo(g,P,message_size,100,NULL,&taboo,"taboo",f);
	//test_one_algo(g,P,message_size,1000,NULL,&recuit,"recuit",f);

	//test_one_algo(g,P,message_size,100,NULL,&greedy_deadline_assignment,"GreedyDeadline",f);
	


	seed = time(NULL);


	fprintf(f,"\n --------- \n Statistical Multiplexing .Testing the chain reaction of multiplexing ...   \n \n");
	printf("\n --------- \n Statistical Multiplexing. Testing the chain reaction of multiplexing ...  \n");
	printf("Fifo : \n");
	fprintf(f,"Fifo \n");
	int last_time_ellapsed =0;
	int time_ellapsed = 0;
	int nb_periods=1;
	while(1)
	{
		srand(seed);
		time_ellapsed = multiplexing(g, P, message_size, nb_periods, FIFO,tmax);
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
	if(time_ellapsed < tmax)
	{
		printf(GRN "OK\n" RESET);
	}
	else
	{
		printf(RED "Not OK --\n" RESET);
	}

	
	printf("\n ------- \nTime ellapsed first . Testing the chain reaction of multiplexing ... \n");
	fprintf(f,"\n ------- \nTime ellapsed first . Testing the chain reaction of multiplexing ... \n");
	last_time_ellapsed =0;
	time_ellapsed = 0;
	nb_periods=1;
	while(1)
	{
		srand(seed);
		time_ellapsed = multiplexing(g, P, message_size, nb_periods, DEADLINE,tmax);
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
	if(time_ellapsed < tmax)
	{
		printf(GRN "OK\n" RESET);
	}
	else
	{
		printf(RED "Not OK --\n" RESET);
	}

	

	

	printf("\n printing graphvitz ...");print_graphvitz(g,"../view/view.dot");printf("Ok.\n");
	printf("\n printing python ...");print_python(g);printf("Ok.\n");
	printf("\n printing json arcs ..."); print_json_arcs(g);
	printf("Logs in logs.txt\n");
	free_graph(g);
	fclose(f);
	 //jsontest();

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
	long long moy;
	char buf[256];
	sprintf(buf,"../data/%s",nom);
	FILE * f = fopen(buf,"w");

	Assignment a;
	int MarginMax = MARGIN_MAX;
	int MarginMin = MARGIN_MIN;

	//Generation of the pdf of fails
	int cmpt_fail = 0;
	char buf_dot[256];
	if(!PARALLEL)
	{
		sprintf(buf,"mkdir -p ../FAIL/%s",nom);
		if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	}

	printf("ALGORITHM : %s \n",nom);
	if(!f)perror("Error while opening file\n");
	if(TMAX_MOD)
	{
		printf("		TMax is fixed to %d .\n",TMAX);
		MarginMin = 0;
		MarginMax = 0; 
	}
	if(FIXED_PERIOD_MOD)
	{
		printf("		The period is fixed to %d .\n",PERIOD);
	}
	for(int margin=MarginMin;margin<=MarginMax;margin+=MARGIN_GAP)
	{
		nb_success=0;
		moy_routes_scheduled = 0;
		moy = 0;
		if(!PARALLEL)
		{
			cmpt_fail = 0;
			sprintf(buf,"mkdir -p ../FAIL/%s/margin%d",nom,margin);
			if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
		}
		

		#pragma omp parallel for private(g,P,a,tmax)  if(PARALLEL)
		for(int i=0;i<NB_SIMULS;i++)
		{
			g= init_graph_random_tree(STANDARD_LOAD);
			if(FIXED_PERIOD_MOD)
			{
				if(PERIOD < load_max(g)*MESSAGE_SIZE)
					printf("			WARNING, not enought space to schedule all the message on the loadest link.\n");
				P = PERIOD;
			}
			else
				P= (load_max(g)*MESSAGE_SIZE)/STANDARD_LOAD;
			if(TMAX_MOD)
			{
				if(longest_route(g)*2 > TMAX)
					printf("			WARNING! TMAX is higher than the longest route. \n");
				tmax = TMAX;
			}
			else
				tmax = longest_route(g)*2 + margin;
			
			a = ptrfonction( g, P, message_size,tmax);
			if(a->all_routes_scheduled)
			{
				//if(travel_time_max( g, tmax, a) != -1)
				if(travel_time_max_buffers( g) <= tmax)
				{
					#pragma omp atomic update
						nb_success++;
					#pragma omp atomic update
						moy+=travel_time_max_buffers( g);
				}
				else
				{
					if(!SYNCH)
					{
						printf("Error, this should not happend, the algorithm has to return an assignment in which the travel time is ok.\n");
						exit(56);	
					}
				}
				

			}
			else
			{
				if(!PARALLEL)
				{
				
		
					sprintf(buf_dot,"../FAIL/%s/margin%d/%df.dot",nom,margin,cmpt_fail);
					print_assignment(g,a,P,buf_dot);
					sprintf(buf,"dot -Tpdf %s -o ../FAIL/%s/margin%d/%df.pdf",buf_dot,nom,margin,cmpt_fail);
					if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
					sprintf(buf,"rm -rf %s",buf_dot);
					if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
					sprintf(buf_dot,"../FAIL/%s/margin%d/%db.dot",nom,margin,cmpt_fail);
					print_assignment_backward(g,a,P,buf_dot);
					sprintf(buf,"dot -Tpdf %s -o ../FAIL/%s/margin%d/%db.pdf",buf_dot,nom,margin,cmpt_fail);
					if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
					sprintf(buf,"rm -rf %s",buf_dot);
					if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
				
			
					cmpt_fail++;
				}
			}
			

			#pragma omp atomic update
				moy_routes_scheduled += a->nb_routes_scheduled;
			free_assignment(a);
	
			free_graph(g);
			fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
			fflush(stdout);
		}	
		printf("\nMargin : %d success : %d/%d ",margin,nb_success,NB_SIMULS);
		if(nb_success)
		{
			printf("moy = %lld.\n",moy/nb_success);
		}
		else
		{
			printf("\n");
		}
		fprintf(f,"%d %f %f ",margin,nb_success/(float)NB_SIMULS,moy_routes_scheduled/(float)NB_SIMULS);
		if(nb_success)
		{
			fprintf(f,"%lld \n",moy/nb_success);
		}
		else
		{
			fprintf(f,"-1 \n");
		}
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
	int tmax;
	Assignment a;
	double loadMin=LOAD_MIN;
	double loadMax=LOAD_MAX;
	char buf[256];
	sprintf(buf,"../data/%s",nom);
	FILE * f = fopen(buf,"w");
	if(!f)perror("Error while opening file\n");


	//Generation of the pdf of fails
	int cmpt_fail = 0;
	char buf_dot[256];
	if(!PARALLEL)
	{
		sprintf(buf,"mkdir -p ../FAIL/%s",nom);
		if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
	}

	printf("ALGORITHM : %s \n",nom);
	if(TMAX_MOD)
	{
		printf("		TMax is fixed to %d .\n",TMAX);
	}
	if(FIXED_PERIOD_MOD)
	{
		printf("		The period is fixed to %d .\n",PERIOD);
		loadMin = 0.0;
		loadMax = 0.0;
	}
	for(double load=loadMin;load<=loadMax;load+=LOAD_GAP)
	{
		
		nb_success=0;
		moy_routes_scheduled = 0;
		if(!PARALLEL)
		{
			cmpt_fail = 0;
			sprintf(buf,"mkdir -p ../FAIL/%s/load%f",nom,load);
			if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
		}
		

		#pragma omp parallel for private(g,P,a,tmax)  if(PARALLEL)
		for(int i=0;i<NB_SIMULS;i++)
		{
			
			//printf("%f %d\n",load, P);

			g= init_graph_random_tree(load);


			if(FIXED_PERIOD_MOD)
			{
				if(PERIOD < load_max(g)*MESSAGE_SIZE)
					printf("			WARNING, not enought space to schedule all the message on the loadest link.\n");
				P = PERIOD;
			}
			else
				P= (load_max(g)*MESSAGE_SIZE)/load;

			if(TMAX_MOD)
			{
				if(longest_route(g)*2 > TMAX)
					printf("			WARNING! TMAX is higher than the longest route. \n");
				tmax = TMAX;
			}
			else
				tmax = longest_route(g)*2;
			
			
			a = ptrfonction( g, P, message_size);
			if(a->all_routes_scheduled)
			{
					
				if(travel_time_max( g, tmax, a) != -1)
				{
					#pragma omp atomic update
						nb_success++;
				}
				else
				{
					if(!SYNCH)
					{
						printf("Error, this should not happend, the algorithm has to return an assignment in which the travel time is ok since we have no waiting times.\n");
						exit(56);
					}
					
				}
						
			}
			else
			{
				if(!PARALLEL)
				{

					sprintf(buf_dot,"../FAIL/%s/load%f/%df.dot",nom,load,cmpt_fail);
					print_assignment(g,a,P,buf_dot);
					sprintf(buf,"dot -Tpdf %s -o ../FAIL/%s/load%f/%df.pdf",buf_dot,nom,load,cmpt_fail);
					if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
					sprintf(buf,"rm -rf %s",buf_dot);
					if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
					sprintf(buf_dot,"../FAIL/%s/load%f/%db.dot",nom,load,cmpt_fail);
					print_assignment_backward(g,a,P,buf_dot);
					sprintf(buf,"dot -Tpdf %s -o ../FAIL/%s/load%f/%db.pdf",buf_dot,nom,load,cmpt_fail);
					if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
					sprintf(buf,"rm -rf %s",buf_dot);
					if(system(buf) == -1){printf("Error during the command %s .\n",buf);exit(76);}
					cmpt_fail++;
				}
			}

			#pragma omp atomic update
			moy_routes_scheduled += a->nb_routes_scheduled;
			free_assignment(a);
			free_graph(g);
			fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
			fflush(stdout);
		}	
		printf("\nLoad : %f success : %d/%d \n",load,nb_success,NB_SIMULS);
		fprintf(f,"%f %f %f\n",load,nb_success/(float)NB_SIMULS,moy_routes_scheduled/(float)NB_SIMULS);
	}
	fclose(f);	
}




void print_distrib_margin_algo_waiting(int seed,Assignment (*ptrfonction)(Graph,int,int,int),char * nom)
{
	
	srand(seed);
	int message_size = MESSAGE_SIZE;
	Graph g;
	int P ;

	char buf[256];
	sprintf(buf,"../data/%s.plot",nom);
	FILE * f = fopen(buf,"w");

	Assignment a=NULL;
	

	printf("ALGORITHM : %s \n",nom);
	if(!f)perror("Error while opening file\n");
	int time;
	#pragma omp parallel for private(g,P,a,time)  if(PARALLEL)
	for(int i=0;i<NB_SIMULS;i++)
	{
		a = NULL;

		g= init_graph_random_tree(STANDARD_LOAD);

		if(FIXED_PERIOD_MOD)
		{
			if(PERIOD < load_max(g)*MESSAGE_SIZE)
				printf("			WARNING, not enought space to schedule all the message on the loadest link.\n");
			P = PERIOD;
		}
		else
			P= (load_max(g)*MESSAGE_SIZE)/STANDARD_LOAD;
		
		
		a = ptrfonction( g, P, message_size,20);

		time = travel_time_max_buffers(g);
			
		#pragma omp critical
			fprintf(f,"%d\n",time);		
		if(a)
			free_assignment(a);

		free_graph(g);
		fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
		fflush(stdout);
	}	
	
	fclose(f);
	
		
}
void print_distrib_margin_algo_waiting_int(int seed,int (*ptrfonction)(Graph,int,int),char * nom)
{

	srand(seed);
	int message_size = MESSAGE_SIZE;
	Graph g;
	int P ;

	char buf[256];
	sprintf(buf,"../data/%s.plot",nom);
	FILE * f = fopen(buf,"w");


	

	printf("ALGORITHM : %s \n",nom);
	if(!f)perror("Error while opening file\n");
	int time;
	#pragma omp parallel for private(g,P,time)  if(PARALLEL)
	for(int i=0;i<NB_SIMULS;i++)
	{
		

		g= init_graph_random_tree(STANDARD_LOAD);

		if(FIXED_PERIOD_MOD)
		{
			if(PERIOD < load_max(g)*MESSAGE_SIZE)
				printf("			WARNING, not enought space to schedule all the message on the loadest link.\n");
			P = PERIOD;
		}
		else
			P= (load_max(g)*MESSAGE_SIZE)/STANDARD_LOAD;
		
		
		time = ptrfonction( g, P, message_size);
		
			

		#pragma omp critical
			fprintf(f,"%d\n",time);		
		

		free_graph(g);
		fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
		fflush(stdout);
	}	
	
	fclose(f);
	
		
}

void simuldistrib(int seed)
{
	
	int nb_algos = 6 ;
	char * noms[] = {"BorneInfSimons","BorneInfSort","Descente","Taboo","DescenteX","GreedyDeadline"};

	srand(seed);
	int message_size = MESSAGE_SIZE;
	Graph g;
	int P ;


	char buf[256];
	FILE * f[nb_algos];
	for(int i=0;i<nb_algos;i++)
	{
		sprintf(buf,"../data/%s.plot",noms[i]);
		f[i] = fopen(buf,"w");
		if(!f[i])perror("Error while opening file\n");
	}


	Assignment a=NULL;
	

	
	
	int time[nb_algos];
	int res[nb_algos][NB_SIMULS];
	float nb_pas[3];
	
	for(int i=0;i<3;i++)nb_pas[i] = 0;
	#pragma omp parallel for private(g,P,a,time)  if(PARALLEL)
	for(int i=0;i<NB_SIMULS;i++)
	{
		saut:
		a = NULL;

		g= init_graph_random_tree(STANDARD_LOAD);
		int l = 2*longest_route(g);
		
		//printf("%d \n",longest_route);
		if(FIXED_PERIOD_MOD)
		{
			if(PERIOD < load_max(g)*MESSAGE_SIZE)
				printf("			WARNING, not enought space to schedule all the message on the loadest link.\n");
			P = PERIOD;
		}
		else
			P= (load_max(g)*MESSAGE_SIZE)/STANDARD_LOAD;
		
		for(int algo = 0;algo<nb_algos;algo++)
		{
			//printf("thread %d Starting algo %d :\n",omp_get_thread_num(),algo);
			switch(algo){
				case 0:
					time[algo] = borneInf( g, P, message_size)-l;	
					//printf("%d longest_route\n",l);
				break;
				case 1:
					time[algo] = borneInf2( g, P, message_size)-l;	
				break;
				case 2:
					//printf("DESCENTE \n\n\n\n\n\n\n\n\n");
					a = descente( g, P, message_size,0);
					if(a)
						nb_pas[0] += a->nb_routes_scheduled;
				break;
				case 3:
					a = taboo( g, P, message_size,100);
					if(a)
						nb_pas[1] += a->nb_routes_scheduled;
				break;
				case 4:
					a = best_of_x( g, P, message_size,10);
					if(a)
						nb_pas[2] += a->nb_routes_scheduled;
				break;
				case 5:
					a =  greedy_deadline_assignment( g, P, message_size,0);
				break;

				}
				if((algo > 1))
				{
					if(a)
						time[algo] = travel_time_max_buffers(g)-l;
					
					/*printf("algo %d \n",algo);
					int lenght=0;
					for(int j=1;j<g.nb_routes;j++)
					{
						lenght = route_length_with_buffers(g,j);

						printf("taille de la route %d = %d , %d\n",j,lenght,2*route_length(g,j));
						
						
					}*/
				}

				#pragma omp critical
					res[algo][i]=time[algo];

				if(algo > 1){
					if(!a)
					{
						goto saut;
					}
					free_assignment(a);
				}
					
				a=NULL;
				reset_periods(g,P);
			
		}
		if((time[2] > time[5]) )
			printf("La descente est moins bonne que l'algo greedy d'init \n");
		if((time[3]>time[5]) )
			printf("Le taboo est moins bon que l'algo greedy d'init \n");
		if((time[1]>time[0]) )
		{
			printf("Pb de born inf la les garcons %d %d %d\n",time[0]+l,time[1]+l,l);
			print_graphvitz(g,"../view/view.dot");
			affiche_graph(g,P,stdout);
			exit(45);
		}
		for(int k=2;k<nb_algos;k++)
		{
			if((time[k]<time[0]) || (time[k]<time[1]))
			{
				printf("On dépasse la borne inf, c'est chelou algo %d tps algo %d tmps borne 1 %d tmps borne 2 %d\n",k,time[k],time[0],time[1]);
				
				exit(4);

			}
		}
		
		free_graph(g);
		fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
		fflush(stdout);
	}	

	int max=0;
	for(int i=0;i<nb_algos;i++)
	{
		tri_bulles_classique_croissant(res[i],NB_SIMULS);
		if(res[i][NB_SIMULS-1] > max)
			max = res[i][NB_SIMULS-1];
		
	}
	int interval_size = max / NB_POINTS;
	for(int i=0;i<nb_algos;i++)
	{
		for(int j=0;j<NB_SIMULS;j++)
		{
			fprintf(f[i],"%d \n",(res[i][j]/interval_size)  * interval_size);
		}
		fclose(f[i]);
	}
	
	char * ylabels2[] = {"Nombre d'instances"};
	print_gnuplot_distrib("waiting",noms, nb_algos, "Cumulative distribution of the Latency", "Latency", ylabels2);
	
	printf("Nombre de pas moyen : Descente %f | Taboo %f | DescenteX %f \n",nb_pas[0]/NB_SIMULS,nb_pas[1]/NB_SIMULS,nb_pas[2]/NB_SIMULS);
	
		
}
