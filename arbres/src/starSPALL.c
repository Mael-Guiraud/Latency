#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "simons.h"
#include "test.h"
#include "treatment.h"


typedef struct 
{
	int val;
	int sens; //0 gauche 1 droite
} element_sjt;
void print_tab(element_sjt * tab, int taille)
{
	for(int i=0;i<taille;i++)
	{
		if(tab[i].sens == 0)printf(".");
		printf("%d",tab[i].val);
		if(tab[i].sens == 1)printf(".");
	}
	printf("\n");
}
int id_mobile(element_sjt * tab,int taille)
{
	int id = -1;
	if((tab[0].sens == 1) && (tab[0].val > tab[1].val))
			id = 0;
	for(int i =1 ;i<taille-1;i++)
	{
		if((tab[i].sens == 0) && (tab[i].val > tab[i-1].val) )
		{
			if((tab[i].val > tab[id].val) || (id == -1))
				id = i;
		}
		if((tab[i].sens == 1) && (tab[i].val > tab[i+1].val) )
		{
			if((tab[i].val > tab[id].val) || (id == -1))
				id = i;
		}
	}
	if((tab[taille-1].sens == 0) && (tab[taille-1].val > tab[taille-2].val))
		if((tab[taille-1].val > tab[id].val) || (id == -1))
			id = taille-1;

	return id;
}
void swap_greater(element_sjt* tab, int taille,int nb)
{

	for(int i=0;i<taille;i++)
	{
		if(tab[i].val > nb)
		{
			tab[i].sens = 1 -tab[i].sens;
		}
	}
}
//Effectue une iteration de l'algo SJT
void algo_sjt(element_sjt* tab, int taille)
{


	element_sjt tmp;
	int mobile;
	int id = id_mobile(tab,taille);
	mobile = tab[id].val;

	if(id == -1)
	{
		printf("Fin d'execution sur ce tableau.\n");
		return;
	}
	if(!tab[id].sens)
	{
		tmp.val = tab[id].val;
		tmp.sens = tab[id].sens;
		tab[id].val = tab[id-1].val;
		tab[id].sens = tab[id-1].sens;
		tab[id-1].val=tmp.val;
		tab[id-1].sens=tmp.sens;

	}
	else
	{
		tmp.val = tab[id].val;
		tmp.sens = tab[id].sens;
		tab[id].val = tab[id+1].val;
		tab[id].sens = tab[id+1].sens;
		tab[id+1].val=tmp.val;
		tab[id+1].sens=tmp.sens;
	}

	
	swap_greater(tab,taille,mobile);

}
element_sjt * init_sjt(int taille)
{
	element_sjt * tab = malloc(sizeof(element_sjt)*taille);
	for(int i=0;i<taille;i++)
	{
		tab[i].val = i;
		tab[i].sens = 0;
	}	
	return tab;
}
long long fact(int a)
{
	if(a==2)
	{
		return 2;
	}
	else
		return a * fact(a-1);
}




void compute_tabs(element_sjt * tab,int * m_i,int * release, int * deadline, Graph g, int P, int message_size,int tmax)
{
	int offset;
	int longest = route_length(g,0);
	int tmp;
	for(int i=1;i<g.nb_routes;i++)
	{
		tmp = route_length(g,i);
		if(tmp > longest)
		{
			longest = tmp;
		}
	}
	offset = longest;
	for(int i=0;i<g.nb_routes;i++)
	{
		m_i[tab[i].val] = (offset-route_length_untill_arc(g,tab[i].val,&g.arc_pool[g.nb_routes],FORWARD)) % P;
		release[tab[i].val] = m_i[tab[i].val]+route_length(g,tab[i].val)+route_length_untill_arc(g,tab[i].val,&g.arc_pool[g.nb_routes],BACKWARD);
		deadline[tab[i].val] = message_size+tmax+m_i[tab[i].val]-(route_length(g,tab[i].val)-route_length_untill_arc(g,tab[i].val,&g.arc_pool[g.nb_routes],BACKWARD));
		offset+=message_size;
	}
}
Assignment fpt_spall(Graph g, int P, int message_size, int tmax)
{
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;

	int m_i[g.nb_routes];
	int release[g.nb_routes];
	int deadline[g.nb_routes];


	element_sjt * tab = init_sjt(g.nb_routes);
	long long facto=fact(g.nb_routes);
	int * res;
	int travel_time;

	//Pour tout les ordres de routes :
	for(int i=0;i<facto;i++)
	{
		compute_tabs(tab,m_i,release,deadline,g,P,message_size,tmax);
		
		res = FPT_PALL(release,deadline,g.nb_routes,message_size,P);
		if(res)
		{	
		
			for(int j=0;j<g.nb_routes;j++)
			{
				a->offset_forward[j] = m_i[j];
				a->waiting_time[j] = res[j];
				a->offset_backward[j] = m_i[j] + res[j] +  route_length(g,j);
			}
			a->all_routes_scheduled=1;
			a->nb_routes_scheduled = g.nb_routes;
			travel_time = travel_time_max(g, tmax,a);
			if(travel_time != -1)
			{
				free(tab);
				free(res);
				return a;
			}
			else
			{
				for(int j=0;j<g.nb_routes;j++)
				{
					a->offset_forward[j] = -1;
					a->waiting_time[j] = -1;
					a->offset_backward[j] = -1;
				}
				a->all_routes_scheduled=0;
				a->nb_routes_scheduled = 0;
			}
		}
		if(i!=facto-1)
			algo_sjt(tab,g.nb_routes);
		
	}
	free(tab);
	return a;
}