void simul(int seed,Assignment (*ptrfonction)(Graph,int,int,int),char * nom);
void test();
void simul_period(int seed,Assignment (*ptrfonction)(Graph,int,int),char * nom);
void print_distrib_margin_algo_waiting(int seed,Assignment (*ptrfonction)(Graph,int,int,int),char * nom);
void trouve_topology();
void print_distrib_margin_algo_waiting_int(int seed,int (*ptrfonction)(Graph,int,int),char * nom);
void simuldistrib(int seed);