void simul(int seed,Assignment (*ptrfonction)(Graph,int,int,int),char * nom);
void test(unsigned int seed);
void simul_period(int seed,Assignment (*ptrfonction)(Graph,int,int),char * nom);
void print_distrib_margin_algo_waiting(int seed,Assignment (*ptrfonction)(Graph,int,int,int),char * nom);
void trouve_topology();
void print_distrib_margin_algo_waiting_int(int seed,int (*ptrfonction)(Graph,int,int),char * nom);
void simuldistrib(int seed);
void testcoupefpt(int seed);
void testfpt(int seed);
void simultiplexing(int seed);

void simuldescente(int seed);

void simultaboo(int seed);
void simulrecuit(int seed);
void simulfptvssto(int seed);