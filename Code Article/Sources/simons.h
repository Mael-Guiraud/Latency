
int simons(Graphe g, int taille_paquet, int TMAX,int periode, int mode,int * offsets);
int simons_periodique(Graphe g, int taille_paquet,int TMAX, int periode, int * m_i);
int FPT_PALL(Graphe g, int taille_paquet,int TMAX, int periode, int * m_i);


int* retourne_offset(Graphe g, int taille_paquet, int* permutation,int mode,int periode);
int *retourne_departs(Graphe g, int * offsets);
