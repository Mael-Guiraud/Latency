#include <stdio.h> 
#include <stdlib.h>
#include <math.h>

int main()
{
	
	FILE * f = fopen("nb.data","w");
	for(int i=1;i<257;i++)fprintf(f,"%d ",i);
	fclose(f);

}
