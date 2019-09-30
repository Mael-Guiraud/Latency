#include "structs.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "treatment.h"
#include "test.h"


Devices * devices()
{

	char str[64];
	char * file = "../../onos/Topolexample/devices";
	FILE* f = fopen(file,"r");
	if(!file)
	  {
	    fprintf(stderr,"Error opening %s .\n",file);
	    exit(32);
	  }
	int nb_dev;

	fscanf(f,"%d",&nb_dev);
	Devices * devs = malloc(sizeof(Devices)* nb_dev);
	for(int i = 0;i<nb_dev;i++)
	{
		fscanf("%d", &devs[i].id);
		fscanf("%s", &devs[i].address);
	}

	for(int i = 0;i<nb_dev;i++)
	{
		printf("id %d adress %s ", devs[i].id,devs[i].address);

	}
	fclose(f);
}

Graph links(Graph g)
{
	char * file = "../../onos/Topolexample/links";
	FILE* f = fopen(file,"r");
	if(!file)
	  {
	    fprintf(stderr,"Error opening %s .\n",file);
	    exit(32);
	  }
	int nb_links = fscanf(f,"%d");

}
Graph parseinput()
{
	Graph g;
	g.arc_pool = NULL;
	g.routes = NULL;
	g.size_routes = NULL;
	g.arc_pool_size = 0;
	int period = 0;
 	int tmax = 0;

  	for(int i=0;i<g.arc_pool_size;i++)
  	{
    	g.arc_pool[i].period_f = calloc(period,sizeof(int));
    	g.arc_pool[i].period_b = calloc(period,sizeof(int));
	}

	affiche_graph(g,period,stdout);
  	free_graph(g);
}

void testonos()
{
	devices();
}
