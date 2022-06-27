
void tri_bulles(int* tab,int* ordre,int taille);
void tri_bulles_inverse(int* tab,int* ordre,int taille);

int is_ok(Graphe g, int taille_paquet, int * mi, int * wi, int p);
void fisher_yates(int * tab, int taille);

int greater(int * tab,int taille);
int lower(int * tab,int taille);

#define max(a, b) (((a) >= (b)) ? (a) : (b))
#define min(a, b) (((a) <= (b)) ? (a) : (b))

//int max(int a, int b);

//int min(int a,int b);

int longest_route(Graphe g);
