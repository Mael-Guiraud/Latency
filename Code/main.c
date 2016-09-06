#include "graph.h"

int main(){
	struct timeval trecv;
	gettimeofday(&trecv,NULL);
	srandom(trecv.tv_usec);
	clock_t debut = clock();
 

	//0 pour avoir des alea partout, 1 pour des 0 vers feuilles, 2 pour des0 vers sources
	
	//simulation(0);
	//simul_bruteforce();
	//genere_distrib();
	//genere_distrib_cumulee();
	etude_exp_bruteforce();
	//simulationsTmax();
	//simulationsWindow();
	
	
	/*printf("Star %d\n",recherche_lineaire_star(g));
	printf("Prime %d\n",recherche_lineaire_prime(g));
	TwoWayTrip t = shortest_to_longest(g);
	printf("SL %d\n",t.window_size);*/
	/*for(int i = 0;i<1000;i++)
	{
		Graphe g = topologie1(4,4,0);
		printf("%d\n",recherche_lineaire_prime(g)-recherche_lineaire_brute(g));
	}*/
	//afficheTwoWayTrip(algo_bruteforce(6,19500));
 
   //la structure clock
	//Graphe g = topologie1(4,4,0);
	//freeGraphe(g);
   
    
   printf("temps d'execution %f s\n", (double)(clock () - debut) / CLOCKS_PER_SEC);
	return 0;
}
