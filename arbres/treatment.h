int message_collisions(Graph g,int route,int offset,int message_size,Period_kind kind,int P);
void free_assignment(Assignment a);
void free_graph(Graph g);
void fill_period(Graph g,int route,int offset,int message_size,Period_kind kind,int P);
int route_length(Graph g,int route);
void tri_bulles(int* tab,int* ordre,int taille);
int travel_time_max(Graph g, int tmax, Assignment a);
int * load_links(Graph g);
int  load_max(Graph g);