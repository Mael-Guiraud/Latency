#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "starSPALL.h"
#include "treatment.h"
#include "voisinage.h"
#include "simons.h"
#include <limits.h>
/*
typedef struct 
{
	int val;
	int sens; //0 gauche 1 droite
} element_sjt;
void print_tab(element_sjt * tab, int taille);
int id_mobile(element_sjt * tab,int taille);
void swap_greater(element_sjt* tab, int taille,int nb);
void algo_sjt(element_sjt* tab, int taille);
element_sjt * init_sjt(int taille);
long long fact(int a);*/


element_sjt * init_sjt_tab(int *tabl, int taille)
{
	element_sjt * tab = malloc(sizeof(element_sjt)*taille);
	for(int i=0;i<taille;i++)
	{
		tab[i].val = tabl[i];
		tab[i].sens = 0;
	}	
	return tab;
}

int  load_links_CL(Graph g,int CL)
{
	int nb = 0;
	for(int i=0;i<g.arc_pool_size;i++)
	{
		if(g.arc_pool[i].contention_level == CL)
			nb++;
	}
	int load[nb];
	int  id[nb];
	int j=0;
	
	for(int i=0;i<g.arc_pool_size;i++)
	{
		if(g.arc_pool[i].contention_level == CL)
		{
			
			load[j] = g.arc_pool[i].nb_routes;
			id[j] = i;
			j++;
		}
		
	}
	tri_bulles(load,id,nb);
	return id[0];
}

int coreBorneInf(Graph g, int P, int message_size,int budget,int arc_id)
{
	


	// On a l'arc le plus chargé
	//int arc_id = load_links_CL(g,0);


	int taille_tab=g.arc_pool[arc_id].nb_routes;
	/*printf("\n\n\n\nArc le plus chargé : %d routes \n routes sur l'arc \n",taille_tab);
	for(int i=0;i<taille_tab;i++)
		printf("%d ",g.arc_pool[arc_id].routes_id[i]);
	printf("\n");*/
	//On init le tableau des permutations

	int release[taille_tab];
	int deadline[taille_tab];
	int ids[taille_tab];

	//printf("BUDGETABCD = %d \n",budget);
	for(int i=0;i<taille_tab;i++)
	{
		release[i] = route_length_untill_arc_without_delay(g,g.arc_pool[arc_id].routes_id[i],&g.arc_pool[arc_id],FORWARD);
		//printf("(%d ",release[i]);
		deadline[i] = release[i]+message_size+budget - 2* route_length(g,g.arc_pool[arc_id].routes_id[i]);
		
		//printf("%d (%d + %d - %d)",deadline[i],release[i],budget,route_length(g,g.arc_pool[arc_id].routes_id[i]));	
		ids[i]=g.arc_pool[arc_id].routes_id[i];
		//printf("%d)\n",ids[i]);
	}

	int *res = FPT_PALL(g,ids,release,deadline,taille_tab,message_size,P);
	int max =0;
	int taille_route;
	if(res)
	{	

		for(int i=0;i<taille_tab;i++)
		{
			taille_route =  res[i]+ 2* route_length(g,g.arc_pool[arc_id].routes_id[i]);
			//printf("route %d buff %d lenght %d (%d)\n",g.arc_pool[arc_id].routes_id[i],res[i],2*route_length(g,g.arc_pool[arc_id].routes_id[i])+res[i],2*route_length(g,g.arc_pool[arc_id].routes_id[i]));
			if(taille_route > max)
				max = taille_route; 
		}
	}
	else
	{
		//printf("no res  %d \n", budget);
		return 0;
	}

    free(res);

    //printf("%d \n",max);
	return max;

}

int borneInfDicho(Graph g, int P, int message_size,int arcid)
{
	int min = 2*longest_route(g);
	int max = min +P;
	int milieu;
	int res=0;
	int tmp;
	while(min != max)
	{
		milieu =min+ (max - min ) / 2;
		tmp = coreBorneInf(g,P,message_size,milieu,arcid);
		//printf("-------------------------------------------abc %d %d \n",tmp,milieu);
		if(tmp)
		{
			max = milieu;
			res = tmp;
		}
		else
		{
			
			if(min == max -1)
			{
				if(!res)
				{
					printf("Etrange, on aurait du trouver un resultat avec la dichotomie\n");
					exit(44);
				}
				return res;
			}
			min = milieu;
		}
		
	}	
	if(!res)
	{
		printf(" TRES Etrange, on aurait du trouver un resultat avec la dichotomie\n");
		exit(45);
	}
	return res;
}
int borneInf(Graph g, int P, int message_size)
{
	int * t = load_links(g);
	int max = 0;
	int tmp;
	
	for(int i=0;i<g.arc_pool_size;i++)
	{
		tmp = borneInfDicho(g,P,message_size,t[i]);
	
		if(tmp>max)
		{
			max = tmp;
			
		}
	}
	free (t);
	return max;
}
int borneInf2_core(Graph g, int P, int message_size,int arc_id)
{
	
	
	int taille_tab=g.arc_pool[arc_id].nb_routes;
	int taille_route[taille_tab];
	int value_to_add=INT_MAX;
	int tmp;

	for(int i=0;i<taille_tab;i++)
	{
		tmp = route_length_untill_arc_without_delay(g,g.arc_pool[arc_id].routes_id[i],&g.arc_pool[arc_id],FORWARD);
		if(tmp<value_to_add)
		{
			value_to_add = tmp;
		}
		taille_route[i] =  2* route_length(g,g.arc_pool[arc_id].routes_id[i])-tmp;
	}
	tri_bulles_classique_decroissant(taille_route,taille_tab);
	int max = 0;
	for(int i=0;i<taille_tab;i++)
	{
		if(max < taille_route[i]+i*message_size)
		{
			max = taille_route[i]+i*message_size;
		}
	}

	return (max+value_to_add>2*longest_route(g))?max+value_to_add:2*longest_route(g);
}
int borneInf2(Graph g, int P, int message_size)
{
	int * t = load_links(g);
	int max = 0;
	int tmp;
	//printf("BORNE2 2\n");
	for(int i=0;i<g.arc_pool_size;i++)
	{
		tmp = borneInf2_core(g,P,message_size,t[i]);
		//printf("arc %d red %d \n routes on arc :",i,tmp);
		/*for(int j=0;j<g.arc_pool[t[i]].nb_routes;j++)
		{
			printf("%d - ",g.arc_pool[t[i]].routes_id[j]);
		}
		printf("\n");*/
		if(tmp>max)
		{
			max = tmp;
			//printf("--------------NEW MAX ");
		}
		//printf("\n");

			
	}
	free (t);
	return max;
}