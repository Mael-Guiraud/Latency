#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#define PERIODE 50
#define NB_ROUTES 45
#define TAILLE_ROUTES 100
#define NB_SIMUL 10000

#define DEBUG 0

void affiche_solution(int * p, int * p2, int taille,int nb_routes_placees,int nb_routes)
{
	printf("\n Solution avec %d routes placées sur %d : \n",nb_routes_placees,nb_routes);
	printf("Aller : [");
	for(int i=0;i<taille-1;i++){
		printf("%d,",p[i]);
	}
	printf("%d]\n",p[taille-1]);
	printf("Retour : [");
	for(int i=0;i<taille-1;i++){
		printf("%d,",p2[i]);
	}
	printf("%d]\n",p2[taille-1]);
}
void affiche_star(int * decalages, int nb_routes)
{
	printf("Décalages \n");
	for(int i=0;i<nb_routes;i++){
		printf("Route %d: Décalage = %d\n",i,decalages[i]);
	}
	printf("\n");
}
//remplit le tableau dispo avec les indices dans p(periode à l'aller) des places disponibles pour le message
//renvoie le nombre de places dispo (donc la taille utile du tableau dispo)
int compte_places_dispo(int *dispo,int * p, int * p2, int decalage, int taille,int periode)
{
	int nb_places_libres = 0;
	for(int i=0;i<periode;i++)
	{
		if(!p[i] && !p2[(i+decalage) % periode])
			{
				dispo[nb_places_libres]=i;
				nb_places_libres++;
			}
	}
	return nb_places_libres;
}
//renvoie l'id dans la periode à l'aller de la place choisie au hasard
//renvoie -1 si il n'y a aucune place disponible
int random_position(int * p, int * p2, int decalage, int taille,int periode)
{
	//init du tableau dispo
	int dispo[taille];
	memset(dispo,0,sizeof(int)*taille);
	//compte le nombre de places libres et met leurs indices dans les premieres case du tableau dispo
	int nb_places_libres = compte_places_dispo(dispo,p,p2,decalage,taille,periode);

	//Si aucune place libre, ça echoue
	if(nb_places_libres == 0)
		return -1;

	//Choisi un indice au hasard dans ceux disponibles
	int numero_gagnant = rand()%nb_places_libres;

	//renvoie l'indice dans p de la place choisie
	return dispo[numero_gagnant];
}

int* init_etoile(int nb_routes, int taille_max)
{
	int * decalages = NULL;
	assert(decalages = (int*)malloc(sizeof(int)*nb_routes));

	for(int i=0;i<nb_routes;i++)
	{
		decalages[i] = rand()%taille_max;
	}
	return decalages;
}
//Renvoie le nombre de route que l'algo à reussi à scheduler
int greedy_random_star(int periode, int nb_routes, int taille_max)
{
	int aller[periode];
	int retour[periode];
	memset(aller,0,sizeof(int)*periode);
	memset(retour,0,sizeof(int)*periode);
	int * decalages = init_etoile(nb_routes,taille_max);


	int position_tiree;
	int nb_routes_placees = 0;
	for(int i=0;i<nb_routes;i++)
	{
		position_tiree = random_position(aller,retour,decalages[i],nb_routes,periode);
		if(position_tiree != -1)
		{
			aller[position_tiree]=1;
			retour[ (position_tiree+decalages[i])%periode] = 1;
			nb_routes_placees++;
		}
		
	}
	if(DEBUG && nb_routes_placees < nb_routes)
	{
		affiche_star(decalages,nb_routes);
		affiche_solution(aller,retour,periode,nb_routes_placees,nb_routes);
	}
	free(decalages);
	return nb_routes_placees;
}

double prob_set(int n, int m){
	double res = 1;
	for(int i = 0; i < m-n; i++){
		res*= ((double)(i + 2*n - m + 1 ))/((double)(n + i + 1));
	}
	return res;
}

double prob_theo(int n, int m){
	double res = 0;  
	for(int i = m/2; i < n; i++){
		res += (1-res)*prob_set(i,m);
	}
	return res;
}

int main()
{
	srand(time(NULL));
	printf("Paramètres :\n -Periode %d\n-Nombre de routes %d\n-Taille maximum des routes %d\n-Nombre de simulations %d\n",PERIODE,NB_ROUTES,TAILLE_ROUTES,NB_SIMUL);
	int cmpt_sucess = 0;
	long long moy_routes = 0;
	int val;
	for(int i=0;i<NB_SIMUL;i++)
	{
		val = greedy_random_star(PERIODE,NB_ROUTES,TAILLE_ROUTES);
		if(val == NB_ROUTES)
		{
			cmpt_sucess++;
		}
		moy_routes += val;
		fprintf(stdout,"\r%d/%d",i+1,NB_SIMUL);
	}
	printf("\nL'algorithme à réussi à placer toutes les routes %d fois sur %d (%lld routes placées en moyenne).\n",cmpt_sucess,NB_SIMUL,moy_routes/NB_SIMUL);
	printf("Proba d'échec empirique : %f, théorique : %f\n", 1 - (cmpt_sucess/ (double)NB_SIMUL), prob_theo(NB_ROUTES,PERIODE));
}