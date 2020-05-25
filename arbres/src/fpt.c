#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "starSPALL.h"

#include <limits.h>
#include "config.h"
#include "voisinage.h"
#include "treatment.h"
#include "greedy_waiting.h"
#include "borneInf.h"
#include "enum.h"
/* 
Ce fichier permet de séléctionner les coupes à activer ou à désactiver pour le FPT.
*/


int AFFICHE_RES ;

///// COUPES A LA FIN D'UN ARC ///////////
//Coupe avec le calcul de borne inf à la fin de chaque arc.
int BORNEINF_ON ;

//Si une route dans la seconde periode peut être placée dans la première quelque part.
int SECONDE_DANS_PREMIERE ;

//Si i peut etre en premiere periode sans retarder i+1
int I_PLUS_1_PAS_COLLE ;

/////// COUPES AU FUR ET A MESURE DU CALCUL DES ROUTES //////
int MOD;

//Si la route i est collée dans la première periode, on ne fait pas la seconde periode
int I_COLLE ;
//Si après avoir mis i en première periode, je pourrais faire rentrer une des routes suivantes avant i dans la première periode, je ne fais pas avec i en première periode
int ROUTES_SUIVANTES_AVANT_I ;
//Si j'ai une route avec 0 de délai qui n'est pas de l'id le plus petit, je ne fait pas l'odre, car je le verrais à un autre moment.
int PAS_PLUS_PETIT_ID ;
long long nb_feuilles;


char* noms_coupes[]= {"seconde dans premiere","i+1 pas collé","i Collé","routes suivantes avant i","BorneInf","plus petit id","NotFillInArc"};
long long int nb_coupes[NB_COUPES];
double coupe_moy[NB_COUPES];



int rec_arcs(Graph *g,int arcid, int P, int message_size,int borneinf);
//Fait l'arbre recursif avec tous les sous ensemble de routes 

int rec_orders(Graph* g, int arcid, int message_size, int P,int profondeur,int borneinf,int offset,int begin,int * r_t)
{

	int val_D = INT_MAX;
	int val_G;
	int nb_routes =g->arc_pool[arcid].nb_routes;
	int premier;
	
	int old_offset;
	//printf("arc %d profondeur %d \n",arcid,profondeur);
	if(profondeur == 0)
	{

		premier = g->arc_pool[arcid].routes_order_f[profondeur];
		begin = r_t[profondeur];
		g->arc_pool[arcid].routes_delay_f[premier] =  0;

		return rec_orders(g,arcid,message_size,P,profondeur+1,borneinf,begin+message_size,begin,r_t);
	}
	else
	{

		
			if(profondeur == nb_routes)
			{
				//Avant de calculer quoi que ce soit, on fait les eventuelles coupes
				for(int i=1;i<nb_routes;i++)
				{
					//route dans la seconde periode
					if((r_t[i]+begin)%P+g->arc_pool[arcid].routes_delay_f[g->arc_pool[arcid].routes_order_f[i]] >= P)
					{

						/*if(I_PLUS_1_PAS_COLLE)
						{
							if(i<nb_routes-1)
							{
								if((r_t[i+1]+begin)%P+g->arc_pool[arcid].routes_delay_f[g->arc_pool[arcid].routes_order_f[i+1]] < (begin)%P+P)//j dans la première periode
								{
									if ((begin+r_t[i])%P + message_size  <=  (begin+P)%P)//c'était possible de mettre dans la premiere periode
									{
										if((begin+r_t[i])%P + message_size < (r_t[i+1]+begin)%P+g->arc_pool[arcid].routes_delay_f[g->arc_pool[arcid].routes_order_f[i+1]] )
										{
											nb_coupes[1]++;
											coupe_moy[1]+=g->nb_bbu+g->nb_collisions-1 - arcid;
											return INT_MAX;
										}
									}
									
								}
							}
							
						}*/
						if(SECONDE_DANS_PREMIERE)
						{
							//Si on a la place en premiere periode pour placer la route, on coupe
							int temps_min_i = (begin+r_t[i])%P;
							int temps_min_j;
							int j ;

							//Recherche de la route qui termine juste après i ou qui commence avant i
							for(j=0;j<nb_routes;j++)
							{
								if((r_t[j]+begin)%P+g->arc_pool[arcid].routes_delay_f[g->arc_pool[arcid].routes_order_f[j]] > P)//j aussi dans la seconde periode
								{
									continue;
								}
								temps_min_j = (begin+r_t[j]+g->arc_pool[arcid].routes_delay_f[g->arc_pool[arcid].routes_order_f[j]])%P;
								if((temps_min_j < temps_min_i) && (temps_min_j+message_size > temps_min_i))
								{
									//la route j finit juste apres le temps d'arrivé de i, on prends j = i+1 et on fixe la date de départ de i a la fin du message
									temps_min_i = temps_min_j+message_size;
									j=i+1;
									break;
								}
								if(temps_min_j>temps_min_i)
								{
									break;
								}
							}
							

							//printf("%d est en seconde periode  tempsmin i = %d \n",i,temps_min_i);
							if(temps_min_i < P)
							{
								for(;j<nb_routes;j++)
								{
									//printf("route j = %d \n",j);
									if((r_t[j]+begin)%P+g->arc_pool[arcid].routes_delay_f[g->arc_pool[arcid].routes_order_f[j]] > P)//j aussi dans la seconde periode
									{
										continue;
									}
									temps_min_j = (begin+r_t[j]+g->arc_pool[arcid].routes_delay_f[g->arc_pool[arcid].routes_order_f[j]])%P;
									//printf("temps_min_j = %d \n",temps_min_j);
									//Si il y a la place de mettre i en première preiode, on coupe
									if( abs( temps_min_j-temps_min_i) >= message_size)
									{
										//printf("on coupe\n");
										nb_coupes[0]++;
										coupe_moy[0]+=g->nb_bbu+g->nb_collisions-1 - arcid;
										return INT_MAX;
									}
									//On met i après le message qu'on vient de voir en première periode
									temps_min_i = temps_min_j+message_size;
										
								}
							}
						}
											
						

					}
					
				}
				
				if(arcid == 0)
				{	
					nb_feuilles++;
					if(MOD)
					{
						int max = 0;
						int result;
						for(int i= g->nb_bbu;i<g->nb_bbu+g->nb_collisions;i++)
						{
						//	printf("%d ",i);
							int release[g->arc_pool[i].nb_routes];
							int delai[g->arc_pool[i].nb_routes];
							for(int j=0;j<g->arc_pool[i].nb_routes;j++)
							{

								release[j] = route_length_with_buffers_forward(g,g->arc_pool[i].routes_id[j])
									+route_length_untill_arc(g,g->arc_pool[i].routes_id[j],&g->arc_pool[i],BACKWARD);
								delai[j]=route_length_with_buffers( g,g->arc_pool[i].routes_id[j]);
								//printf("(%d %d %d %d) \n",release[j],delai[j],route_length_with_buffers_forward(g,g->arc_pool[i].routes_id[j]),route_length_untill_arc(g,g->arc_pool[i].routes_id[j],&g->arc_pool[i],BACKWARD));
							}
							result = optim(release, delai, g->arc_pool[i].nb_routes,  P, message_size);
							//printf("%d %d \n",result,max);
							max = (result>max)?result:max;
							

							//printf("%d\n",max);
						
						}
						
						return (max==0)?INT_MAX:max;
					}
					else			 
						return (!assignment_with_orders_vois1FPT(g, P, message_size, borneinf))?INT_MAX:travel_time_max_buffers(g);

					
				}
				else
				{
					
					//Quand on arrive ici, on a calculé le delay pour toutes les routes, on met donc l'arc a "bounded", ce qui simplifie le calcul pour la borne inf
					g->arc_pool[arcid].bounded = 1;
					//coupe si on dépasse la borneinf du greedy
					if(BORNEINF_ON)
					{
						int cut = borneInfFPT( g, P, message_size,borneinf);
						if(( cut > borneinf)||(cut == 0))
						{
							nb_coupes[4]++;
							coupe_moy[4]+=g->nb_bbu+g->nb_collisions-1 - arcid;
							return INT_MAX;
						}
					}
					return rec_arcs(g,arcid-1,P,message_size,borneinf);	
				}
				
			}
			else
			{
				old_offset = offset;
				//Parcours ou on ne change pas les periodes
				int current_route = g->arc_pool[arcid].routes_order_f[profondeur];
				//printf("current_route %d arc %d\n",current_route,arcid);
				//r_t = route_length_untill_arc(g,current_route,&g->arc_pool[arcid],FORWARD);
			
				//printf("rt %d offset %d\n",r_t,offset);
				retval r = calcul_delay(begin,offset,P,r_t[profondeur],message_size,0);//ici le dernier argument est a 0 car on met la route dans la permiere periode
				
				g->arc_pool[arcid].routes_delay_f[current_route] =  r.delay;
				offset = r.new_offset;

				//Si la route rentre
				if(offset-message_size <= begin+P-message_size*(nb_routes-profondeur))
				{

					if(r.delay == 0)
					{
						if((PAS_PLUS_PETIT_ID)&&(current_route < g->arc_pool[arcid].routes_order_f[0]))// pas plus petit id
						{
							//printf("coupepetitid\n");
							nb_coupes[6]++;
							coupe_moy[6]+=g->nb_bbu+g->nb_collisions-1 - arcid;
							goto findroite;
							
						}
						
						if((ROUTES_SUIVANTES_AVANT_I)&&(old_offset+2*message_size <= offset))
						{
							for(int i=profondeur+1;i<nb_routes;i++)
							{
								//printf("Offset %d, begin %d, rt %d [%d < %d] %d %d",offset,begin,r_t[i],(begin+r_t[i])%P+message_size,offset-begin-message_size,r_t[profondeur],old_offset);
								if( (begin+r_t[i])%P+message_size<= (offset+begin)%P-message_size)//un message suivant rentrerais dans le trou laissé par la route en cours
								{
									//printf("Coupe coupe, le message %d rentrerais avant %d\n",g->arc_pool[arcid].routes_order_f[i],current_route);
									nb_coupes[3]++;
									coupe_moy[3]+=g->nb_bbu+g->nb_collisions-1 - arcid;
									goto findroite;
								}
							}
							
						}
						
						
					
					}
					
					val_D =  rec_orders(g,arcid,message_size,P,profondeur+1,borneinf,offset,begin,r_t);

					//Opti N1, on ne la fait que dans le cas ou la route ne peut pas etre d'office placée que en seconde periode
					// (dans ce cas la fonction calcul delay renvoie 0 de delay, ce qui est filtré par le test d'avant)
					// On remonte, donc on enleve la route qu'on viens de mettre 
					g->arc_pool[arcid].routes_delay_f[current_route]=0;
				
					if(I_COLLE)
					{
						if( old_offset+message_size == offset )//on est collé 
						{
							nb_coupes[2]++;
							coupe_moy[2]+=g->nb_bbu+g->nb_collisions-1 - arcid;
							return val_D;
						
						}
					}
				
				}
				else
				{
					nb_coupes[5]++;
					coupe_moy[5]+=g->nb_bbu+g->nb_collisions-1 - arcid;
				}
				findroite:
				
				g->arc_pool[arcid].routes_delay_f[current_route]=0;
				offset = old_offset;
				r = calcul_delay(begin,offset,P,r_t[profondeur],message_size,1);//ici le dernier argument est a 1 car seconde periode
				
				g->arc_pool[arcid].routes_delay_f[current_route] =  r.delay;
				offset = r.new_offset;
				
			
				val_G =  rec_orders(g,arcid,message_size,P,profondeur+1,borneinf,offset,begin,r_t);
				// On remonte, donc on enleve la route qu'on viens de mettre 
				g->arc_pool[arcid].routes_delay_f[current_route]=0;
		
				return (val_G>val_D)?val_D:val_G;

			}
	}
	
}

int rec_arcs(Graph *g,int arcid, int P, int message_size,int borneinf)
{
	
	element_sjt permuts[g->arc_pool[arcid].nb_routes]; 
	for(int i=0;i<g->arc_pool[arcid].nb_routes;i++)
	{
		permuts[i].val = i;
		permuts[i].sens = 0;
	}	
	
	int returnvalue;
	
	long long facto=fact(g->arc_pool[arcid].nb_routes);
	

	int tab[g->arc_pool[arcid].nb_routes];
	int r_t[g->arc_pool[arcid].nb_routes];
	for(int j=0;j<g->arc_pool[arcid].nb_routes;j++)
	{

		tab[j]=g->arc_pool[arcid].routes_id[j];

				
	}
	//Pour tout les ordres de routes :
	for(int i=0;i<facto;i++)
	{
	
		for(int j=0;j<g->arc_pool[arcid].nb_routes;j++)
		{
			if(arcid < NB_BBU)
			{
				g->arc_pool[arcid].routes_order_f[j] = tab[permuts[j].val];
				g->arc_pool[arcid].routes_order_b[j] = tab[permuts[j].val];	
			}
			else
			{
				
				g->arc_pool[arcid].routes_order_f[j] = tab[permuts[j].val];
				
			}
			r_t[j] = route_length_untill_arc(g,tab[permuts[j].val],&g->arc_pool[arcid],FORWARD);
			//printf("j %d rt %d route %d\n",j,r_t[j], tab[permuts[j].val]);
			//printf("%d",tab[permuts[j].val]);
		}
		//printf("\n");
		//print_tab(permuts,g->arc_pool[arcid].nb_routes);		
		returnvalue = rec_orders(g, arcid,  message_size,  P,0,borneinf,0,0,r_t);

		if(returnvalue < borneinf)
			borneinf = returnvalue;
		if(i!=facto-1)
			algo_sjt(permuts,g->arc_pool[arcid].nb_routes);

		memset(g->arc_pool[arcid].routes_delay_f,0,sizeof(int)*g->arc_pool[arcid].id_max);
		//printf("%d %p %p %p\n",g->arc_pool[arcid].routes_delay_f,g->arc_pool[arcid].routes_delay_b,&g->arc_pool[i]);
		memset(g->arc_pool[arcid].routes_delay_b,0,sizeof(int)*g->arc_pool[arcid].id_max);
		
		g->arc_pool[arcid].bounded = 0;
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

long long count_feuilles_arbre(Graph * g)
{
	long long nb = 1;
	for(int i=0;i<g->nb_bbu+g->nb_collisions;i++)
	{
		nb *= ( facto(g->arc_pool[i].nb_routes) * pow2n(g->arc_pool[i].nb_routes));
	}
	return nb;
}



int branchbound(Graph * g,int P, int message_size,int * coupes,double * coupes_m, int m)
{
		
	AFFICHE_RES = 0;

	BORNEINF_ON =1;
	PAS_PLUS_PETIT_ID =1;
	SECONDE_DANS_PREMIERE = coupes[0];
	I_PLUS_1_PAS_COLLE =coupes[1];
	I_COLLE =coupes[2];
	ROUTES_SUIVANTES_AVANT_I =coupes[3];
	
	MOD = m;
    struct timeval tv1, tv2;

	nb_feuilles = 0;
	
	for(int i=0;i<NB_COUPES;i++)
	{
		nb_coupes[i]=0;
		coupe_moy[i]=0.0;
		//printf("%d ",coupes[i]);
	}
	//printf("\n");

	int borneinf=greedy_deadline_assignment( g, P, message_size);
	if(!borneinf)
		return 0;
	//printf("borneinf %d \n",borneinf);
	//printf("Taille de l'arbre FPT = %d \n",count_feuilles_arbre(g));
	reinit_delays(g);

	 gettimeofday (&tv1, NULL);
	int ret = rec_arcs(g,g->nb_bbu+g->nb_collisions-1,P,message_size,borneinf);
	reset_periods(g,P);
	gettimeofday (&tv2, NULL);	
	if(AFFICHE_RES)
	{
		printf("%f%% (%lld/%lld) Des feuilles explorées \n \n",(double)nb_feuilles*100/(double)count_feuilles_arbre(g),nb_feuilles,count_feuilles_arbre(g));
		for(int i=0;i<NB_COUPES;i++)
		{
			printf("Coupe %s : \n 	-Nombre : %lld\n 	Hauteur moyenne de coupe %f \n",noms_coupes[i],nb_coupes[i],coupe_moy[i]/nb_coupes[i]);
		
		}

		printf("Temps de calcul : %f ms.\n",time_diff(tv1,tv2));
	}
	//printf("%lld %f",nb_coupes[0],coupe_moy[0]/nb_coupes[0]);
	for(int i=0;i<NB_COUPES;i++)
	{
	
		coupes[i] = nb_coupes[i];
		if(coupes[i])
			coupes_m[i] = coupe_moy[i]/nb_coupes[i];
		else
			coupes_m[i] = 0.0;
	
	
	}
	

	return ret;
}



