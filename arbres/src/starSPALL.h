
typedef struct 
{
	int val;
	int sens; //0 gauche 1 droite
} element_sjt;
void print_tab(element_sjt * tab, int taille);
int id_mobile(element_sjt * tab,int taille);
void swap_greater(element_sjt* tab, int taille,int nb);
void algo_sjt(element_sjt* tab, int taille);
element_sjt * init_sjt(int taille);
long long fact(int a);

Assignment fpt_spall(Graph * g, int P, int message_size, int tmax);
void compute_tabs(element_sjt * tab,int * m_i,int * release, int * deadline, Graph * g, int P, int message_size,int tmax);