#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <limits.h>
#include <math.h>
#include <sys/time.h>

#include "struct.h"
#include "PAZL.h"
#include "PALL.h"
#include "simons.h"
#include "init.h"
#include "operations.h"
#include "tests.h"
#include "random.h"

#define PARALLEL 1

//Effectue une recherche linéaire pour les algos PAZL afin de trouver la plus petite periode moyenne
void simuls_periode_PAZL(int nb_routes, int taille_message, int taille_routes,int nb_simuls)
{
	FILE * F = fopen("../datas/results_periode.data","w");
	Graphe g;
	long long int total_3NT, total_brute,total_sl,total_search;
	int res_sl,res_brute,res_3NT,res_search;

	
	for(int j = 2 ; j<=nb_routes;j++)
	{
		printf("Calculs pour %d routes: \n",j);
		
		total_3NT = 0;
		total_brute = 0;
		total_sl =0;
		total_search = 0;
		#pragma omp parallel for private(res_sl,res_brute,res_3NT,res_search,g) if (PARALLEL) schedule (static)
		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(j*2 + 1);
			graphe_etoile(g,taille_routes);
			res_3NT = linear_3NT(g,taille_message);
			//if(res_3NT > total_3NT)total_3NT = res_3NT;
			if(res_3NT != -1){
				#pragma omp atomic
				total_3NT+=res_3NT;
			}
			//else printf("error (3nt = -1)\n");

			//res_brute = linear_brute(g,taille_message);
			res_brute = 1;
			//if(res_brute > total_brute)total_brute = res_brute;
			if(res_brute != -1){
				#pragma omp atomic
				total_brute+=res_brute;
			}
			//else printf("error (brute = -1)\n");

			res_sl = algo_shortest_longest(g,3*taille_message*nb_routes,taille_message);
			//if(res_sl > total_sl)total_sl = res_sl;
			if(res_sl != -1){
				#pragma omp atomic
				total_sl+=res_sl;
			}
			//else printf("error (Sl = -1)\n");
			//res_search = linear_search(g,taille_message);
			res_search = 1;
			//res_brute = res_search;
			//if(res_sl > total_sl)total_sl = res_sl;
			if(res_search != -1){
				#pragma omp atomic
				total_search+=res_search;
			}

			if( res_search != res_brute )
			{
				printf("(%d -%d) \n",res_brute,res_search);
				affiche_etoile(g);
				exit(15);
				
			}



			//if(res_sl < res_brute)affiche_matrice(g);
			fprintf(stdout,"\rStep%5d /%d",i+1,nb_simuls);fflush(stdout);
			libere_matrice(g);
		}
		printf("\n");
		
		fprintf(F, "%d %f %f %f %f %d %d\n",j,(taille_message*j)/(total_3NT/(float)nb_simuls),(taille_message*j)/(total_brute/(float)nb_simuls),(taille_message*j)/(total_search/(float)nb_simuls),(taille_message*j)/(total_sl/(float)nb_simuls),j*taille_message,3*j*taille_message);
		fprintf(stdout, "%d %f %f %f %f %d %d\n",j,(taille_message*j)/(total_3NT/(float)nb_simuls),(taille_message*j)/(total_brute/(float)nb_simuls),(taille_message*j)/(total_search/(float)nb_simuls),(taille_message*j)/(total_sl/(float)nb_simuls),j*taille_message,3*j*taille_message);
		//fprintf(stdout, "%d %lld %lld %lld %lld %d %d\n",j,total_3NT/nb_simuls,total_brute/nb_simuls,total_search/nb_simuls,total_sl/nb_simuls,j*taille_message,3*j*taille_message);
		//fprintf(F, "%d %lld %lld %lld %d %d\n",j,total_3NT,total_brute,total_sl,j*taille_message,3*j*taille_message);
		printf("\n");
	}
	fclose(F);
}


//taux de reussite de chaque algo PAZL en fonction de la periode
void echec_PAZL(int nb_routes, int taille_message,int taille_routes, int nb_simuls)
{
	FILE * F = fopen("../datas/results_echec_longues.data","w");
	Graphe g;
	long long int total_3NT, total_brute, total_sl,total_theorique;
	int res_brute;

	for(int j = taille_message*nb_routes ; j<=50000;j+=500)
	{
		
		total_3NT = 0;
		total_brute = 0;
		total_sl = 0;
		total_theorique = 0;

		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(nb_routes *2 +1);
			graphe_etoile(g,taille_routes);
			if(algo_3NT(g,j,taille_message) != -1) total_3NT++;
			res_brute = search(g,taille_message,j);
			//if(res_brute == -2) total_theorique++;
			//else
				if(res_brute != -1 ){ total_brute++;total_theorique++;}
			if(algo_shortest_longest(g,j,taille_message)!= -1) total_sl++;
			libere_matrice(g);
			fprintf(stdout,"\rStep = %5d/%d [Period : %d]",i+1,nb_simuls,j);fflush(stdout);
		}

	
		//fprintf(F, "%d %lld %lld %lld %lld\n",j,total_sl/(nb_simuls/100),total_3NT/(nb_simuls/100),total_brute/(nb_simuls/100),total_theorique/(nb_simuls/100));
		fprintf(F, "%f %lld %lld %lld %lld\n",((float)taille_message*nb_routes)/j*100,total_sl/(nb_simuls/100),total_3NT/(nb_simuls/100),total_brute/(nb_simuls/100),total_theorique/(nb_simuls/100));

	}
	fclose(F);
	printf("\n");
}

//Taux de reussite de départ PALL avec un retour GP sur une periode donnée (on fait varier la marge)
void sucess_aller_PALL(int nb_routes, int taille_paquets,int taille_route,int marge_max, int nb_simuls, int periode)
{

	char nom[64];
	sprintf(nom,"../datas/compare_departs_%d.data",periode);
	FILE * F = fopen(nom,"w");
	Graphe g ;
	int resa,resb,resc,resd,rese;
	float a,b,c,d,e;
	int tmax;
	int nb_rand = 1000;
	float moyenne_etape;
	
		
	for(int marge=0;marge<= marge_max;marge += 100)
	{
		a=0;
		b=0;
		c=0;
		d=0;
		e=0;
		moyenne_etape = 0;
		#pragma omp parallel for private(resa,resb,resc,resd,rese,g,tmax) if (PARALLEL) schedule (static)
		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(2*nb_routes+1);
			graphe_etoile(g,taille_route);
			tmax = marge + longest_route(g);
			//affiche_etoile(g);
			//printf("TMAX = %d\n",tmax);
			//printf("tmax = %d(%d + %d) \n",tmax,longest_route(g),marge);
			
			for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
				{
					resa = longest_etoile_periodique(g,taille_paquets,periode,tmax,0);
					if(resa != -2)
					{	
						if(resa != -1)
						{
							
							#pragma omp atomic
								a++;

								//printf("%f\n",moyenne_etape);
							#pragma omp atomic
								moyenne_etape+=(compteur_rand+1);
								
								
							//printf("%f(%d)\n",a,omp_get_thread_num());

							//printf("On a trouvé pour a au %d ieme tick (%d) \n",compteur_rand,resa);printf("%f\n",moyenne_etape);
							//exit(19);
							break;
						}
						
					}

				}

			resb = longest_etoile_periodique(g,taille_paquets,periode,tmax,1);
			resc = longest_etoile_periodique(g,taille_paquets,periode,tmax,2);
			resd = longest_etoile_periodique(g,taille_paquets,periode,tmax,3);
			rese = longest_etoile_periodique(g,taille_paquets,periode,tmax,4);


			if(resb != -2)
			{
				if(resb != -1)	
				{
					#pragma omp atomic
					b++;
				}
			}
			if(resc != -2)
			{
				if(resc != -1)
				{
					#pragma omp atomic
					c++;
				}
			}
			if(resd != -2)
			{
				if(resd != -1)
				{
					#pragma omp atomic
					d++;
				}
			}
			if(rese != -2)
			{
				if(rese != -1)
				{
					#pragma omp atomic
					e++;
				}
			}
			//printf("-----------------------------------------\n");
			libere_matrice(g);
		}
		
	
   		     fprintf(F,"%d %f %f %f %f %f\n",marge,a/nb_simuls*100,b/nb_simuls*100,c/nb_simuls*100,d/nb_simuls*100,e/nb_simuls*100);
		fprintf(stdout,"%d %f %f %f %f %f %f\n",marge,a/nb_simuls*100,b/nb_simuls*100,c/nb_simuls*100,d/nb_simuls*100,e/nb_simuls*100,moyenne_etape/nb_simuls);
	

	}
	fclose(F);
}

//Taux de reussite des algos PALL avec un aller random sur une periode donnée (on fait varier la marge)
void sucess_retour_PALL(int nb_routes, int taille_paquets,int taille_route,int marge_max, int nb_simuls, int periode)
{

	char nom[64];
	sprintf(nom,"../datas/compare_retour_%d.data",periode);
	FILE * F = fopen(nom,"w");
	Graphe g ;
	int resgp,ress,ressp;
	float gp,s,sp;
	int tmax;
	int nb_rand = 1000;

	
		
	for(int marge=0;marge<= marge_max;marge += 150)
	{
		gp=0;
		s=0;
		sp =0;

		#pragma omp parallel for private(resgp,ress,ressp,g,tmax) if (PARALLEL) schedule (dynamic)
		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(2*nb_routes+1);
			graphe_etoile(g,taille_route);
			tmax = marge + longest_route(g);
			//affiche_etoile(g);
			//printf("TMAX = %d\n",tmax);
			//printf("tmax = %d(%d + %d) \n",tmax,longest_route(g),marge);
			
			for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
				{
					resgp = longest_etoile_periodique(g,taille_paquets,periode,tmax,0);
					if(resgp != -2)
					{	
						if(resgp != -1)
						{
							
							#pragma omp atomic
								gp++;


							break;
						}
						
					}
				}
				
						
				for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
				{
					ress = simons(g,taille_paquets,tmax,periode,0);
					if(ress != -2)
					{	
						if(ress != -1)
						{
							
							#pragma omp atomic
								s++;


							break;
						}
						
					}
				}

				for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
				{
					ressp = simons_periodique(g,taille_paquets,tmax,periode,0);
					if(ressp != -2)
					{	
						if(ressp != -1)
						{
							
							#pragma omp atomic
								sp++;


							break;
						}
						
					}
				}


		
			//printf("-----------------------------------------\n");
			libere_matrice(g);
		}
		
	
   			      fprintf(F,"%d %f %f %f \n",marge,gp/nb_simuls*100,s/nb_simuls*100,sp/nb_simuls*100);
   		     fprintf(stdout,"%d %f %f %f \n",marge,gp/nb_simuls*100,s/nb_simuls*100,sp/nb_simuls*100);
		

	}
	fclose(F);
}

//Taux de reussite des diferents nombres d'instances générées
void nombre_random_PALL(int nb_routes, int taille_paquets,int taille_route, int nb_simuls, int periode)
{

	char nom[64];
	sprintf(nom,"../datas/nombre_randoms.data");
	FILE * F = fopen(nom,"w");
	Graphe g ;
	int res;
	float a;
	int tmax;
	

	for(int nb_rand=1;nb_rand<= 100000;nb_rand *= 10)
	{
		a=0;
		#pragma omp parallel for private(res,g,tmax) if (PARALLEL) schedule (static)
		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(2*nb_routes+1);
			graphe_etoile(g,taille_route);
			tmax =  longest_route(g);
			//affiche_etoile(g);
			//printf("TMAX = %d\n",tmax);
			//printf("tmax = %d(%d + %d) \n",tmax,longest_route(g),marge);
			
			for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
				{
					//res = longest_etoile_periodique(g,taille_paquets,periode,tmax,0);
					res = simons_periodique(g,taille_paquets,tmax,periode,0);
					if(res != -2)
					{	
						if(res != -1)
						{
							
							#pragma omp atomic
								a++;


							break;
						}
						
					}
				}
				
						


		
			//printf("-----------------------------------------\n");
			libere_matrice(g);
		}
		
	
   			      fprintf(F,"%d %f \n",nb_rand,a/nb_simuls*100);
   			      fprintf(stdout,"%d %f \n",nb_rand,a/nb_simuls*100);
 
		

	}
	fclose(F);
}


//Tmax moyen pour sto vs sp
void marge_PALL_stochastique(int nb_routes,int taille_paquets,int taille_route, int nb_simuls, int periode_max)
{

	char nom[64];
	sprintf(nom,"../datas/marge_PALL_stochastique.data");

	FILE * F = fopen(nom,"w");
	FILE * F2 = fopen("../datas/donnes_random.data","w");
	Graphe g ;
	long long int total_sto,total_sp;

	int ressto;

	int ressp;
	int nb_rand = 1000;

	int periode;
	int distrib[200];
	int distribsp[200];

	//for(int periode=taille_paquets*nb_routes;periode<periode_max ;periode+=1000)
	for(float load = 0.95;load >=0.39;load-=0.55)
	{
		periode = (int)(taille_paquets*nb_routes / load);
		total_sto = 0;
		total_sp = 0;
		for(int i=0;i<200;i++){distrib[i]=0;distribsp[i]=0;}
		printf("load = %f %d\n",load, periode);
		#pragma omp parallel for private(ressp,ressto,g) if (PARALLEL) schedule (static)
		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(2*nb_routes+1);
			graphe_etoile(g,taille_route);
	

		 	ressp = linear_simons_per(g, periode, taille_paquets,nb_rand);
		 	
		 	#pragma omp atomic
				total_sp+= ressp;
			if(ressp<15000)
			{
				if(periode == 21052)	
				{
					#pragma omp atomic
					distribsp[ressp/150]++;
				}
				else{
					#pragma omp atomic
					distribsp[ressp/150+100]++;
				}
			}
			ressto=stochastic(g,taille_paquets,periode,1000,1);
			ressto -=  longest_route(g);
			if(ressto<15000)
			{
				
				if(periode == 21052)	
				{
					#pragma omp atomic
					distrib[ressto/150]++;
				}
				else{
					#pragma omp atomic
					distrib[ressto/150+100]++;
				}
			}
				
			#pragma omp atomic
				total_sto+= ressto;

			//fprintf(F2,"%d %d %d\n",periode,ressto,ressp);
			//fprintf(stdout,"\r Period %d : step %4d",periode,i);fflush(stdout);
			libere_matrice(g);
		}
		for(int i=0;i<100;i++)
			fprintf(F2,"%d %d %d %d %d\n",i,distrib[i],distrib[i+100],distribsp[i],distribsp[i+100]);
	
   			      fprintf(F,"%d %f %f \n",periode,total_sto/(float)nb_simuls,total_sp/(float)nb_simuls);
   			      fprintf(stdout,"%d %f %f\n",periode,total_sto/(float)nb_simuls,total_sp/(float)nb_simuls);
   		

 
		

	}
	fclose(F);
	fclose(F2);
}
double time_diff(struct timeval tv1, struct timeval tv2)
{
    return (((double)tv2.tv_sec*(double)1000 +(double)tv2.tv_usec/(double)1000) - ((double)tv1.tv_sec*(double)1000 + (double)tv1.tv_usec/(double)1000));
}


double time_search(int nb_routes, int taille_message,int taille_routes)
{
	
	Graphe g;
	struct timeval tv1, tv2;
	double timer=0.0;
	
	g = init_graphe(nb_routes *2 +1);
	graphe_etoile(g,taille_routes);
	gettimeofday (&tv1, NULL);
	int period = taille_message*nb_routes*1.05;
	//printf("%d \n",period);
	search(g,taille_message,period);
	gettimeofday (&tv2, NULL);
	timer = time_diff(tv1,tv2);
		
	//printf("%f\n",timer);
		return timer;

}
void search_efficiency(int taille_message,int taille_routes, int nb_simuls)
{
	FILE * f = fopen("../datas/search_efficiency.data","w");
	double average=0;
	double max = 0.0;
	double result;
	for(int nb_routes = 1;nb_routes<=16;nb_routes+=1)
	{
		max = 0.0;
		for(int i=0;i<nb_simuls;i++)
		{
			result = time_search(nb_routes,2500,taille_routes);
			average += result;
			if(result>max)
				max=result;
			fprintf(stdout,"\r %d routes : %d%%",nb_routes,i+1);fflush(stdout);
		}
		fprintf(f,"%d %f %f\n",nb_routes,average / nb_simuls,max);
		fprintf(stdout,"\n%d routes : Average = %f ms , max = %f ms\n",nb_routes,average / nb_simuls,max);
	}
	fclose(f);
}

//Fichiers en 3D pour PALL
void succes_PALL_3D(int nb_routes, int taille_paquets,int taille_route, int nb_simuls, int mode)
{

	char nom[64];
	sprintf(nom,"../3d/gvsgp3D_%d.data",mode);

	FILE * F = fopen(nom,"w");

	Graphe g ;
	int resa,resb,resc,resd;
	float a,b,c,d;
	int tmax;

	int nb_rand = 100;


	for(int marge=0;marge<=1000;marge+=100)
	{
		for(int j=taille_paquets*nb_routes;j<taille_paquets*nb_routes*1.5;j+=500)
			{
				a=0.0;
				b=0.0;
				c=0.0;
				d=0.0;

				for(int i = 0;i<nb_simuls;i++)
				{
					g = init_graphe(2*nb_routes+1);
					graphe_etoile(g,taille_route);
					tmax = marge + longest_route(g);
					//affiche_etoile(g);
					//printf("TMAX = %d\n",tmax);
					//printf("tmax = %d(%d + %d) \n",tmax,longest_route(g),marge);
					if(mode != 0)
					{
						
						
						resa = longest_etoile_periodique(g,taille_paquets,j, tmax,mode);
						resb = longest_etoile_2(g,taille_paquets,j,tmax,mode);
						resc = simons(g,taille_paquets,tmax,j,mode);
						resd = simons_periodique(g,taille_paquets,tmax,j,mode);
						//resd = 0;
						//printf("%d %d \n",resa,resb);
						if(resa != -2)
						{	
							if(resa != -1)
							{

								a++;
							}
							
						}
						if(resb != -2)
						{
							if(resb != -1)	
							{
								b++;
							}
						}
						if(resc != -2)
						{
							if(resc != -1)
							{

								c++;
								
							}
						}
						if( ( (resc == -2)||(resc == -1) ) && ( (resb != -2)&&(resb !=-1) )  ) {printf("%d %d \n",resb,resc);exit(11);}
						if(resd != -1)
						{

							d++;
						}


					}
					else
					{
						for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
						{
							resa = longest_etoile_periodique(g,taille_paquets,j, tmax,mode);
							if(resa != -2)
							{	
								if(resa != -1)
								{

									a++;
									//printf("On a trouvé pour a au %d ieme tick (%d) \n",compteur_rand,resa);
									break;
								}
								
							}
						}
						for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
						{
							resb = longest_etoile_2(g,taille_paquets,j,tmax,mode);
							if(resb != -2)
							{	
								if(resb != -1)
								{

									b++;
									//printf("On a trouvé pour b au %d ieme tick (%d) \n",compteur_rand,resb);
									break;
								}
								
							}
						}
						for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
						{
							resc = simons(g,taille_paquets,tmax,j,mode);
			
							if(resc != -2)
							{
								if(resc != -1)
								{

									c++;
									break;
								}
							}
												
							
						}	
						for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
						{
							resd = simons_periodique(g,taille_paquets,tmax,j,mode);
							//resd = 0;
			
								if(resd != -1)
								{

									d++;
									//printf("On a trouvé pour e au %d ieme tick\n",compteur_rand);
									break;
								}
								
							
						}
						//if(resc == -1){printf("ON NA PAS TROUVE POUR e (Tmax = %d)\n",tmax);exit(40);}
					}
					


					//printf("-----------------------------------------\n");
					libere_matrice(g);
				}
			
		   		     fprintf(F,"%d %d %f %f %f %f\n",marge,j,a/nb_simuls*100,b/nb_simuls*100,c/nb_simuls*100,d/nb_simuls*100);
				fprintf(stdout,"%d %d %f %f %f %f\n",marge,j,a/nb_simuls*100,b/nb_simuls*100,c/nb_simuls*100,d/nb_simuls*100);
			//fprintf(stdout,"%d %d %f %f\n",j,k,c/nb_simuls*100,d/nb_simuls*100);
		}
		fprintf(F, "\n" );
	}
	fclose(F);
}

