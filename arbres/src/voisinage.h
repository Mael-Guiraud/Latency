int descente(Graph * g, int P, int message_size,int tmax);
int best_of_x(Graph * g, int P, int message_size,int tmax);
int cols_check(int *P, int offset, int message_size,int per, int nb_routes);
int taboo(Graph * g, int P, int message_size,int nb_steps);
int assignment_with_orders(Graph * g, int P, int message_size,int print);
void fill_Per(int * P, int route, int offset, int message_size, int per);
int cols_check(int *P, int offset, int message_size,int per, int nb_routes);
int recuit(Graph * g, int P, int message_size,int param);
int assignment_with_orders_vois1FPT(Graph* g, int P, int message_size, int print);
int assignment_with_orders_vois1(Graph * g, int P, int message_size, int print);
int assignOneArc(Graph * g,int arcid, Period_kind kind, int message_size, int P,int print);
void reinit_delays(Graph * g);
typedef struct{
	int delay;
	int new_offset;
} retval;
retval calcul_delay(int begin,int offset,int P, int r_t,int message_size,int bool_p);