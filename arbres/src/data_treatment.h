void print_gnuplot(char * outputname,char ** files, int nb_files, char* title, char * xlabel, char** ylabel);
void print_gnuplot_distrib(char * outputname,char ** files, int nb_files, char* title, char * xlabel, char** ylabel);
void print_graphvitz(Graph * g,char * nom);
void print_python(Graph * g);
char* sprint_periode_color(int * p, int size,char * string);
void print_assignment(Graph * g,  int p,char * path);
void print_assignment_backward(Graph * g, int p,char * path);
void print_json_arcs(Graph * g);