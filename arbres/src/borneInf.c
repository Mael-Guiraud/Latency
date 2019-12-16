#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "starSPALL.h"
#include "treatment.h"
#include "voisinage.h"
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

int borneInf(Graph g, int P, int message_size)
{

	int offset ;
	int current_route;
	int dl;
	int ok; 
	int buff;
	int max;
	int min = INT_MAX;
	offset = 0;
	// On a l'arc le plus chargé
	//int arc_id = load_links_CL(g,0);
	int * t = load_links(g);
	int arc_id = t[0];
	free (t);

	int taille_tab=g.arc_pool[arc_id].nb_routes;

	//On init le tableau des permutations

	//On trie le tableau pour etre sur
	tri_bulles_classique_croissant(g.arc_pool[arc_id].routes_id, taille_tab);
	element_sjt * perms = init_sjt_tab(g.arc_pool[arc_id].routes_id, taille_tab);
	long long facto=fact(taille_tab); // nombre de permutations

	//Pour tout les ordres de routes :
	for(int i=0;i<facto;i++)
	{
		max =0 ;
		offset = 0;
		int Per[taille_tab];
		for(int k=0;k<taille_tab;k++)
		{
			buff = 0;
			current_route = perms[k].val;
		
			dl = route_length_untill_arc(g,current_route,&g.arc_pool[arc_id],FORWARD);
			
			if(dl < offset )
			{
				buff =  offset - dl;
			}
			else
			{
				offset = dl;
			}
			ok = 0;
			for(int add =0;add<P;add++)
			{

				if(cols_check(Per,offset+add,message_size,P,k))
				{
					
					buff += add;
					offset += add;
					ok = 1;
					break;
				}

			}
			
			if(ok == 0)
			
			{
				free(perms);
				continue;
			}
			Per[k]=offset;
			offset+=message_size;
			if(route_length(g,current_route)*2 + buff > max)
				max = route_length(g,current_route)*2 + buff ;
			
		}
		if(max < min)
			min = max;
		
		if(i!=facto-1)
			algo_sjt(perms,taille_tab);//Nouvelle permutation
		
	}
    free(perms);

    
	return min;

}
