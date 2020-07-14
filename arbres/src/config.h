// CONFIG OF THE TOPOLOGY

#include "nb_bbu.h"
#define MAX_LENGTH 20000

#define MESSAGE_SIZE 2500

#define NB_COLLISIONS 4

#define NB_ROUTES_PER_FLOW 1
#define PROBA 1
#define DISTRIBUTED 1

//Mod synch or not
#define SYNCH 1

// CONFIG OF THE SIMULATION FOR ALGORITHMS WITHOUT WAITING TIME
#define LOAD_MIN 0.3
#define LOAD_MAX 1
#define LOAD_GAP 0.9

// CONFIG OF THE SIMULATIONS FOR ALGORITHMS WITH WAITING TIME
#define MARGIN_MIN 8000
#define MARGIN_MAX 10000
#define MARGIN_GAP 1000
#define STANDARD_LOAD 0.8
#define LAT_GAP 100


//CONFIG IF WE WANT TO use some given parameters (algorithms with waiting time)
#define TMAX_MOD  0//1 activated, 0 dislabed
#define TMAX 7812
#define FIXED_PERIOD_MOD 0

#define PERIOD 50000	//BY SETTING A PERIOD, it is possible that an instance have a link with a load > 100%


//For star simulation
#define NB_ROUTES 8

#define NB_SIMULS 1000


//openMP parallel
#define PARALLEL 1

//for logs 
#define SHOW_EVERY 100

#define NB_POINTS 40

#define NB_COUPES 7

#define VOISINAGE 1

#define DATA_CENTER_ROUTES 0

