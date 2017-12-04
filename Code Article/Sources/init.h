

int ** declare_matrice(int N);



Graphe init_graphe(int N);

void libere_matrice(Graphe g);

void graphe_etoile(Graphe g,int taille_liens);

void graphe_etoile_opti(Graphe g,int taille_liens);
void graphe_etoile_dur(Graphe g,int taille_liens,int diff);