#include <stdio.h>
#include <stdlib.h>
#include "simons.h"


/*

int* FPT_PALL(int *RELEASE, int *DEADLINE, int nbr_route, int taille_paquet, int periode)

Assignment FPT_SPAZL(Graph g, int P, int message_size)
{
	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = malloc(sizeof(int)*g.nb_routes);
	a->offset_backward = malloc(sizeof(int)*g.nb_routes);
	a->waiting_time = malloc(sizeof(int)*g.nb_routes);
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;


}*/
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
		printf("%d",tab[i].val+1);
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

int main()
{
	element_sjt * tab = init_sjt(4);
	for(int i=0;i<25;i++)
	{
		print_tab(tab,4);
		algo_sjt(tab,4);
	}
	free(tab);
}