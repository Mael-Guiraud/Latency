#include "structs.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "treatment.h"
#include "test.h"


Devices  devices()
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
	int id; 
	fscanf(f,"%d",&nb_dev);
	Devices D;
	D.nb_devs = nb_dev;
	D.devs = malloc(sizeof(device)* nb_dev);
	for(int i = 0;i<nb_dev;i++)
	{
		fscanf(f,"%d", &id);
		D.devs[i].id = id;
		fscanf(f,"%s", str);
		strcpy(D.devs[i].address,str);
	}

	for(int i = 0;i<nb_dev;i++)
	{
		printf("id %d adress %s \n", D.devs[i].id,D.devs[i].address);

	}
	fclose(f);
	return D;
}
int match(char * str, Devices D)
{
	for(int i=0;i<D.nb_devs;i++)
	{
		if(!strcmp(str,D.devs[i].address))
			return D.devs[i].id;
	}
	return -1;
}
Links links(Devices D)
{

	char str[64];
	char * file = "../../onos/Topolexample/links";
	FILE* f = fopen(file,"r");
	if(!file)
	  {
	    fprintf(stderr,"Error opening %s .\n",file);
	    exit(32);
	  }
	int nb_links;
	int id; 
	fscanf(f,"%d",&nb_links);
	Links L;
	L.nb_links = nb_links;
	L.links = malloc(sizeof(linkk)* nb_links);
	for(int i = 0;i<nb_links;i++)
	{
		fscanf(f,"%d", &id);
		L.links[i].id = id;
		fscanf(f,"%d", &id);
		L.links[i].latency = id;
		fscanf(f,"%s", str);
		strcpy(L.links[i].src,str);
		fscanf(f,"%d", &id);
		L.links[i].src_port = id;
		L.links[i].src_id = match(str,D);
		if(L.links[i].src_id == -1)
		{
			fprintf(stderr,"Matching error in inputonos.c\n");
	   		 exit(33);
		}
		fscanf(f,"%s", str);
		strcpy(L.links[i].dst,str);
		fscanf(f,"%d", &id);
		L.links[i].dst_port = id;
		L.links[i].dst_id = match(str,D);
		if(L.links[i].dst_id == -1)
		{
			fprintf(stderr,"Matching error in inputonos.c\n");
	   		 exit(33);
		}
	}

	for(int i = 0;i<nb_links;i++)
	{
		printf("id %d src %s port %d dst %s port %d latency %d\n", L.links[i].id,L.links[i].src,L.links[i].src_port,L.links[i].dst,L.links[i].dst_port,L.links[i].latency);

	}
	fclose(f);
	return L;
}

int match_link(char* src, int portsrc, char* dst, int portdst, Links L)
{

	for(int i=0;i<L.nb_links;i++)
	{
		printf("%s / %s | %d %d | %s / %s | %d %d \n",src,L.links[i].src,portsrc , L.links[i].src_port,dst,L.links[i].dst,portdst , L.links[i].dst_port);
		if(!strcmp(src,L.links[i].src) && (portsrc == L.links[i].src_port) && !strcmp(dst,L.links[i].dst) && (portdst == L.links[i].dst_port))
			return L.links[i].id;
	}
	printf("nope\n");
	return -1;
}


Graph parseinput()
{
	

	int period = 0;								// ALLER CHERCHER LA PERIODE
 	int tmax = 0;
 	Devices D = devices();
	Links L = links(D);
	
	Graph g;
	g.nb_bbu = -1;
	g.nb_collisions = -1;
	g.kind = TREE;

	g.arc_pool_size = L.nb_links;

	g.arc_pool = malloc(sizeof(Arc)*L.nb_links);


  	for(int i=0;i<g.arc_pool_size;i++)
  	{

		g.arc_pool[i].length = L.links[i].latency;
		g.arc_pool[i].nb_routes = 0;

		g.arc_pool[i].first = L.links[i].src_id;
		g.arc_pool[i].last = L.links[i].dst_id;
		g.arc_pool[i].bbu_dest = -1;
		g.arc_pool[i].seen = 0;
  		
    	g.arc_pool[i].period_f = calloc(period,sizeof(int));
    	g.arc_pool[i].period_b = calloc(period,sizeof(int));
	}


	////PARSEUR PATH
	
	char * file = "../../onos/Topolexample/paths";
	FILE* f = fopen(file,"r");
	if(!file)
	  {
	    fprintf(stderr,"Error opening %s .\n",file);
	    exit(32);
	  }
	int nb_paths;
	int id; 
	int id_arc;
	char src[64];
	char dst[64];
	int portsrc;
	int portdst;
	fscanf(f,"%d",&nb_paths);
	g.nb_routes = nb_paths;
	g.routes = malloc(sizeof(Route*)*nb_paths);
	g.size_routes = malloc(sizeof(int)*nb_paths);


	for(int i = 0;i<nb_paths;i++)
	{
		//Id , osef c'est le même que i
		fscanf(f,"%d", &id);


		//taille en arcs
		fscanf(f,"%d", &id);
		g.routes[i]=malloc(sizeof(Route)*id);
		g.size_routes[i] = id;
		for(int j=0;j<id;j++)
		{
			fscanf(f,"%s", src);
			fscanf(f,"%d", &portsrc);
			fscanf(f,"%s", dst);
			fscanf(f,"%d", &portdst);

			id_arc = match_link(src,portsrc,dst,portdst,L);
			if(id_arc == -1)
			{
				fprintf(stderr,"Erreur de matching du lien.\n");
				exit(35);
			}
			g.routes[i][j] =  &g.arc_pool[id_arc];
			g.arc_pool[id_arc].routes_id[g.arc_pool[id_arc].nb_routes] = i;
			g.arc_pool[id_arc].nb_routes++;
		}
	
		
	}

	fclose(f);
	//////
	file = "../../onos/Topolexample/params";
	f = fopen(file,"r");
	if(!file)
	  {
	    fprintf(stderr,"Error opening %s .\n",file);
	    exit(32);
	  }

	fscanf(f,"%d",&tmax);
	fscanf(f,"%d",&period);
	fclose(f);
	
	

	affiche_graph(g,period,stdout);
	free(L.links);
	free(D.devs);
  	return g;
}
