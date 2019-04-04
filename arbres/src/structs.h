typedef struct arc{
	int length;
	int nb_routes;
	int routes_id[124];
	int * period_f;
	int * period_b;
	int first;
	int last;
	int seen;
	int bbu_dest;
} Arc;

typedef Arc** Route;


typedef struct graph{
	int nb_routes;
	Arc * arc_pool;
	int arc_pool_size;
	Route * routes;
	int * size_routes;
	int nb_bbu;
	int nb_collisions;
} Graph;

struct assignment{
	int * offset_forward;
	int * offset_backward;
	int * waiting_time;
	int nb_routes_scheduled;
	int all_routes_scheduled;
};
typedef struct assignment * Assignment;

typedef enum periode_kind{
	FORWARD, BACKWARD
} Period_kind;

typedef struct preElems{
	int nb_elems;
	int *release;
	int *deadline;
} PreElems;