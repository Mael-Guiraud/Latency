#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <strings.h>


#define NOMBRE_ROUTE 6
#define PERIODE 100
#define TAILLE 10
#define DEBUG 0
#define MAX(X,Y) ((X) < (Y)) ? Y : X


/* Définition d'un BITSET pour optimisation potentielle de la boucle de recherche*/

typedef uint64_t BITSET; 

inline int get_bit(BITSET x, int pos){
	return x & (1 << pos);
}

inline BITSET set_bit_zero(BITSET x, int pos){
	return x & (~(1 << pos));
}


inline BITSET set_bit_one(BITSET x, int pos){
	return x | (1 << pos);
}

inline int first_zero(BITSET x){
	return ffs(~x);
}

void print_bitset(unsigned int x){
	for(int i = 0; i < 32; i++) printf("%d",get_bit(x,i));
		printf("\n");
}

/*  Définition de la représentation de la solution */

typedef struct{
	int depart;// temps de depart dans la periode quand elle est placée
	char numero; // numero de la route
	char seconde_periode; //1 si attend la seconde période, 0 sinon
} SOLUTION;


void print_solution(int nombre_routes_traitees, SOLUTION *s, int *disponible_periode){
	for(int i = 0; i < nombre_routes_traitees; i++) printf("(%d,%d,%d,%d) ", s[i].numero, s[i].seconde_periode, disponible_periode[(int) s[i].numero],s[i].depart);
	printf("\n");
}


/* Algo principal d'énumération */


long long unsigned int enumeration(int *disponible, int nombre_route, int P){
	SOLUTION s[nombre_route];//stocke la solution partielle courante
	int disponible_periode[nombre_route];// temps auquel la route est disponible dans la période
	int nombre_routes_traitees;//taille de la solution partielle 
	int add; //0 si on vient d'enlever un élément, 1 sinon
	long long unsigned int compteur = 0;

	char routes_utilisees[nombre_route]; //routes utilisées dans la solution courantes (1 en position i quand i est utilisé), on pourrait faire un bit set
	for(int i = 0; i < nombre_route; i++) routes_utilisees[i] = 0;
	int i, decalage;
	s[0].seconde_periode = 0;
	
	//pour optimiser, on fixe la route de départ, et on calcule la disponibilité dans la période
	for(int j = 0; j < nombre_route; j++){
		//on fixe j comme la première route
		s[0].numero = j;
		if(j>0) routes_utilisees[j-1] = 0; //on libère l'ancienne première route si elle existe
		routes_utilisees[j] = 1;
		add = 1;
		nombre_routes_traitees = 1;
		//on calcule les valeurs de disponible_periode
		if(DEBUG) printf("Calcul des disponibilités selon le premier élément courant: ");
		for(i = 0; i < nombre_route ; i++) {
			decalage = (disponible[j]%P) - (disponible[i]%P);
			disponible_periode[i] = (decalage >= 0) ? decalage : P + decalage;
			if(DEBUG) printf("%d ", disponible_periode[i]);
		}
		if(DEBUG) printf("\n");
		while(nombre_routes_traitees > 0){
			if(DEBUG){ 
				printf("%d   %llu   :",nombre_routes_traitees,compteur);
			 	print_solution(nombre_routes_traitees, s, disponible_periode);
			}
			//traitement de la solution complete
			if(nombre_routes_traitees == nombre_route ){
				//coupe à faire sur solution complète à ajouter ici -> je ne sais plus lesquelles 
				compteur++;
				nombre_routes_traitees--;//retourne en arriere
				add = 0;
			}
			//on grossit la solution partielle en essayant d'ajouter un élément
			else{   
				if(add || s[nombre_routes_traitees].seconde_periode){//on vient d'ajouter un element ou on veut le prochain élément 
					i = 0;
					if(!add) {
						i = s[nombre_routes_traitees].numero + 1;
						routes_utilisees[i-1] = 0;//on retire la route précédemment utilisée à ce niveau
					}
					//on ajoute le premier élément dispo en première période, à partir de la première route non traitée à cette position
					for(; i < nombre_route && routes_utilisees[i]; i++){}  //si on fait un bitset, trouver la première route disponible est plus facile
					if(i == nombre_route){//la route courante retirée était la plus grande route possible, retourne un cran en arriere
						nombre_routes_traitees--;
						add = 0;
					}
					else{
						s[nombre_routes_traitees].depart = MAX(disponible_periode[i],s[nombre_routes_traitees - 1].depart + TAILLE);
						s[nombre_routes_traitees].numero = i;
						s[nombre_routes_traitees].seconde_periode = 0;
						routes_utilisees[i] = 1;
						add = 0;//par défaut, ça coupe et si toutes les conditions sont vérifiées on passe à la suite
						if( s[nombre_routes_traitees].depart + (nombre_route - nombre_routes_traitees)*TAILLE <= PERIODE &&
							(s[nombre_routes_traitees].numero > j || s[nombre_routes_traitees].depart > disponible_periode[(int) s[nombre_routes_traitees].numero])){
							//vérifie qu'on a la place pour prolonger la solution et que la solution est canonique (l'élément placé en première position est le plus petit avec 0 délai)
							int gap = s[nombre_routes_traitees].depart - TAILLE; //debut potentiel du gap
							int large_gap = gap - s[nombre_routes_traitees-1].depart >= TAILLE;
							if(s[nombre_routes_traitees-1].seconde_periode || large_gap){
								//il y a un trou/element de la seconde période dans lequel on pourrait mettre un element precedent de la deuxieme période
								int k;
								for(k = 0; k < nombre_routes_traitees && !( s[k].seconde_periode && disponible_periode[(int) s[k].numero] <= gap); k++){}
								if(k < nombre_routes_traitees) continue;		
							}	
							//il y a un trou dans lequel on pourrait mettre un élément non encore placé
							if(large_gap){//on a trouvé un trou
								int k;
								for(k = 0; k < nombre_route && (routes_utilisees[k] || disponible_periode[k] <= gap); k++){}
								if(k < nombre_route) continue;
							}
							nombre_routes_traitees++;	
							add = 1;	
						}
					}
				}
				else{//on vient d'enlever un element, on passe l'élément précédent dans la deuxième période
					s[nombre_routes_traitees].seconde_periode = 1;
					s[nombre_routes_traitees].depart = s[nombre_routes_traitees-1].depart + TAILLE;
					if(s[nombre_routes_traitees].depart < disponible_periode[(int) s[nombre_routes_traitees].numero] &&
						s[nombre_routes_traitees].depart + (nombre_route - nombre_routes_traitees)*TAILLE <= PERIODE){
						//on passe l'élément en deuxième période uniquement si ça fait gagner du temps, qu'il reste assez de place et qu'on ne peut 
						//pas l'échanger avec un autre élément dans la deuxième période
						int k;
						for(k = 0; k < nombre_routes_traitees && !( s[k].seconde_periode && disponible_periode[(int) s[k].numero] <= s[nombre_routes_traitees-1].depart); k++){}
						if(k == nombre_routes_traitees){	
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
	
	srand(time(NULL));	
	int *disponible = malloc(sizeof(int)*NOMBRE_ROUTE);
	int *delai = malloc(sizeof(int)*NOMBRE_ROUTE);
	for(int i = 0; i < NOMBRE_ROUTE; i++) {
		disponible[i] = rand()%PERIODE;
		delai[i] = rand()%PERIODE;
	}
	long long unsigned int nb_enum = enumeration(disponible,NOMBRE_ROUTE,PERIODE);
	long long unsigned int nb_exhaustif = 1;
	for(int i = 2; i <= NOMBRE_ROUTE; i++) nb_exhaustif *= 2*i;
	printf("Nombre de solutions énumérées %llu\n",nb_enum);
	printf("Nombre de solutions total %llu, proportion %f\n",nb_exhaustif, (float) nb_enum / nb_exhaustif);
}