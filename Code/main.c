#include "graph.h"

int main(){
	struct timeval trecv;
	gettimeofday(&trecv,NULL);
	srandom(trecv.tv_usec);
	//0 pour avoir des alea partout, 1 pour des 0 vers feuilles, 2 pour des0 vers sources
	//simulation(1);
	//simulations();
	algo_bruteforce(7500);

	return 0;
}
