#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include "structs.h"
#include "color.h"

const char* COLORS[NB_COLORS] = {"blue3","brown4","chartreuse","darkorchid4","gold1","magenta1","chocolate1",
						"deepskyblue", "darkorange4", "green4","deeppink","black","gray30"  };
const char * RED_FAIL = "red";

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


void print_graphvitz(Graph g,char * nom){
	for(int i=0;i<g.arc_pool_size;i++){g.arc_pool[i].seen=0;}
	FILE* f;
	int vertex_id ;
	int previous_end ;
	int next_begin ;
	if( !(f = fopen(nom,"w")) )
	{
		perror("Opening dot file failure\n");exit(2);
	}

	fprintf(f,"graph G { \n node[shape=point]\n");
	vertex_id = 0;

	for(int i=0;i<g.nb_bbu+g.nb_collisions;i++)
	{
		if(g.kind == STAR)
		{
			
			if(i<g.nb_bbu)
			{
				g.arc_pool[i].first = 0;
				g.arc_pool[i].last = vertex_id+1;
				g.arc_pool[i].seen = 1;
				fprintf(f,"%d [shape = \"box\",label=\"%d\"]\n",vertex_id+1,i);	
				fprintf(f,"%d -- %d [label = \"%d\"]\n",0,vertex_id+1,g.arc_pool[i].length);
				vertex_id++;
			}
			else
			{
				g.arc_pool[i].last = 0;
				g.arc_pool[i].first = vertex_id+1;
				g.arc_pool[i].seen = 1;
				fprintf(f,"%d -- %d [label = \"%d\"]\n",vertex_id+1,0,g.arc_pool[i].length);
				vertex_id+=2;
			}
			
			
		}
		else
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
char* sprint_periode_color(int * bufs,int * p, int size,char * string)
{
	int old, current;
	
	old = 0;
	sprintf(string,"<");
	for(int i=0;i<size;i++)
	{
		current = p[i];
		if(old != current)
		{
			if(old == 0)
			{
				if(current == -1)
				{
					sprintf(string,"%s <font color=\"%s\">%d[(%d)%d-",string,COLORS[0],0,bufs[0],i);
				}
				else
				{
					sprintf(string,"%s <font color=\"%s\">%d[(%d)%d-",string,COLORS[current],current,bufs[current],i);
				}
			}
			else
			{
				if(current == 0)
				{
					sprintf(string,"%s %d]</font> ",string,i-1);
				}
				else
				{
					if(current == -1)
					{
						sprintf(string,"%s %d]</font>  <font color=\"%s\">%d[(%d)%d-",string,i-1,COLORS[0],0,bufs[0],i);
					}
					else
					{
						sprintf(string,"%s %d]</font>  <font color=\"%s\">%d[(%d)%d-",string,i-1,COLORS[current],current,bufs[current],i);
					}
				}
			}
			
		}
		old = current;
	}
	if(old != 0)
		sprintf(string,"%s %d]</font>",string,size-1);
	sprintf(string,"%s>",string);
	return string;
}



void print_assignment(Graph g, Assignment a, int p,char * path){
	for(int i=0;i<g.arc_pool_size;i++){g.arc_pool[i].seen=0;}
	FILE* f;
	int vertex_id ;
	int previous_end ;
	int next_begin ;
	char * str = malloc(sizeof(char)*2048);
	if( !(f = fopen(path,"w")) )
	{
		perror("Opening dot file failure\n");exit(2);
	}

	 fprintf(f,"graph G { \n node[shape=point]\n 9000 [shape =\"box\",label=\"Period %d\"] \n ",p);
	vertex_id = 0;

	for(int i=0;i<g.nb_bbu+g.nb_collisions;i++)
	{
		if(g.kind == STAR)
		{
			
			if(i<g.nb_bbu)
			{
				g.arc_pool[i].first = 0;
				g.arc_pool[i].last = vertex_id+1;
				g.arc_pool[i].seen = 1;
				fprintf(f,"%d [shape = \"box\",label=\"%d\"]\n",vertex_id+1,i);	
				fprintf(f,"%d -- %d [label = \"%d\"]\n",0,vertex_id+1,g.arc_pool[i].length);
				vertex_id++;
			}
			else
			{
				g.arc_pool[i].last = 0;
				g.arc_pool[i].first = vertex_id+1;
				g.arc_pool[i].seen = 1;
				str = sprint_periode_color(g.arc_pool[g.nb_routes].routes_delay_f,g.arc_pool[g.nb_routes].period_f,p,str);
				fprintf(f,"%d [shape = \"box\",label=%s]\n",vertex_id+1,str);	
				fprintf(f,"%d -- %d [label = \"%d\"]\n",vertex_id+1,0,g.arc_pool[i].length);
				vertex_id+=2;
			}
			
			
		}
		else
		{
			g.arc_pool[i].first = vertex_id;
			g.arc_pool[i].last = vertex_id+1;
			g.arc_pool[i].seen = 1;
			str = sprint_periode_color(g.arc_pool[g.nb_routes].routes_delay_f,g.arc_pool[i].period_f,p,str);
			fprintf(f,"%d [shape = \"box\",label=%s]\n",vertex_id,str);	
			if(i<g.nb_bbu)
			{
				fprintf(f,"%d [shape = \"box\",label=\"%d\"]\n",vertex_id+1,i);	
			}
			fprintf(f,"%d -- %d [label = \"%d\"]\n",vertex_id,vertex_id+1,g.arc_pool[i].length);
			vertex_id+=2;
		}
		
		
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
					if(a->offset_forward[i] == -1)
						sprintf(str,"%s",RED_FAIL);
					else
						sprintf(str,"%s",COLORS[i]);
						
					fprintf(f,"%d [shape = \"circle\",label=<<font color=\"%s\">%d</font><font color=\"black\">(%d)</font>>]\n",vertex_id,str,a->offset_forward[i],g.routes[i][j]->bbu_dest);
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
	free(str);
	fclose(f);

}


void print_assignment_backward(Graph g, Assignment a, int p,char * path){
	for(int i=0;i<g.arc_pool_size;i++){g.arc_pool[i].seen=0;}
	FILE* f;
	int vertex_id ;
	int previous_end ;
	int next_begin ;
	char * str = malloc(sizeof(char)*2048);
	if( !(f = fopen(path,"w")) )
	{
		perror("Opening dot file failure\n");exit(2);
	}

	 fprintf(f,"graph G { \n node[shape=point]\n 9000 [shape =\"box\",label=\"Period %d\"] \n ",p);
	vertex_id = 0;

	for(int i=0;i<g.nb_bbu+g.nb_collisions;i++)
	{

		if(g.kind == STAR)
		{
			str = sprint_periode_color(g.arc_pool[g.nb_routes].routes_delay_b,g.arc_pool[g.nb_routes].period_b,p,str);
			fprintf(f,"%d [shape = \"box\",label=%s]\n",0,str);	
			
			if(i<g.nb_bbu)
			{
				g.arc_pool[i].first = 0;
				g.arc_pool[i].last = vertex_id+1;
				g.arc_pool[i].seen = 1;
				fprintf(f,"%d [shape = \"box\",label=\"%d\"]\n",vertex_id+1,i);	
				fprintf(f,"%d -- %d [label = \"%d\"]\n",0,vertex_id+1,g.arc_pool[i].length);
				vertex_id++;
			}
			else
			{
				g.arc_pool[i].last = 0;
				g.arc_pool[i].first = vertex_id+1;
				g.arc_pool[i].seen = 1;
				fprintf(f,"%d -- %d [label = \"%d\"]\n",vertex_id+1,0,g.arc_pool[i].length);
				vertex_id+=2;
			}
			
			
		}
		else
		{
			g.arc_pool[i].first = vertex_id;
			g.arc_pool[i].last = vertex_id+1;
			g.arc_pool[i].seen = 1;
			
			
			str = sprint_periode_color(g.arc_pool[g.nb_routes].routes_delay_b,g.arc_pool[i].period_b,p,str);
			if(strcmp(str,"<>") == 0)
				fprintf(f,"%d [shape = \"point\"]\n",vertex_id+1);	
			else
				fprintf(f,"%d [shape = \"box\",label=%s]\n",vertex_id+1,str);	
			
			fprintf(f,"%d -- %d [label = \"%d\"]\n",vertex_id,vertex_id+1,g.arc_pool[i].length);
			vertex_id+=2;
		}

		
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
					if(a->offset_backward[i] == -1)
						sprintf(str,"%s",RED_FAIL);
					else
						sprintf(str,"%s",COLORS[i]);
						
					fprintf(f,"%d [shape = \"circle\",label=<<font color=\"%s\">%d</font><font color=\"black\">(%d)</font>>]\n",vertex_id,str,i,g.routes[i][j]->bbu_dest);
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
	free(str);
	fclose(f);

}


void print_json_arcs(Graph g){
	for(int i=0;i<g.arc_pool_size;i++){g.arc_pool[i].seen=0;}
	FILE* f;
	int vertex_id ;
	int arcid = 0;
	int previous_end ;
	int next_begin ;
	if( !(f = fopen("../view/arcs.json","w")) )
	{
		perror("Opening json file failure\n");exit(2);
	}

	 fprintf(f,"{ \n ");
	vertex_id = 0;

	for(int i=0;i<g.nb_bbu+g.nb_collisions;i++)
	{

		g.arc_pool[i].first = vertex_id;
		g.arc_pool[i].last = vertex_id+1;
		g.arc_pool[i].seen = 1;
		
		
		fprintf(f,"\"%d\":{ \n \"from\":%d, \n \"to\":%d, \n \"length\":%d \n }, \n  ",arcid,g.arc_pool[i].first,g.arc_pool[i].last,g.arc_pool[i].length);
		vertex_id+=2;
		arcid++;
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
				g.routes[i][j]->first = previous_end;
				fprintf(f,"\"%d\":{ \n \"from\":%d, \n \"to\":%d, \n \"length\":%d \n }, \n  ",arcid,g.routes[i][j]->first,g.routes[i][j]->last,g.routes[i][j]->length);
				
				g.routes[i][j]->last = next_begin;
				g.routes[i][j]->seen = 1;
				arcid++;
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