//A COMMON HEADER TO ALL C FILES

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define MAX_TIC_TOC  2000000
#define taille_paquet 2558
#include <math.h>
#include <string.h>
#include <time.h>
int solutions;
int best_window;
//Structures de données

//Un sommet du graphe, pointe sur ses voisins qui ont une arrete vers lui.
typedef struct vertex{
  int index;
  int edge_number;
  int *weight;
  int *neighboors;
} Vertex;

//Une route, suite de sommets
typedef struct route{
	Vertex * vertices;
	int route_lenght;
} Route;

//Un graphe, (sources = nombre de routes)
typedef struct graphe{
  Vertex* vertices;
  int sources;
  int vertex_number;
  Route * routes;
} Graphe;

//Un Twowaytrip définis par les offsets de départs et d'arrivée
typedef struct twoWayTrip{
	int taille;
	int window_size;
	int *M;
	int *W;
}TwoWayTrip;

typedef struct routeStar{
	int nb_routes;
	int *x;
	int *y;
}RouteStar;


//TOPOLOGIE.c fonctions de créations de graphe
Graphe topologie1(int sources, int leaves, int mode);
Graphe renverse(Graphe g);
Graphe topologie1_manuelle(int sources, int leaves,int * tab);

//heuristiques.c Differents algos
TwoWayTrip greedy_prime(Graphe g, int P);
TwoWayTrip algo_bruteforce(int P);
TwoWayTrip greedy_star(Graphe g, int P);
TwoWayTrip shortest_to_longest(Graphe g);
TwoWayTrip longest_shortest(Graphe g);
TwoWayTrip dichotomique(Graphe g,int P,int mode);
void bruteforce(int * tab,RouteStar r,int * dispo,int * offsets,int * offsetsr, int taille, int nb_dispo, int budget, int offset, int P);


//Fichiers.c Fonctions de gestions des fichiers
void creationfichierGraph(Graphe g,char * nom);
void ecrire_fichierGraph(Graphe g);
void creationfichier(int i, int tmax,int piretmax,char * nom);
void creationfichierWindow(int* tab, int taille,char * nom);
void ecrire_bornesWindow(int l, int t);
void ecrire_bornesTMax(int i);

//Test.c fonctions d'affichage
void affiche_graphe(Graphe g);
void afficheTwoWayTrip(TwoWayTrip t);
void affichetab(int* tab, int taille);
void afficheRouteStar(RouteStar r);
void simulation(int mode);
void simulationsTmax();

//Calculs.c Fonctions de petits calculs basiques
long int rand_entier(int n);
int distance(Route r, int v);
int vertex_index(Route r, Vertex v);
int longest(Route * r,int taille);
int longest_on_sources(Route * r,int taille);
int shortest_on_sources(Route * r,int taille);
int greater(int * tab,int taille);
int lower(int * tab,int taille);
int full(int * tab, int taille);
int factorielle(int a);
int max(int a, int b);
void ajoutetab(int * t1, int * t2,int taille);
int taille_fenetre(int * tab, int taille);
int tMax(Graphe g,TwoWayTrip t);
int indiceTMax(Graphe g,TwoWayTrip t);
int longerOfeEligible(int * Dl, int * lambda, int * arrivee,int offset, int taille);
int collision_a_b(int a, int b, int P);
int test_collisions_offset(int * retour, RouteStar r, int indice, int j,int P);
int test_collisions(int * tab, int taille, int P);
int i_dispo(int * tab, int taille, int i);
int * cpy_tab(int * tab1, int* tab2, int taille);
RouteStar cree_routestar(Graphe g);
int rentre_dans(int * tab, int a, int b);
int decaler_a(int a);
void occuper_p(int * tab, int a, int b);
void freeGraphe(Graphe g);
void freeTwoWayTrip(TwoWayTrip t);
void freeRouteStar(RouteStar r);


