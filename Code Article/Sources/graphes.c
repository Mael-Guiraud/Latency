/***
Copyright (c) 2018 Guiraud Maël
All rights reserved.
*///

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "struct.h"
typedef struct{
	int x; 
	int y;
} Point;
double calcul_y(double longueur, double xCentre, double yCentre, double xPt,int coef)
{
	return (coef)*sqrt(longueur*longueur - (xPt - xCentre)*(xPt - xCentre))+yCentre;	
}

Point calcul_tables_y(int nb_routes,double* table, double*table2)
{
	double millieu_x = (double)nb_routes/2 - 0.5;

	//REcherche de la plus petite longueur
	double min_lenght= INT_MAX;
	for(int i=0;i<nb_routes;i++)
	{
		if(table[i] < min_lenght)
		{
			min_lenght = table[i];
		}
		if(table2[i] < min_lenght)
		{
			min_lenght = table2[i];
		}
	}

	//Normalisation de toutes les tailles de routes 
	for(int i=0;i<nb_routes;i++)
	{
		table[i] = table[i]*millieu_x/min_lenght;
		table2[i] = table2[i]*millieu_x/min_lenght;

	}
	
	//centre du graphe
	double max = 0.0;
	for(int i=0;i<nb_routes;i++)
	{
		if(table[i]>max)
		{
			max= table[i];
		}
	}
	max = (int)max+1;

	//Calcul des y 
	for(int i=0;i<nb_routes;i++)
	{
		table[i]=calcul_y(table[i],millieu_x,max,i,-1);
		table2[i]=calcul_y(table2[i],millieu_x,max,i,1);
	}
	Point centre;
	centre.x = millieu_x;
	centre.y = max;
	return centre;
}

void print_dot(Graphe g)
{

	int nb_routes = g.N/2;
	double table[nb_routes];
	double table2[nb_routes];
	for(int i=0;i<nb_routes;i++)
	{
		table[i] = g.matrice[nb_routes][i];
		table2[i] = g.matrice[nb_routes][i+nb_routes+1];
	}
	Point centre = calcul_tables_y(nb_routes,table,table2);

	char nom[64];
	static int nb = 0;
	sprintf(nom,"../datas/graphes/graph%d.dot",nb);
	nb++;
	FILE * F = fopen(nom,"w");
	fprintf(F,"graph { \n c[pos=\"%d,%d!\"]\n",centre.x,centre.y);
	for(int i=0;i<nb_routes;i++)
	{
		fprintf(F, "t%d[pos=\"%d,%f!\"] \n s%d[pos=\"%d,%f!\"] \n",i,i,table[i],i,i,table2[i]);
	}

	for(int i=0;i<nb_routes;i++)
	{
		fprintf(F,"t%d -- c [label=\"%d\"] \n c -- s%d [label=\"%d\"] \n",i,g.matrice[nb_routes][i],i,g.matrice[nb_routes][i+nb_routes+1]);
	}
	fprintf(F, "}" );
	fclose(F);
}












