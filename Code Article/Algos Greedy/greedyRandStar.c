#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#define PERIODE 100
#define NB_ROUTES 100
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
	printf("\n");
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

//Pour évaluer le profit correctement, il faut gérer les répétitons,
//mais comme ce bout de profit est un invariant qui ne dépend pas de quand on 
//place la route, ne sert à rien
/*int frequence(int val, int *tab, int taille){ //calcule la fréquence du premier élément dans le tableau
	int cmpt = -1;
	for(int i = 0; i < taille; i++){
		cmpt += (val == tab[i]);
	}
	return cmpt;
}
*/

int greedy_advanced(entree e)
{
	int *profit_route = calloc(e.nb_routes,sizeof(int));
	int *profit_aller = calloc(e.periode,sizeof(int));
	int *profit_retour = calloc(e.periode,sizeof(int));
	
	int nb_routes_placees = 0;
	if(DEBUG){
		printf("Instance : \n");
		for(int i = 0; i<e.nb_routes; i++){
			printf("%d ",e.decalages[i]);
		}
		printf("\n");	
	}
	for(int i=0;i<e.nb_routes;i++)
	{
		int delta_profit = -e.nb_routes; //cannot be smallest than that
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
					profit_route[route];// + frequence(e.decalages[route],e.decalages+i, e.nb_routes -i);
					if(temp_prof > delta_profit){
						max_pos = pos;
						max_route = route;
						delta_profit = temp_prof;
					}
				}
			}
		}
		if(DEBUG){
			printf("\nDelta profit: %d, max pos: %d,max route: %d, decalage: %d\n",delta_profit, max_pos,max_route,e.decalages[max_route]);
		}
		if(max_pos != -1)
		{
			int max_pos_retour = (max_pos+e.decalages[max_route])%e.periode; 
			for(int pos=0; pos < e.periode;pos++){//remove one for the positions
				//not profitable for route_max anymore because it is fixed
				profit_retour[(e.decalages[max_route] + pos)%e.periode] -= e.aller[pos];
				profit_aller[pos] -= e.retour[(e.decalages[max_route] + pos)%e.periode];
			}
			e.aller[max_pos]=1;
			e.retour[max_pos_retour] = 1;
			nb_routes_placees++;
			//the max_route is put in position i to not be used again
			int temp = e.decalages[i];
			e.decalages[i] = e.decalages[max_route];
			e.decalages[max_route] = temp;
			temp = profit_route[i];
			profit_route[i] = profit_route[max_route];
			profit_route[max_route] = temp;

			//add one for each active position because we use max_position now
			//update the corresponding routes
			for(int route=i+1; route < e.nb_routes; route++){
				profit_retour[(e.decalages[route] + max_pos)%e.periode]++;
				profit_route[route] += e.retour[(e.decalages[route] + max_pos)%e.periode];
				profit_aller[(-e.decalages[route] + max_pos_retour + e.periode)%e.periode]++ ;
				profit_route[route] += e.aller[(-e.decalages[route] + max_pos_retour + e.periode)%e.periode];
			}
			
			//affichage des profits
				if(DEBUG){
				printf("Profit aller: \n");
				for(int i = 0; i < e.periode; i++){
					printf("%d ",profit_aller[i]);
				}
				printf("\nProfit retour: \n");
				for(int i = 0; i < e.periode; i++){
					printf("%d ",profit_retour[i]);
				}
				printf("\nProfit route: \n");
				for(int i = 0; i < e.nb_routes; i++){
					printf("%d ",profit_route[i]);
				}
			}
		}	
		else{
			return nb_routes_placees;
		}
	}
	if(DEBUG) print_solution(e.aller,e.retour,e.periode);
	return nb_routes_placees;
}

int schedule(entree e, int*offsets, int route){//schedule the route at the first possible position
	int pos = first_fit(e,route);
	if(pos != -1){
		offsets[route] = pos;
		e.aller[pos]=1;
		e.retour[ (pos+e.decalages[route])%e.periode] = 1;
		return 1;
	}
	return 0;
}

void unschedule(entree e, int *offsets, int route){//remove the route from the partial solution

	if(offsets[route] == -1) {
		printf("unscheduled une route non scheduled : %d\n",route);
		printf("Offsets");
		for(int i=0; i < e.nb_routes; i++) printf("%d ",offsets[i]);
		printf("\nDecalages");
		for(int i=0; i < e.nb_routes; i++) printf("%d ",e.decalages[i]);
		print_solution(e.aller,e.retour,e.periode);

	}
	if(route <0 || route >= e.nb_routes) printf("un probleme \n");
	e.aller[offsets[route]] = 0;
	e.retour[(offsets[route]+e.decalages[route])%e.periode] = 0;
	offsets[route] = -1;
}

void test_sol(entree e, int *offsets){//test that offsets correspond to traces
	int * aller = calloc(e.periode,sizeof(int));
	int * retour = calloc(e.periode,sizeof(int));
	//construction à partir des offsets
	for(int i = 0; i < e.nb_routes; i++){
		if(offsets[i] == -1 ) 
		{
			printf("offset à -1\n");
		}
		else{
			aller[offsets[i]]++;
			retour[(offsets[i] + e.decalages[i])%e.periode]++;
		}
	}
	for(int i = 0; i < e.periode; i++){
		if(e.aller[i] != aller[i] || e.retour[i]!= retour[i]) printf("Incohérence offset/trace \n");
	}
}

int all_fit(entree e, int *offsets){
	int res = 1;
	for(int i = 0; i < e.nb_routes; i++){
		if(offsets[i] == -1){
			res &= schedule(e,offsets,i);
		}
	}
	return res; //return wether the procedure find all solutions or not
}

int eval_pos(entree e, int pos){
	int val = 0;	
	for(int i = 0; i < e.nb_routes; i++){
		val += e.retour[(pos + e.decalages[i])%e.periode];
	}
	return val;
}

int first_unscheduled(int size, int *offsets){//select the first unscheduled route
	int route;
	for(route = 0; route < size && offsets[route]>=0; route++){}
		return route;
}

int route_from_first_period(entree e, int *offsets, int pos){
	int i;
	for( i = 0; i < e.nb_routes && offsets[i] != pos; i++){}
	return i;
}

int route_from_second_period(entree e, int *offsets, int pos){
	int i;
	for(i = 0; i < e.nb_routes &&
	 (offsets[i] == -1 ||((offsets[i] + e.decalages[i]) % e.periode) != pos); i++){}
	return i;
}

int improve_potential(entree e, int *offsets)
{
	int route = first_unscheduled(e.nb_routes,offsets);
	
	//look for the first permutation which increases the potential
	for(int i = 0; i < e.periode; i++){
		int pos_retour = (i + e.decalages[route]) % e.periode;
		if(!e.aller[i] && e.retour[pos_retour]){
			int route_to_remove = route_from_second_period(e,offsets, pos_retour);	
			if(eval_pos(e,route) > eval_pos(e,route_to_remove)){
				unschedule(e,offsets,route_to_remove);
				e.aller[i] = 1;
				e.retour[pos_retour] = 1;
				offsets[route] = i;
				return 1;//success, the potential has been improved
			}		
		}
	}
	return 0;
}

int exchange(entree e, int *offsets, int route){//try to find an offset for route
	//such that any route moved because of that can be rescheduled
	int route1,route2;
	for(int i = 0; i < e.periode; i++){//first case, empty position in the second period
		int pos_retour = (i + e.decalages[route])%e.periode;
		if(!e.retour[pos_retour]){
			route1 = route_from_first_period(e, offsets, i);
			//remove route1
			unschedule(e,offsets,route1);
			e.aller[i] = 1;
			e.retour[pos_retour]=1;
			offsets[route] = i;
			if(schedule(e,offsets,route1)){
				return 1;//success the route has been rescheduled
			}
			else{//route1 cannot be moved, we rollback the changes
				offsets[route1]= i;
				e.retour[(i + e.decalages[route1])%e.periode] = 1;
				e.retour[pos_retour] = 0;
				offsets[route] = -1;
			}
		}
	}
	for(int i = 0; i < e.periode; i++){//second case, empty position in the first period
		int pos_retour = (i + e.decalages[route])%e.periode;
		if(!e.aller[i]){
			route1 = route_from_second_period(e, offsets, pos_retour);
			//remove route1
			int temp = offsets[route1];
			unschedule(e,offsets,route1);
			e.aller[i] = 1;
			e.retour[pos_retour]=1;
			offsets[route] = i;
			if(schedule(e,offsets,route1)){
				return 1;//success the route has been rescheduled
			}
			else{//route1 cannot be moved, we rollback the changes
				offsets[route1]= temp;
				offsets[route] = -1;
				e.aller[temp] = 1;
				e.aller[i]=0;
			}
		}
	}
	for(int i = 0; i < e.periode; i++){//third case, both routes should move
		int pos_retour = (i + e.decalages[route])%e.periode;
		if(e.aller[i] && e.retour[pos_retour]){
			route1 = route_from_first_period(e, offsets, i);
			route2 = route_from_second_period(e, offsets, pos_retour);
			//remove route1 and 2
			int temp = offsets[route2];
			if(route1 == route2) continue;
			unschedule(e,offsets,route1);
			unschedule(e,offsets,route2);//unschedule ecrit à -1
			e.aller[i] = 1;
			e.retour[pos_retour]=1;
			offsets[route] = i;
			if(schedule(e,offsets,route1)){
				if(schedule(e,offsets,route2)){
					return 1;//the two routes have been rescheduled
				}
				unschedule(e,offsets,route1);
			}
			if(schedule(e,offsets,route2)){//try to schedule in the order route2 then route1
				if(schedule(e,offsets,route1)){
					return 1;//the two routes have been rescheduled
				}
				unschedule(e,offsets,route2);
			}
			//remove route and reschedule both route1 and route2 at their original position
			offsets[route] = -1;
			offsets[route1] = i;
			offsets[route2] = temp;
			e.aller[temp] = 1;
			e.retour[(i + e.decalages[route1])%e.periode] = 1;
		}
	}
	return 0;
}

int swap(entree e){
	int *offsets = malloc(sizeof(int)*e.nb_routes);
	for(int i = 0; i < e.nb_routes; i++){
		offsets[i] = -1;
	}
	while(1){
		if(all_fit(e,offsets)) {test_sol(e,offsets);return e.nb_routes;}
		while(improve_potential(e,offsets)){}//improve the potential as much as possible
		if(all_fit(e,offsets)) {test_sol(e,offsets);return e.nb_routes;}//try to add routes again now the potential has been improved
										//this can only improve the potential again
		if (!exchange(e, offsets, first_unscheduled(e.nb_routes,offsets))) return 0;//should write the number of scheduled routes
		//try to schedule a route by moving other routes, if it fails, the algorithm fails
	}
}


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


float statistique(int periode, int nb_routes, int taille_max, int nb_simul, int seed, int (algo)(entree),char* name){

	srand(seed);
	entree e;
	e.periode = PERIODE;
	e.nb_routes = nb_routes;
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
	float return_value = (double)success/(double)nb_simul;
	free(e.aller); free(e.retour); free(e.decalages);
	return return_value;
}


void print_gnuplot(char ** algos, int nb_algos)
{

	char buf[64];
	sprintf(buf,"success.gplt");
	FILE* f_GPLT = fopen(buf,"w");
	
	if(!f_GPLT){perror("Opening gplt file failure\n");exit(2);}

	for(int i=0;i<nb_algos;i++)
	{
		if(i>0)
		{
			fprintf(f_GPLT,"re");
		}	
		fprintf(f_GPLT,"plot '%s.plot' using 1:2 with lines title \"%s\" \n",algos[i],algos[i]);
	}

	
	fprintf(f_GPLT,"set term postscript color solid\n"

	//"set title \"Performance of different algorithms for PAZL tau = %d , P = %d , nbroutes = %d\"\n"
		"set notitle\n"
	"set xlabel \"Nb routes\" \n"
	//"set xtics 10\n" 

	"set key bottom left \n"
	"set ylabel \"Success rate\"\n"
	"set output '| ps2pdf - success_tau1.pdf'\nreplot\n");
	fclose(f_GPLT);
	

}
int main()
{
	int seed = time(NULL); 
	printf("Paramètres :\n -Periode %d\n-Nombre de routes %d\n-Taille maximum des routes %d\n-Nombre de simulations %d\n",PERIODE,NB_ROUTES,TAILLE_ROUTES,NB_SIMUL);
		//Toujours mettre exhaustivesearch en derniere
	int nb_algos = 5;
	char * noms[] = {"GreedyUniform","Theoric","FirstFit","Profit","Swap"};
	char buf[256];
	FILE * f[nb_algos];
	for(int i=0;i<nb_algos;i++)
	{
		sprintf(buf,"%s.plot",noms[i]);
		printf("Opening %s ...",buf);
		f[i] = fopen(buf,"w");
		if(!f[i])perror("Error while opening file\n");
		printf("OK\n");
	}
	for(int i=0;i<NB_ROUTES;i++)
	{
		printf("%d Routes \n",i);
		fprintf(f[0],"%f %f\n",i/(float)NB_ROUTES,statistique(PERIODE,i, PERIODE,NB_SIMUL,seed,greedy_uniform,"GreedyUniform")); //ça n'est pas sur les memes entrees a cause du rand
		fprintf(f[1],"%f %f\n",i/(float)NB_ROUTES,prob_theo(i,PERIODE));
		fprintf(f[2],"%f %f\n",i/(float)NB_ROUTES,statistique(PERIODE,i, PERIODE,NB_SIMUL,seed,greedy_first_fit,"FirstFit"));
		fprintf(f[3],"%f %f\n",i/(float)NB_ROUTES,statistique(PERIODE,i, PERIODE,NB_SIMUL,seed,greedy_profit,"Profit"));
		//statistique(PERIODE,NB_ROUTES, PERIODE,NB_SIMUL,seed,greedy_advanced,"advanced_profit");
		//algo bugué, ne marche pas pour 50%
		fprintf(f[4],"%f %f \n",i/(float)NB_ROUTES,statistique(PERIODE,NB_ROUTES, PERIODE,NB_SIMUL,seed,swap,"Swap"));
	}
	for(int i=0;i<nb_algos;i++)
	{
		fclose(f[i]);
	}
	print_gnuplot( noms,  nb_algos);
	return 0;
}
