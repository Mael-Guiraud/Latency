#include <stdio.h> 
#include <stdlib.h>
#include <math.h>

#define taille 1000
#define nb_periodes 30

int donnes[nb_periodes][taille];
float moyennes[nb_periodes];
float en[nb_periodes];
float ep[nb_periodes];



//trié de la plus courte a la plus longue
void tri_bulles(int* tab,int t)
{
	int sorted;
	int tmp;

	for(int i=t-1;i>=1;i--)
	{
		sorted = 1;
		for(int j = 0;j<=i-1;j++)
		{
			if(tab[j+1]<tab[j])
			{
				tmp = tab[j+1];
				tab[j+1]= tab[j];
				tab[j]= tmp;
				sorted = 0;
			}
		}
		if(sorted){return;}
	}

}




void lire_fichier()
{
	FILE * f = fopen("../datas/donnes_random.data","r");
	if(!f){perror("Error opening file");exit(5);}
	int trash;
	for(int i=0;i<nb_periodes;i++)
	{
		for(int j=0;j<taille;j++)
		{
			//fscanf(f,"%d %d %d",&trash,&trash,&donnes[i][j]);
			fscanf(f,"%d %d %d",&trash,&donnes[i][j],&trash);

		}
	}
	fclose(f);

}
float moyenne(int numero_periode)
{
	float moy = 0;
	for(int i=0;i<taille;i++)
	{
		moy += donnes[numero_periode][i];
	}
	return moy/taille;
}
float ecart_type_negatif(int numero_periode)
{
	float e = 0;
	int t;
	for(int i=0;i<taille;i++)
	{
		if(donnes[numero_periode][i] < moyennes[numero_periode])
		{
			e += ((donnes[numero_periode][i]-moyennes[numero_periode])*((donnes[numero_periode][i]-moyennes[numero_periode])));
			t++;
		}
	}


	e /= t;

	return sqrt(e);
}
float ecart_type_positif(int numero_periode)
{
	float e = 0;
	int t;
	for(int i=0;i<taille;i++)
	{
		if(donnes[numero_periode][i] > moyennes[numero_periode])
		{
			e += ((donnes[numero_periode][i]-moyennes[numero_periode])*((donnes[numero_periode][i]-moyennes[numero_periode])));
			t++;
		}
	}


	e /= t;

	return sqrt(e);
}
void calcul_ecarts()
{
	FILE * f = fopen("traitement.data","w");
	lire_fichier();
	for(int i=0;i<nb_periodes;i++)
	{
		moyennes[i]=moyenne(i);
		en[i]=ecart_type_negatif(i);
		ep[i]=ecart_type_positif(i);
		fprintf(f,"%d %f %f %f \n",20000+i*1000,moyennes[i],moyennes[i]-en[i],moyennes[i]+ep[i]);
		fprintf(stdout,"%d %f %f %f \n",20000+i*1000,moyennes[i],moyennes[i]-en[i],moyennes[i]+ep[i]);
	}
	fclose(f);
}
int max(int numero_periode)
{
	int max = donnes[numero_periode][0];
	for(int i=1;i<taille;i++)
	{
		if(donnes[numero_periode][i] > max)
			max = donnes[numero_periode][i];
	}
	return max;
}


void trace_distribs(int taille_periode)
{
	char nom[64];
	sprintf(nom,"distrib_%d.data",taille_periode);
	FILE * f = fopen(nom,"w");
	lire_fichier();
	int lissage = 100;
	int numero_periode = (taille_periode-20000)/1000;


	int occurences[lissage];
	for(int i=0;i<lissage;i++)occurences[i]=0;

	int taille_intervalle = max(numero_periode)/lissage;
	for(int i=0;i<taille;i++)
	{

		occurences[donnes[numero_periode][i]/taille_intervalle]++;
	}

	for(int i=0;i<lissage;i++)fprintf(f,"%d %d \n",taille_intervalle*i,occurences[i]);
	fclose(f);

}
void quartiles()
{
	FILE * f = fopen("traitement.data","w");
	lire_fichier();
	for(int i = nb_periodes-1;i>0;i--)
	{
		tri_bulles(donnes[i],taille);
		fprintf(f,"%d %d %d %d \n",(int)(20000/(20000+(float)i*1000)*100),donnes[i][taille/4+1],donnes[i][taille/2+1],donnes[i][3*taille/4+1]);
		fprintf(stdout,"%d %d %d %d \n",(int)(20000/(20000+(float)i*1000)*100),donnes[i][taille/4+1],donnes[i][taille/2+1],donnes[i][3*taille/4+1]);
		//fprintf(f,"%d %d %d %d 0\n",(int)(20000/(20000+i*1000.0)*100),donnes[i][taille/4+1]/20 ,donnes[i][taille/2+1]/20,donnes[i][3*taille/4+1]/20);
		//fprintf(stdout,"%d %d %d %d 0\n",(int)(20000/(20000+i*1000.0)*100),donnes[i][taille/4+1]/20 ,donnes[i][taille/2+1]/20,donnes[i][3*taille/4+1]/20);

	}
	fclose(f);
}
void transfo()
{
	FILE * f = fopen("transfo.data","w");
	lire_fichier();
	for(int j=0;j<taille;j++)
	{
		for(int i = 0;i<nb_periodes;i+=5)
		{
			fprintf(f,"%d ",donnes[i][j]);
			
		}
		fprintf(f,"\n");
	}
	fclose(f);
}
void genere_plot()
{
	FILE * f = fopen("box.gplt","w");
	lire_fichier();
	int j=1;
	fprintf(f,"set style fill solid 0.25 border -1\n set style boxplot outliers pointtype 7\n set style data boxplot\n set xtics (");
	for(int i=0;i<nb_periodes;i+=5)
	{
		fprintf(f,"'%d' %d",20000+i*1000,j);
		j++;
		if(i < nb_periodes-2)
			fprintf(f, ", ");
	}
	fprintf(f,")\n plot for [i=1:%d] 'transfo.data' using (i):i notitle\n",j-1);
	fprintf(f,"set output '| ps2pdf - box.pdf'\n");
	fclose(f);
}
int nb_non_null(int numero_periode)
{
	int nb = 0;
	for(int i=0;i<taille;i++)
	{
		if(donnes[numero_periode][i])
			nb ++;
	}
	return nb;
}
float moy_non_null(int numero_periode)
{
	float m = 0.0;
	int nb=0;

	for(int i=0;i<taille;i++)
	{
		if(donnes[numero_periode][i])
		{
			m+= donnes[numero_periode][i];
			nb++ ;
			printf("%d - ",donnes[numero_periode][i] );
		}
	
	}
	printf("\n");
	return m/nb;
}
void worst()
{
	FILE * f = fopen("traitement.data","w");
	lire_fichier();
	for(int i = nb_periodes-1;i>=0;i--)
	{
		printf("%d : %d %d %f\n", i, max(i), nb_non_null(i),moy_non_null(i));
		//fprintf(f,"%d %d %d %d 0\n",(int)(20000/(20000+i*1000.0)*100),donnes[i][taille/4+1]/20 ,donnes[i][taille/2+1]/20,donnes[i][3*taille/4+1]/20);
		//fprintf(stdout,"%d %d %d %d 0\n",(int)(20000/(20000+i*1000.0)*100),donnes[i][taille/4+1]/20 ,donnes[i][taille/2+1]/20,donnes[i][3*taille/4+1]/20);

	}
	fclose(f);
}
int main()
{
	/*
	calcul_ecarts();
	//*/
	/*for(int i=20000;i<49000;i+=5000)
	trace_distribs(i);
	//*/
	quartiles();
	//worst();
	//transfo();
	//genere_plot();

}