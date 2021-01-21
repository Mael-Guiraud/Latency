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
#include "starSPALL.h"
#include "enum.h"


void test(unsigned int seed)
{
	//unsigned int seed = 1591624286;
	//unsigned int seed = time(NULL);
	//seed = 1596121638;
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
				
	

	
	Graph g = init_graph_etoile(NB_ROUTES, PERIOD);
	P = PERIOD;
	/*Graph  g = init_graph_random_tree(STANDARD_LOAD);
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
		tmax = longest_route(&g)*2 + margin;*/
	/*
			
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
	*/

	printf(" ---- \n Graph * generated ...\n");
	fprintf(f,"\n ---- \n Graph * generated ...\n");
	//affiche_graph(&g,P,f);
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
	/*
	reset_periods(&g,P);reinit_delays(&g);


	//int fpt = branchbound( &g, P,  message_size,NULL,NULL,0);
	int fpt = 0;
	
	//printf("FPT = %d \n",fpt);
	
	reset_periods(&g,P);reinit_delays(&g);
		
	float nb_pas = 0;	
	int recuits = recuit( &g, P, message_size,100,&nb_pas);
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
		exit(64);
		
	}
	else
	{
		printf(GRN "FPT trouve au moins aussi bien que le recuit ! \n" RESET);
	}
	nb_pas = 0.0;	
	int descent = descente( &g, P, message_size,0,&nb_pas);
	printf("descente %d , %f pas\n",descent,nb_pas);	
	sprintf(nom,"descente");
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
	if(descent<fpt)
	{
		printf(RED "!!!!!!!!!!FPT trouve moins bien que la descente!!!!!!!!!!!! \n" RESET);
		exit(64);
	}
	else
	{
		printf(GRN "FPT trouve au moins aussi bien que la descente ! \n" RESET);
	}
	
	int tabo = taboo( &g, P, message_size,100,100);
	printf("taboo %d \n",tabo);	
	sprintf(nom,"taboo");
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
	if(tabo<fpt)
	{
		printf(RED "!!!!!!!!!!FPT trouve moins bien que le taboo!!!!!!!!!!!! \n" RESET);
		exit(44);
	}
	else
	{
		printf(GRN "FPT trouve au moins aussi bien que le taboo ! \n" RESET);
	}
	
	
	int gd = greedy_deadline_assignment( &g, P, message_size);
	printf("greedy %d \n",gd);	
	sprintf(nom,"greedy");
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
	if(gd<fpt)
	{
		printf(RED "!!!!!!!!!!FPT trouve moins bien que le greedy!!!!!!!!!!!! \n" RESET);
		exit(44);
	}
	else
	{
		printf(GRN "FPT trouve au moins aussi bien que le greedy ! \n" RESET);
	}
	int gd2 = greedy_deadline_assignment2( &g, P, message_size);
	printf("greedy 2 %d \n",gd2);	
	sprintf(nom,"greedy2");
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
	if(gd2<fpt)
	{
		printf(RED "!!!!!!!!!!FPT trouve moins bien que le greedy 2!!!!!!!!!!!! \n" RESET);
		exit(44);
	}
	else
	{
		printf(GRN "FPT trouve au moins aussi bien que le greedy 2! \n" RESET);
	}
	int gd3 = greedy_deadline_assignment3( &g, P, message_size);
	printf("greedy 3 %d \n",gd3);	
	sprintf(nom,"greedy3");
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
	if(gd2<fpt)
	{
		printf(RED "!!!!!!!!!!FPT trouve moins bien que le greedy 3!!!!!!!!!!!! \n" RESET);
		exit(44);
	}
	else
	{
		printf(GRN "FPT trouve au moins aussi bien que le greedy 3! \n" RESET);
	}
	printf("\n printing graphvitz ...");print_graphvitz(&g,"../view/view.dot");printf("Ok.\n");
	printf("\n printing python ...");print_python(&g);printf("Ok.\n");
	printf("\n printing json arcs ..."); print_json_arcs(&g);
	printf("Logs in logs.txt\n");
*/

	
	//printf("multiplexing = %d %d %d\n",multiplexing(&g, P, message_size, 1, DEADLINE),multiplexing(&g, P, message_size, 10, DEADLINE),multiplexing(&g, P, message_size, 100, DEADLINE));
//	printf("multiplexing = %d %d %d\n",multiplexing(&g, P, message_size, 1, FIFO),multiplexing(&g, P, message_size, 10, FIFO),multiplexing(&g, P, message_size, 100, FIFO));
	//printf("%d \n",multiplexing(&g, P, message_size, 10, DEADLINE,0));
	//printf("%d \n",multiplexing(&g, P, message_size, 10, FIFO,0));
	//int nb_pas = 0;
	//recuit( &g, P, message_size,100,&nb_pas);
	int timebe=0;
	//printf("%d \n",multiplexing(&g, P, message_size, 1, DEADLINE,1,&timebe));
	printf("\n printing graphvitz ...");print_graphvitz(&g,"../view/view.dot");printf("Ok.\n");
	free_graph(&g);
	fclose(f);


}


void simuldistrib(int seed)
{
	srand(seed);
	int nb_algos =7;
	//"Hybrid Greedy Deadline","Greedy Packed",
	char * noms[] = {"Hybrid Greedy Normalized","Lower Bound","Hill Climbing Init HGN","Hybrid Hill Climbing 100","Tabu Search","Simulated Annealing","Branch and Bound"};

	
	
	int message_size = MESSAGE_SIZE;
	Graph  g;
	int P ;
	

	char buf[256];
	FILE * f[nb_algos];
	for(int i=0;i<nb_algos;i++)
	{
		sprintf(buf,"../data/%s.plot",noms[i]);
		f[i] = fopen(buf,"w");
		if(!f[i])perror("Error while opening file\n");
	}


	int a=0;
	
	float nb;
	
	
	int time[nb_algos];
	int res[nb_algos][NB_SIMULS];
	float nb_pas[4];
	float running_time[nb_algos];
	  struct timeval tv1, tv2;
	  for(int i=0;i<nb_algos;i++)running_time[i]=0.0;
	for(int i=0;i<4;i++)nb_pas[i] = 0;
	#pragma omp parallel for private(g,P,a,time,nb,tv1,tv2)  if(PARALLEL)
	for(int i=0;i<NB_SIMULS;i++)
	{

	
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

			
				
			//printf("Algorithm %s \n",noms[algo]);
			a= 0;
			nb = 0;
			//printf("thread %d Starting algo %d :\n",omp_get_thread_num(),algo);
				gettimeofday (&tv1, NULL);	
		
			switch(algo){
				/*case 0:
					a =  greedy_deadline_assignment( &g, P, message_size);

				break;
				case 1:
					a =  greedy_deadline_assignment2( &g, P, message_size);
					

				break;*/
				case 0:
					a =  greedy_deadline_assignment3( &g, P, message_size);
					
				break;
				
				case 1:
					time[algo] = borneInf( &g, P, message_size)-l;	
					//printf("%d longest_route\n",l);
				break;
				case 2:
				{
					a = descente( &g, P, message_size,0,&nb);
					#pragma omp critical
						nb_pas[0] += nb;
				}
				break;
				case 3:
				
					
					a = best_of_x( &g, P, message_size,100,&nb);
					#pragma omp critical
						nb_pas[1] += nb;
					
					
				break;
				case 4:
					a =  taboo( &g, P, message_size,500,500,&nb);
					
					#pragma omp critical
						nb_pas[2] += nb;
					
				break;
				
				
				case 5:
				 nb = 20;
					a = recuit( &g, P, message_size,1000,&nb);
					//a = branchbound( &g, P, message_size,coupes,coupes_m,1);
					#pragma omp critical
						nb_pas[3] += nb;
					break;
				case 6:
					a = branchbound( &g, P, message_size,NULL,NULL,1);
					break;
					
				}

				gettimeofday (&tv2, NULL);	
				#pragma omp critical
					running_time[algo] += time_diff(tv1,tv2);


				if(a)
				{
					time[algo] = a-l;

				}
					
				else
				{
					if((algo != 1))
						time[algo] = INT_MAX;

				}
					
		
				
				//printf("Algo %d a = %p \n",algo,a);
				reset_periods(&g,P);
				
				reinit_delays(&g);
			
		}
		for(int algo = 0;algo<nb_algos;algo++)
		{

				#pragma omp critical
					res[algo][i]=time[algo];
		}
		/*
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
		

		for(int k=2;k<nb_algos;k++)
		{
			if((time[7]>time[k])&&k!=5)
			{
				printf(RED "Algo %d meilleur que fpt (%d %d).\n" RESET,k,time[k],time[7]);
				printf("\n printing graphvitz ...");print_graphvitz(&g,"../view/view.dot");printf("Ok.\n");
				//exit(24);
			}
			
			if((time[k]<time[5]) || (time[k]<time[1]))
			{
				printf("On dépasse la borne inf, c'est chelou algo %d tps algo %d tmps borne 1 %d tmps borne 2 %d lenght %d\n",k,time[k],time[5],time[1],l);
				
				exit(4);

			}
		}
		*/
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
	long long moy = 0;
	int cmpt ;
	for(int i=0;i<nb_algos;i++)
	{
		cmpt = 0;
		moy = 0;
	
		printf("%s : %f ms ",noms[i],running_time[i]/NB_SIMULS);
		for(int j=0;j<NB_SIMULS;j++)
		{
			if(res[i][j]!=INT_MAX)
			{
				cmpt++;
				moy += res[i][j];
				fprintf(f[i],"%d \n",res[i][j]);
			}
			
			//fprintf(f[i],"%d \n",(res[i][j]/interval_size)  * interval_size);
		}
		printf("Sucess algo %s = %f \n",noms[i],(float)cmpt/NB_SIMULS);
		printf("%lld \n",moy/NB_SIMULS);
		fclose(f[i]);
	}
	
	sprintf(buf,"%d_%d_%d_%d_%f",NB_BBU,NB_COLLISIONS,MAX_LENGTH,DISTRIBUTED,STANDARD_LOAD);
	char * ylabels2[] = {"Number of instances"};
	print_gnuplot_distrib(buf,noms, nb_algos, "Cumulative distribution of the margin", "Margin (tics)", ylabels2);
	
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
				branchbound( &g, P, message_size,coupes,coupes_m,0);
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
void testfpt(int seed)
{	

	
	Graph g;
	int P;
	int message_size = MESSAGE_SIZE;
	int a,b;
	int max = 0;
	int result;
	int nb = 10000;
	struct timeval tv1, tv2;
	float temps_moyen_fpt=0.0;
	float temps_moyen_simons=0.0;
	int min;
	int ret;
	int maxsimons;
	

	//printf("Seed = %u \n",seed);
	printf("Nb_bbu = %d \n",NB_BBU);
	for(int k=0;k<nb;k++)
	{
		seed++;
		srand(seed);
		g= init_graph_random_tree(STANDARD_LOAD);
		P= (load_max(&g)*MESSAGE_SIZE)/STANDARD_LOAD;
		reinit_delays(&g);
		min = 2*longest_route(&g);
		maxsimons = min+P;

		//printf("Periode = %d \n",P);
		max = 0;
		//printf("Input algo FPT yann :\n");
		for(int i= g.nb_bbu;i<g.nb_bbu+g.nb_collisions;i++)
		{
			
			int release[g.arc_pool[i].nb_routes];
			int delai[g.arc_pool[i].nb_routes];
			for(int j=0;j<g.arc_pool[i].nb_routes;j++)
			{

				release[j] = route_length_with_buffers_forward(&g,g.arc_pool[i].routes_id[j])
					+route_length_untill_arc(&g,g.arc_pool[i].routes_id[j],&g.arc_pool[i],BACKWARD);
				delai[j]=route_length_with_buffers( &g,g.arc_pool[i].routes_id[j]);
				//printf(" (%d %d),",release[j],delai[j]);
			}	
			//printf("\n");
			gettimeofday (&tv1, NULL);	
			result = optim(release, delai, g.arc_pool[i].nb_routes,  P, message_size,INT_MAX);
			gettimeofday (&tv2, NULL);	
			temps_moyen_fpt += time_diff(tv1,tv2);
			max = (result>max)?result:max;
		
		}
		a = max;
		max = 0;
		//printf("Input algo simons : \n");
		for(int i= g.nb_bbu;i<g.nb_bbu+g.nb_collisions;i++)
		{	
			gettimeofday (&tv1, NULL);
			ret =  dichosimons(&g, P,  message_size,i , BACKWARD, min,maxsimons);
			gettimeofday (&tv2, NULL);	
			//if(!simonslastarc(&g,  P,  message_size, a,i ,BACKWARD))
			if(!ret)
				printf(RED"Simons Last Arc ne trouve pas une solution trouvée par le fpt\n"RESET);
			
			simonslastarc(&g,P,message_size,ret,i,BACKWARD) ;
			if(ret > min)
				min = ret;
			temps_moyen_simons += time_diff(tv1,tv2);
			result =travel_time_max_buffers(&g);
			max = (result>max)?result:max;
			
		}

		b=max;

		reinit_delays(&g);
		 if(a<b)
		 {
			printf(RED"LES DEUX ALGOS NE DONNENT PAS LE MÊME RESULTAT simons : %d FPT : %d\n"RESET,b,a);
			print_graphvitz(&g,"../view/view.dot");
			//system("dot -Tpdf ../view/view.dot -o ../view/view.pdf");
			//affiche_graph_routes(&g,stdout);
			exit(2);
		 }
		 	
		 if(a>b){
		 	printf(GRN"LES DEUX ALGOS NE DONNENT PAS LE MÊME RESULTAT  FPT : %d simons : %d \n"RESET,a,b);
		 	print_graphvitz(&g,"../view/view.dot");
		 	//system("dot -Tpdf ../view/view.dot -o ../view/view.pdf");
		 	//affiche_graph_routes(&g,stdout);
		 	exit(3);
		}
		
		 free_graph(&g);
		 fprintf(stdout,"\r%d/%d",k+1,nb);fflush(stdout);
	}
	//printf("\nTemps moyen Fpt    = %f \nTemps moyen Simons = %f \n",temps_moyen_fpt/nb,temps_moyen_simons/nb);
	FILE *f =fopen("resulttime","a+");

	fprintf(f,"%d %f %f\n",NB_BBU *NB_ROUTES_PER_FLOW,temps_moyen_fpt/nb,temps_moyen_simons/nb);
	fclose(f);
	//sleep(1);
	return;
}


void simuldescente(int seed)
{
	srand(seed);
	int nb_algos =4 ;
	char * noms[] = {"Init HGN","Hybrid 1","Hybrid 10","Hybrid 100"};
	//char * noms[] = {"Init HGN","Init 1 random","Init 10 randoms","Init 100 randoms"};
	
	
	
	int message_size = MESSAGE_SIZE;
	Graph  g;
	int P ;
	

	char buf[256];
	FILE * f[nb_algos];
	for(int i=0;i<nb_algos;i++)
	{
		sprintf(buf,"../data/%s.plot",noms[i]);
		f[i] = fopen(buf,"w");
		if(!f[i])perror("Error while opening file\n");
	}


	int a=0;
	
	float nb;
	
	
	int time[nb_algos];
	int res[nb_algos][NB_SIMULS];
	float nb_pas[4];
	float running_time[nb_algos];
	  struct timeval tv1, tv2;
	  for(int i=0;i<nb_algos;i++)running_time[i]=0.0;
	for(int i=0;i<4;i++)nb_pas[i] = 0;
	#pragma omp parallel for private(g,P,a,time,nb,tv1,tv2)  if(PARALLEL)
	for(int i=0;i<NB_SIMULS;i++)
	{
	
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
			//printf("Algorithm %s \n",noms[algo]);
			a= 0;
			nb = 0;
			//printf("thread %d Starting algo %d :\n",omp_get_thread_num(),algo);
				gettimeofday (&tv1, NULL);	
		
			switch(algo){
				case 0:
				{
					a = descente( &g, P, message_size,0,&nb);
					#pragma omp critical
						nb_pas[0] += nb;
				}
				break;
				case 1:
				
					
					a = best_of_x( &g, P, message_size,1,&nb);
					#pragma omp critical
						nb_pas[1] += nb;
					
					
				break;
				case 2:
				
					
					a = best_of_x( &g, P, message_size,10,&nb);
					#pragma omp critical
						nb_pas[2] += nb;
					
					
				break;
				case 3:
				
					
					a = best_of_x( &g, P, message_size,100,&nb);
					#pragma omp critical
						nb_pas[3] += nb;
					
					
				break;
			}
				
				gettimeofday (&tv2, NULL);	
				#pragma omp critical
					running_time[algo] += time_diff(tv1,tv2);


				if(a)
				{
					time[algo] = a-l;

				}
					
				else
				{
					time[algo] = INT_MAX;

				}
					
		
				
				//printf("Algo %d a = %p \n",algo,a);
				reset_periods(&g,P);
				
				reinit_delays(&g);
			
		}
		for(int algo = 0;algo<nb_algos;algo++)
		{
				#pragma omp critical
					res[algo][i]=time[algo];
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
	long long moy = 0;
	int cmpt ;
	for(int i=0;i<nb_algos;i++)
	{
		cmpt = 0;
		moy = 0;
	
		printf("%s : %f ms - temps moyen =  , NB_pas = %f",noms[i],running_time[i]/NB_SIMULS,nb_pas[i]/NB_SIMULS);
		for(int j=0;j<NB_SIMULS;j++)
		{
			if(res[i][j]!=INT_MAX)
			{
				cmpt++;
				moy += res[i][j];
				fprintf(f[i],"%d \n",res[i][j]);
			}
			
			//fprintf(f[i],"%d \n",(res[i][j]/interval_size)  * interval_size);
		}
		printf("Sucess algo %s = %f \n",noms[i],(float)cmpt/NB_SIMULS);
		printf("%lld \n",moy/NB_SIMULS);
		fclose(f[i]);
	}
	
	sprintf(buf,"descentes_%d_%d_%d",NB_BBU,NB_COLLISIONS,MAX_LENGTH);
	char * ylabels2[] = {"Number of instances"};
	print_gnuplot_distrib(buf,noms, nb_algos, "Cumulative distribution of the margin", "Margin (tics)", ylabels2);
	
	
}


void simultaboo(int seed)
{
	srand(seed);
	int nb_algos =5 ;
	char * noms[] = {"M = 10","M = 50","M = 100","M = 200","M = 500"};
	
	
	
	int message_size = MESSAGE_SIZE;
	Graph  g;
	int P ;
	

	char buf[256];
	FILE * f[nb_algos];
	for(int i=0;i<nb_algos;i++)
	{
		sprintf(buf,"../data/%s.plot",noms[i]);
		f[i] = fopen(buf,"w");
		if(!f[i])perror("Error while opening file\n");
	}


	int a=0;
	
	float nb;
	
	
	int time[nb_algos];
	int res[nb_algos][NB_SIMULS];
	float nb_pas[nb_algos];
	float nb_pasmoy[nb_algos];
	float running_time[nb_algos];
	  struct timeval tv1, tv2;
	  for(int i=0;i<nb_algos;i++)running_time[i]=0.0;
	for(int i=0;i<nb_algos;i++)
		{
			nb_pas[i] = 0;
			nb_pasmoy[i] = 0;
		}
	#pragma omp parallel for private(g,P,a,time,nb,tv1,tv2)  if(PARALLEL)
	for(int i=0;i<NB_SIMULS;i++)
	{
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
			/*if(algo < 2)
				algo = 2;
			if(algo > 2)
				algo = 9;*/
		//	printf("Algorithm %s \n",noms[algo]);
			a= 0;
			nb = 0;
			//printf("thread %d Starting algo %d :\n",omp_get_thread_num(),algo);
				gettimeofday (&tv1, NULL);	
		
			switch(algo){
				case 0:
					a = taboo( &g, P, message_size,500,10,&nb);
					#pragma omp critical
						nb_pasmoy[algo] += nb;
					if(nb>nb_pas[algo])
					{
						#pragma omp critical
							nb_pas[algo] = nb;
					}
					
				break;
				case 1:
					a = taboo( &g, P, message_size,500,50,&nb);
					#pragma omp critical
						nb_pasmoy[algo] += nb;
					if(nb>nb_pas[algo])
					{
						#pragma omp critical
							nb_pas[algo] = nb;
					}
				break;
				case 2:
					a = taboo( &g, P, message_size,500,100,&nb);
					#pragma omp critical
						nb_pasmoy[algo] += nb;
					if(nb>nb_pas[algo])
					{
						#pragma omp critical
							nb_pas[algo] = nb;
					}
				break;
				case 3:
					a = taboo( &g, P, message_size,500,200,&nb);
					#pragma omp critical
						nb_pasmoy[algo] += nb;
					if(nb>nb_pas[algo])
					{
						#pragma omp critical
							nb_pas[algo] = nb;
					}
				break;
				case 4:
					a = taboo( &g, P, message_size,500,500,&nb);
					#pragma omp critical
						nb_pasmoy[algo] += nb;
					if(nb>nb_pas[algo])
					{
						#pragma omp critical
							nb_pas[algo] = nb;
					}
				break;
				
				
				
				
				}
				gettimeofday (&tv2, NULL);	
				#pragma omp critical
					running_time[algo] += time_diff(tv1,tv2);


				if(a)
				{
					time[algo] = a-l;

				}
					
				
				
				
				reset_periods(&g,P);
				
				reinit_delays(&g);
			
		}
		for(int algo = 0;algo<nb_algos;algo++)
		{
				#pragma omp critical
					res[algo][i]=time[algo];
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
	long long moy = 0;
	int cmpt ;
	for(int i=0;i<nb_algos;i++)
	{
		cmpt = 0;
		moy = 0;
	
		printf("%s : %f ms , nb_pasmoy = %f, nb_pasmax = %f\n",noms[i],running_time[i]/NB_SIMULS,nb_pasmoy[i]/NB_SIMULS,nb_pas[i]);
		for(int j=0;j<NB_SIMULS;j++)
		{
			if(res[i][j]!=INT_MAX)
			{
				cmpt++;
				moy += res[i][j];
				fprintf(f[i],"%d \n",res[i][j]);
			}
			
			//fprintf(f[i],"%d \n",(res[i][j]/interval_size)  * interval_size);
		}
		printf("Sucess algo %s = %f \n",noms[i],(float)cmpt/NB_SIMULS);
		printf("%lld \n",moy/NB_SIMULS);
		fclose(f[i]);
	}
	
	sprintf(buf,"taboo");
	char * ylabels2[] = {"Number of instances"};
	print_gnuplot_distrib(buf,noms, nb_algos, "Cumulative distribution of the Latency", "Additional latency (tics)", ylabels2);
	
		
}

void simultiplexing(int seed)
{

	srand(seed);

	int message_size = MESSAGE_SIZE;
	
	int P;
				
	FILE* f = fopen("resmult6020distrib","w");
	int nbsim = 1000;


	int multfifo ;
	int multfifo2 ;
	int multdeadline;
	int multcomputed;

	int fpt ;
	int timebefifo;
	int timebefifo2;
	int timebedeadline;
	int timebecomputed;
	int timebedistrib;

	int moyfifo;
	int moyfifo2;
	int moydeadline;
	int moycomputed;
	int moydistrib;
	
	int longest;
	long long moymoyfifo = 0;
	long long moymoyfifo2 = 0;
	long long moymoydeadline = 0;
	long long moymoycpomputed = 0;
	long long moymoydistrib = 0;
	system("rm -f distrififo distrififo2 distrideadline districomputed distridistrib");
	for(int i=0;i<nbsim;i++)
	{
		Graph g = init_graph_etoile(NB_ROUTES, PERIOD);
		P = PERIOD;
		timebefifo=0;
		timebefifo2=0;
		timebedeadline=0;
		timebecomputed=0;
		timebedistrib=0;
		longest= 2*longest_route(&g);

		FILE* fichier = fopen("distrififo","a");
		//printf("FIFO \n");
		multfifo =multiplexing(&g, P, message_size, 10, FIFO,0,&timebefifo,fichier,1,&moyfifo,0) - longest;
		fclose(fichier);
			fichier = fopen("distrififo2","a");
		//printf("FIFO \n");
		multfifo2 =multiplexing(&g, P, message_size, 10, FIFO,0,&timebefifo2,fichier,1,&moyfifo2,1) - longest;
		fclose(fichier);
		//printf("DEADLINE \n");
			 fichier = fopen("distrideadline","a");
		multdeadline =multiplexing(&g, P, message_size, 10, DEADLINE,0,&timebedeadline,fichier,0,&moydeadline,0) - longest;
	fclose(fichier);

		/*fpt  = dichostarspall(&g,  P,  message_size);
		printf("\n\n\nFPT = %d \n \n\n",fpt);
		fpt = FPT_PALL_star(&g,P,message_size,fpt);
		printf("FPT apres = %d  longest %d\n",fpt,longest);
		*/
		
		fpt = 0;
		int l = 0;
		while(!fpt)
		{
			fpt =  FPT_PALL_star(&g,P,message_size,longest +l);
			if(fpt)
				break;
			l++;
			//fprintf(stdout,"\r fpt1     %d",l );fflush(stdout);
		}
		
			 fichier = fopen("districomputed","a");
		multcomputed =multiplexing(&g, P, message_size, 10, DEADLINE,1,&timebecomputed,fichier,0,&moycomputed,0) ;
		fclose(fichier);
		
		fpt = 1;
		int l2 = 0;
		while(fpt)
		{
			fpt =  FPT_PALL_star(&g,P,message_size+l2,longest);
			if(!fpt)
			{
				//printf("%d %d \n",fpt,l);
				fpt =  FPT_PALL_star(&g,P,message_size+l2-1,longest);
				break;
			}
			l2+=1;
			//fprintf(stdout,"\r    %d",l );fflush(stdout);
		}
		
			 fichier = fopen("distridistrib","a");
		multcomputed =multiplexing(&g, P, message_size, 10, DEADLINE,1,&timebedistrib,fichier,0,&moydistrib,0) ;
		fclose(fichier);

		
		free_graph(&g);
		moymoyfifo += moyfifo;
		moymoyfifo2 += moyfifo2;
		moymoydeadline += moydeadline;
		moymoycpomputed += moycomputed;
		moymoydistrib += moydistrib;
		fprintf(f,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",i,multfifo,multfifo2,multdeadline,multcomputed,l,timebefifo,timebefifo2,timebedeadline,timebecomputed,timebedistrib,moyfifo,moyfifo2,moydeadline,moycomputed,moydistrib ,message_size+l2);
		fprintf(stdout,"\r %d/%d",i+1,nbsim);fflush(stdout);
	}
	printf("\nFIFO %d %d\nFIFO 2 %d %d\nDeadline %d %d\n Computed %d %d\n,Distrib%d %d\n",moymoyfifo/nbsim,timebefifo,moymoyfifo2/nbsim,timebefifo2,moymoydeadline/nbsim,timebedeadline,moymoycpomputed/nbsim,timebecomputed,moymoydistrib/nbsim,timebedistrib);
		
		
	
	printf("\n");
	fclose(f);
}


void simulrecuit(int seed)
{
	srand(seed);
	int nb_algos =1 ;
	char * noms[] = {"100","20","50","100"};
	
	
	
	int message_size = MESSAGE_SIZE;
	Graph  g;
	int P ;
	

	char buf[256];
	FILE * f[nb_algos];
	for(int i=0;i<nb_algos;i++)
	{
		sprintf(buf,"../data/%s.plot",noms[i]);
		f[i] = fopen(buf,"w");
		if(!f[i])perror("Error while opening file\n");
	}


	int a=0;
	
	float nb;
	
	
	int time[nb_algos];
	int res[nb_algos][NB_SIMULS];
	float nb_pas[nb_algos];
	float running_time[nb_algos];
	  struct timeval tv1, tv2;
	  for(int i=0;i<nb_algos;i++)running_time[i]=0.0;
	for(int i=0;i<nb_algos;i++)nb_pas[i] = 0;
	#pragma omp parallel for private(g,P,a,time,nb,tv1,tv2)  if(PARALLEL)
	for(int i=0;i<NB_SIMULS;i++)
	{
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
			/*if(algo < 2)
				algo = 2;
			if(algo > 2)
				algo = 9;*/
		//	printf("Algorithm %s \n",noms[algo]);
			a= 0;
			nb = 0;
			//printf("thread %d Starting algo %d :\n",omp_get_thread_num(),algo);
				gettimeofday (&tv1, NULL);	
		
			switch(algo){
				case 0:
					//a =  test_nb_vois_egal(&g, P,  message_size);
					a = recuit( &g, P, message_size,100,&nb);
					
					#pragma omp critical
						nb_pas[algo] += nb;
				break;
				case 1:
					a = recuit( &g, P, message_size,20,&nb);
					
					#pragma omp critical
						nb_pas[algo] += nb;
				break;
				case 2:
					a = recuit( &g, P, message_size,50,&nb);
					
					#pragma omp critical
						nb_pas[algo] += nb;
				break;
				case 3:
					a = recuit( &g, P, message_size,100,&nb);
					
					#pragma omp critical
						nb_pas[algo] += nb;
				break;
				
				
				
				}
				gettimeofday (&tv2, NULL);	
				#pragma omp critical
					running_time[algo] += time_diff(tv1,tv2);


				if(a)
				{
					time[algo] = a-l;

				}
					
				
				
				
				reset_periods(&g,P);
				
				reinit_delays(&g);
			
		}
		for(int algo = 0;algo<nb_algos;algo++)
		{
				#pragma omp critical
					res[algo][i]=time[algo];
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
	long long moy = 0;
	int cmpt ;
	for(int i=0;i<nb_algos;i++)
	{
		cmpt = 0;
		moy = 0;
	
		printf("%s : %f ms , nb_pas = %f\n",noms[i],running_time[i]/NB_SIMULS,nb_pas[i]/NB_SIMULS);
		for(int j=0;j<NB_SIMULS;j++)
		{
			if(res[i][j]!=INT_MAX)
			{
				cmpt++;
				moy += res[i][j];
				fprintf(f[i],"%d \n",res[i][j]);
			}
			
			//fprintf(f[i],"%d \n",(res[i][j]/interval_size)  * interval_size);
		}
		printf("Sucess algo %s = %f \n",noms[i],(float)cmpt/NB_SIMULS);
		printf("%lld \n",moy/NB_SIMULS);
		fclose(f[i]);
	}
	
	sprintf(buf,"recuit");
	char * ylabels2[] = {"Number of instances"};
	print_gnuplot_distrib(buf,noms, nb_algos, "Cumulative distribution of the Latency", "Additional latency (tics)", ylabels2);
	
		
}


/*
void simulrecuit(int seed)
{
	srand(1);
	int nb_algos =1 ;
	char * noms[] = {"100","20","50","100"};
	
	
	int P ;
	int message_size = MESSAGE_SIZE;
	Graph  g[100];
	for(int i=0;i<100;i++)
	{
		g[i]= init_graph_random_tree(STANDARD_LOAD);
		
		//printf("%p %d\n",g+i,g[i].nb_routes);
		
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
	
	float nb;
	
	
	int time[nb_algos];
	int res[nb_algos][NB_SIMULS];
	float nb_pas[nb_algos];
	float running_time[nb_algos];
	  struct timeval tv1, tv2;
	  for(int i=0;i<nb_algos;i++)running_time[i]=0.0;
	for(int i=0;i<nb_algos;i++)nb_pas[i] = 0;
	//#pragma omp parallel for private(g,P,a,time,nb,tv1,tv2)  if(PARALLEL)
	for(int i=0;i<NB_SIMULS;i++)
	{
		a = 0;
		//printf("%p %d\n",g+i,g[i].nb_routes);
		int l = 2*longest_route(g+i);
		//printf("%d \n",longest_route);
		if(FIXED_PERIOD_MOD)
		{
			if(PERIOD < load_max(g+i)*MESSAGE_SIZE)
				printf("			WARNING, not enought space to schedule all the message on the loadest link.\n");
			P = PERIOD;
		}
		else
			P= (load_max(g+i)*MESSAGE_SIZE)/STANDARD_LOAD;
		for(int algo = 0;algo<nb_algos;algo++)
		{
			
		//	printf("Algorithm %s \n",noms[algo]);
			a= 0;
			nb = 0;
			//printf("thread %d Starting algo %d :\n",omp_get_thread_num(),algo);
				gettimeofday (&tv1, NULL);	
		
			switch(algo){
				case 0:
					//a =  test_nb_vois_egal(&g, P,  message_size);
					a = recuit( g+i, P, message_size,100,&nb);
					
					#pragma omp critical
						nb_pas[algo] += nb;
				break;
		
				
				
				
				}
				gettimeofday (&tv2, NULL);	
				#pragma omp critical
					running_time[algo] += time_diff(tv1,tv2);


				if(a)
				{
					time[algo] = a-l;

				}
					
				
				
				
				reset_periods(g+i,P);
				
				reinit_delays(g+i);
			
		}
		for(int algo = 0;algo<nb_algos;algo++)
		{
				#pragma omp critical
					res[algo][i]=time[algo];
		}
		
		
		free_graph(g+i);
		fprintf(stdout,"\r%d/%d",i+1,NB_SIMULS);
		fflush(stdout);
	}	

	
	for(int i=0;i<nb_algos;i++)
	{
		tri_bulles_classique_croissant(res[i],NB_SIMULS);
	
		
	}

	long long moy = 0;
	int cmpt ;
	for(int i=0;i<nb_algos;i++)
	{
		cmpt = 0;
		moy = 0;
	
		printf("%s : %f ms , nb_pas = %f\n",noms[i],running_time[i]/NB_SIMULS,nb_pas[i]/NB_SIMULS);
		for(int j=0;j<NB_SIMULS;j++)
		{
			if(res[i][j]!=INT_MAX)
			{
				cmpt++;
				moy += res[i][j];
				fprintf(f[i],"%d \n",res[i][j]);
			}
			
			//fprintf(f[i],"%d \n",(res[i][j]/interval_size)  * interval_size);
		}
		printf("Sucess algo %s = %f \n",noms[i],(float)cmpt/NB_SIMULS);
		printf("%lld \n",moy/NB_SIMULS);
		fclose(f[i]);
	}
	
	sprintf(buf,"taboo");
	char * ylabels2[] = {"Number of instances"};
	print_gnuplot_distrib(buf,noms, nb_algos, "Cumulative distribution of the Latency", "Additional latency (tics)", ylabels2);
	
		
}*/