#include <stdlib.h>
#include "config.h"

float rand01()
{
	return (float)rand() / (float)RAND_MAX;
}

int ** random_biparti(int nb_datacenters, int nb_switches)
{
	int ** graph = malloc(sizeof(int*)*nb_datacenters);
	for(int i=0;i<nb_datacenters;i++)
	{
		graph[i] = calloc(nb_switches,sizeof(int));
		for(int j=0;j<nb_switches;j++)
		{
			graph[i][j] = (rand01() < PROBA);
		}
	}
	return graph;

}
void free_biparti(int ** graph,int nb_datacenters)
{
	for(int i=0;i<nb_datacenters;i++)
	{
		free(graph[i]);
	}
	free(graph);
}