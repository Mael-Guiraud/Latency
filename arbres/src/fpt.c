#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "starSPALL.h"
#include <limits.h>
#include "config.h"
#include "voisinage.h"
#include "treatment.h"


int rec_arcs(Graph g,int arcid,Period_kind kind, int P, int message_size);
//Fait l'arbre recursif avec tous les sous ensemble de routes 
int rec_orders(Graph g, int arcid,Period_kind kind, int message_size, int P,int profondeur)
{

	int val_D;
	int val_G;
	int nb_routes = g.arc_pool[arcid].nb_routes;
	int retour;
	//printf("Arc %d kind %d profondeur %d(%droutes)\n",arcid,kind,profondeur,nb_routes);
	int a;
	if(profondeur == nb_routes)
	{
		/*printf("arcid %d nbroutes %d kind %d order :",arcid,g.arc_pool[arcid].nb_routes,kind);
		if(kind == FORWARD)
			for(int i=0;i<g.arc_pool[arcid].nb_routes;i++)printf("%d ",g.arc_pool[arcid].routes_order_b[i]);
		else
			for(int i=0;i<g.arc_pool[arcid].nb_routes;i++)printf("%d ",g.arc_pool[arcid].routes_order_b[i]);
		printf("\n");*/
		if((arcid == g.nb_bbu + g.nb_collisions-1)&&(kind == BACKWARD))
		{	
			//printf("Calcul assingment \n");

			a = assignment_with_orders_vois1(g, P, message_size, 0);
			retour = travel_time_max_buffers(g);
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
			if(!assignOneArc( g, arcid,  kind, message_size,  P, 0))
				return INT_MAX;

			if(arcid == 0)
			{
				//printf("Repart backward \n");

				return  rec_arcs(g,g.nb_bbu,BACKWARD,P,message_size);
			}
			else
			{
				if(kind == FORWARD)
				{
					//printf("Enfonce forward \n");
					return rec_arcs(g,arcid-1,kind,P,message_size);
				}
				else
				{
					//printf("remonte backward\n");
					return rec_arcs(g,arcid+1,kind,P,message_size);
				}
			}
		}
		
	}
	
	else
	{
		//Parcours ou on ne change pas les periodes
		val_D =  rec_orders(g,arcid,kind,message_size,P,profondeur+1);
			
		//Parcours ou on change la periode
		if(arcid < NB_BBU)
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
		
		val_G =  rec_orders(g,arcid,kind,message_size,P,profondeur+1);

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
		return (val_G>val_D)?val_D:val_G;

	}
}

int rec_arcs(Graph g,int arcid,Period_kind kind, int P, int message_size)
{
	//printf("RECARCS arc %d kind %d \n",arcid,kind);
	element_sjt permuts[g.arc_pool[arcid].nb_routes]; 
	for(int i=0;i<g.arc_pool[arcid].nb_routes;i++)
	{
		permuts[i].val = i;
		permuts[i].sens = 0;
	}	
	
	int min = INT_MAX;
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
		returnvalue = rec_orders(g, arcid, kind,  message_size,  P,0);
		if(returnvalue < min)
			min = returnvalue;
		if(i!=facto-1)
			algo_sjt(permuts,g.arc_pool[arcid].nb_routes);
	}
	printf("Sortie de l'appel recursif arc %d kind %d res %d\n",arcid,kind,min);
	return min;
}

