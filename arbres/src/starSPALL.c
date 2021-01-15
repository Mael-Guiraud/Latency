#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "simons.h"
#include "test.h"
#include <limits.h>
#include "treatment.h"
#include "starSPALL.h"

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
			if(id == -1)
				id = i;
			else
				if(tab[i].val > tab[id].val) 
					id = i;
		}
		if((tab[i].sens == 1) && (tab[i].val > tab[i+1].val) )
		{
			if(id == -1)
				id = i;
			else
				if(tab[i].val > tab[id].val)
					id = i;
		}
	}
	if( (tab[taille-1].sens == 0) && (tab[taille-1].val > tab[taille-2].val) )
	{
		if(id == -1)
			id = taille-1;
		else
			if(tab[taille-1].val > tab[id].val) 
				id = taille-1;
	}
	
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


	if(a<=2)
	{
		return a;
	}
	else
		return a * fact(a-1);
}




void compute_tabs(element_sjt * tab,int * m_i,int * release, int * deadline, Graph * g, int P, int message_size,int tmax)
{
	int offset;
	int longest = route_length(g,0);
	int tmp;
	for(int i=1;i<g->nb_routes;i++)
	{
		tmp = route_length(g,i);
		if(tmp > longest)
		{
			longest = tmp;
		}
	}
	offset = longest;
	for(int i=0;i<g->nb_routes;i++)
	{
		m_i[tab[i].val] = (offset-route_length_untill_arc(g,tab[i].val,&g->arc_pool[g->nb_routes],FORWARD)) % P;
		release[tab[i].val] = m_i[tab[i].val]+route_length(g,tab[i].val)+route_length_untill_arc(g,tab[i].val,&g->arc_pool[g->nb_routes],BACKWARD);
		deadline[tab[i].val] = message_size+tmax+m_i[tab[i].val]-(route_length(g,tab[i].val)-route_length_untill_arc(g,tab[i].val,&g->arc_pool[g->nb_routes],BACKWARD));
		offset+=message_size;
	}
}
Assignment fpt_spall(Graph * g, int P, int message_size, int tmax)
{
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g->nb_routes);
	a->offset_backward = malloc(sizeof(int)*g->nb_routes);
	a->waiting_time = malloc(sizeof(int)*g->nb_routes);
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;

	int m_i[g->nb_routes];
	int release[g->nb_routes];
	int deadline[g->nb_routes];


	element_sjt * tab = init_sjt(g->nb_routes);
	long long facto=fact(g->nb_routes);
	int * res;
	int travel_time;

	//Pour tout les ordres de routes :
	for(int i=0;i<facto;i++)
	{
		compute_tabs(tab,m_i,release,deadline,g,P,message_size,tmax);
		

		//DEUX PREMIERS PARAMETRES RAJOUTES , CODE A REFAIRE. CE SIMONS LA CHERCHE LE MEILLEUR DE TT LES RESULTATS
		//ET NON JUSTE LE PREMIR RESULTAT OK
		res = FPT_PALL(g,NULL,release,deadline,g->nb_routes,message_size,P);
		if(res)
		{	
		
			for(int j=0;j<g->nb_routes;j++)
			{
				a->offset_forward[j] = m_i[j];
				a->waiting_time[j] = res[j];
				a->offset_backward[j] = m_i[j] + res[j] +  route_length(g,j);
			}
			a->all_routes_scheduled=1;
			a->nb_routes_scheduled = g->nb_routes;
			travel_time = travel_time_max(g, tmax,a);
			if(travel_time != -1)
			{
				free(tab);
				free(res);
				return a;
			}
			else
			{
				for(int j=0;j<g->nb_routes;j++)
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
			algo_sjt(tab,g->nb_routes);
		
	}
	free(tab);
	return a;
}
//Permute les elements d'un tableau
void fisher_yates(int * tab, int taille)
{
	int r;
	int tmp;
	for(int i=taille-1;i>0;i--)
	{
		r=rand()%(i+1);
		tmp = tab[i];
		tab[i]= tab[r];
		tab[r]=tmp;
	}
}


int* retourne_offset(Graph*  g, int taille_paquet, int* permutation)
{
	int nbr_route = g->nb_routes;
	int * offsets;
	offsets=(int*)malloc(sizeof(int)*nbr_route);
	int offset = 0;

	for(int i=0;i<nbr_route;i++)
	{
			offsets[permutation[i]]=offset;
			offset+=taille_paquet;

	}

	
	return offsets;
}
int *retourne_departs(Graph*  g, int * offsets)
{
	int nbr_route = g->nb_routes;
	int* m_i = (int*)malloc(sizeof(int)*nbr_route);
	int min = INT_MAX;
	for(int i=0;i<nbr_route;i++)
	{
		m_i[i] = offsets[i] - route_length_untill_arc(g,i,&g->arc_pool[g->nb_routes],FORWARD);
		if(m_i[i]<min)
			min = m_i[i];
	}
	for(int i=0;i<nbr_route;i++)
	{
		m_i[i] += (-min);
	}
	return m_i;
}
int FPT_PALL_star(Graph * g, int P, int message_size, int tmax)
{
	int *m_i;
	int release[g->nb_routes];
	int deadline[g->nb_routes];
	int ids[g->nb_routes];
	int permutation[g->nb_routes];
	int * offsets;
	int min = INT_MAX;
	int temps_restant;   


	reinit_delays(g);
	for(int compteur_rand = 0;compteur_rand<1000;compteur_rand++)
	{
		//printf("\ntirage aleatoire n %d \n\n",compteur_rand);

		for(int k=0;k<g->nb_routes;k++)
		{
			permutation[k]=k;
		}
		fisher_yates(permutation, g->nb_routes);
		/*int *random = random_inters(P-g->nb_routes*message_size, g->nb_routes);

		offsets=(int*)malloc(sizeof(int)*g->nb_routes);
		int offset = 0;

		for(int i=0;i<g->nb_routes;i++)
		{
				offsets[permutation[i]]=offset;
				offset+=message_size;

		}
		free(random);*/

		offsets= retourne_offset(g, message_size, permutation);
		
		m_i = retourne_departs( g, offsets);



		for(int i=0;i<g->nb_routes;i++)
		{
			ids[i]=i;


			release[i] = m_i[i]+route_length_with_buffers_forward(g,i)
			+route_length_untill_arc(g,i,&g->arc_pool[g->nb_routes],BACKWARD);
			temps_restant = route_length(g,i) - route_length_untill_arc_without_delay(g,i,&g->arc_pool[g->nb_routes],BACKWARD);
			deadline[i] = m_i[i]+tmax  +message_size - temps_restant;
			//printf("rel %d  deadl %d mi %d tmax %d routelenth %d routeback %d\n",release[i],deadline[i],m_i[i],tmax, route_length(g,i),route_length_untill_arc_without_delay(g,i,&g->arc_pool[g->nb_routes],BACKWARD));
		}
		
		int *res = FPT_PALL(g,ids,release,deadline,g->nb_routes,message_size,P);
		if(res)
		{	
			for(int i=0;i<g->nb_routes;i++)
			{
				//printf("res[%d] = %d \n",i,res[i]);	
				g->routes[i][0]->routes_delay_f[i] = m_i[i];
				g->arc_pool[g->nb_routes].routes_delay_b[i] = res[i];
			}
			int val = travel_time_max_buffers(g);
			//printf("Val = %d \n",val);
			if(val<min)
				return val;
			
		}
		
	}
	
	return 0;
}

int dichostarspall(Graph * g, int P, int message_size)
{
	int min = 2*longest_route(g);
	int max = min +P;
	int milieu;
	int res=0;
	int tmp;
	while(min != max)
	{
		milieu =min+ (max - min ) / 2;
		tmp = FPT_PALL_star(g,P,message_size,milieu);
		//printf("-------------------------------------------abc %d %d \n",tmp,milieu);
		if(tmp)
		{
			//printf("arc %d On a trouvé, tmp = %d, res = %d\n",arcid,tmp, res);
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