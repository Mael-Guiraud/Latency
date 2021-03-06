#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <strings.h>
#include <limits.h>
#include <string.h>


#define DEBUG 0
#define MAX(X,Y) ((X) < (Y)) ? Y : X

/*  Définition de la représentation de la solution */

typedef struct{
	int depart;// temps de depart dans la periode quand elle est placée
	char numero; // numero de la route
	char seconde_periode; //1 si attend la seconde période, 0 sinon
} SOLUTION;


void print_solutions(int nombre_routes_traitees, SOLUTION *s, int *disponible_periode){
	for(int i = 0; i < nombre_routes_traitees; i++) printf("(%d,%d,%d,%d) ", s[i].numero, s[i].seconde_periode, disponible_periode[(int) s[i].numero],s[i].depart);
	printf("\n");
}


int optim(int *disponible, int *delai, int nombre_route, int P,int message_size,int bornesup){
	
	SOLUTION s[nombre_route];//stocke la solution partielle courante
	SOLUTION meilleure[nombre_route];//stocke la solution partielle courante
	int disponible_periode[nombre_route];// temps auquel la route est disponible dans la période
	int nombre_routes_traitees;//taille de la solution partielle 
	int add; //0 si on vient d'enlever un élément, 1 sinon
	int min_delai_meilleure_sol = bornesup;//valeur qu'on va calculer, plus petit delai pour une solution, initialisée de manière brutale
	long long unsigned int compteur = 0;
	int min_delai[nombre_route];//pile des valeurs de delai de la solution courante
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
		min_delai[0] = delai[j];
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
			 	print_solutions(nombre_routes_traitees, s, disponible_periode);
			 	printf("%d \n",min_delai[nombre_routes_traitees - 1]);
			}
			//traitement de la solution complete
			if(nombre_routes_traitees == nombre_route ){
				//si on arrive à ce niveau, la solution est garantie d'être meilleure
				min_delai_meilleure_sol = min_delai[nombre_routes_traitees - 1];
				compteur++;
				nombre_routes_traitees--;//retourne en arriere
				add = 0;
				memcpy(meilleure,s,sizeof(SOLUTION)*nombre_route);
			}
			else
			{
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
					}
					else{
						int un_buffer = disponible_periode[i] < s[nombre_routes_traitees - 1].depart + message_size;
						s[nombre_routes_traitees].depart = un_buffer ? s[nombre_routes_traitees - 1].depart + message_size : disponible_periode[i] ;
						s[nombre_routes_traitees].numero = i;
						s[nombre_routes_traitees].seconde_periode = 0;
						routes_utilisees[i] = 1;
						//par défaut ça coupe et si toutes les conditions sont vérifiées on passe à la suite

						if(DEBUG)printf("courante :%d premiere :%d un gap ? :%d\n",i,j,un_buffer);

						if( s[nombre_routes_traitees].depart + (nombre_route - nombre_routes_traitees)*message_size <= P &&
							(i > j || un_buffer)){
							//vérifie qu'on a la place pour prolonger la solution et que la solution est canonique (l'élément placé en première position est le plus petit avec 0 délai)
							int debut_gap = s[nombre_routes_traitees].depart - message_size; //debut potentiel du gap
							int large_gap = debut_gap - s[nombre_routes_traitees-1].depart >= message_size;
							if(s[nombre_routes_traitees-1].seconde_periode || large_gap){
								//il y a un trou/element de la seconde période dans lequel on pourrait mettre un element precedent de la deuxieme période
								int k;
								for(k = 0; k < nombre_routes_traitees && ( !s[k].seconde_periode || disponible_periode[(int) s[k].numero] > debut_gap); k++){}
								if(k < nombre_routes_traitees)  continue;		
							}	
							//il y a un trou dans lequel on pourrait mettre un élément non encore placé
							if(large_gap){//on a trouvé un trou
								int k;
								for(k = 0; k < nombre_route && (routes_utilisees[k] || disponible_periode[k] > debut_gap); k++){}
								if(k < nombre_route)  continue;
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
					s[nombre_routes_traitees].depart = s[nombre_routes_traitees-1].depart + message_size;
					int route_courante = (int) s[nombre_routes_traitees].numero;
					if(s[nombre_routes_traitees].depart < disponible_periode[route_courante] &&
						s[nombre_routes_traitees].depart + (nombre_route - nombre_routes_traitees)*message_size <= P){
						//on passe l'élément en deuxième période uniquement si ça fait gagner du temps, qu'il reste assez de place et qu'on ne peut 
						//pas l'échanger avec un autre élément dans la deuxième période
						int k;
						for(k = 0; k < nombre_routes_traitees && (!s[k].seconde_periode || disponible_periode[(int) s[k].numero] > s[nombre_routes_traitees].depart); k++){}
						if(k == nombre_routes_traitees){	
							min_delai[nombre_routes_traitees] =  MAX(min_delai[nombre_routes_traitees - 1], 
							delai[route_courante] + P + s[nombre_routes_traitees].depart - disponible_periode[route_courante]);
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
	if(DEBUG)
	{
		printf("Retour algo fpt yann %d \n",min_delai_meilleure_sol); 
		printf("Meilleure solution : ");
		int premiere = meilleure[0].numero;
		printf("Première route %d, date de dispo %d", premiere, disponible[premiere]);
		for(int i=0;i<nombre_route;i++)
		{
			int numero = meilleure[i].numero;
			int disponible_periode =   disponible[numero]%P - disponible[premiere]%P + (disponible[premiere]%P > disponible[numero]%P)*P;
			printf("\n-Route : %d(sec_periode %d), disponible %d, depart %d, ",meilleure[i].numero,meilleure[i].seconde_periode,disponible_periode,meilleure[i].depart);
			
			if(meilleure[i].seconde_periode == 0)
			{
				printf("date départ = %d,",disponible[numero] + meilleure[i].depart - disponible_periode );
			}
			else
			{
				printf("date départ = %d,",disponible[numero] + meilleure[i].depart - disponible_periode + P);
			}
		}
		printf("\n");
	}
	//printf("Nombre de solutions visitées %llu \n", compteur);
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
/*

int main(){
	
	srand(time(NULL));	
	int *disponible = malloc(sizeof(int)*NOMBRE_ROUTE);
	int *delai = malloc(sizeof(int)*NOMBRE_ROUTE);
	for(int i = 0; i < NOMBRE_ROUTE; i++) {
		disponible[i] = rand()%PERIODE;
		delai[i] = rand()%PERIODE + disponible[i];
	}
	
	long long unsigned int nb_enum = enumeration(disponible,NOMBRE_ROUTE,PERIODE);
	long long unsigned int nb_exhaustif = 1;
	for(int i = 2; i <= NOMBRE_ROUTE; i++) nb_exhaustif *= 2*i;
	printf("Nombre de solutions énumérées %llu\n",nb_enum);
	printf("Nombre de solutions total %llu, proportion %f\n",nb_exhaustif, (float) nb_enum / nb_exhaustif);
	
	printf("Affichage des disponibilités et délais: ");
	for(int i = 0; i < NOMBRE_ROUTE; i++) printf("(%d %d)",disponible[i],delai[i]);
	printf("\nSolution optimale de délai %d \n", optim(disponible,delai,NOMBRE_ROUTE,PERIODE));
	
}*/