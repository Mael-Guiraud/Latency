#include <stdio.h>
#include <stdlib.h>
#define TEMPS_TACHE 6
typedef struct element{
	int index;
	int release;
	int deadline;
	struct element * next;
} Element;

typedef struct ensemble{
	int numero_element; // vaut -1 si c'est un ensemble
	int temps_depart;
	struct ensemble * filsG;
	struct ensemble * frereG;
	struct ensemble * frereD;
} Ensemble;

int max(int a, int b)
{
	if(a>b)
		return a;
	return b;
}


	




void affiche_ensemble(Ensemble * ens)
{
	//affiche_1_ensemble(ens);
	if(ens == NULL)
	{
		printf("ENSEMBLE VIDE");
		return;
	}
	if(ens->numero_element == -1) // si c'est un ensemble
	{
		//si c'est un ensemble, par définition il a forcement un fils gauche
		//qui est soit un élément, soit un ensemble, donc appel recursif sur le fils gauche
		printf("Sous ensemble{ ");
		affiche_ensemble(ens->filsG);
		printf("}");
		
	}
	else //si c'est un élément
	{
		printf("%d(%d) ",ens->numero_element,ens->temps_depart);
	}
	ens = ens->frereD;
	while(ens)
	{
		if(ens->numero_element == -1)
		{
			affiche_ensemble(ens);
			
		}
		else
			printf("%d(%d) ",ens->numero_element,ens->temps_depart);
		ens = ens->frereD;
	}
	
}
int main()
{
	Ensemble * e = malloc(sizeof(Ensemble));
	Ensemble * e2 = malloc(sizeof(Ensemble));
	Ensemble * efils = malloc(sizeof(Ensemble));
	Ensemble * efilsfrere = malloc(sizeof(Ensemble));
	
	e->numero_element = -1;
	e->temps_depart = 0;
	e->filsG = efils;
	e->frereD = e2;
	e->frereG = NULL;
	e2->numero_element = 2;
	e2->temps_depart = 12;
	e2->filsG = NULL;
	e2->frereD = NULL;
	e2->frereG = e;
	efils->numero_element = 3;
	efils->temps_depart = 0;
	efils->filsG = NULL;
	efils->frereD = efilsfrere;
	efils->frereG = NULL;
	efilsfrere->numero_element = 5;
	efilsfrere->temps_depart = 6;
	efilsfrere->filsG = NULL;
	efilsfrere->frereD = NULL;
	efilsfrere->frereG = efils;
	affiche_ensemble(e);printf("\n");
	affiche_ensemble(efils);printf("\n");
	affiche_ensemble(efilsfrere);printf("\n");
}
