#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <strings.h>
#include <limits.h>
#include <string.h>

#define NOMBRE_ROUTE 13
#define PERIODE 140
#define TAILLE 10
#define DEBUG 0
#define VERBOSE 0
#define NOMBRE_EXPERIENCE 1
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

void print_bitset(BITSET x){
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

/* Stockage des solutions */

typedef struct{
	int pos;
	int taille_max;
	int *contenu;
} STOCK;

STOCK initialise_stock(){//
	STOCK st = {0,100000*NOMBRE_ROUTE,malloc(sizeof(int)*NOMBRE_ROUTE*100000)};
	//printf("Allocation initiale de memoire pour le stock, suffisant pour %d Solutionss\n",st.taille_max/NOMBRE_ROUTE);
	return st;
}

void corrige_ensemble(STOCK *st, int pos){ //enlève toutes les solutions inférieures à celle en position pos
	for(int i = pos + NOMBRE_ROUTE; i < st->pos; i+= NOMBRE_ROUTE){
		int j;
		for(j=0; j< NOMBRE_ROUTE && st->contenu[pos + j] <= st->contenu[i+j] ;j++){}//si la solution courante est plus grande que celle en i
			//elle est strictement plus grande et doit être éliminée
		if( j == NOMBRE_ROUTE){
			memcpy(st->contenu + i, st->contenu + st->pos - NOMBRE_ROUTE, NOMBRE_ROUTE*sizeof(int));//on écrase la solution
																									//en la remplaçant par la dernière
			st->pos -= NOMBRE_ROUTE;
			i--;//on retraite la nouvelle route en i
		}
	}

}


int insere_solution(SOLUTION *s, int *disponible_periode, int *disponible, STOCK *st){//return 1 if the solution has been inserted, 0 otherwise
	if(st->pos == st->taille_max){//on redimensionne
		st->taille_max *= 2;
		//printf("Reallocation de memoire pour le stock, suffisant pour %d solutions\n",st->taille_max/NOMBRE_ROUTE);
		st->contenu = realloc(st->contenu, sizeof(int)*st->taille_max);
	}
	// on contruit la solution directement en mémoire
	int i;
	for(i = 0; i < NOMBRE_ROUTE; i++){
		st->contenu[st->pos + s[i].numero] = disponible[(int) s[i].numero] + s[i].depart - disponible_periode[(int) s[i].numero] + PERIODE*(s[i].seconde_periode);
	}
	//on a pas besoin de la vraie valeur -> on pourrait virer des choses dans l'équation

	//for(i = 0; i < NOMBRE_ROUTE; i++) printf("%d ",st->contenu[st->pos + i] );
	//	printf("\n");
	for(i = 0; i < st->pos; i+=NOMBRE_ROUTE){
		int j = 0;
		while(1){//on pourrait mettre while 1, pas deux solutions égales
			if(st->contenu[i + j] < st->contenu[st->pos + j]){//la solution à insérer a une valeur strictement supérieure 
				for(j++; j < NOMBRE_ROUTE && st->contenu[i + j] <= st->contenu[st->pos + j]; j++){}
				if(j == NOMBRE_ROUTE){//on a trouvé une solution strictement plus petite que celle qu'on veut insérer, on ne l'insère pas
					printf("refus de la solution \n");
					for(j = 0; j < NOMBRE_ROUTE; j++) printf("%d ", st->contenu[i + j]);
					printf("\n");
					return 0;
				}
				break;
			}
			if(st->contenu[i + j] > st->contenu[st->pos + j]){//la solution à insérer a une valeur strictement supérieure
				for(j++; j < NOMBRE_ROUTE && st->contenu[i + j] >= st->contenu[st->pos + j]; j++){}
				if(j == NOMBRE_ROUTE){//on a trouvé une solution strictement plus grande que celle qu'on veut insérer, on la remplace
					memcpy(st->contenu + i,st->contenu + st->pos, NOMBRE_ROUTE*sizeof(int));
					corrige_ensemble(st,i);
					return 1;
				}
				break;
			}
			j++;
		}
	}//si on arrive ici, la nouvelle solution est incomparable avec les solutions sotckées
	st->pos += NOMBRE_ROUTE; //on valide la solution qui était déjà copiée à la fin du tableau
	return 1;
}

void afficher_stock(STOCK st){
	printf("Affichage des solutions stockées:");
	for(int i = 0; i < st.pos; i++){
		if(i % NOMBRE_ROUTE == 0) printf("\n");
		printf("%d ",st.contenu[i]);
	}
	printf("\n");
}

float moyenne_minimale = 0;


//Élimine des solutions qu'on peut optimiser 

int filtre(SOLUTION *s, int *disponible_periode, int nombre_route){//renvoie 0 si on détecte une permutation permettant d'améliorer la solution, 1 sinon
	for(int i = 0; i < nombre_route; i++){ //inverser la boucle pour perf ?
		if(s[i].seconde_periode && disponible_periode[(int)s[i].numero] <= PERIODE - TAILLE ){//pour chaque route en position 2, qu'il est possible de placer en seconde période
			int depart = s[i+1].depart - TAILLE;//on veut placer un paquet qui en profiterai ici 
			int dispo = disponible_periode[(int)s[i].numero];//disponibilité du paquet, on veut le mettre en premiere periode, donc on cherche une place à partir de là
			int new = 1;
			while(new){//tant qu'on trouve un élément qu'on peut améliorer
			//trouve l'élément avec qui échanger l'élément courant qui parte après dispo 
			//et qui soit de dispo minimum
				new = 0;
				int j;
				for(j = i; j < nombre_route -1 && s[j+1].depart < dispo + TAILLE; j++){}//trouve la première route qui si on la déplacait permettrait de placer la route courante
				//if(j == nombre_route -1) on peut toujours déplacer le dernier élément
				for(; j < nombre_route; j++){//on cherche la dispo minimum des routes restantes
					if(s[j].seconde_periode || disponible_periode[(int) s[j].numero] <= depart) return 0;//on a trouvé un élément qu'on peut mettre à la place de l'élément i
					if(disponible_periode[(int) s[j].numero] < dispo){//on a trouvé un nouvel élément qu'on peut permuter et placer avant
						dispo = disponible_periode[(int) s[j].numero];
						new = 1;
					}					
				}	
			}	
		}
	}
	return 1;
}


/* Algo principal d'énumération */

long long unsigned int enumeration(int *disponible, int nombre_route, int P){
	SOLUTION s[nombre_route];//stocke la solution partielle courante
	int disponible_periode[nombre_route];// temps auquel la route est disponible dans la période
	int first_gap = 0;//O valeur par défaut pour dire qu'il n'y a pas de gap dans la solution, sinon position dans l'ordre de l'élément après le gap
	int nombre_routes_traitees;//taille de la solution partielle 
	int add; //0 si on vient d'enlever un élément, 1 sinon
	long long unsigned int compteur = 0;
	STOCK st = initialise_stock();

	char routes_utilisees[nombre_route]; //routes utilisées dans la solution courantes (1 en position i quand i est utilisé), on pourrait faire un bit set
	for(int i = 0; i < nombre_route; i++) routes_utilisees[i] = 0;
	s[0].seconde_periode = 0;
	s[0].depart = 0;
	
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
		for(int i = 0; i < nombre_route ; i++) {
			disponible_periode[i] = disponible[i]%P - disponible[j]%P + (disponible[j]%P > disponible[i]%P)*P;//formule pour les deux cas: i disponible avnt j ou le contraire
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
				int k = nombre_route;
				if(s[nombre_routes_traitees-1].depart <= P - 2*TAILLE){
					for(k = 0; k < nombre_route && (!s[k].seconde_periode || disponible_periode[(int) s[k].numero] > P-TAILLE); k++){}	
				}
				//if(!filtre(s,disponible_periode,nombre_route)) printf("filtre activé \n");
				if(k == nombre_route && filtre(s,disponible_periode,nombre_route)){
				//ne prend pas en compte la solution s'il y a un gap à la fin et un élément en deuxième période qu'on pourrait y mettre 
					compteur++;
					int res = insere_solution(s,disponible_periode,disponible,&st); 
					if(!res) {
						printf("solution générée mais pas minimale \n");
						print_solution(nombre_routes_traitees, s, disponible_periode);
						for(int i = 0; i < NOMBRE_ROUTE; i++) printf("%d ", disponible[i]);
						printf("\n");
						return 1;
					}
				}
				nombre_routes_traitees--;//retourne en arriere
				if (first_gap > nombre_routes_traitees) first_gap = 0;//the gap has been removed from the solution, change state
				add = 0;
			}
			else{
				//on grossit la solution partielle en essayant d'ajouter un élément
				if(add || s[nombre_routes_traitees].seconde_periode){//on vient d'ajouter un element ou on veut le prochain élément 
					int i = 0;
					if(!add) {
						i = s[nombre_routes_traitees].numero + 1;
						routes_utilisees[i-1] = 0;//on retire la route précédemment utilisée à ce niveau
					}
					add = 0;
					//on ajoute le premier élément dispo en première période, à partir de la première route non traitée à cette position
					for(; i < nombre_route && routes_utilisees[i]; i++){}  //si on fait un bitset, trouver la première route disponible est plus facile
					if(i == nombre_route){//la route courante retirée était la plus grande route possible, retourne un cran en arriere
						nombre_routes_traitees--;
						if (first_gap > nombre_routes_traitees) first_gap = 0;//the gap has been removed from the solution, change state
					}
					else{
						int un_buffer = disponible_periode[i] < s[nombre_routes_traitees - 1].depart + TAILLE;
						s[nombre_routes_traitees].depart = un_buffer ? s[nombre_routes_traitees - 1].depart + TAILLE : disponible_periode[i];
						s[nombre_routes_traitees].numero = i;
						s[nombre_routes_traitees].seconde_periode = 0;
						routes_utilisees[i] = 1;
						//par défaut ça coupe et si toutes les conditions sont vérifiées on passe à la suite
						if(s[nombre_routes_traitees].depart + (nombre_route - nombre_routes_traitees)*TAILLE <= PERIODE &&
							(i > j || un_buffer)){
							//vérifie qu'on a la place pour prolonger la solution et que la solution est canonique (l'élément placé en première position est le plus petit avec 0 délai)
							int debut_gap = s[nombre_routes_traitees].depart - TAILLE; //debut potentiel du gap
							int large_gap = debut_gap >= s[nombre_routes_traitees-1].depart + TAILLE;
							if(s[nombre_routes_traitees-1].seconde_periode || large_gap){
								//il y a un trou/element de la seconde période dans lequel on pourrait mettre un element precedent de la deuxieme période
								int k;
								for(k = 0; k < nombre_routes_traitees && (!s[k].seconde_periode || disponible_periode[(int) s[k].numero] > debut_gap); k++){}
								if(k < nombre_routes_traitees) continue;		
							}	
							//il y a un trou dans lequel on pourrait mettre un élément non encore placé
							if(large_gap){//on a trouvé un trou
								int k;
								for(k = 0; k < nombre_route && (routes_utilisees[k] || disponible_periode[k] > debut_gap); k++){}
								if(k < nombre_route) continue;
							}
							//coupe si on peut échanger l'élément i positionné avec un gars placé dans la deuxième période précédemment
							int k;
							for(k = 0; k < nombre_routes_traitees && (!s[k].seconde_periode || 
																	disponible_periode[(int) s[k].numero] > s[nombre_routes_traitees].depart || 
																	disponible_periode[i] > s[k].depart); k++){}
							if(k == nombre_routes_traitees){
								nombre_routes_traitees++;	
								if(!first_gap && large_gap) first_gap = nombre_routes_traitees;
								add = 1;	
							}	
						}
					}
				}
				else{//on vient d'enlever un element, on passe l'élément précédent dans la deuxième période
					s[nombre_routes_traitees].seconde_periode = 1;
					s[nombre_routes_traitees].depart = s[nombre_routes_traitees-1].depart + TAILLE;
					if(!first_gap && s[nombre_routes_traitees].depart < disponible_periode[(int) s[nombre_routes_traitees].numero] && //on passe l'élément en deuxième période uniquement si ça fait gagner du temps
						s[nombre_routes_traitees].depart + (nombre_route - nombre_routes_traitees)*TAILLE <= P //qu'il reste assez de place 
						){
						int k;
						for(k = 0; k < nombre_routes_traitees && (!s[k].seconde_periode || disponible_periode[(int) s[k].numero] > s[nombre_routes_traitees].depart); k++){}
							//on vérifie qu'on ne peut pas l'échanger avec un autre élément dans la deuxième période qui passerait en première période
						if(k == nombre_routes_traitees){	
							nombre_routes_traitees++;
							add = 1;	
						}
					}
				}
			}	
		}
	}
	//if(DEBUG) afficher_stock(st);
    //if(VERBOSE) printf("Solutions minimales au nombre de %d\n", st.pos/NOMBRE_ROUTE);
	moyenne_minimale += (float) st.pos/(NOMBRE_ROUTE*compteur);
	return compteur;
}

//Autre borne inf en listant les routes dans l'autre sens et en gérant les dispo.


int verifie_delai(int pos, int *depart_max, int * ordre, char* routes_utilisees){//renvoie 1 si ça peut passer, 0 si on est sur de faire pire que la meilleur sol
	int i;
	for(i = 0; i < NOMBRE_ROUTE && (routes_utilisees[ordre[i]] || depart_max[i] >= pos); i++){
		pos += !routes_utilisees[ordre[i]]*TAILLE;
	}
	return i == NOMBRE_ROUTE;
}
	

/*	
 verifie_delai(int pos, int *depart_max, char* routes_utilisees){//renvoie 1 si ça peut passer, 0 si on est sur de faire pire que la meilleur sol
	int i; // version plus simple qui prend en compte que le delai
	for(i = 0; i < NOMBRE_ROUTE && (routes_utilisees[i] || depart_max[i] > pos); i++){}
	return  i == NOMBRE_ROUTE;
}*/


//pourrait etre optimisé si nécessaire
void trier(int *val, int *ordre, int taille){
	for(int i = 0; i < taille; i++) ordre[i] = i;
	//fais un tri à bulle car il y a peu de valeurs 
	//printf("Avant le tri:\n");
	//for(int i = 0; i < taille; i++) printf("%d ",val[i]);
	int mod = 1;
	for(int i = 0; i < taille && mod ; i++){
		mod = 0;
		for(int k = taille -1; k > i ; k --){// les j premiers elements sont triés
			if(val[k-1] > val[k]){
				int temp = val[k];
				val[k] = val[k-1];
				val[k-1] = temp;
				temp = ordre[k];
				ordre[k] = ordre[k-1];
				ordre[k-1] = temp;
				mod = 1;
			}
		}
	}
	//printf("Resultat du tri:\n");
	//for(int i = 0; i < taille; i++) printf("%d ",val[i]);
	//printf("\n");
	//for(int i = 0; i < taille; i++) printf("%d ",ordre[i]);
}


//les coupes manquantes ne paraissent pas intéressantes dans ce contexte

int optim(int *disponible, int *delai, int nombre_route, int P){
	SOLUTION s[nombre_route];//stocke la solution partielle courante
	int disponible_periode[nombre_route];// temps auquel la route est disponible dans la période
	int nombre_routes_traitees;//taille de la solution partielle 
	int add; //0 si on vient d'enlever un élément, 1 sinon
	int min_delai_meilleure_sol = 0;//valeur qu'on va calculer, plus petit delai pour une solution, initialisée de manière brutale
	for(int i = 0; i < nombre_route; i++){
		if(min_delai_meilleure_sol < delai[i]) min_delai_meilleure_sol = delai[i];
	}
	min_delai_meilleure_sol += P;//au pire le temps total est égal au pire délai plus la période 
	long long unsigned int compteur = 0;
	int min_delai[nombre_route];//pile des valeurs de delai de la solution courante
	char routes_utilisees[nombre_route]; //routes utilisées dans la solution courantes (1 en position i quand i est utilisé), on pourrait faire un bit set
	int depart_max[nombre_route];
	int ordre_depart_max[nombre_route];
	for(int i = 0; i < nombre_route; i++) routes_utilisees[i] = 0;
	s[0].seconde_periode = 0;
	s[0].depart = 0;
	//pour optimiser, on fixe la route de départ, et on calcule la disponibilité dans la période
	for(int j = 0; j < nombre_route; j++){
		//on fixe j comme la première route
		s[0].numero = j;
		if(j>0) routes_utilisees[j-1] = 0; //on libère l'ancienne première route si elle existe
		routes_utilisees[j] = 1;
		add = 1;
		nombre_routes_traitees = 1;
		min_delai[0] = delai[j];
		
		if(DEBUG) printf("\nCalcul des disponibilités selon le premier élément courant: \n");
		for(int i = 0; i < nombre_route ; i++) { 
			disponible_periode[i] = disponible[i]%P - disponible[j]%P + (disponible[j]%P > disponible[i]%P)*P;//formule pour les deux cas: i disponible avnt j ou le contraire
			if(DEBUG) printf("%d ", disponible_periode[i]);
		}
		if(DEBUG) printf("\nCalcul des temps de départ max selon le premier élément courant et le min_delai actuel: \n");
		for(int i = 0; i < nombre_route ; i++) {
			depart_max[i] = disponible_periode[i] + min_delai_meilleure_sol - delai[i];
			if(disponible_periode[i] > P - TAILLE) depart_max[i] -= P;
			if(DEBUG) printf("%d ", depart_max[i]);
		}
		trier(depart_max,ordre_depart_max,nombre_route);
		if(DEBUG) printf("\n");
		while(nombre_routes_traitees > 0){
			if(DEBUG){ 
				printf("%d   %llu   :",nombre_routes_traitees,compteur);
			 	print_solution(nombre_routes_traitees, s, disponible_periode);
			 	printf("%d \n",min_delai[nombre_routes_traitees - 1]);
			}
			//traitement de la solution complete
			if(nombre_routes_traitees == nombre_route ){
				//si on arrive à ce niveau, la solution est garantie d'être meilleure
				int dif = min_delai_meilleure_sol - min_delai[nombre_routes_traitees - 1];
				for(int i = 0; i < nombre_route; i++) depart_max[i] -= dif; 
				min_delai_meilleure_sol = min_delai[nombre_routes_traitees - 1];
				compteur++;
				nombre_routes_traitees--;//retourne en arriere
				add = 0;
			}
			else{
				//on grossit la solution partielle en essayant d'ajouter un élément
				if(add || s[nombre_routes_traitees].seconde_periode){//on vient d'ajouter un element ou on veut le prochain élément 
					int i = 0;
					if(!add){
						i = s[nombre_routes_traitees].numero + 1;
						routes_utilisees[i-1] = 0;//on retire la route précédemment utilisée à ce niveau
					}
					add = 0;
					//on ajoute le premier élément dispo en première période, à partir de la première route non traitée à cette position
					for(; i < nombre_route && routes_utilisees[i]; i++){}  //si on fait un bitset, trouver la première route disponible est plus facile
					if(i == nombre_route){//la route courante retirée était la plus grande route possible, retourne un cran en arriere
						nombre_routes_traitees--;
					}
					else{
						int un_buffer = disponible_periode[i] < s[nombre_routes_traitees - 1].depart + TAILLE;
						s[nombre_routes_traitees].depart = un_buffer ? s[nombre_routes_traitees - 1].depart + TAILLE : disponible_periode[i];
						s[nombre_routes_traitees].numero = i;
						s[nombre_routes_traitees].seconde_periode = 0;
						routes_utilisees[i] = 1;
						//par défaut ça coupe et si toutes les conditions sont vérifiées on passe à la suite
						if(s[nombre_routes_traitees].depart + (nombre_route - nombre_routes_traitees)*TAILLE <= PERIODE &&
							(i > j || un_buffer) && verifie_delai(s[nombre_routes_traitees].depart +TAILLE, depart_max, ordre_depart_max, routes_utilisees)){
							//vérifie qu'on a la place pour prolonger la solution, que la solution est canonique (l'élément placé en première position est le plus petit avec 0 délai)
							//et que les elements non places pourront partit à temps
							int debut_gap = s[nombre_routes_traitees].depart - TAILLE; //debut potentiel du gap
							int large_gap = debut_gap - s[nombre_routes_traitees-1].depart >= TAILLE;
							if(s[nombre_routes_traitees-1].seconde_periode || large_gap){
								//il y a un trou/element de la seconde période dans lequel on pourrait mettre un element precedent de la deuxieme période
								int k;
								for(k = 0; k < nombre_routes_traitees && (!s[k].seconde_periode || disponible_periode[(int) s[k].numero] > debut_gap); k++){}
								if(k < nombre_routes_traitees) continue;		
							}	
							//il y a un trou dans lequel on pourrait mettre un élément non encore placé
							if(large_gap){//on a trouvé un trou
								int k;
								for(k = 0; k < nombre_route && (routes_utilisees[k] || disponible_periode[k] > debut_gap); k++){}
								if(k < nombre_route) continue;
							}
							min_delai[nombre_routes_traitees] =  MAX(min_delai[nombre_routes_traitees -1], 
							delai[i] + s[nombre_routes_traitees].depart - disponible_periode[i]);
							if(min_delai[nombre_routes_traitees] < min_delai_meilleure_sol){
								nombre_routes_traitees++;	
								add = 1;	
							}	
						}
					}
				}
				else{//on vient d'enlever un element, on passe l'élément précédent dans la deuxième période
					s[nombre_routes_traitees].seconde_periode = 1;
					s[nombre_routes_traitees].depart = s[nombre_routes_traitees-1].depart + TAILLE;
					int route_courante = (int) s[nombre_routes_traitees].numero;
					if(s[nombre_routes_traitees].depart < disponible_periode[route_courante] &&
						s[nombre_routes_traitees].depart + (nombre_route - nombre_routes_traitees)*TAILLE <= PERIODE){
						//on passe l'élément en deuxième période uniquement si ça fait gagner du temps, qu'il reste assez de place et qu'on ne peut 
						//pas l'échanger avec un autre élément dans la deuxième période
						int k;
						for(k = 0; k < nombre_routes_traitees && (!s[k].seconde_periode || disponible_periode[(int) s[k].numero] > s[nombre_routes_traitees].depart); k++){}
						if(k == nombre_routes_traitees){	
							min_delai[nombre_routes_traitees] =  MAX(min_delai[nombre_routes_traitees - 1], 
							delai[route_courante] + PERIODE + s[nombre_routes_traitees].depart - disponible_periode[route_courante]);
							if(min_delai[nombre_routes_traitees] < min_delai_meilleure_sol){
								nombre_routes_traitees++;	
								add = 1;	
							}
						}
					}
				}
			}	
		}
	}
	if(VERBOSE) printf("Nombre de solutions visitées %llu \n", compteur);
	return min_delai_meilleure_sol;
}

/* Structure de données pour un graphe de conflit */

typedef struct{
	int disponible; //temps auquel le message est disponible dans un arc donné
	int delai; //temps total sur la route
	int suivant_niveau; //niveau du point de contention suivant (-1 si pas de suivant)
	int suivant_indice;//indice du prochain point de contention
} MESSAGE_CONTENTION;

typedef struct{
	int nombre_message;
	MESSAGE_CONTENTION *mess;
}POINT_CONTENTION;

typedef struct{
	int nombre_point;//nombre de point de contention du niveau
	POINT_CONTENTION *cont;
} NIVEAU;

typedef struct{
	int nombre_niveau;
	NIVEAU *niv;
}GRAPHE;

//todo: génération aléatoire d'un graphe -> matrice des distances aléatoires
//puis on tire des routes au hasard jusqu'au nombre fixé
//en paramètre les largeurs de chaque niveau
//todo: affichage d'un graphe  


int main(){
	
	srand(time(NULL));	
	int *disponible = malloc(sizeof(int)*NOMBRE_ROUTE);
	int *delai = malloc(sizeof(int)*NOMBRE_ROUTE);
	float moyenne_proportion = 0;
	moyenne_minimale = 0;
	long long unsigned int nb_exhaustif = 1;
	for(int i = 2; i <= NOMBRE_ROUTE; i++) nb_exhaustif *= 2*i;


	for(int j = 0; j < NOMBRE_EXPERIENCE; j++){
		for(int i = 0; i < NOMBRE_ROUTE; i++) {
			disponible[i] = rand()%PERIODE;
			delai[i] = rand()%PERIODE + disponible[i];
		}
		
		long long unsigned int nb_enum = enumeration(disponible,NOMBRE_ROUTE,PERIODE);
		if(VERBOSE){
			printf("Nombre de solutions énumérées %llu\n",nb_enum);
			printf("Nombre de solutions total %llu, proportion %f\n",nb_exhaustif, (float) nb_enum / nb_exhaustif);	
		}
		moyenne_proportion += (float) nb_enum / (float) nb_exhaustif;
		/*
		if(VERBOSE) {
			printf("Affichage des disponibilités et délais: ");
			for(int i = 0; i < NOMBRE_ROUTE; i++) printf("(%d %d)",disponible[i],delai[i]);
			printf("\nSolution optimale de délai %d \n", optim(disponible,delai,NOMBRE_ROUTE,PERIODE));
		}
		else{
			optim(disponible,delai,NOMBRE_ROUTE,PERIODE);	
		}
		*/
	}		
	printf("Moyenne de la proportion de solutions énumérées %f.\n",moyenne_proportion/NOMBRE_EXPERIENCE);
	printf("Moyenne de la proportion de solution minimales parmi les énuméres %f.\n",moyenne_minimale/NOMBRE_EXPERIENCE);
	
}
