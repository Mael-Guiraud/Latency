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
	for(int i=0;i<g.arc_pool_size;i++){g.arc_pool[i].seen=0;}
	FILE* f;
	int vertex_id ;
	int previous_end ;
	int next_begin ;
	if( !(f = fopen("../view/view.dot","w")) )
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
		if(i<g.nb_bbu)
		{
			fprintf(f,"%d [shape = \"box\",label=\"%d\"]\n",vertex_id+1,i);	
		}
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
					fprintf(f,"%d [shape = \"circle\",label=\"%d\"]\n",vertex_id,g.routes[i][j]->bbu_dest);
					vertex_id++;

				}
				g.routes[i][j]->first = previous_end;
				fprintf(f,"%d -- %d [label = \"%d\"]\n",previous_end,next_begin,g.routes[i][j]->length);
				
				g.routes[i][j]->last = next_begin;
				g.routes[i][j]->seen = 1;
			}
			previous_end = g.routes[i][j]->last;
		}
	}



	fprintf(f,"} \n");

	fclose(f);

}


void print_python(Graph g)
{
	for(int i=0;i<g.arc_pool_size;i++){g.arc_pool[i].seen=0;}
	FILE* f;
	int vertex_id ;
	int previous_end ;
	int next_begin ;
	if( !(f = fopen("../view/topol.py","w")) )
	{
		perror("Opening py file failure\n");exit(2);
	}

	fprintf(f,"from mininet.topo import Topo \n"
	"class MyTopo( Topo ):\n"
    "	def __init__( self ):\n"
     "		Topo.__init__( self )\n");
	vertex_id = 0;
	int vertex_kind[1024];
	for(int i=0;i<1024;i++)
		vertex_kind[i] = 0;
	for(int i=0;i<g.nb_bbu+g.nb_collisions;i++)
	{

		g.arc_pool[i].first = vertex_id;
		g.arc_pool[i].last = vertex_id+1;
		g.arc_pool[i].seen = 1;
		if(i<g.nb_bbu)
		{
			fprintf(f,"		bbu%d = self.addHost( 'bbu%d' )\n",i,i);	
			fprintf(f,"		s%d = self.addSwitch( 's%d' )\n",vertex_id,vertex_id);	
			vertex_kind[vertex_id]=1;
			vertex_kind[vertex_id+1]=2;
			fprintf(f,"		self.addLink( bbu%d, s%d ) \n",i,vertex_id);
		}
		else
		{
			fprintf(f,"		s%d = self.addSwitch( 's%d' )\n",vertex_id,vertex_id);	
			fprintf(f,"		s%d = self.addSwitch( 's%d' )\n",vertex_id+1,vertex_id+1);	
			vertex_kind[vertex_id]=1;
			vertex_kind[vertex_id+1]=1;
			fprintf(f,"		self.addLink( s%d, s%d ) \n",vertex_id,vertex_id+1);
		}
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
					vertex_kind[previous_end]=3;
					fprintf(f,"		rrh%d = self.addHost( 'rrh%d' )\n",vertex_id,vertex_id);
					vertex_id++;

				}
				g.routes[i][j]->first = previous_end;
				if(vertex_kind[previous_end]==3)
					fprintf(f,"		self.addLink( rrh%d, s%d ) \n",previous_end,next_begin);
				else
					fprintf(f,"		self.addLink( s%d, s%d ) \n",previous_end,next_begin);
				g.routes[i][j]->last = next_begin;
				g.routes[i][j]->seen = 1;
			}
			previous_end = g.routes[i][j]->last;
		}
	}



	fprintf(f,"topos = { 'mytopo': ( lambda: MyTopo() ) } \n");

	fclose(f);

}