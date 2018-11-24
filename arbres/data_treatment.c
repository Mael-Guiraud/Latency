#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void print_gnuplot(char * outputname,char ** files, int nb_files, char* title, char * xlabel, char* ylabel)
{

	char buf[64];
	sprintf(buf,"%s.gplt",outputname);
	FILE* f_GPLT = fopen(buf,"w");
	if(!f_GPLT){perror("Opening gplt file failure\n");exit(2);}

	for(int i=0;i<nb_files;i++)
	{
		if(i>0)
			fprintf(f_GPLT,"re");
		fprintf(f_GPLT,"plot '%s' title \"%s\" \n",files[i],files[i]);
	}

	
	fprintf(f_GPLT,"set term postscript color solid\n"

	"set title \"%s\"\n"
	"set xlabel \"%s\" \n"
	//"set xtics 10\n" 

	"set key bottom right \n"
	"set ylabel \"%s\"\n"
	"set output '| ps2pdf - %s'\nreplot\n",title,xlabel,ylabel,outputname);
	fclose(f_GPLT);

}