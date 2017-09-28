void simuls_periode_PAZL(int nb_routes, int taille_message, int taille_routes,int nb_simuls);
void echec_PAZL(int nb_routes, int taille_message,int taille_routes, int nb_simuls);
void sucess_aller_PALL(int nb_routes, int taille_paquets,int taille_route,int marge_max, int nb_simuls, int periode);
void sucess_retour_PALL(int nb_routes, int taille_paquets,int taille_route,int marge_max, int nb_simuls, int periode);
void succes_PALL_3D(int nb_routes, int taille_paquets,int taille_route, int nb_simuls, int mode);
void nombre_random_PALL(int nb_routes, int taille_paquets,int taille_route, int nb_simuls, int periode);
void search_efficiency(int taille_message,int taille_routes, int nb_simuls);
void marge_PALL_stochastique(int nb_routes,int taille_paquets,int taille_route, int nb_simuls, int periode_max);