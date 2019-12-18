Assignment descente(Graph g, int P, int message_size,int tmax);
Assignment best_of_x(Graph g, int P, int message_size,int tmax);
int cols_check(int *P, int offset, int message_size,int per, int nb_routes);
Assignment taboo(Graph g, int P, int message_size,int nb_steps);
Assignment assignment_with_orders(Graph g, int P, int message_size,int print);
void fill_Per(int * P, int route, int offset, int message_size, int per);
int cols_check(int *P, int offset, int message_size,int per, int nb_routes);
Assignment recuit(Graph g, int P, int message_size,int param);