#include "structs.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "treatment.h"
#include "test.h"
#include "greedy_without_waiting.h"
#include "greedy_waiting.h"
#include "color.h"
#include <time.h>
#include "multiplexing.h"
#include "reusePrime.h"

double mult;
Devices  devices()
{
	
	char str[64];
	char * file = "../../onos/Topolexample/devices";
	FILE* f = fopen(file,"r");
	if(!f)
	  {
	    fprintf(stderr,"Error opening %s .\n",file);
	    exit(32);
	  }
	int nb_dev;
	int id;
	int retval; 
	retval = fscanf(f,"%d",&nb_dev);
	if(retval == EOF)
	{
		printf("Error retval \n");
		exit(34);
	}
	Devices D;
	D.nb_devs = nb_dev;
	D.devs = malloc(sizeof(device)* nb_dev);
	for(int i = 0;i<nb_dev;i++)
	{
		retval = fscanf(f,"%d", &id);
		D.devs[i].id = id;
		retval = fscanf(f,"%s", str);
		strcpy(D.devs[i].address,str);
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
	if(!f)
	  {
	    fprintf(stderr,"Error opening %s .\n",file);
	    exit(32);
	  }
	int nb_links;
	int id; 
	int retval; 
	retval = fscanf(f,"%d",&nb_links);
	Links L;
	L.nb_links = nb_links;
	L.links = malloc(sizeof(linkk)* nb_links);
	for(int i = 0;i<nb_links;i++)
	{
		retval = fscanf(f,"%d", &id);
		if(retval == EOF)
		{
			printf("Error retval \n");
			exit(34);
		}
		L.links[i].id = id;
		retval = fscanf(f,"%d", &id);
		L.links[i].latency = id;
		retval = fscanf(f,"%s", str);
		strcpy(L.links[i].src,str);
		retval = fscanf(f,"%d", &id);
		L.links[i].src_port = id;
		L.links[i].src_id = match(str,D);
		if(L.links[i].src_id == -1)
		{
			fprintf(stderr,"Matching error in inputonos.c\n");
	   		 exit(33);
		}
		retval = fscanf(f,"%s", str);
		strcpy(L.links[i].dst,str);
		retval = fscanf(f,"%d", &id);
		L.links[i].dst_port = id;
		L.links[i].dst_id = match(str,D);
		if(L.links[i].dst_id == -1)
		{
			fprintf(stderr,"Matching error in inputonos.c\n");
	   		 exit(33);
		}
	}

	
	fclose(f);
	return L;
}

int match_link(char* src, int portsrc, char* dst, int portdst, Links L)
{

	for(int i=0;i<L.nb_links;i++)
	{
		//printf("%s / %s | %d %d | %s / %s | %d %d \n",src,L.links[i].src,portsrc , L.links[i].src_port,dst,L.links[i].dst,portdst , L.links[i].dst_port);
		if(!strcmp(src,L.links[i].src) && (portsrc == L.links[i].src_port) && !strcmp(dst,L.links[i].dst) && (portdst == L.links[i].dst_port))
			return L.links[i].id;
	}
//	printf("nope\n");
	return -1;
}
void print_list(int * p, int size, FILE * f)
{
	int old, current;
	old = p[0];
	fprintf(f,"\"0%d ",old);
	int cmpt = 0;
	int id;
	for(int i=1;i<size;i++)
	{
		current = p[i];
		if(old != current)
		{
			if(current == -1)
				id = 1;
			if(current > 0)
				id = current+1;
			if(current == 0)
				id = 0;
			
			fprintf(f,"%d\", \"0%d ",(int)(mult*cmpt),id);
			cmpt = 0;
		}
		old = current;
		cmpt++;
	}
	fprintf(f,"%d\"",(int)(mult*cmpt));


}



void gcl(Graph * g,int period,Devices D, Links L)
{
	char* file = "../../onos/Topolexample/gcl.json";

	int first ;
	int first_dev = 1;
	FILE * f = fopen(file,"w");
	if(!f)
	  {
	    fprintf(stderr,"Error opening %s .\n",file);
	    exit(32);
	  }	
	fprintf(f,"{\n\t\"devices\" : \n\t{\n ");

	 for(int i=0;i<D.nb_devs;i++)
	 {
	 	first = 1;
	 	if(first_dev)
	 		first_dev = 0;
	 	else
	 		fprintf(f,",\n");
	 	fprintf(f,"\t\t\"%s\" :\n\t\t{\n\t\t\t\"srest\" : \n\t\t\t[ \n",D.devs[i].address);
	 	for(int j=0;j<L.nb_links;j++)
	 	{

	 		if(!strcmp(D.devs[i].address,L.links[j].src))
	 		{
	 			
	 			if(g->arc_pool[j].first != i)
	 			{
	 				//printf("%d %d %d\n",D.devs[i].id,L.links[j].src_id,g->arc_pool[j].first);
	 				fprintf(stderr,"Une source n'a pas le bon id, ce n'est pas normal.");
	 				exit(35);
	 			}
	 			if(g->arc_pool[j].nb_routes)
	 			{
	 				if(!first)
	 				{
	 					fprintf(f,",\n");
	 					
	 				}
	 				else
	 					first = 0;
	 				
	 				fprintf(f,"\t\t\t\t{\n\t\t\t\t\t\"port\":\"%d\",\n\t\t\t\t\t\"gcl\":[",L.links[j].src_port);	
 					print_list(g->arc_pool[j].period_f,  period, f);
 					//print_list(g->arc_pool[j].period_f,  period, stdout);
 					//affiche_periode(g->arc_pool[j].period_f,  period, stdout);
 					fprintf(f,"]\n\t\t\t\t}");
	 			}
	 		}
	 		if(!strcmp(D.devs[i].address,L.links[j].dst))
	 		{
	 			
	 			if(g->arc_pool[j].last != i)
	 			{
	 				//printf("%d %d %d\n",D.devs[i].id,L.links[j].src_id,g->arc_pool[j].first);
	 				fprintf(stderr,"Une source n'a pas le bon id, ce n'est pas normal.");
	 				exit(35);
	 			}
	 			if(g->arc_pool[j].nb_routes)
	 			{
	 				if(!first)
	 				{
	 					fprintf(f,",\n");
	 					
	 				}
	 				else
	 					first = 0;
	 				
	 				fprintf(f,"\t\t\t\t{\n\t\t\t\t\t\"port\":\"%d\",\n\t\t\t\t\t\"gcl\":[",L.links[j].dst_port);	
 					print_list(g->arc_pool[j].period_b,  period, f);
 					//print_list(g->arc_pool[j].period_b,  period, stdout);
 					//affiche_periode(g->arc_pool[j].period_b,  period, stdout);
 					fprintf(f,"]\n\t\t\t\t}");
	 			}
	 		}
	 	}
	 	fprintf(f,"\n\t\t\t]\n\t\t}");
	 }
	 fprintf(f,"\n\t}\n}");

	fclose(f);
	return;
}


Graph parseinput()
{	
	mult = 1.0;
	
	int retval; 
	int period = 0;						
 	int tmax = 0;
 	int message_size = 2500;
 	char* file = "../../onos/Topolexample/params";
	FILE* f = fopen(file,"r");
	if(!f)
	  {
	    fprintf(stderr,"Error opening %s .\n",file);
	    exit(32);
	  }

	retval = fscanf(f,"%d",&tmax);
	if(retval == EOF)
	{
		printf("Error retval \n");
		exit(34);
	}
	retval = fscanf(f,"%d",&period);
	fclose(f);
	printf("\nLecture des devices...");
 	Devices D = devices();
 	printf("OK.\n");
 	printf("Lecture des Links...");
	Links L = links(D);
	printf("OK.\n");
	Graph * g;
	g->nb_bbu = -1;
	g->nb_collisions = -1;
	g->kind = TREE;

	g->arc_pool_size = L.nb_links;

	g->arc_pool = malloc(sizeof(Arc)*L.nb_links);

	printf("Lecture des Paths...");
  	for(int i=0;i<g->arc_pool_size;i++)
  	{

		g->arc_pool[i].length = L.links[i].latency;
		g->arc_pool[i].nb_routes = 0;

		g->arc_pool[i].first = L.links[i].src_id;
		g->arc_pool[i].last = L.links[i].dst_id;
		g->arc_pool[i].bbu_dest = -1;
		g->arc_pool[i].seen = 0;
  		
    	g->arc_pool[i].period_f = calloc(period,sizeof(int));
    	g->arc_pool[i].period_b = calloc(period,sizeof(int));
	}


	////PARSEUR PATH
	
	 file = "../../onos/Topolexample/paths";
	 f = fopen(file,"r");
	if(!f)
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
	retval = fscanf(f,"%d",&nb_paths);
	g->nb_routes = nb_paths;
	g->routes = malloc(sizeof(Route*)*nb_paths);
	g->size_routes = malloc(sizeof(int)*nb_paths);


	for(int i = 0;i<nb_paths;i++)
	{
		//Id , osef c'est le même que i
		retval = fscanf(f,"%d", &id);


		//taille en arcs
		retval = fscanf(f,"%d", &id);
		g->routes[i]=malloc(sizeof(Route)*id);
		g->size_routes[i] = id;
		for(int j=0;j<id;j++)
		{
			retval = fscanf(f,"%s", src);
			retval = fscanf(f,"%d", &portsrc);
			retval = fscanf(f,"%s", dst);
			retval = fscanf(f,"%d", &portdst);

			id_arc = match_link(src,portsrc,dst,portdst,L);
			if(id_arc == -1)
			{
				fprintf(stderr,"Erreur de matching du lien.\n");
				exit(35);
			}
			g->routes[i][j] =  &g->arc_pool[id_arc];
			g->arc_pool[id_arc].routes_id[g->arc_pool[id_arc].nb_routes] = i;
			g->arc_pool[id_arc].nb_routes++;
		}
	
		
	}

	fclose(f);
	//////
	printf("Ok.\n\n");

	printf("Algorithme DetNet...");
	Assignment a = Prime_all_routes(g,period,message_size,tmax);
	
	if((a->all_routes_scheduled) && (travel_time_max( g, tmax, a) != -1) )
	{
		printf(GRN "OK | " RESET);
		printf(" Travel time max = %d \n",travel_time_max( g, tmax, a));
	}
	else
	{
		printf(RED "Not OK --\n" RESET);
		fprintf(stdout,"No assignment found\n");
	}

	int seed = time(NULL);

	printf("\n --------- \n Statistical Multiplexing-> Testing the chain reaction of multiplexing ...  \n");
	printf("Fifo : \n");
	fprintf(f,"Fifo \n");
	int last_time_ellapsed =0;
	int time_ellapsed = 0;
	int nb_periods=1;
	while(1)
	{
		srand(seed);
		time_ellapsed = multiplexing(g, period, message_size, nb_periods, FIFO,tmax);
		printf("For %d period(s), the max time ellapsed is %d \n",nb_periods,time_ellapsed);
		if(time_ellapsed > (last_time_ellapsed+last_time_ellapsed/10) )
		{
			nb_periods *= 10;
			printf("This time is more than 10%% higher than the previous. \n We increase the number of periods to %d.\n",nb_periods);
			last_time_ellapsed = time_ellapsed;
		}
		else
			break;
	}
	
	if(time_ellapsed < tmax)
	{
		printf(GRN "OK\n" RESET);
	}
	else
	{
		printf(RED "Not OK --\n" RESET);
	}

	printf("\n\n Ecriture des GCL...");
	gcl(g,period,D,L);
	printf("Ok.\n");
	free(L.links);
	free(D.devs);
	//affiche_graph(g,period,stdout);
  	return g;
}
