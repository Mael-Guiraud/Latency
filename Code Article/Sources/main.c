#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "simulations.h"

#include "struct.h"
#include "init.h"
#include "random.h"

#include "operations.h"
int main()
{
	srand(time(NULL));

	/*****
	Effectue une recherche linéaire pour les algos PAZL afin de trouver la plus petite periode moyenne
	*****/
	//simuls_periode_PAZL(8,2500,700,1000);

	/******************************************/	

	/*****
	Taux de reussite de chaque algo PAZL en fonction de la periode
	*****/
	//echec_PAZL(8,2500,20000,1000);


	/******************************************/


	/*****
	Fichiers en 3D pour PALL
	*****/
	/*
	for(int i=3;i<5;i++)
	succes_PALL_3D(8,2500,5000, 1000,i);
	*/

	/******************************************/
	
	/*****
	Taux de reussite de départ PALL avec un retour GP sur une periode donnée (on fait varier la marge)
	*****/
	//sucess_aller_PALL(8,2500,20000,3000,10000,25000);





	/******************************************/


	/*****
	Taux de reussite des algos PALL avec un aller random sur une periode donnée (on fait varier la marge)
	*****/

	//sucess_retour_PALL(8,2500,20000,3000,10000,21052);


	/******************************************/

	/*****
	//Taux de reussite des diferents nombres d'instances générées
	*****/

    //nombre_random_PALL(8,2500,20000,10000, 21052);

	/*****
	Tmax moyen pour sto vs sp
	*******/

	marge_PALL_stochastique(8,2500,20000,1000,50000);



	/****Nombre de routes pouvant être calculées en moins de "max" ms
	****/

	//search_efficiency(2500,20000,100);


	/******
	Bout de code pour tester la stabilisation de la marge necessaire quand on simule plusieurs periodes en stochastique

	Graphe g;
	int tmax;
	for(int i = 1;i<=1000;i*=10)
	{
		tmax = 0;
		for(int j=0;j<10000;j++)
		{
			
			g = init_graphe(2*8+1);
			graphe_etoile(g,20000);

			tmax = max(tmax,stochastic(g,2500,20000,i,1));
		}
		printf(" %d : max = %d\n",i,tmax);

	}
	****/
	return 0;
}
