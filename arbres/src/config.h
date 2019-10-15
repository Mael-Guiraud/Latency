// CONFIG OF THE TOPOLOGY

#define MAX_LENGTH 700
#define MESSAGE_SIZE 2500
#define NB_BBU 2
#define NB_COLLISIONS 3
#define NB_ROUTES_PER_FLOW 3
#define PROBA 0.5

//Mod synch or not
#define SYNCH 1

// CONFIG OF THE SIMULATION FOR ALGORITHMS WITHOUT WAITING TIME
#define LOAD_MIN 0.3
#define LOAD_MAX 1
#define LOAD_GAP 0.1


// CONFIG OF THE SIMULATIONS FOR ALGORITHMS WITH WAITING TIME
#define MARGIN_MIN 0
#define MARGIN_MAX 40000
#define MARGIN_GAP 1000
#define STANDARD_LOAD 0.9


//CONFIG IF WE WANT TO use some given parameters (algorithms with waiting time)
#define TMAX_MOD  0//1 activated, 0 dislabed
#define TMAX 7812
#define FIXED_PERIOD_MOD 0

#define PERIOD 50000	//BY SETTING A PERIOD, it is possible that an instance have a link with a load > 100%


//For star simulation
#define NB_ROUTES 7

#define NB_SIMULS 100


//openMP parallel
#define PARALLEL 1

//for logs 
#define SHOW_EVERY 100
