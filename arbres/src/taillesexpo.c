#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int logarithme(int base, int nb)
{
	if(nb == 0)
		return 0;
	return log((double)nb) / log((double)base);
}
//renvoie la taille en bits du nombre nb en base base
int taille_bits(int base, int nb)
{
	return (int)logarithme(base,nb) +1;
}
//Renvoie le nombre "nb" sous un tableau d'int en base "base"
void chgt_base(int base, int nb, int * tab)
{
	int taille = taille_bits(base,nb);

	for(int i=0;i<taille;i++)
	{
		tab[i] = nb % base;
		nb /= base;
	}
	return;

}
//affiche l'ecriture en base "base" du nombre stocké dans un tableau d'entiers de taille "taille"
void affiche_nb(int * tab ,int taille, int base)
{
	int nb=0;
	printf("Ecriture en base %d :",base);
	for(int i=taille-1;i>=0;i--)
	{
		nb += tab[i]*pow(base,(double)i);
		printf("%d",tab[i]);
	}
	printf(": du nombre %d.\n",nb);
}

int main(int argc, char const *argv[])
{
	int base = atoi(argv[1]);
	int nb_max = atoi(argv[2]);
	int taille_max=taille_bits(base,nb_max);
	int tab[taille_max];
	//tab[0]=0;

	for(int i=0;i<nb_max;i++)
	{
		for(int j=0;j<taille_max;j++)
		{
			tab[j+1]=0;
		}
		chgt_base(base,i,tab);
		affiche_nb(tab,taille_max,base);

	}
	return 0;
}