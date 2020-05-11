#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NOMBRE_ROUTE 10
#define PERIODE 100
#define TAILLE 10


void print_solution(int nombre_routes_traitees, int *ordre, int *periode){
	for(int i = 0; i < nombre_routes_traitees; i++) printf("(%d,%d) ", ordre[i], periode[i]);
	printf("\n");
}

long long unsigned int enumeration(int *disponible, int nombre_route, int P){
	int ordre[nombre_route];   // ordre des routes dans la période commençant au temps de disponibilité de la première route de l'ordre  
	int periode[nombre_route]; // passage dans la première période ou la deuxième (1 ou 2)
	int depart[nombre_route];  // temps de depart dans la periode
	int routes_utilisees[nombre_route]; //routes utilisées dans la solution courantes (1 en position i quand i est utilisé)
	int nombre_routes_traitees = 0;
	int add = 1; //0 si on vient d'enlever un élément, 1 sinon
	long long unsigned int compteur = 0;

	for(int i = 0; i < nombre_route; i++) routes_utilisees[i] = 0;
	int i;

	while(nombre_routes_traitees > -1){
		//printf("%d   %d   :",nombre_routes_traitees,compteur);
		//print_solution(nombre_routes_traitees, ordre, periode);
		if(nombre_routes_traitees == nombre_route ){//traitement de la solution complete
			compteur++;
			nombre_routes_traitees--;//retourne en arriere
			add = 0;
		}
		else{//on grossit la solution
			if(add){//on vient d'ajouter un element  
				//on ajoute le premier élément dispo en première période
				for(i = 0; i < nombre_route && routes_utilisees[i]; i++){}
				ordre[nombre_routes_traitees] = i;
				periode[nombre_routes_traitees] = 1;
				routes_utilisees[i] = 1;
				nombre_routes_traitees++;
			}
			else{//on vient d'enlever un element, on passe au suivant
				if(periode[nombre_routes_traitees] == 1){//passe la route en deuxième période
					periode[nombre_routes_traitees] = 2;
					nombre_routes_traitees++;
					add = 1;
				}
				else{
					//on ajoute le premier élément dispo en première période
					int route_courante = ordre[nombre_routes_traitees];
					routes_utilisees[route_courante] = 0;
					for(i = route_courante + 1; i < nombre_route && routes_utilisees[i]; i++){}
					if(i == nombre_route){//on a déjà traité la plus grande route possible, retourne un cran en arriere
						nombre_routes_traitees--;
					}
					else{//on a trouvé une route à ajouter
						ordre[nombre_routes_traitees] = i;
						periode[nombre_routes_traitees] = 1;
						routes_utilisees[i] = 1;
						nombre_routes_traitees++;
						add = 1;	
					}
				}
			}	
		}
	}
	return compteur;
}




int main(){
	int *disponible = malloc(sizeof(int)*NOMBRE_ROUTE);
	srand(time(NULL));
	for(int i = 0; i < NOMBRE_ROUTE; i++) disponible[i] = rand()%PERIODE;
	printf("Nombre de solutions énumérées %llu",enumeration(disponible,NOMBRE_ROUTE,PERIODE));
}