#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#define PERIODE 100
#define NB_ROUTES 90
#define TAILLE_ROUTES 100
#define NB_SIMUL 100

#define DEBUG 0


typedef struct{
	int *aller;
	int *retour;
	int *decalages;
	int periode;
	int nb_routes;
} entree;


void init_etoile(entree e, int taille_max)
{
	for(int i=0;i<e.nb_routes;i++)
	{
		e.decalages[i] = rand()%taille_max;
	}
	memset(e.aller,0,sizeof(int)*e.periode);
	memset(e.retour,0,sizeof(int)*e.periode);
}

void print_solution(int* aller, int *retour, int taille){
	printf("\nAller :\n");
	for(int i = 0; i < taille; i++){
		printf("%d ",aller[i]);
	}
	printf("\n Retour : \n");
	for(int i = 0; i < taille; i++){
		printf("%d ",retour[i]);
	}
}

int choix_uniforme(entree e, int route_courante){
	int cmpt = 0;
	for(int i=0; i<e.periode; i++){
		cmpt += (!e.aller[i] && !e.retour[ (i+e.decalages[route_courante])%e.periode]);
	}
	if (cmpt == 0) return -1;
	int alea = rand()%cmpt;
	cmpt = -1;
	for(int i=0; ; i++){
		cmpt += (!e.aller[i] && !e.retour[ (i+e.decalages[route_courante])%e.periode]);
		if(cmpt == alea) return i;		
	}
}

int first_fit(entree e, int route_courante){
	int i;
	for(i=0; i<e.periode && (e.aller[i] || e.retour[(i+e.decalages[route_courante])%e.periode]); i++){}
	return (i == e.periode) ? -1 : i;
}


int profit(entree e, int route_courante){

	int max_profit = -1;
	int max_pos = -1;
	for(int i=0; i<e.periode; i++){
		if(!e.aller[i] && !e.retour[(i+e.decalages[route_courante])%e.periode]){
			int temp =  0;
			for(int j = route_courante + 1; j<e.nb_routes; j++){
				temp+=  e.retour[(i + e.decalages[j])%e.periode] + 
				e.aller[(i + e.decalages[route_courante] + e.periode -e.decalages[j])%e.periode];
			}
			if(temp > max_profit){
				max_profit = temp;
				max_pos = i;
			}
		}
	}
	return max_pos;
}


//Renvoie le nombre de route que l'algo a réussi à scheduler
//en argument la règle de choix avec en argument aller, retour, decalages, periode, nombre de routes, route courante
int greedy(entree e, int (choix)(entree,int))
{
	
	int nb_routes_placees = 0;

	for(int i=0;i<e.nb_routes;i++)
	{
		int position = choix(e,i);
		if(position != -1)
		{
			e.aller[position]=1;
			e.retour[ (position+e.decalages[i])%e.periode] = 1;
			nb_routes_placees++;
		}
		
	}
	return nb_routes_placees;
}


int greedy_uniform(entree e){
	return greedy(e, choix_uniforme);
}


int greedy_first_fit(entree e){
	return greedy(e, first_fit);
}

int greedy_profit(entree e){
	return greedy(e,profit);
}

//le profit n'est pas évalué parfaitement
//il faut gérer les répétitions correctement

int greedy_advanced(entree e)
{
	int *profit_route = calloc(e.nb_routes,sizeof(int));
	int *profit_aller = calloc(e.periode,sizeof(int));
	int *profit_retour = calloc(e.periode,sizeof(int));
	int nb_routes_placees = 0;
	

	for(int i=0;i<e.nb_routes;i++)
	{
		//printf("Route %d:",i);
		int delta_profit = -e.nb_routes; //cannot be smallest thant that
		//it evalutes the difference in profit with the previous step, should be
		//positive most of the time
		int max_pos = -1;
		int max_route = -1;
		for(int route = i; route < e.nb_routes; route++){
			for(int pos = 0; pos< e.periode; pos++){
				if(!e.aller[pos] && !e.retour[(pos + e.decalages[route])%e.periode]){
					//if the route can be placed at this position
					int temp_prof = profit_aller[pos] + 
					profit_retour[(pos + e.decalages[route])%e.periode] -
					profit_route[route];
					if(temp_prof > delta_profit){
						max_pos = pos;
						max_route = route;
						delta_profit = temp_prof;
					}
				}
			}
		}
		//printf("Delta profit: %d, max pos: %d,max route: %d\n",delta_profit, max_pos,max_route);
		if(max_pos != -1)
		{
			e.aller[max_pos]=1;
			e.retour[ (max_pos+e.decalages[max_route])%e.periode] = 1;
			nb_routes_placees++;
			
			//update the profit of the routes and of the positions
			for(int pos=0; pos < e.periode;pos++){//remove one for the position
				//not profitable for route_max anymore
				profit_retour[(e.decalages[max_route] + pos)%e.periode] -= e.aller[pos];
				profit_aller[pos] -= e.retour[(e.decalages[max_route] + pos)%e.periode];
			}
			//add one because we use max_position now
			for(int route=i; route < e.nb_routes; route++){//remove one for the position
				//not profitable for route_max anymore
				profit_retour[(e.decalages[route] + max_pos)%e.periode]++;
				profit_route[route] += e.retour[(e.decalages[route] + max_pos)];
				profit_aller[(-e.decalages[route] + max_pos + e.decalages[max_route]+e.periode)%e.periode]++ ;
				profit_route[route] += e.aller[(-e.decalages[route] + max_pos + e.decalages[max_route]+e.periode)%e.periode];
			}
			//the used route is put in position i to not be used again
			int temp = e.decalages[i];
			e.decalages[i] = e.decalages[max_pos];
			e.decalages[max_pos] = temp; 
		}	
		else{
			return nb_routes_placees;
		}
	}
	//print_solution(e.aller,e.retour,e.periode);
	return nb_routes_placees;
}
//TODO: faire un truc qui optimise le profit en choissant la route à chaque étape et pas juste la
//la position et en prenant en compte la perte liée au choix de la route


double prob_set(int n, int m){
	double res = 1;
	for(int i = 0; i < m-n; i++){
		res*= ((double)(i + 2*n - m + 1 ))/((double)(n + i + 1));
	}
	//printf("%f ",res);
	return res;
}

double prob_theo(int n, int m){ //question, est-ce que faire le produit simplifie les termes ?
	double res = 1;  
	for(int i = m/2; i < n; i++){
		res *= (1-prob_set(i,m));
	}
	return res;
}


void statistique(int periode, int nb_routes, int taille_max, int nb_simul, int seed, int (algo)(entree),char* name){

	srand(seed);
	entree e;
	e.periode = PERIODE;
	e.nb_routes = NB_ROUTES;
	e.aller = malloc(sizeof(int)*e.periode);
	e.retour = malloc(sizeof(int)*e.periode);
	e.decalages = malloc(sizeof(int)*e.nb_routes);
	int success = 0;
	for(int i=0;i<nb_simul;i++){
		init_etoile(e,taille_max);
		success += (algo(e) == nb_routes);
		fprintf(stdout,"\r%d/%d",i+1,NB_SIMUL);
	}
	printf("Algo %s: %f réussite\n",name, (double)success/(double)nb_simul);
	free(e.aller); free(e.retour); free(e.decalages);
}


int main()
{
	int seed = time(NULL); 
	printf("Paramètres :\n -Periode %d\n-Nombre de routes %d\n-Taille maximum des routes %d\n-Nombre de simulations %d\n",PERIODE,NB_ROUTES,TAILLE_ROUTES,NB_SIMUL);
	statistique(PERIODE,NB_ROUTES, PERIODE,NB_SIMUL,seed,greedy_uniform,"greedy_uniform");
	printf("Proba de réussite théorique de l'algo uniforme: %f \n",prob_theo(NB_ROUTES,PERIODE));
	statistique(PERIODE,NB_ROUTES, PERIODE,NB_SIMUL,seed,greedy_first_fit,"first_fit");//ça n'est pas sur les memes entrees a cause du rand
	statistique(PERIODE,NB_ROUTES, PERIODE,NB_SIMUL,seed,greedy_profit,"profit");//ça n'est pas sur les memes entrees a cause du rand
	statistique(PERIODE,NB_ROUTES, PERIODE,NB_SIMUL,seed,greedy_advanced,"advanced_profit");//ça n'est pas sur les memes entrees a cause du rand
}