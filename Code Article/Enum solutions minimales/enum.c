#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NOMBRE_ROUTE 5
#define PERIODE 100
#define TAILLE 10
#define DEBUG 0
#define MAX(X,Y) ((X) < (Y)) ? Y : X


void print_solution(int nombre_routes_traitees, int *ordre, int *seconde_periode){
	for(int i = 0; i < nombre_routes_traitees; i++) printf("(%d,%d) ", ordre[i], seconde_periode[i]);
	printf("\n");
}

//optimisation : mettre les 4 tableaux en un tableau de 4 valeurs

long long unsigned int enumeration(int *disponible, int nombre_route, int P){
	int ordre[nombre_route];   // ordre des routes dans la période commençant au temps de disponibilité de la première route de l'ordre  
	int seconde_periode[nombre_route]; // 1 si attend la seconde période, 0 sinon
	int disponible_periode[nombre_route];//temps de de disponibilité dans la période
	int depart[nombre_route];  // temps de depart dans la periode
	int routes_utilisees[nombre_route]; //routes utilisées dans la solution courantes (1 en position i quand i est utilisé)
	int nombre_routes_traitees;
	int add; //0 si on vient d'enlever un élément, 1 sinon
	long long unsigned int compteur = 0;

	for(int i = 0; i < nombre_route; i++) routes_utilisees[i] = 0;
	int i, decalage;

	seconde_periode[0] = 0;
	
	//pour optimiser, on fixe la route de départ, et on calcule la disponibilité dans la période
	for(int j = 0; j < nombre_route; j++){
		//on fixe j comme la première route
		ordre[0] = j;
		if(j>0) routes_utilisees[j-1] = 0; //on libère l'ancienne première route
		routes_utilisees[j] = 1;
		add = 1;
		nombre_routes_traitees = 1;
		disponible_periode[0] = 0;
		//on calcule les valeurs de disponible_periode
		for(i = 1; i < nombre_route ; i++) {
			decalage = (disponible[0]%P) - (disponible[i]%P);
			disponible_periode[i] = (decalage >= 0) ? decalage : P - decalage;
		}
		while(nombre_routes_traitees > 0){
			if(DEBUG) printf("%d   %llu   :",nombre_routes_traitees,compteur);
			if(DEBUG) print_solution(nombre_routes_traitees, ordre, seconde_periode);
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
					seconde_periode[nombre_routes_traitees] = 0;
					routes_utilisees[i] = 1;
					depart[i] = MAX(disponible_periode[i],depart[i-1]+TAILLE);
					nombre_routes_traitees++;
				}
				else{//on vient d'enlever un element, on passe au suivant
					if(!seconde_periode[nombre_routes_traitees]){//passe la route en deuxième période
						seconde_periode[nombre_routes_traitees] = 1;
						depart[i] = depart[i-1]+TAILLE;
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
							seconde_periode[nombre_routes_traitees] = 0;
							routes_utilisees[i] = 1;
							depart[i] = MAX(disponible_periode[i],depart[i-1]+TAILLE);
							nombre_routes_traitees++;
							add = 1;	
						}
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