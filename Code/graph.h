//A COMMON HEADER TO ALL C FILES

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define MAX_TIC_TOC  2000000
#include <math.h>
#include <string.h>

typedef struct vertex{
  int index;
  int edge_number;
  int *weight;
  int *neighboors;
} Vertex;
typedef struct route{
	Vertex * vertices;
	int route_lenght;
} Route;
typedef struct graphe{
  Vertex* vertices;
  int sources;
  int vertex_number;
  Route * routes;
} Graphe;
typedef struct twoWayTrip{
	int taille;
	int *M;
	int *W;
}TwoWayTrip;
typedef struct routeStar{
	int nb_routes;
	int *x;
	int *y;
}RouteStar;
typedef struct listeroute{
	int index;
	struct listeroute * suiv;
} ListeRoute;
void affiche_graphe(Graphe g);
void creationfichierGraph(Graphe g,char * nom);
long int rand_entier(int n);
Graphe topologie1(int sources, int leaves, int mode);
int distance(Route r, int v);
int vertex_index(Route r, Vertex v);
Graphe renverse(Graphe g);
int longest(Route * r,int taille);
int full(int * tab, int taille);
void affichetab(int* tab, int taille);
TwoWayTrip heuristique_top_1(Graphe g, int P);
int greater(int * tab,int taille);
void ajoutetab(int * t1, int * t2,int taille);
void afficheTwoWayTrip(TwoWayTrip t);
int tMax(Graphe g,TwoWayTrip t);
int lower(int * tab,int taille);
int longerOfeEligible(int * Dl, int * lambda, int * arrivee,int offset, int taille);
void simulation();
void ecrire_fichierGraph(Graphe g);
void resultats();
void bruteforce(RouteStar r,int * tab, int * dispo,int * offsets, int taille, int nb_dispo, int budget, int offset, int P);
int factorielle(int a);
void trouver_mieux_par_bruteforce(Graphe g, int tmax);
int decaler_a(int a);
TwoWayTrip algo_yann(Graphe g, int P);
int taille_fenetre(int * tab, int taille);
int indiceTMax(Graphe g,TwoWayTrip t);
void tests();
void creationfichierResultats(int i, int tmax,int window,char * nom);
int max(int a, int b);
TwoWayTrip shortest_to_longest(Graphe g, int P);
void ecrire_bornes(int l, int t);
TwoWayTrip algo_bruteforce(int P);
int i_dispo(int * tab, int taille);
int * cpy_tab(int * tab1, int* tab2, int taille);
int test_collisions(int * tab, int taille);
int test_collisions_brute(RouteStar r,int * tab, int taille,int j);



