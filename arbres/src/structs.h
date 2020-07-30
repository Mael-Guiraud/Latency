
typedef enum periode_kind{
	FORWARD, BACKWARD, NONE
} Period_kind;


//for lists in multiplexing simulations
typedef struct elem{
	int numero_route;
	int arrival_in_queue;
	int time_elapsed;
	int deadline;
	int arc_id;
	Period_kind kind_p;
	struct elem *suiv;
	int kind_message;
} Elem;


typedef enum policy{
	FIFO, DEADLINE
} Policy;



typedef struct arc{
	int length;
	int nb_routes;
	int routes_id[128]; 
	int * period_f;
	int * period_b;
	int first;
	int last;
	int seen;
	int bbu_dest;
	//for multiplexing
	Elem * elems_f;
	Elem * elems_b;
	int state_f; //0 free, 1 used
	int state_b; //0 free, 1 used

	//pour le voisinage
	int *routes_order_f;
	int *routes_order_b;
	int *routes_delay_f;
	int *routes_delay_b;
	int contention_level;
	int pos_swap;
	int bounded;
	int id_max;

} Arc;

typedef enum event_kind{
	MESSAGE, ARC
} Event_kind;
typedef enum graph_kind{
	STAR, TREE
} Graph_kind;

typedef struct {
	int id;
	char address[64];
}device;

typedef struct{
	int nb_devs;
	device * devs;
}Devices;
typedef struct {
	int id;
	char src[64];
	int src_port;
	char  dst[64];
	int dst_port;
	int latency;
	int src_id;
	int dst_id;
}linkk;

typedef struct{
	int nb_links;
	linkk * links;
}Links;


typedef struct event{
	int date;
	Event_kind kind;
	int route;
	int deadline;
	int time_elapsed;
	Period_kind kind_p;
	int arc_id;
	
	int kind_message;
	struct event * suiv;
} Event;

typedef Arc** Route;


typedef struct graph{
	int period;
	Graph_kind kind;
	int nb_routes;
	Arc * arc_pool;
	int arc_pool_size;
	Route * routes;
	Route * contention;
	int * size_routes;
	int * nb_levels;
	int nb_bbu;
	int nb_collisions;
	int contention_level;
} Graph;

struct assignment{
	int * offset_forward;
	int * offset_backward;
	int * waiting_time;
	int time;
	int nb_routes_scheduled;
	int all_routes_scheduled;
};
typedef struct assignment * Assignment;

