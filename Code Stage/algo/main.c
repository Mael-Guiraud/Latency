#include <stdio.h>
#include <stdlib.h>
#define TEMPS_TACHE 6
typedef struct element{
	int index;
	int release;
	int deadline;
	int fail;
	int scheduled;
	int begin;
} Element;

typedef struct ensemble{
	int nb_fils;
	int nb_scheduled;
	int taille;
	int begin_time;
	int date_fin;
	int** schedul;
	int* intervalles;
	Element* elems;
	struct ensemble * fils;
} Ensemble;

void afficheschedul(int ** schedul, int taille)
{
	int i;
	for(i=0;i<taille;i++)
	{
		printf("%d(%d) ",schedul[0][i],schedul[1][i]);
	}
	printf("\n");
}

void affichejobs(Element* elems, int taille)
{
	int i;
	for(i=0;i<taille;i++)
	{
		printf("%d(%d;%d)[%d] ",elems[i].index,elems[i].release,elems[i].deadline,elems[i].scheduled);
	}
	printf("\n");
}
//renvoie l'element eligible
int eligible(Element * elems,int taille, int time)
{
	int i;
	int deadline_min=9999;
	for(i=0;i<taille;i++)
	{
		if((elems[i].release <= time)&&(elems[i].scheduled == 0))//si dispo
		{ 
			if(elems[i].deadline < deadline_min)
			{
				elems[i].begin = time;
				elems[i].scheduled = 1;
				return i;
			}
		}
	}

	return eligible(elems,taille,time+1);
	
	
	
}

int is_first_job(Ensemble e, Element job)
{
	int i;
	int j;
	for(i=0;i<e.nb_fils;i++)
	{
		for(j=0;j<e.fils[i].taille;j++)
		{
			if(job.index == e.fils[i].elems[j].index)//si un job est dans un rs
			{
				return 0;
			}
		}
	}
	return 1;
}
Ensemble crisis(Ensemble e,Element job)
{
	int i;
	int counter;
	Ensemble efils;
	efils.elems = malloc(0);
	efils.taille = 0;
	int nombre_intervalle = 1;
	efils.intervalles = malloc(sizeof(int)*1);
	efils.intervalles[0] = 0;
	//pull et décompte des intervalles
	for(i=e.nb_scheduled-1;i<=0;i++)
	{
		
		if(e.elems[e.schedul[0][i]].deadline > job.deadline)//On trouve pull X
		{
			e.schedul[0][i] = -1;
			e.schedul[1][i] = -1; // on enleve l'element de l'agencement.
			
			efils.taille = counter;
			efils.begin_time = e.schedul[1][i];
			efils.nb_scheduled = 0;
			efils.schedul = malloc(sizeof(int*)*2);
			efils.schedul[0] = malloc(sizeof(int*)*counter);
			efils.schedul[1] = malloc(sizeof(int*)*counter);
			int i;
			for(i=0;i<efils.taille;i++)
			{
				efils.schedul[0][i] = -1;
				efils.schedul[1][i] = -1;
			}

		}
		e.schedul[0][i] = -1;
		e.schedul[1][i] = -1; // on enleve l'element de l'agencement.
		efils.taille++;
		//efils.elems = realloc(efils.elems,sizeof(Element)*efils.taille);
		efils.elems[efils.taille-1] = e.elems[e.schedul[0][1]];
		efils.intervalles[nombre_intervalle-1]++;
		if(is_first_job(e,e.elems[e.schedul[0][i]]))//si le job est une first job
		{
			
		}
		counter++;
	}
}

//Algo naif
Ensemble main_routine(Ensemble e)
{
	int i;
	int date = 0;
	int eligibletmp;
	affichejobs(e.elems,e.taille);
	for(i=0;i<e.taille;i++)
	{
		eligibletmp = eligible(e.elems,e.taille,date);
		if(e.elems[eligibletmp].begin+TEMPS_TACHE > e.elems[eligibletmp].deadline)//CRISIS
		{
			e.nb_fils++;
			//e.fils = realloc(sizeof(Ensemble)*e.nb_fils);
			//e.fils(nb_fils-1)=crisis(e,e.elems[eligibletmp]);
			printf("crisis sur la tache %d\n",eligibletmp);
			affichejobs(e.elems,e.taille);
			return;
		}
		else // pas crisis
		{
			e.schedul[0][i] = eligibletmp;
			e.schedul[1][i] = e.elems[eligibletmp].begin;
			e.nb_scheduled++;
		}
		date = e.elems[eligibletmp].begin + TEMPS_TACHE;
		afficheschedul(e.schedul,e.nb_scheduled);
	}
	return;
}
int main()
{
	//initialisation
	Ensemble e;
	e.nb_fils = 0;
	e.taille = 11;
	e.begin_time = 0;
	e.nb_scheduled = 0;
	e.schedul = malloc(sizeof(int*)*2);
	e.schedul[0] = malloc(sizeof(int*)*e.taille);
	e.schedul[1] = malloc(sizeof(int*)*e.taille);
	int i;
	for(i=0;i<e.taille;i++)
	{
		e.schedul[0][i] = -1;
		e.schedul[1][i] = -1;
	}
	e.intervalles = malloc(sizeof(int)*1);
	e.intervalles[0] = 11;
	e.elems = malloc(sizeof(Element)*11);
	e.fils = malloc(0);
	
	e.elems[0].index = 0;
	e.elems[0].release = 0;
	e.elems[0].deadline = 74;
	e.elems[0].fail = 0;
	
	
	e.elems[1].index = 1;
	e.elems[1].release = 21;
	e.elems[1].deadline = 46;
	e.elems[1].fail = 0;
	
	e.elems[2].index = 2;
	e.elems[2].release = 2;
	e.elems[2].deadline = 60;
	e.elems[2].fail = 0;
	
	e.elems[3].index = 3;
	e.elems[3].release = 50;
	e.elems[3].deadline = 68;
	e.elems[3].fail = 0;
	
	e.elems[4].index = 4;
	e.elems[4].release = 4;
	e.elems[4].deadline = 34;
	e.elems[4].fail = 0;
	
	e.elems[5].index = 5;
	e.elems[5].release = 10;
	e.elems[5].deadline = 36;
	e.elems[5].fail = 0;
	
	e.elems[6].index = 6;
	e.elems[6].release = 28;
	e.elems[6].deadline = 38;
	e.elems[6].fail = 0;

	e.elems[7].index = 7;
	e.elems[7].release = 54;
	e.elems[7].deadline = 62;
	e.elems[7].fail = 0;
	
	e.elems[8].index = 8;
	e.elems[8].release = 30;
	e.elems[8].deadline = 48;
	e.elems[8].fail = 0;
	
	e.elems[9].index = 9;
	e.elems[9].release = 52;
	e.elems[9].deadline = 68;
	e.elems[9].fail = 0;
	
	e.elems[10].index = 10;
	e.elems[10].release = 25;
	e.elems[10].deadline = 40;
	e.elems[10].fail = 0;
	
	for(i=0;i<e.taille;i++)
	{
		e.elems[i].scheduled = 0;
		e.elems[i].begin = -1;
	}
	
	main_routine(e);
}
