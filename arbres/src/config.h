// CONFIG OF THE TOPOLOGY

#define MAX_LENGTH 3
#define MESSAGE_SIZE 1
#define NB_BBU 2
#define NB_COLLISIONS 3
#define NB_ROUTES_PER_FLOW 3
#define PROBA 0.5

//Mod synch or not
#define SYNCH 0

// CONFIG OF THE SIMULATION FOR ALGORITHMS WITHOUT WAITING TIME
#define LOAD_MIN 0.3
#define LOAD_MAX 1
#define LOAD_GAP 0.1


// CONFIG OF THE SIMULATIONS FOR ALGORITHMS WITH WAITING TIME
#define MARGIN_MIN 0
#define MARGIN_MAX 20
#define MARGIN_GAP 1
#define STANDARD_LOAD 0.5


//CONFIG IF WE WANT TO use some given parameters (algorithms with waiting time)
#define TMAX_MOD  0//1 activated, 0 dislabed
#define TMAX 7812
#define FIXED_PERIOD_MOD 0
#define PERIOD 9


//For star simulation
#define NB_ROUTES 7

#define NB_SIMULS 10


//openMP parallel
#define PARALLEL 0

//for logs 
#define SHOW_EVERY 100
