typedef struct arc{
	int length;
	int nb_routes;
	int routes_id[124];
	int * period_f;
	int * period_b;
} Arc;

typedef Arc** Route;


typedef struct graph{
	int nb_routes;
	Arc * arc_pool;
	Route * routes;
	int * size_routes;
} Graph;

struct assignment{
	int * offset_forward;
	int * offset_backward;
};
typedef struct assignment * Assignment;

typedef enum periode_kind{
	FORWARD, BACKWARD
} Period_kind;