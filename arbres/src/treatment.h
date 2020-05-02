 #include <inttypes.h>

#include <sys/time.h>
int message_no_collisions(Graph * g,int route,int offset,int message_size,Period_kind kind,int P);
void free_assignment(Assignment a);
void free_graph(Graph * g);
void fill_period(Graph * g,int route,int offset,int message_size,Period_kind kind,int P);
int route_length(Graph * g,int route);
int longest_route(Graph * g);
void tri_bulles(int* tab,int* ordre,int taille);
void tri_bulles_inverse(int* tab,int* ordre,int taille);
int route_length_untill_arc(Graph * g,int route, Arc * a,Period_kind kind);
int travel_time_max(Graph * g, int tmax, Assignment a);
int * load_links(Graph * g);
int  load_max(Graph * g);
int nb_collisions_route(Graph * g, int route);
int * sort_longest_routes_on_arc(Graph * g, Arc a);
int * routes_by_id(Arc a);
int * sort_routes_by_collisions(Graph * g, Arc a);
void reset_periods(Graph * g, int P);
int * routes_sorted_lenght_arcs_bbu(Graph * g);

void convert_graph_order(Graph * g, int P);
int logarithme(int base, int nb);
int taille_bits(int base, int nb);
void chgt_base(int base, int nb, int * tab);
int mod(int a, int b);
int route_length_with_buffers(Graph * g,int route);
int travel_time_max_buffers(Graph * g);
int route_length_with_buffers_forward(Graph * g,int route);
void tri_bulles_classique_croissant(int* tab,int taille);
int verifie_solution(Graph * g,int message_size);
int route_length_untill_arc_without_delay(Graph * g,int route, Arc * a,Period_kind kind);
void tri_bulles_classique_decroissant(int* tab,int taille);

void reinit_delays(Graph * g);
double time_diff(struct timeval tv1, struct timeval tv2);