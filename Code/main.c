#include "graph.h"

int main(){
	struct timeval trecv;
	gettimeofday(&trecv,NULL);
	srandom(trecv.tv_usec);
	//0 pour avoir des alea partout, 1 pour des 0 vers feuilles, 2 pour des0 vers sources
	//simulation(0);

	//simulationsTmax();
	//simulationsWindow();
	//simulationsSuccess();
	algo_bruteforce(2*taille_paquet+1000);

	return 0;
}
