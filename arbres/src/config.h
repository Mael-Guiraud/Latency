// CONFIG OF THE TOPOLOGY

#define MAX_LENGTH 700
#define MESSAGE_SIZE 2500
#define NB_BBU 2
#define NB_COLLISIONS 2
#define NB_ROUTES_PER_FLOW 2
#define PROBA 0.5

//Mod synch or not
#define SYNCH 1

// CONFIG OF THE SIMULATION FOR ALGORITHMS WITHOUT WAITING TIME
#define LOAD_MIN 0.5
#define LOAD_MAX 1
#define LOAD_GAP 0.1


// CONFIG OF THE SIMULATIONS FOR ALGORITHMS WITH WAITING TIME
#define MARGIN_MIN 0
#define MARGIN_MAX 15000
#define MARGIN_GAP 1000
#define STANDARD_LOAD 0.7


//CONFIG IF WE WANT TO use some given parameters (algorithms with waiting time)
#define TMAX_MOD 1 //1 activated, 0 dislabed
#define TMAX 7812
#define FIXED_PERIOD_MOD 1
#define PERIOD 19531	//BY SETTING A PERIOD, it is possible that an instance have a link with a load > 100%


#define NB_SIMULS 100
