#include "structs.h"
#include "stdlib.h"
#include "limits.h"


int hash_graph(Graph g,int size)
{
	int val_reset = (int)sizeof(int)*8/5;//on utilise 5 bits pour coder la taille des routes
	int decalage = 0;
	int key=0;
	int val;
	for(int i=0;i<g.arc_pool_size;i++)
	{
		//printf("%d %d \n",decalage,val_reset);
		for(int j=0;j<g.arc_pool[i].nb_routes;j++)
		{
			val = g.arc_pool[i].routes_order_f[j];
			if(val == INT_MAX)
				val = 0;
			if(val < 0)
				val = -val;
			key ^= val<<(5*decalage);
			decalage++;
			decalage = (decalage >= val_reset)?0:decalage;
			val = g.arc_pool[i].routes_order_b[j];
			if(val == INT_MAX)
				val = 0;
			if(val < 0)
				val = -val;
			key ^= val<<(5*decalage);
			decalage++;
			decalage = (decalage >= val_reset)?0:decalage;
		}
		
	}
	return key%size; 
}