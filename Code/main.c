#include "graph.h"

int main(){
	struct timeval trecv;
	gettimeofday(&trecv,NULL);
	srandom(trecv.tv_usec);
	clock_t debut = clock();
 

	//0 pour avoir des alea partout, 1 pour des 0 vers feuilles, 2 pour des0 vers sources
	simulation(0);

	//simulationsTmax();
	//simulationsWindow();
	//simulationsSuccess();
	
	//afficheTwoWayTrip(algo_bruteforce(6,19500));

   //la structure clock
   printf("temps d'execution %f s\n", (double)(clock () - debut) / CLOCKS_PER_SEC);
	return 0;
}
