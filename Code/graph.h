//A COMMON HEADER TO ALL C FILES

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define MAX_TIC_TOC  2000000
#include <math.h>

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
typedef struct listeroute{
	int index;
	struct listeroute * suiv;
} ListeRoute;
void affiche_graphe(Graphe g);
void creationfichier(Graphe g,char * nom);
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
void ecrire_fichier(Graphe g);
void resultats();
