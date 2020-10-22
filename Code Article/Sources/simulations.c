/***
Copyright (c) 2018 Guiraud Maël
All rights reserved.
*///

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <limits.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#include "struct.h"
#include "PAZL.h"
#include "PALL.h"
#include "simons.h"
#include "init.h"
#include "operations.h"
#include "tests.h"
#include "random.h"
#include "graphes.h"

#define PARALLEL 1

double time_diff(struct timeval tv1, struct timeval tv2)
{
    return (((double)tv2.tv_sec*(double)1000 +(double)tv2.tv_usec/(double)1000) - ((double)tv1.tv_sec*(double)1000 + (double)tv1.tv_usec/(double)1000));
}

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
			res_search = linear_search(g,taille_message);
			//res_search = 1;
			res_brute = res_search;
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

	for(int j = taille_message*nb_routes ; j<=taille_message*nb_routes/0.4;j+=(taille_message*nb_routes/0.4 - taille_message*nb_routes )*0.0083)
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
	float a0,a1,a2,b,c,d,e;
	int tmax;
	int nb_rand = 1;
	int * m_i;
	int * offsets;
	int permutation[nb_routes];
	int t0,t1,t2;
	
		
	for(int marge=0;marge<= marge_max;marge += 100)
	{
		a0=0;
		a1=0;
		a2=0;
		b=0;
		c=0;
		d=0;
		e=0;
		#pragma omp parallel for private(t0,t1,t2,m_i,offsets,permutation,resa,resb,resc,resd,rese,g,tmax) if (PARALLEL) schedule (static)
		for(int i = 0;i<nb_simuls;i++)
		{

			g = init_graphe(2*nb_routes+1);
			graphe_etoile(g,taille_route);
			tmax = marge + longest_route(g);
			for(int k=0;k<nb_routes;k++)
			{
				permutation[k]=k;
			}
			
			//affiche_etoile(g);
			//printf("TMAX = %d\n",tmax);
			//printf("tmax = %d(%d + %d) \n",tmax,longest_route(g),marge);
			
			t0=0;t1=0;t2=0;
			for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
			{
				fisher_yates(permutation,nb_routes);
				if(!t0)
				{
					
					offsets= retourne_offset(g, taille_paquets, permutation,0,periode);
					m_i = retourne_departs( g, offsets);
					resa = longest_etoile_periodique(g,taille_paquets,periode,tmax,-1,m_i);

				
					if((resa != -1)&&(resa != -2))
					{	
						t0 = 1;
						#pragma omp atomic
							a0++;						
					}
					free(m_i);
					free(offsets);
				}
				if(!t1)
				{
					
					offsets= retourne_offset(g, taille_paquets, permutation,1,periode);
					m_i = retourne_departs( g, offsets);
					resa = longest_etoile_periodique(g,taille_paquets,periode,tmax,-1,m_i);

					
					if((resa != -1)&&(resa != -2))
					{	
						t1 = 1;
						#pragma omp atomic
							a1++;						
					}
					free(m_i);
					free(offsets);
				}

				if(!t2)
				{
					
					offsets= retourne_offset(g, taille_paquets, permutation,2,periode);
					m_i = retourne_departs( g, offsets);
					resa = longest_etoile_periodique(g,taille_paquets,periode,tmax,-1,m_i);

				
					if((resa != -1)&&(resa != -2))
					{	
						t2 = 1;
						#pragma omp atomic
							a2++;						
					}
					free(m_i);
					free(offsets);
				}
					
				if(t0 && t1 && t2)
					break;
			}

			resb = longest_etoile_periodique(g,taille_paquets,periode,tmax,1,NULL);
			resc = longest_etoile_periodique(g,taille_paquets,periode,tmax,2,NULL);
			resd = longest_etoile_periodique(g,taille_paquets,periode,tmax,3,NULL);
			rese = longest_etoile_periodique(g,taille_paquets,periode,tmax,4,NULL);


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
		
	
   		     fprintf(F,"%d %f %f %f %f %f %f %f\n",marge,a0/nb_simuls*100,a1/nb_simuls*100,a2/nb_simuls*100,b/nb_simuls*100,c/nb_simuls*100,d/nb_simuls*100,e/nb_simuls*100);
		fprintf(stdout,"%d %f %f %f %f %f %f %f\n",marge,a0/nb_simuls*100,a1/nb_simuls*100,a2/nb_simuls*100,b/nb_simuls*100,c/nb_simuls*100,d/nb_simuls*100,e/nb_simuls*100);
	

	}
	fclose(F);
}

//Taux de reussite des algos PALL avec un aller random sur une periode donnée (on fait varier la marge)
void sucess_retour_PALL(int nb_routes, int taille_paquets,int taille_route,int marge_max, int nb_simuls, int periode,int nb_rand)
{

	char nom[64];
	sprintf(nom,"../datas/compare_retour_%d.data",periode);
	FILE * F = fopen(nom,"w");
	FILE * F2 = fopen("../datas/exec_time.data","w");
	Graphe g ;
	int resgp,ress,ressp,resfpt;
	float gp,s,sp,fpt;
	int tmax;

	int * m_i;
	int * offsets;
	int permutation[nb_routes];
	int sp_found,fpt_found,gp_found,s_found;
	struct timeval tv1, tv2;
	double timesp = 0.0;
	double timefpt=0.0;
	
			
		
	for(int marge=0;marge<= marge_max;marge += 150)
	{
		gp=0;
		s=0;
		fpt = 0;
		sp =0;
		timesp = 0.0;
		timefpt=0.0;
		

		#pragma omp parallel for private(resgp,ress,ressp,resfpt,g,tmax,m_i,offsets,permutation,gp_found,s_found,sp_found,fpt_found,tv1,tv2) if (PARALLEL) schedule (dynamic)
		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(2*nb_routes+1);
			graphe_etoile(g,taille_route);
			//graphe_etoile_dur( g,taille_route,1000);
			//graphe_etoile_Psur2( g, taille_route,1000 );
			tmax = marge + longest_route(g);
			//printf("-------------\nGraphe : \n");
			//affiche_etoile(g);
			//printf("\n");
			//printf("TMAX = %d\n",tmax);
			//printf("tmax = %d(%d + %d) \n",tmax,longest_route(g),marge);
			

				
				gp_found = 0;
				s_found = 0;
				sp_found = 0;
				if(!SYNCH)
					fpt_found = 0;
				else
					fpt_found = 1;
				
				for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
				{

					for(int k=0;k<nb_routes;k++)
					{
						permutation[k]=k;
					}
					fisher_yates(permutation, nb_routes);
					offsets= retourne_offset(g, taille_paquets, permutation,0,periode);
					m_i = retourne_departs( g, offsets);
					if(!gp_found)
					{
						resgp = longest_etoile_periodique(g,taille_paquets,periode,tmax,0,m_i);
						//resgp = 1;
						if(resgp != -2)
						{	
							if(resgp != -1)
							{
								gp_found = 1;
								#pragma omp atomic
									gp++;

							}
							
						}
					}
					if(!s_found)
					{
						ress = simons(g,taille_paquets,tmax,periode,0,m_i);
						//ress = 1;
						if(ress != -2)
						{	
							if(ress != -1)
							{	
								s_found = 1;				
								#pragma omp atomic
									s++;
							}
							
						}
					}
					
					
					if(!fpt_found)
					{
						gettimeofday (&tv1, NULL);
						resfpt = FPT_PALL(g,taille_paquets, tmax, periode, m_i);
						gettimeofday (&tv2, NULL);
						timefpt += time_diff(tv1,tv2);
						if(resfpt != -1)
						{
							fpt_found = 1;
							#pragma omp atomic
								fpt++;
						}
					}
					if(!sp_found)
					{
						gettimeofday (&tv1, NULL);
						ressp = simons_periodique(g,taille_paquets,tmax,periode,m_i);
						gettimeofday (&tv2, NULL);
						timesp += time_diff(tv1,tv2);
						if(ressp != -1)
						{
							sp_found = 1;
							#pragma omp atomic
								sp++;
						}
					}
					free(m_i);
					free(offsets);
					if((sp_found)&&(fpt_found)&&(s_found)&&(gp_found))
						break;
					
				}
				if(!SYNCH)
					if(fpt_found && (!sp_found))
					{
						affiche_etoile(g);
						print_dot(g);
					}
				if( (!fpt_found) && (sp_found))
				{
					printf("IMPOSSIBLE\n");
					exit(21);
				}
				//exit(55);
				


		
			//printf("-----------------------------------------\n");
			libere_matrice(g);

		}
		
	
   			      fprintf(F,"%d %f %f %f %f\n",marge,gp/nb_simuls*100,s/nb_simuls*100,sp/nb_simuls*100,fpt/nb_simuls*100);
   		     fprintf(stdout,"%d %f %f %f %f\n",marge,gp/nb_simuls*100,s/nb_simuls*100,sp/nb_simuls*100,fpt/nb_simuls*100);
   		     	 fprintf(F2,"%d %f %f \n",marge,timesp/nb_simuls,timefpt/nb_simuls);
   		     fprintf(stdout,"%d %f %f \n",marge,timesp/nb_simuls,timefpt/nb_simuls);



	}
	fclose(F);
	fclose(F2);

}

//Taux de reussite des diferents nombres d'instances générées
void nombre_random_PALL(int nb_routes, int taille_paquets,int taille_route, int nb_simuls, int periode)
{

	char nom[64];
	sprintf(nom,"../datas/nombre_randoms.data");
	FILE * F = fopen(nom,"w");
	Graphe g ;
	int ressp,resgp,resfpt;
	//float sp,gp,fpt;
	int tmax;
	
	int * m_i;
	int * offsets;
	int permutation[nb_routes];
	int sp_found,fpt_found,gp_found;
	float resugp[5];
	float resusp[5];
	float resufpt[5];
	for(int i=0;i<5;i++)
	{
		resugp[i]=0;
		resusp[i]=0;
		resufpt[i]=0;
	}
	
		
		#pragma omp parallel for private(ressp,resgp,resfpt,sp_found,gp_found,fpt_found,g,tmax,m_i,offsets,permutation) if (PARALLEL) schedule (static)
		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(2*nb_routes+1);
			graphe_etoile(g,taille_route);
			tmax =  longest_route(g);

			//affiche_etoile(g);
			//printf("TMAX = %d\n",tmax);
			//printf("tmax = %d(%d + %d) \n",tmax,longest_route(g),marge);
			sp_found = 0;
			fpt_found=0;
			gp_found=0;
			double did;
			int id;
			for(int compteur_rand = 1;compteur_rand<=100000;compteur_rand++)
				{

					for(int k=0;k<nb_routes;k++)
					{
						permutation[k]=k;
					}
					fisher_yates(permutation, nb_routes);
					offsets= retourne_offset(g, taille_paquets, permutation,0,periode);
					m_i = retourne_departs( g, offsets);
					if(!gp_found)
					{
						resgp = longest_etoile_periodique(g,taille_paquets,periode,tmax,0,m_i);
						//resgp = 1;
						if(resgp != -2)
						{	
							if(resgp != -1)
							{
								gp_found = 1;
								did = log10(compteur_rand);
								id = (int)did;
								if(did != id)
									id ++;
								#pragma omp atomic
									resugp[id]++;

							}
							
						}
					}
					
					if(!fpt_found)
					{
					
						resfpt = FPT_PALL(g,taille_paquets, tmax, periode, m_i);
				
						if(resfpt != -1)
						{
							fpt_found = 1;
							did = log10(compteur_rand);
							id = (int)did;
							if(did != id)
								id ++;
							#pragma omp atomic
								resufpt[id]++;
						}
					}
					if(!sp_found)
					{
					
						ressp = simons_periodique(g,taille_paquets,tmax,periode,m_i);
					
						if(ressp != -1)
						{
							sp_found = 1;
							did = log10(compteur_rand);
							id = (int)did;
							if(did != id)
								id ++;
						//	printf ("%d %f %d\n",id,did,compteur_rand);
							#pragma omp atomic
								resusp[id]++;
						}
					}
					free(m_i);
					free(offsets);
					if((sp_found)&&(fpt_found)&&(gp_found))
						break;

				}
				
						


		
			//printf("-----------------------------------------\n");
			libere_matrice(g);
			fprintf(stdout,"\r %d  per cent of experiments done", ((i+1)*100)/(nb_simuls));
			fflush(stdout); 
			
		}
		float sumgp = 0;
		float sumsp = 0;
		float sumfpt = 0;
		for(int i=0;i<5;i++)
		{
			//printf("%d %f %f %f\n",i,resugp[i],resusp[i],resufpt[i]);
			sumgp += resugp[i];
			sumsp += resusp[i];
			sumfpt += resufpt[i];
			 fprintf(F,"%f %f %f %f\n",pow(10,i),sumgp/nb_simuls*100,sumsp/nb_simuls*100,sumfpt/nb_simuls*100);     
			 fprintf(stdout,"%f %f %f %f\n",pow(10,i),sumgp/nb_simuls*100,sumsp/nb_simuls*100,sumfpt/nb_simuls*100);     
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
			ressto=stochastic(g,taille_paquets,periode,1000,1,1);
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



//Taux de reussite des allers sur pmls avec 0 de marge
void allers_random_PMLS(int nb_routes, int taille_paquets,int taille_route, int nb_simuls, int pmin,int pmax)
{

	char nom[64];
	
	Graphe g ;
	int ressp;
	int t0,t1,t2;
	float sp0,sp1,sp2;
	int tmax;
	int nb_rand = 1000;

	int * m_i;
	int * offsets;
	int permutation[nb_routes];
	sprintf(nom,"../datas/allers_random.data");
	FILE * F = fopen(nom,"w");		

	for(int periode=pmin;periode<pmax;periode+=100)
	{		
		sp0 =0;
		sp1 =0;
		sp2 =0;
		#pragma omp parallel for private(ressp,t0,t1,t2,g,tmax,m_i,offsets,permutation) if (PARALLEL) schedule (dynamic)
		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(2*nb_routes+1);
			graphe_etoile(g,taille_route);
			tmax = longest_route(g);
			for(int k=0;k<nb_routes;k++)
			{
				permutation[k]=k;
			}
			
			
			//affiche_etoile(g);
			//printf("TMAX = %d\n",tmax);
			//printf("tmax = %d(%d + %d) \n",tmax,longest_route(g),marge);
		
			t0=0;t1=0;t2=0;
			for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
			{
				fisher_yates(permutation,nb_routes);
				if(!t0)
				{
					
					offsets= retourne_offset(g, taille_paquets, permutation,0,periode);
					m_i = retourne_departs( g, offsets);
					ressp = simons_periodique(g,taille_paquets,tmax,periode,m_i);

				
					if(ressp != -1)
					{	
						t0 = 1;
						#pragma omp atomic
							sp0++;						
					}
					free(m_i);
					free(offsets);
				}
				if(!t1)
				{
					
					offsets= retourne_offset(g, taille_paquets, permutation,1,periode);
					m_i = retourne_departs( g, offsets);
					ressp = simons_periodique(g,taille_paquets,tmax,periode,m_i);

				
					if(ressp != -1)
					{	
						t1 = 1;
						#pragma omp atomic
							sp1++;						
					}
					free(m_i);
					free(offsets);
				}

				if(!t2)
				{
					
					offsets= retourne_offset(g, taille_paquets, permutation,2,periode);
					m_i = retourne_departs( g, offsets);
					ressp = simons_periodique(g,taille_paquets,tmax,periode,m_i);

				
					if(ressp != -1)
					{	
						t2 = 1;
						#pragma omp atomic
							sp2++;						
					}
					free(m_i);
					free(offsets);
				}
					
				if(t0 && t1 && t2)
					break;
			}
			//printf("-----------------------------------------\n");
			libere_matrice(g);
			
		}
		  fprintf(F,"%f %f %f %f\n",20000/(float)periode*100,sp0/nb_simuls*100,sp1/nb_simuls*100,sp2/nb_simuls*100);
		     fprintf(stdout,"%f %f %f %f\n",20000/(float)periode*100,sp0/nb_simuls*100,sp1/nb_simuls*100,sp2/nb_simuls*100);
	
	
	}
	

		fclose(F);    
	
	
}



void stochastic_vs_PMLS(int nb_routes, int taille_paquets,int taille_route, int nb_simuls, int marge_max)
{
char nom[64];
	sprintf(nom,"../datas/stochastic.data");
	FILE * F = fopen(nom,"w");
	Graphe g ;
	int ressto,ressp;
	float sto1,sto2,sp;
	int tmax;
	int nb_rand = 1000;

	int * m_i;
	int * offsets;
	int permutation[nb_routes];
	int sp_found;

	
	int periode95 = (nb_routes*taille_paquets)/0.95;
	int periode40 = (nb_routes*taille_paquets)/0.40;
			printf("%d %d \n",periode95,periode40);
	for(int marge=0;marge<= marge_max;marge += 500)
	{

		sto1=0;
		sto2=0;
		sp =0;
		

		#pragma omp parallel for private(ressto,ressp,g,tmax,m_i,offsets,permutation,sp_found) if (PARALLEL) schedule (dynamic)
		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(2*nb_routes+1);
			graphe_etoile(g,taille_route);
			//graphe_etoile_Psur2( g, taille_route,800 );
			tmax = marge + longest_route(g);
			//printf("-------------\nGraphe : \n");
			//affiche_etoile(g);
			//printf("\n");
			//printf("TMAX = %d\n",tmax);
			//printf("tmax = %d(%d + %d) \n",tmax,longest_route(g),marge);
		
			sp_found = 0;
			
			
			for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
			{

				for(int k=0;k<nb_routes;k++)
				{
					permutation[k]=k;
				}
				fisher_yates(permutation, nb_routes);
				offsets= retourne_offset(g, taille_paquets, permutation,2,periode95);
				m_i = retourne_departs( g, offsets);
				

				if(!sp_found)
				{

					ressp = simons_periodique(g,taille_paquets,tmax,periode95,m_i);
					if(ressp != -1)
					{
						sp_found = 1;
						#pragma omp atomic
							sp++;
					}
				}
				
				free(m_i);
				free(offsets);
				if(sp_found)
					break;
				
			}
			ressto=stochastic(g,taille_paquets,periode95,100,1,0);
			if(ressto <= tmax)
			{
				#pragma omp atomic
					sto1++;
			}
			ressto=stochastic(g,taille_paquets,periode40,100,1,0);
			if(ressto <= tmax)
			{
				#pragma omp atomic
					sto2++;
			}
				//exit(55);
			libere_matrice(g);

		}
		
	
   			      fprintf(F,"%d %f %f %f \n",marge,sp/nb_simuls*100,sto1/nb_simuls*100,sto2/nb_simuls*100);
   			      fprintf(stdout,"%d %f %f %f\n",marge,sp/nb_simuls*100,sto1/nb_simuls*100,sto2/nb_simuls*100);




	}
	fclose(F);



}



void distrib_margins_departs(int nb_routes, int taille_paquets,int taille_route,int margin_max, int nb_simuls, int periode,int nb_rand)
{

	char nom[64];
	FILE * F;

	Graphe g ;
	int ressp;

	int tmax;
	int * m_i;
	int * offsets;
	int permutation[nb_routes];
	int sp_found;
	float sp;
	float sum;
	int step = 100;
			
	int res[margin_max/step];
	printf("%d %d %d\n",margin_max,step,margin_max/step);
	for(int margin=100;margin<= margin_max;margin *= 2)
	{
		for(int i=0;i*step<margin_max;i++){res[i]=0;}
		sp = 0;
		sprintf(nom,"../datas/distrib_departs%d.data",margin);
		F = fopen(nom,"w");
		

		#pragma omp parallel for private(ressp,g,tmax,m_i,offsets,permutation,sp_found) if (PARALLEL) schedule (dynamic)
		for(int i = 0;i<nb_simuls;i++)
		{

			g = init_graphe(2*nb_routes+1);
			//graphe_etoile(g,taille_route);
			//graphe_etoile_dur( g,periode,margin);
			graphe_etoile_Psur2( g, periode,margin);
			
			//printf("-------------\nGraphe : \n");
			//affiche_etoile(g);
			//printf("\n");
			//printf("TMAX = %d\n",tmax);
			//printf("tmax = %d(%d + %d) \n",tmax,longest_route(g),marge);
			

				
			for(int marge=0;marge< 3000;marge+=100)
			{

				sp_found =0;
				tmax = marge + longest_route(g);
				for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
				{

					for(int k=0;k<nb_routes;k++)
					{
						permutation[k]=k;
					}
					fisher_yates(permutation, nb_routes);
					offsets= retourne_offset(g, taille_paquets, permutation,0,periode);
					m_i = retourne_departs( g, offsets);
					

					//printf("%d %d %d %d\n",taille_paquets,tmax,periode,longest_route(g));
					//ressp = simons_periodique(g,taille_paquets,tmax,periode,m_i);
					ressp =  FPT_PALL(g,taille_paquets, tmax, periode, m_i);
					if(ressp != -1)
					{
						
						#pragma omp atomic
							res[marge/step]++;
						//fprintf(F,"%d %d \n",marge,1);
						//fprintf(stdout,"%d %d \n",ressp,1);
						#pragma omp atomic
							sp++;
						sp_found = 1;
						free(m_i);
						free(offsets);
						break;
					}
					
					
				
					free(m_i);
					free(offsets);
					
				}
				if(sp_found)
					break;

			}

			//printf("-----------------------------------------\n");
			libere_matrice(g);

			{fprintf(stdout,"\r%d/%d",i,nb_simuls);fflush(stdout);}
			

		}
		sum = 0.0;
		for(int i=0;i*step<margin_max;i++)
		{
			sum += (float)res[i]/nb_simuls;
			fprintf(F,"%d %f \n",i*step,sum);
		}
		fprintf(stdout,"%d %f\n",margin,sp/nb_simuls);
		fclose(F);
	
	}


}




void tps_FPT_PALL(int nb_routes_max, int taille_paquets,int taille_route,int marge, int nb_simuls,int nb_rand,float load)
{

	Graphe g ;
	int resfpt,ressp;
	float fpt,sp;
	int tmax;

	int * m_i;
	int * offsets;
	double time_fisher;
	
	int fpt_found,sp_found;
	struct timeval tv1, tv2;
	int periode;
	double timefpt=0.0;
	double timesp=0.0;
	
			
		
	for(int nb_routes=8;nb_routes<= nb_routes_max;nb_routes += 4)
	{
		timefpt=0.0;
		timesp=0.0;
		periode= (int)nb_routes*taille_paquets / load;
		taille_route = periode;
		int permutation[nb_routes];
		fpt = 0.0;
		sp = 0.0;
		#pragma omp parallel for private(resfpt,ressp,g,tmax,m_i,offsets,permutation,fpt_found,sp_found,tv1,tv2,time_fisher) if (PARALLEL) schedule (dynamic)
		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(2*nb_routes+1);
			graphe_etoile(g,taille_route);

			tmax = marge + longest_route(g);
			//printf("-------------\nGraphe : \n");
			//affiche_etoile(g);
			//printf("\n");
			//printf("TMAX = %d\n",tmax);
			//printf("tmax = %d(%d + %d) \n",tmax,longest_route(g),marge);
			
				fpt_found = 0;
				sp_found = 0;
				
				for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
				{

					gettimeofday (&tv1, NULL);
					for(int k=0;k<nb_routes;k++)
					{
						permutation[k]=k;
					}
					fisher_yates(permutation, nb_routes);
					offsets= retourne_offset(g, taille_paquets, permutation,0,periode);
					m_i = retourne_departs( g, offsets);
					gettimeofday (&tv2, NULL);
					time_fisher = time_diff(tv1,tv2);
					
					
					if(!fpt_found)
					{
						gettimeofday (&tv1, NULL);
						resfpt = FPT_PALL(g,taille_paquets, tmax, periode, m_i);
						
						if(resfpt != -1)
						{
							fpt_found = 1;
							#pragma omp atomic
								fpt++;
						}
						gettimeofday (&tv2, NULL);
						#pragma omp atomic
							timefpt += (time_diff(tv1,tv2)+time_fisher);
					}
					
					
					if(!sp_found)
					{
						gettimeofday (&tv1, NULL);
						
						ressp = simons_periodique(g,taille_paquets, tmax, periode, m_i);
						
						if(ressp != -1)
						{
							sp_found = 1;
							#pragma omp atomic
								sp++;
						}
						gettimeofday (&tv2, NULL);
						#pragma omp atomic
							timesp += (time_diff(tv1,tv2)+time_fisher);
					}
					

					free(m_i);
					free(offsets);
					if(fpt_found && sp_found)
						break;
					
				}
				
				

			//printf("-----------------------------------------\n");
			libere_matrice(g);
			if(i%(nb_simuls/100) == 0){fprintf(stdout,"\r%3d%%",i);fflush(stdout);}

		}

   	 fprintf(stdout,"nb routes : %d\n FPT : %f %f \n SP : %f %f \n rapport = %f \n\n",nb_routes,timefpt/nb_simuls,fpt/nb_simuls,timesp/nb_simuls,sp/nb_simuls,timefpt/timesp);


	}

}
