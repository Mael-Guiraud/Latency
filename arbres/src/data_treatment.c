#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

void print_gnuplot(char * outputname,char ** files, int nb_files, char* title, char * xlabel, char** ylabel)
{

	char buf[64];
	sprintf(buf,"../gnuplot/%s_sucess.gplt",outputname);
	FILE* f_GPLT = fopen(buf,"w");
	sprintf(buf,"../gnuplot/%s_nb_routes.gplt",outputname);
	FILE* f_GPLT2 = fopen(buf,"w");
	if(!f_GPLT){perror("Opening gplt file failure\n");exit(2);}
	if(!f_GPLT2){perror("Opening gplt file failure\n");exit(2);}

	for(int i=0;i<nb_files;i++)
	{
		if(i>0)
		{
			fprintf(f_GPLT,"re");
			fprintf(f_GPLT2,"re");
		}	
		fprintf(f_GPLT,"plot '../data/%s' using 1:2 title \"%s\" \n",files[i],files[i]);
		fprintf(f_GPLT2,"plot '../data/%s' using 1:3 title \"%s\" \n",files[i],files[i]);
	}

	
	fprintf(f_GPLT,"set term postscript color solid\n"

	"set title \"%s\"\n"
	"set xlabel \"%s\" \n"
	//"set xtics 10\n" 

	"set key bottom right \n"
	"set ylabel \"%s\"\n"
	"set output '| ps2pdf - ../pdfs/%s_sucess.pdf'\nreplot\n",title,xlabel,ylabel[0],outputname);
	fclose(f_GPLT);
	fprintf(f_GPLT2,"set term postscript color solid\n"

	"set title \"%s\"\n"
	"set xlabel \"%s\" \n"
	//"set xtics 10\n" 

	"set key bottom right \n"
	"set ylabel \"%s\"\n"
	"set output '| ps2pdf - ../pdfs/%s_nb_routes.pdf'\nreplot\n",title,xlabel,ylabel[1],outputname);
	fclose(f_GPLT2);

}

void print_graphvitz(Graph g){
	FILE* f;
	int vertex_id ;
	int previous_end ;
	int next_begin ;
	if( (f = fopen("../view/view.dot","w")) )
	{
		perror("Opening dot file failure\n");exit(2);
	}

	fprintf(f,"graph G { \n node[shape=point]\n");
	vertex_id = 0;

	for(int i=0;i<g.nb_bbu+g.nb_collisions;i++)
	{
		g.arc_pool[i].first = vertex_id;
		g.arc_pool[i].last = vertex_id+1;
		g.arc_pool[i].seen = 1;
		fprintf(f,"%d -- %d [label = \"%d\"]\n",vertex_id,vertex_id+1,g.arc_pool[i].length);
		vertex_id+=2;
	}

	for(int i=0;i<g.nb_routes;i++)
	{
		previous_end = -1;
		next_begin = -1;
		for(int j=0;j<g.size_routes[i];j++)
		{
			if(g.routes[i][j]->seen == 0)
			{
				if(j==(g.size_routes[i]-1))
				{
					printf("This situation is weyrd(data_treatment.c)\n");
					exit(491);
				}
				next_begin = g.routes[i][j+1]->first;
				if(previous_end == -1)
				{
					previous_end = vertex_id;
					vertex_id++;

				}
				
				fprintf(f,"%d -- %d [label = \"%d\"]\n",previous_end,next_begin,g.routes[i][j]->length);

				
				g.routes[i][j]->seen = 1;
			}
		}
	}



	fprintf(f,"} \n");

	fclose(f);

}