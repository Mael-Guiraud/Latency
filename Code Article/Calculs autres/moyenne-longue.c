#include <stdio.h> 
#include <stdlib.h>
#include <math.h>


int main()
{
	FILE * f = fopen("results_echec_longues.data","r");

	int periode;
	float brute = 0;
	float greedy = 0;	
	int b,g,t1,t2;

	int p_b = 0;
	int p_g = 0;


 	for(int i=0;i<60;i++)
 	{
 		fscanf(f,"%d %d %d %d %d ",&periode,&t1,&g,&b,&t2);

 		if(g-p_g > 0)
 		{
 			greedy += periode * (g-p_g);
 			
 			p_g = g;
 		}
 		if(b-p_b > 0)
 		{
 			brute += periode * (b-p_b);
 			p_b = b;
 		}
 		
 		
 	}
 	printf("Moyenne Greedy = %f \n Moyenne bruteforce = %f \n",greedy/100,brute/100);

}