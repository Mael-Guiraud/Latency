// FONCTIONS SUR FICHIERS
#include "graph.h"

//Ecris un graphe dans g.dot et son renversé dans gr.dot
void ecrire_fichierGraph(Graphe g)
{
	Graphe gr = renverse(g);
	creationfichierGraph(g,"results/g.dot");
	creationfichierGraph(gr,"results/gr.dot");
	
}

//Appelé par ecrire fichier graphe
void creationfichierGraph(Graphe g,char * nom){

	FILE *f;
	f=fopen(nom,"w+");
	fprintf(f,"graph noeud_%d {\n", g.vertex_number);
	fprintf(f,"node [fixedsize=true width=0.4 height=0.25];\n");
	int i,j;
	for(i=0; i <g.vertex_number;i++){
		for(j=0; j < g.vertices[i].edge_number;j++){
			fprintf(f,"%d -- %d [dir=back, label=\"%d\"]; \n",g.vertices[i].index,g.vertices[i].neighboors[j],g.vertices[i].weight[j]);
		}
	}
	fprintf(f,"}");
	fclose(f);
	
}

//Ecris les resultats données dans le fichier donné
void creationfichier(int i, int moyenne,int pire,char * nom){

	FILE *f;
	f=fopen(nom,"a");
	fprintf(f,"%d %d %d\n",i,moyenne,pire);
	fclose(f);
	
}


//Ecris dans la fenetre 
void creationfichierWindow(int* tab, int taille,char * nom){

	FILE *f;
	f=fopen(nom,"w+");

	int i;
	for(i=0; i <taille;i++){
		fprintf(f,"| %5d -(ROUTE %d)- %5d |\n",tab[i],i,tab[i]+taille_paquet);
		
	}
	fclose(f);
	
}


//ecris Lt et 2LT
void ecrire_bornesWindow(int l, int t)
{
	FILE *f;
	f=fopen("results/bornes.txt","a");
	fprintf(f,"%d %d %d \n",l,l*t,2*l*t);
	fclose(f);
}

//ecris Tmax = 0.4 ms
void ecrire_bornesTMax(int i)
{
	FILE *f;
	f=fopen("results/bornesTmax.txt","a");
	fprintf(f,"%d 7812 \n",i);
	fclose(f);
}


//Ecris les resultats données dans le fichier donné(pour Tmax)
void creationfichierTmax(int i, int moyenne,int pire,int ecart,char * nom){

	FILE *f;
	f=fopen(nom,"a");
	fprintf(f,"%d %d %d %d\n",i,moyenne,pire,ecart);
	fclose(f);
	
}




