#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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