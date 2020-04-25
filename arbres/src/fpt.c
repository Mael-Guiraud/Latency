#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "starSPALL.h"

#include <sys/time.h>
#include <limits.h>
#include "config.h"
#include "voisinage.h"
#include "treatment.h"
#include "greedy_waiting.h"
#include "borneInf.h"


int nb_feuilles;
#define NB_COUPES 4
int nb_coupes[NB_COUPES];
double coupe_moy[NB_COUPES];




int rec_arcs(Graph g,int arcid,Period_kind kind, int P, int message_size,int borneinf);
//Fait l'arbre recursif avec tous les sous ensemble de routes 
int rec_orders(Graph g, int arcid,Period_kind kind, int message_size, int P,int profondeur,int borneinf,int offset,int begin)
{
	int val_D = INT_MAX;
	int val_G;
	int nb_routes = g.arc_pool[arcid].nb_routes;
	int retour;
	int premier;int r_t;
	printf("Arc %d kind %d profondeur %d(%droutes)\n",arcid,kind,profondeur,nb_routes);
	int a;
	int old_offset;
	if(profondeur == 0)
	{
			
		if(kind == FORWARD)
			premier = g.arc_pool[arcid].routes_order_f[profondeur];
		else
			premier = g.arc_pool[arcid].routes_order_b[profondeur];
		printf("premier %d \n",premier);

		begin = route_length_untill_arc(g,premier,&g.arc_pool[arcid],kind);
		if(kind == BACKWARD)
		{
			begin += route_length_with_buffers_forward(g, premier);
		}
		if(kind == FORWARD)
			g.arc_pool[arcid].routes_delay_f[premier] =  0;
		else
			g.arc_pool[arcid].routes_delay_b[premier] =  0;
		offset = begin+message_size;
		printf("begin %d offset %d \n",begin,offset);
		rec_orders(g,arcid,kind,message_size,P,profondeur+1,borneinf,offset,begin);
	}
	else
	{
			if(profondeur == nb_routes)
			{

				//printf("arcid %d nbroutes %d kind %d order :",arcid,g.arc_pool[arcid].nb_routes,kind);
				/*if(kind == FORWARD)
					for(int i=0;i<g.arc_pool[arcid].nb_routes;i++)
					{
						printf("%d ",g.arc_pool[arcid].routes_delay_f[g.arc_pool[arcid].routes_order_f[i]]);
					}
				
				printf("\n");*/
				//if((arcid == g.nb_bbu + g.nb_collisions-1)&&(kind == BACKWARD))
				if(arcid == 0)
				{	
					//printf("Calcul assingment \n");
					nb_feuilles++;
					a = assignment_with_orders_vois1FPT(g, P, message_size, borneinf);
					retour = travel_time_max_buffers(g);
					reset_periods(g,P);
					if(a)
					{

						//free_assignment(a);
						printf("REtour %d \n",retour);
						//exit(12);
						return retour;
					}
					else
					{
						printf("REtour INTMAX \n");
						//exit(13);
						//free_assignment(a);
						return INT_MAX;

					}
					
					
				}
				else
				{
					
					//Quand on arrive ici, on a calculé le delay pour toutes les routes, on met donc l'arc a "bounded", ce qui simplifie le calcul pour la borne inf
					g.arc_pool[arcid].bounded = 1;
					//coupe si on dépasse la borneinf du greedy
					int cut = borneInfFPT( g, P, message_size,borneinf);
					//printf("arc %d kind %d Coupe %d borneinf %d\n",arcid,kind, cut, borneinf);
					if( cut > borneinf)
					{
						nb_coupes[0]++;
						coupe_moy[0]+=g.nb_bbu+g.nb_collisions-1 - arcid;
					//	printf("Coupe > borneinf deja trouvée %d %d \n",cut,borneinf);
						return INT_MAX;
					}
					
					
						if(kind == FORWARD)
						{
							//printf("Enfonce forward \n");
							return rec_arcs(g,arcid-1,kind,P,message_size,borneinf);
						}
						else
						{
							//printf("remonte backward\n");
							return rec_arcs(g,arcid+1,kind,P,message_size,borneinf);
						}
					//}
				}
				
			}
			else
			{
				old_offset = offset;
				//Parcours ou on ne change pas les periodes
				int current_route = g.arc_pool[arcid].routes_order_f[profondeur];
				printf("current_route %d arc %d\n",current_route,arcid);
				r_t = route_length_untill_arc(g,current_route,&g.arc_pool[arcid],FORWARD);
			
				printf("rt %d offset %d\n",r_t,offset);
				retval r = calcul_delay(begin,offset,P,r_t,message_size,0);//ici le dernier argument est a 0 car on met la route dans la permiere periode
				
				g.arc_pool[arcid].routes_delay_f[current_route] =  r.delay;
				offset = r.new_offset;
				 printf("offset %d , rdelay %d begin %d p %d\n",offset,r.delay,begin,P);
				if(offset <= begin+P)
				{
					printf("Appel a droite\n");
					val_D =  rec_orders(g,arcid,kind,message_size,P,profondeur+1,borneinf,offset,begin);

					//Opti N1, on ne la fait que dans le cas ou la route ne peut pas etre d'office placée que en seconde periode
					// (dans ce cas la fonction calcul delay renvoie 0 de delay, ce qui est filtré par le test d'avant)
					// On remonte, donc on enleve la route qu'on viens de mettre 
					g.arc_pool[arcid].routes_delay_f[current_route]=0;
					if(r.delay > 0)//Si on plus de 0 a de delay, on est collé, useless de mettre dans la seconde peride
					{
						nb_coupes[2]++;
						coupe_moy[2]+=g.nb_bbu+g.nb_collisions-1 - arcid;
						printf("coupe opti 1											%d prof %d route %d\n",arcid,profondeur,current_route);
						return val_D;
					
					}

					printf("avant opt 2													%d \n",arcid);
					//Calcul du i+1 pour l'opti numero 2
					if(profondeur < nb_routes-1)
					{
						
						int current_route2  = g.arc_pool[arcid].routes_order_f[profondeur+1];
						int r_t2 = route_length_untill_arc(g,current_route2,&g.arc_pool[arcid],FORWARD);
					
						printf("rt2 %d offset %d\n",r_t2,offset);
						retval r2 = calcul_delay(begin,offset,P,r_t2,message_size,0);//ici le dernier argument est a 0 car on met la route dans la permiere periode
						 printf("offset %d , rdelay2 %d begin %d p %d\n",offset,r2.delay,begin,P);
						//Deja si ca dépasse, on quitte 
						if(offset > begin+P)
						{
							printf("on sort pck ca dépasse\n");
							nb_coupes[1]++;
							coupe_moy[1]+=g.nb_bbu+g.nb_collisions-1 - arcid;
							g.arc_pool[arcid].routes_delay_f[current_route]=0;
							return INT_MAX;
						}
						//Si i+1 pas collé, on ne fait pas valG
						if(r2.delay > 0)
						{
							nb_coupes[3]++;
							coupe_moy[3]+=g.nb_bbu+g.nb_collisions-1 - arcid;
							return val_D;
						}
					}
					printf("apres opt 2\n");
					
					///// FIN OPTI N2 /////
				}
				else
				{
					//printf("Pas d'apel a droite													%d\n",arcid);
					nb_coupes[1]++;
					coupe_moy[1]+=g.nb_bbu+g.nb_collisions-1 - arcid;
				}
				
				
				g.arc_pool[arcid].routes_delay_f[current_route]=0;
				
				
		

				offset = old_offset;

			
				//printf("rt %d offset %d\n",r_t,offset);
				r = calcul_delay(begin,offset,P,r_t,message_size,1);//ici le dernier argument est a 1 car seconde periode
				
				g.arc_pool[arcid].routes_delay_f[current_route] =  r.delay;
				offset = r.new_offset;
				// printf("offset %d , rdelay %d begin %d p %d\n",offset,r.delay,begin,P);
				if(offset > begin+P)
				{
					nb_coupes[1]++;
					coupe_moy[1]+=g.nb_bbu+g.nb_collisions-1 - arcid;
					printf("coupe avant gauche\n");
					g.arc_pool[arcid].routes_delay_f[current_route]=0;
					return val_D;
				}
			
				val_G =  rec_orders(g,arcid,kind,message_size,P,profondeur+1,borneinf,offset,begin);
				// On remonte, donc on enleve la route qu'on viens de mettre 
				g.arc_pool[arcid].routes_delay_f[current_route]=0;
		
				return (val_G>val_D)?val_D:val_G;

			}
	}
	
}

int rec_arcs(Graph g,int arcid,Period_kind kind, int P, int message_size,int borneinf)
{
	
	element_sjt permuts[g.arc_pool[arcid].nb_routes]; 
	for(int i=0;i<g.arc_pool[arcid].nb_routes;i++)
	{
		permuts[i].val = i;
		permuts[i].sens = 0;
	}	
	
	int returnvalue;
	
	long long facto=fact(g.arc_pool[arcid].nb_routes);
	

	int tab[g.arc_pool[arcid].nb_routes];
	for(int j=0;j<g.arc_pool[arcid].nb_routes;j++)
	{
				tab[j]=g.arc_pool[arcid].routes_id[j];
	}
	//Pour tout les ordres de routes :
	for(int i=0;i<facto;i++)
	{
	
		for(int j=0;j<g.arc_pool[arcid].nb_routes;j++)
		{
			if(arcid < NB_BBU)
			{
				g.arc_pool[arcid].routes_order_f[j] = tab[permuts[j].val];
				g.arc_pool[arcid].routes_order_b[j] = tab[permuts[j].val];	
			}
			else
			{
				if(kind == FORWARD)
				{
					g.arc_pool[arcid].routes_order_f[j] = tab[permuts[j].val];
				}
				else
				{
					g.arc_pool[arcid].routes_order_b[j] = tab[permuts[j].val];
				}
			}
		}
		print_tab(permuts,g.arc_pool[arcid].nb_routes);
		returnvalue = rec_orders(g, arcid, kind,  message_size,  P,0,borneinf,0,0);
		if(returnvalue < borneinf)
			borneinf = returnvalue;
		if(i!=facto-1)
			algo_sjt(permuts,g.arc_pool[arcid].nb_routes);
		for(int j=0;j<128;j++)
		{
			g.arc_pool[arcid].routes_delay_f[j]=0;
			g.arc_pool[arcid].routes_delay_b[j]=0;
		}
		g.arc_pool[arcid].bounded = 0;
	}

	return borneinf;
}
int facto(int n)
{
	if(n<=2)
		return n;
	return n*fact(n-1);
}
int pow2n(int n)
{

	return 1<<(n-1);
}

int count_feuilles_arbre(Graph g)
{
	long long nb = 1;
	for(int i=0;i<g.nb_bbu+g.nb_collisions;i++)
	{
		//printf("%d %d \n",facto(g.arc_pool[i].nb_routes) ,pow2n(g.arc_pool[i].nb_routes));
		nb *= ( facto(g.arc_pool[i].nb_routes) * pow2n(g.arc_pool[i].nb_routes));
	}
	return nb;
}
double time_diff(struct timeval tv1, struct timeval tv2)
{
    return (((double)tv2.tv_sec*(double)1000 +(double)tv2.tv_usec/(double)1000) - ((double)tv1.tv_sec*(double)1000 + (double)tv1.tv_usec/(double)1000));
}


int branchbound(Graph g,int P, int message_size)
{
	
    struct timeval tv1, tv2;

	nb_feuilles = 0;
	
	for(int i=0;i<NB_COUPES;i++)
	{
		nb_coupes[i]=0;
		coupe_moy[i]=0.0;
	}

	int borneinf=greedy_deadline_assignment( g, P, message_size);
	//printf("borneinf %d \n",borneinf);
	//printf("Taille de l'arbre FPT = %d \n",count_feuilles_arbre(g));
	reinit_delays(g);
	 gettimeofday (&tv1, NULL);
	int ret = rec_arcs(g,g.nb_bbu+g.nb_collisions-1,FORWARD,P,message_size,borneinf);
	gettimeofday (&tv2, NULL);	
	/*printf("%f%% %d %d Des feuilles explorées \n \n",(double)nb_feuilles*100/(double)count_feuilles_arbre(g),nb_feuilles,count_feuilles_arbre(g));
	for(int i=0;i<NB_COUPES;i++)
	{
		printf("Coupe %d : \n 	-Nombre : %d\n 	Hauteur moyenne de coupe %f \n",i,nb_coupes[i],coupe_moy[i]/nb_coupes[i]);
	
	}

	printf("Temps de calcul : %f ms.\n",time_diff(tv1,tv2));*/
	//printf("Nombre appels fonction arcs = %d nombre appels fonction orders = %d nombre de feuille calculées = %d \n",nb_appels_arc,nb_appels_orders,nb_feuilles);
	return ret;
}



