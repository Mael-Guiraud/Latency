void simuls_periode_PAZL(int nb_routes, int taille_message, int taille_routes,int nb_simuls);
void echec_PAZL(int nb_routes, int taille_message,int taille_routes, int nb_simuls);
void sucess_aller_PALL(int nb_routes, int taille_paquets,int taille_route,int marge_max, int nb_simuls, int periode);
void sucess_retour_PALL(int nb_routes, int taille_paquets,int taille_route,int marge_max, int nb_simuls, int periode, int nb_rand);
void succes_PALL_3D(int nb_routes, int taille_paquets,int taille_route, int nb_simuls, int mode);
void nombre_random_PALL(int nb_routes, int taille_paquets,int taille_route, int nb_simuls, int periode);
void search_efficiency(int taille_message,int taille_routes, int nb_simuls);
void marge_PALL_stochastique(int nb_routes,int taille_paquets,int taille_route, int nb_simuls, int periode_max);
void allers_random_PMLS(int nb_routes, int taille_paquets,int taille_route, int nb_simuls, int pmin,int pmax);
void stochastic_vs_PMLS(int nb_routes, int taille_paquets,int taille_route, int nb_simuls, int marge_max);
void distrib_margins_departs(int nb_routes, int taille_paquets,int taille_route,int margin_max, int nb_simuls, int periode,int nb_rand);
void tps_FPT_PALL(int nb_routes_max, int taille_paquets,int taille_route,int marge, int nb_simuls,int nb_rand,float load);