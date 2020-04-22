#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "starSPALL.h"
#include <limits.h>
#include "config.h"
#include "voisinage.h"
#include "treatment.h"
#include "greedy_waiting.h"
#include "borneInf.h"

int nb_appels_arc;
int nb_appels_orders;




int rec_arcs(Graph g,int arcid,Period_kind kind, int P, int message_size,int borneinf);
//Fait l'arbre recursif avec tous les sous ensemble de routes 
int rec_orders(Graph g, int arcid,Period_kind kind, int message_size, int P,int profondeur,int borneinf,int offset,int begin)
{
	nb_appels_orders++;
	int val_D;
	int val_G;
	int nb_routes = g.arc_pool[arcid].nb_routes;
	int retour;
	int premier;int r_t;
	//printf("Arc %d kind %d profondeur %d(%droutes)\n",arcid,kind,profondeur,nb_routes);
	int a;
	int old_offset;
	if(profondeur == 0)
	{
			
		if(kind == FORWARD)
			premier = g.arc_pool[arcid].routes_order_f[profondeur];
		else
			premier = g.arc_pool[arcid].routes_order_b[profondeur];
		//printf("premier %d \n",premier);

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
		rec_orders(g,arcid,kind,message_size,P,profondeur+1,borneinf,offset,begin);
	}
	if(profondeur == nb_routes)
	{
		/*printf("arcid %d nbroutes %d kind %d order :",arcid,g.arc_pool[arcid].nb_routes,kind);
		if(kind == FORWARD)
			for(int i=0;i<g.arc_pool[arcid].nb_routes;i++)printf("%d ",g.arc_pool[arcid].routes_order_b[i]);
		else
			for(int i=0;i<g.arc_pool[arcid].nb_routes;i++)printf("%d ",g.arc_pool[arcid].routes_order_b[i]);
		printf("\n");*/
		//if((arcid == g.nb_bbu + g.nb_collisions-1)&&(kind == BACKWARD))
		if(arcid == 0)
		{	
			//printf("Calcul assingment \n");

			a = assignment_with_orders_vois1FPT(g, P, message_size, borneinf);
			retour = travel_time_max_buffers(g);
			reset_periods(g,P);
			if(a)
			{

				//free_assignment(a);
				//printf("REtour %d \n",retour);
				//exit(12);
				return retour;
			}
			else
			{
				//printf("REtour INTMAX \n");
				//exit(13);
				//free_assignment(a);
				return INT_MAX;

			}
			
			
		}
		else
		{
			
			//COUPE SI JAMAIS ON NE TROUVE RIEN SUR CET ARC, CA NE SERT A RIEN DE CONTINUER L'ARBRE RECURSIF EN DESSOUS
			g.arc_pool[arcid].bounded = 1;
			/*if(!assignOneArc( g, arcid,  kind, message_size,  P, 0))
			{
				for(int j=0;j<128;j++)
				{
					g.arc_pool[arcid].routes_delay_f[j]=0;
					g.arc_pool[arcid].routes_delay_b[j]=0;
				}
				//printf("Assignonearc ne passe pas \n");
				g.arc_pool[arcid].bounded = 0;
				return INT_MAX;
			}*/
//coupe si on dépasse la borneinf du greedy
			int cut = borneInfFPT( g, P, message_size,borneinf);
			//printf("arc %d kind %d Coupe %d borneinf %d\n",arcid,kind, cut, borneinf);
			if( cut > borneinf)
			{
				//printf("Coupe > borneinf deja trouvée %d %d \n",cut,borneinf);
				return INT_MAX;
			}
			
			
			/*if(arcid == 0)
			{
				//printf("Repart backward \n");

				return  rec_arcs(g,g.nb_bbu,BACKWARD,P,message_size,borneinf);
			}
			else
			{*/
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
		//printf("current_route %d \n",current_route);
		r_t = route_length_untill_arc(g,current_route,&g.arc_pool[arcid],FORWARD);
	
		//printf("rt %d offset %d\n",r_t,offset);
		retval r = calcul_delay(begin,offset,P,r_t,message_size,0);//ici le dernier argument est a 0 car on met la route dans la permiere periode
		
		g.arc_pool[arcid].routes_delay_f[current_route] =  r.delay;
		offset = r.new_offset;
		// printf("offset %d , rdelay %d begin %d p %d\n",offset,r.delay,begin,P);
		if(offset > begin+P)
		{
			
			g.arc_pool[arcid].routes_delay_f[current_route]=0;
			return INT_MAX;
		}
		val_D =  rec_orders(g,arcid,kind,message_size,P,profondeur+1,borneinf,offset,begin);

		// On remonte, donc on enleve la route qu'on viens de mettre 
		g.arc_pool[arcid].routes_delay_f[current_route]=0;
		if(r.delay == 0)//Si on a de delay, on est collé, useless de mettre dans la seconde peride
		{
			return val_D;
		}

		//Calcul du i+1 pour l'opti numero 2
		if(profondeur < nb_routes-1)
		{
			
			int current_route2  = g.arc_pool[arcid].routes_order_f[profondeur+1];
			int r_t2 = route_length_untill_arc(g,current_route2,&g.arc_pool[arcid],FORWARD);
		
			//printf("rt %d offset %d\n",r_t,offset);
			retval r2 = calcul_delay(begin,offset,P,r_t2,message_size,0);//ici le dernier argument est a 0 car on met la route dans la permiere periode
			
			//Deja si ca dépasse, on quitte 
			if(offset > begin+P)
			{
				
				g.arc_pool[arcid].routes_delay_f[current_route]=0;
				return INT_MAX;
			}
			//Si i+1 pas collé, on ne fait pas valG
			if(r2.delay > 0)
			{
				return val_D;
			}
		}
		///// FIN OPTI N2 /////


		offset = old_offset;

	
		//printf("rt %d offset %d\n",r_t,offset);
		r = calcul_delay(begin,offset,P,r_t,message_size,1);//ici le dernier argument est a 1 car seconde periode
		
		g.arc_pool[arcid].routes_delay_f[current_route] =  r.delay;
		offset = r.new_offset;
		// printf("offset %d , rdelay %d begin %d p %d\n",offset,r.delay,begin,P);
		if(offset > begin+P)
		{
			
			g.arc_pool[arcid].routes_delay_f[current_route]=0;
			return INT_MAX;
		}
		/*if(val_D < borneinf)
			borneinf = val_D;*/
		//Parcours ou on change la periode
	/*	if(arcid < NB_BBU)
		{
			if(g.arc_pool[arcid].routes_order_f[profondeur] == 0)
			{
				g.arc_pool[arcid].routes_order_f[profondeur] = INT_MAX;
				g.arc_pool[arcid].routes_order_b[profondeur] = INT_MAX;
			}
			else
			{
				g.arc_pool[arcid].routes_order_f[profondeur] = -g.arc_pool[arcid].routes_order_f[profondeur];
				g.arc_pool[arcid].routes_order_b[profondeur] = -g.arc_pool[arcid].routes_order_b[profondeur];
			}
			//printf("Modif %d \n",g.arc_pool[arcid].routes_order_b[profondeur]);
		}
		else
		{
			if(kind == FORWARD)
			{
				if(g.arc_pool[arcid].routes_order_f[profondeur] == 0)
				{
					g.arc_pool[arcid].routes_order_f[profondeur] = INT_MAX;
				}
				else
				{
					g.arc_pool[arcid].routes_order_f[profondeur] = -g.arc_pool[arcid].routes_order_f[profondeur];
				}
				//printf("Modif %d \n",g.arc_pool[arcid].routes_order_f[profondeur]);
			}
			else
			{
				if(g.arc_pool[arcid].routes_order_b[profondeur] == 0)
				{
					g.arc_pool[arcid].routes_order_b[profondeur] = INT_MAX;
				}
				else
				{
					g.arc_pool[arcid].routes_order_b[profondeur] = -g.arc_pool[arcid].routes_order_b[profondeur];
				}
			}
			//printf("Modif %d \n",g.arc_pool[arcid].routes_order_b[profondeur]);
		}
		*/

		val_G =  rec_orders(g,arcid,kind,message_size,P,profondeur+1,borneinf,offset,begin);
		// On remonte, donc on enleve la route qu'on viens de mettre 
		g.arc_pool[arcid].routes_delay_f[current_route]=0;
/*		
		//Quand on remonte, on remet l'ordre d'avant
		if(arcid < NB_BBU)
		{
			if(g.arc_pool[arcid].routes_order_f[profondeur] == INT_MAX)
			{
				g.arc_pool[arcid].routes_order_f[profondeur] = 0;
				g.arc_pool[arcid].routes_order_b[profondeur] = 0;
			}
			else
			{
				g.arc_pool[arcid].routes_order_f[profondeur] = -g.arc_pool[arcid].routes_order_f[profondeur];
				g.arc_pool[arcid].routes_order_b[profondeur] = -g.arc_pool[arcid].routes_order_b[profondeur];
			}
		}
		else
		{
			if(kind == FORWARD)
			{
				if(g.arc_pool[arcid].routes_order_f[profondeur] == INT_MAX)
				{
					g.arc_pool[arcid].routes_order_f[profondeur] = 0;
				}
				else
				{
					g.arc_pool[arcid].routes_order_f[profondeur] = -g.arc_pool[arcid].routes_order_f[profondeur];
				}
			}
			else
			{
				if(g.arc_pool[arcid].routes_order_b[profondeur] == INT_MAX)
				{
					g.arc_pool[arcid].routes_order_b[profondeur] = 0;
				}
				else
				{
					g.arc_pool[arcid].routes_order_b[profondeur] = -g.arc_pool[arcid].routes_order_b[profondeur];
				}
			}
		}
		*/
		return (val_G>val_D)?val_D:val_G;

	}
}

int rec_arcs(Graph g,int arcid,Period_kind kind, int P, int message_size,int borneinf)
{
	nb_appels_arc++;
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
		//print_tab(permuts,g.arc_pool[arcid].nb_routes);
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

int branchbound(Graph g,int P, int message_size)
{
	nb_appels_arc =0;
	nb_appels_orders = 0;
	int borneinf=greedy_deadline_assignment( g, P, message_size);
	//printf("borneinf %d \n",borneinf);
	reinit_delays(g);
	int ret = rec_arcs(g,g.nb_bbu+g.nb_collisions-1,FORWARD,P,message_size,borneinf);
	
	//printf("%d %d \n",nb_appels_arc,nb_appels_orders);
	return ret;
}
