#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define LAMBDA 14

int loi_de_poisson_naif(float u)
{
	double p = exp (- LAMBDA);
	int x = 0;
	double f = p;
	do
	{
		x += 1;
		p = p*LAMBDA/x;
		f += p;
	} while (u > f);
	return x;
}


int main(){
	srand(time(NULL));
	double variance = 0.0;
	double moyenne = 0.0;

	int nb_simuls = 1000000;
	int result;
	int max = 0;
	float tirage; 
	for(int i=0;i<nb_simuls;i++)
	{
		tirage = (float)rand() / RAND_MAX ;
		result = loi_de_poisson_naif(tirage);
		if(result*5 > max)max=result*5;
		variance += (result*result);
		moyenne += result;
	}
	moyenne /= nb_simuls;


	variance /= nb_simuls;
	variance -= (moyenne*moyenne);

	printf("Variance = %f, Moyenne = %f, MAX = %d\n",variance,moyenne,max);
	return 0;
}