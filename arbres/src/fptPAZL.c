#define DEBUG 0
#include "structs.h"
#include <stdio.h>
#include <stdlib.h>
#include "treatment.h"
//Pour le new bruteforce
typedef struct{
	int *next;
	int margin;
}stack;


void print_solution(int *id, int *start_slot, int *return_slot,  int size, stack *fw, stack *bw){
	printf("Partial solution of size %d:\n", size);
	printf("Forward windows  (forward margin : %d): ",fw[size].margin);
	for(int i = 0; i <= size; i++){
		printf("[%d :(%d,%d)] ",id[i], start_slot[i],fw[size].next[i]);
	}
	printf("\n");
	printf("Backward windows  (backward margin : %d): ",bw[size].margin);
	for(int i = 0; i <= size; i++){
		printf("[%d :(%d,%d)] ",id[i], return_slot[i],bw[size].next[i]);
	}
	printf("\n");
}

int collision(int message_size, int period, int slot, int *messages, int level){ //slot is the number 
	//of the slot in the period in which the message is not stacked against another one
	if(slot > period - message_size) return 1; //special case of the first message
	int i;
	for(i = 0; i < level 
		&& (slot <= messages[i] - message_size 
		|| slot >= messages[i] + message_size ); i++){}
	return (i != level);
}

int research_interval(int slot, int *messages, int level, int *min, int *max){
	//return the index of the element before the one which is inserted
	int previous = 0;
	for(int i = 1; i < level; i++){
		if(slot > messages[i] && messages[i] > *min) {
			*min = messages[i];
			previous = i;
		}
		if(messages[i] > slot && messages[i] < *max) *max = messages[i];
	}
	return previous;
}


void update_solution(int *id, int* start_slot, int *return_slot,stack *fw, stack *bw,
 int added_route, int previous_route, int slot, int route_number, int period, int message_size, 
 int min, int max, int level, int previous_index){
	//update the solution
	id[level] = added_route;
	start_slot[level] = start_slot[previous_route] + message_size;
	return_slot[level] = slot;
	//update the two stacks
	for(int i = 0; i < level; i++){
		fw[level].next[i] = (fw[level-1].next[i] <= added_route) ? added_route + 1 :  fw[level-1].next[i];
		bw[level].next[i] = (bw[level-1].next[i] <= added_route) ? added_route + 1 :  bw[level-1].next[i];
	}
	//one cannot use previous route anymore
	fw[level].next[previous_route] = route_number;

	//compute whether we can put something before or after route_added
	bw[level].next[level] = (return_slot[level] + 2*message_size > max ) ? route_number : 1;
	if(min + 2*message_size > slot) bw[level].next[previous_index] = route_number;

	//find the message after the one we are inserting in the fw_period
	max = period;
	for(int i = 1; i < level; i++){
		if(start_slot[i] > start_slot[level] && start_slot[i] < max) max = start_slot[i];
	}
	// compute if there is some room after the message we have just inserted
	fw[level].next[level] = (max - start_slot[level] < 2*message_size) ? route_number : 1;
}

int recursive_search(int *id, int*start_slot, int *return_slot, stack *fw, stack* bw, int *unused_route, int level,
 int* return_time, int route_number, int message_size, int period){ //renvoie 1 si une solution a été trouvée, 0 sinon
	//level is the current depth in the search tree, it corresponds to solutions with level+1 elements 
	//print_solution(id, start_slot, return_slot,level-1, fw, bw);
	if(level == route_number ) return 1; //we have placed all messages
	//place a message next to another one in the forward windows
	int slot, min, max, previous_index;
	for(int i = 0; i < level; i++){//go through the  messages already placed 
		for(int j = fw[level-1].next[i]; j < route_number; j++){//add a message next to the message number i in the forward windows
			if(unused_route[j]){
				slot = start_slot[i] + message_size + return_time[j];
				if (slot > period) slot -= period; //put retour in the interval modulo period
				//test whether there is a collision
				if(collision(message_size, period, slot, return_slot, level)) continue;
				//compute the min and max
				min = 0; max = period;
				previous_index = research_interval(slot,return_slot,level,&min,&max);
				//update the bw_margin and skip this partial solution if it is smaller than 0	
				bw[level].margin = bw[level -1].margin + (max - slot)/message_size 
				+ (slot - min)/message_size - (max - min)/message_size;
				if(bw[level].margin < 0) continue;
				fw[level].margin = fw[level-1].margin; // this margin does not change
				update_solution(id, start_slot, return_slot,fw, bw, j, i, slot,route_number, period, message_size, min, max, level, previous_index); //update all informations in the stacks and solutions
				unused_route[j] = 0;
				if(recursive_search(id,start_slot,return_slot,fw,bw,unused_route,level+1,return_time,route_number, message_size,period)) return 1; // we have found a solution, exit
					//we have not found a solution go one level back in the tree
				unused_route[j] = 1;
			}
		}
		//place a message next to another one in the backward windows
		//same code but the  role of fw and bw variables are exchanged
		for(int j = bw[level-1].next[i]; j < route_number; j++){//add a message next to the message number i in the forward windows
			if(unused_route[j]){
				slot = return_slot[i] + message_size - return_time[j];
				if (slot < 0) slot += period; //put retour in the interval modulo period
				//test whether there is a collision
				if(collision(message_size, period, slot, start_slot,level)) continue;
				//compute the min and max
				min = 0; max = period;
				previous_index = research_interval(slot,start_slot,level,&min,&max);
				//update the bw_margin and skip this partial solution if it is smaller than 0	
				fw[level].margin = fw[level -1].margin + (max - slot)/message_size 
				+ (slot - min)/message_size - (max - min)/message_size;
				if(fw[level].margin < 0) continue;
				bw[level].margin = bw[level-1].margin; // this margin does not change
				update_solution(id, return_slot, start_slot,bw, fw, j, i, slot,route_number, period, message_size, min, max, level, previous_index); //update all informations in the stacks and solutions
				unused_route[j] = 0;
				if(recursive_search(id,start_slot,return_slot,fw,bw,unused_route,level+1,return_time,route_number, message_size,period)) return 1; // we have found a solution, exit
					//we have not found a solution go one level back in the tree
				unused_route[j] = 1;
			}
		}
	}
	return 0;
}


Assignment search(int* return_time, int route_number,int message_size, int period){
	

	Assignment a = malloc(sizeof(struct assignment));
	a->offset_forward = calloc(route_number,sizeof(int));
	a->offset_backward = calloc(route_number,sizeof(int));
	a->waiting_time = calloc(route_number,sizeof(int));
	for(int i=0;i<route_number;i++)
	{
		a->offset_forward[i]=-1;
		a->offset_backward[i]=-1;
		a->waiting_time[i]=-1;
	}
	a->nb_routes_scheduled = 0;
	a->all_routes_scheduled = 0;
	
	 // return_time[i] = (2 * g->matrice[nbr_route][i + nbr_route + 1])%periode;
	  
	if(DEBUG)printf("Instance aléatoire :\n");//we assume that the value in return time are in [0,period[
	if(DEBUG)for(int i = 0; i < route_number; i++) printf("%d  ",return_time[i]);
	int shift = return_time[0];
	if(DEBUG)printf("\nInstance aléatoire décalée :\n");
	for (int i = 0; i < route_number; i++){//we shift the values so that the first route has return time 0
		return_time[i] -= shift;
		if (return_time[i] < 0) return_time[i] += period;
		if(DEBUG)printf("%d  ",return_time[i]);
	}
	if(DEBUG)printf("\n");
    /* Memory allocation */		
	stack *fw = malloc(sizeof(stack)*route_number);
	stack *bw = malloc(sizeof(stack)*route_number);
	int *id = malloc(route_number*sizeof(int));
  	int *start_slot = malloc(route_number*sizeof(int));
  	int *return_slot = malloc(route_number*sizeof(int));
  	int *unused_route = malloc(route_number*sizeof(int));

  	for(int i = 0; i < route_number; i++){ 
  		fw[i].next = malloc(sizeof(int)*(i+1));
  		bw[i].next = malloc(sizeof(int)*(i+1));
  		unused_route[i] = 1;
  	}
  	/* Initialization, the first route is fixed */
  	
  	fw[0].next[0] = 1;
  	bw[0].next[0] = 1;
  	fw[0].margin = period /message_size - route_number;
  	bw[0].margin = fw[0].margin;
  	id[0] = 0;
  	start_slot[0] = 0;
  	return_slot[0] = 0;
  	
  	/* Call the recursive part with the proper algorithm */
  	int return_value = recursive_search(id, start_slot, return_slot, fw, bw, unused_route, 1, return_time, route_number, message_size, period);
  		//printf("Solution trouvée \n");
  		//print_solution(id, start_slot, return_slot,route_number-1, fw, bw);
  	/* Free the memory */

  	if(return_value)
  	{
 	if(DEBUG)print_solution(id, start_slot, return_slot, route_number-1, fw,bw);
  		//printf("\n");
  	//for (int i = 0; i < route_number; i++){printf("%d %d \n",id[i],start_slot[i]);}
  	//	affiche_etoile(g);
  	for (int i = 0; i < route_number; i++){
  	
  		a->offset_forward[id[i]]=start_slot[i];
		a->offset_backward[i]=0;
		a->waiting_time[i]=0;

		a->nb_routes_scheduled++;
	
	}

	a->all_routes_scheduled = 1;
  }
  

  	free(id);
  	free(start_slot);
  	free(return_slot);
  	free(unused_route);
  	for(int i = 0; i < route_number ; i++) {
  		free(fw[i].next);
  		free(bw[i].next);
  	}
  	free(fw);
  	free(bw);
  	return a;
}

Assignment linear_search(Graph * g, int nb_routes, int P, int message_size)
{
	Assignment a = NULL;
	Assignment olda=NULL;
	for(int i=1;i<=nb_routes;i++)
	{
	
		int return_time[i];
		for(int k=0;k<i;k++)
		{

			return_time[k]=  (2*route_length_untill_arc(g,k, &g->arc_pool[nb_routes],BACKWARD))%P;
		}
		a = search(return_time, i,message_size,P);
		for(int k=0;k<i;k++)
		{
			
			
			a->offset_forward[k] =( a->offset_forward[k] +  route_length_untill_arc(g,k, &g->arc_pool[i],BACKWARD)-route_length(g,k) + P )% P;
			a->offset_backward[k] =( a->offset_forward[k] + route_length(g,k) )% P;
			
		}
		
		
		if(!a->all_routes_scheduled)
		{
			 free_assignment(a);
			
			return olda;
		}
		if(olda)
			free_assignment(olda);
		olda = a;
	
	}
	return olda;
	
}
