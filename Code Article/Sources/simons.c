/***
Copyright (c) 2018 Guiraud Maël
All rights reserved.
*///

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <assert.h>
#include "scheduling.h"
#include "struct.h"
#include "operations.h"
#include "random_inters.h"
 
typedef struct element{
	int index;
	int release;
	int deadline;
	struct element * next;
} Element;
extern int ALGO_JOEL;
typedef struct ensemble{
	int numero_element; // vaut -1 si c'est un ensemble
	int temps_depart;
	struct ensemble * filsG;
	struct ensemble * frereG;
	struct ensemble * frereD;
} Ensemble;

int* wrapper_fun(Taskgroup tg);

Ensemble * crisis(Ensemble * ens,Element * crisise, Element * elemspere,Element * touselems, int taille_paquet,int periode);
Ensemble * init_ensemble(){return NULL;}
Element * init_element(){return NULL;}

void decaler_release(int * release, int* deadline, int periode, int premier,int nbr_route,int taille_paquet);

/** Fonctions d'affichage pour simons *


*****************/

void affiche_ensemble_unique(Ensemble * ens)
{
	if(!ens)
	{
		printf("VIDE\n");return;
	}
	printf("(%p) num %d / date %d /fils g %p /frereG %p /frereD %p\n",ens,ens->numero_element,ens->temps_depart,ens->filsG,ens->frereG,ens->frereD);
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
		printf("Sous ensemble(%d){ ",ens->temps_depart);
		affiche_ensemble(ens->filsG);
		printf("} ");
		
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
			return;
		}
		else
		{
			printf("%d(%d) ",ens->numero_element,ens->temps_depart);
		}
		ens = ens->frereD;
	}
	
}




Element * ajoute_elemt(Element * elem,int index, int release, int deadline)
{
	Element * tmp = elem;
	Element * new = malloc(sizeof(Element));
	new->index = index;
	new->release = release;
	new->deadline = deadline;
	new->next = NULL;
	if(elem == NULL)//0 element
	{
		return new;
	}

	//test pour l'element 1
	if(elem->release > release)//inser avant
	{
		new->next = elem;
		return new;
	}

	while(elem->next)
	{
		
		if(elem->next->release > release)//inser avant
		{
			new->next = elem->next;
			elem->next = new;
			return tmp;
		}
		elem = elem->next;
	}
	elem->next = new;
	return tmp;
}

Element * ajoute_elemt_fin(Element * elem,int index, int release, int deadline)
{
	Element * tmp = elem;
	Element * new = malloc(sizeof(Element));
	new->index = index;
	new->release = release;
	new->deadline = deadline;
	new->next = NULL;
	if(elem == NULL)//0 element
	{
		return new;
	}
	while(elem->next)
	{
		
		elem = elem->next;
	}
	elem->next = new;
	return tmp;
}


Ensemble * cree_ensemble(int numero, int temps)
{
	Ensemble * ens = malloc(sizeof(Ensemble));
	ens->numero_element = numero;
	ens->temps_depart = temps;
	ens->filsG = NULL;
	ens->frereG = NULL;
	ens->frereD = NULL;
	return ens;
}

int compte_elems(Element * l)
{
	if(l==NULL)return 0;
	int compteur = 1;
	while(l->next)
	{
		compteur++;
		l = l->next;
	}
	return compteur;
}

void recalculer_deadlines(Element * elems, int deadline_periode )
{
	while(elems)
	{
		if(elems->deadline > deadline_periode)
			elems->deadline = deadline_periode;
		elems = elems->next;
	}
}
Element* tri_elems(Element * l)
{
	int taille = compte_elems(l);
	if(taille < 2)return l;
	int i,j;
	Element* debut = l;
	Element* tmp;
	for(i=taille-1;i>0;i--)
	{
		l = debut;
		if(l->release > l->next->release)
		{
			tmp = l->next;
			l->next = l->next->next;
			tmp->next = l;
			debut = tmp;
			l=debut;
		}
		for(j=0;j<i-1;j++)
		{
			if(l->next->release > l->next->next->release)
			{
				tmp = l->next;
				l->next = l->next->next;
				tmp->next = l->next->next;
				l->next->next = tmp;
			}
			l = l->next;
		}
	}
	return debut;
}


Element* retire_element_i(Element * elem, int index)
{
	if(elem == NULL)
	{
		printf("aucun element");
		return NULL;
	}
	Element * tmp;
	Element * debut = elem;
	if(elem->index == index)//cas du premier element
	{
		tmp = elem;
		debut = elem->next;
		if(tmp)
			free(tmp);
		return debut;
	}
	while(elem->next)
	{
		if(elem->next->index == index)
		{
			tmp = elem->next;
			elem->next = elem->next->next;
			if(tmp)
				free(tmp);
			return debut;
		}
		elem = elem->next;
	}
	return debut;
}
Element * get_element_i(Element * elem, int index)
{
	while(elem)
	{
		if(elem->index == index)
		{
			//printf("%d %d\n",elem->index,index);
			return elem;
		}
		elem = elem->next;
	}
	return NULL;
}
void freeelems(Element * elem)
{
	
	Element * tmp;
	while(elem)
	{
		tmp = elem;
		elem=elem->next;
		if(tmp)
			free(tmp);
	}
}
void affichejobs(Element* elem)
{
	if(!elem)
		printf("aucun element");
	//printf("Elements\n");
	while(elem)
	{
		printf("%d(%d,%d),  ",elem->index, elem->release, elem->deadline); 
		elem = elem->next;
	}
	printf("\n");
}
Element * cpy_elems(Element * elems)
{
	Element * new = init_element();
	while(elems)
	{
		new = ajoute_elemt(new,elems->index, elems->release,elems->deadline);
		elems = elems->next;
	}
	return new;
}
int date_fin(Ensemble * ens,int taille_paquet)
{
	while(ens->frereD)
	{
		ens = ens->frereD;
	}
	//affiche_ensemble_unique(ens);
	if(ens->numero_element == -1)
	{
		return date_fin(ens->filsG,taille_paquet);
	}
	else
	{
		return ens->temps_depart+taille_paquet;
	}
}

//renvoie l'element eligible
int eligible(Element * elems, int time)
{

	Element * debut =elems;
	if(elems == NULL)
		return -1;
	//printf("pointeur envoyé = %p\n",elems);
	int deadline_min=INT_MAX;
	int elu = -1;
	while(elems)
	{
		if(elems->release <= time)//si dispo
		{ 
			if(elems->deadline < deadline_min)
			{
				deadline_min = elems->deadline;
				elu = elems->index;
			}

		}
		elems = elems->next;
	}
	if(elu != -1)
	{
		
		return elu;
	}
	else
	{
		//Ici, on saute dans la backward periode, on doit regarder si le premier message est le seul a revenir a sa date
		elems = debut;
		Element * retour = elems;
		while(elems)
		{
			if(elems->release > retour->release)
			{
				break;
			}
			//sinon, c'est =, on compare donc les deadlines
			if(elems->deadline < retour->deadline)
			{
				retour = elems;
			}
			elems = elems->next;
		}


		return retour->index;//la liste est triée par release time, donc si on ne trouve pas d'eligible, on saute direct au premier release
	}


}
void libereens(Ensemble * ens)
{

	//printf("entree dans la fonction libere\n");
	if(ens)
	{
		if(ens->numero_element == -1)
		{
			//printf("On libere l'ensemble a la date %d",ens->temps_depart);
			libereens(ens->filsG);
		}
		//printf("On libere l'ensemble a droite de %d \n",ens->numero_element );
		libereens(ens->frereD);
		free(ens);
		ens = NULL;
	}
}
Ensemble * cpyens(Ensemble * ens)
{
	if(ens == NULL)
		return NULL;
	Ensemble * cpy = NULL;
	cpy = cree_ensemble(ens->numero_element,ens->temps_depart);
	if(ens->numero_element == -1) // si c'est un ensemble
	{
		cpy->filsG = cpyens(ens->filsG);
	}
	cpy->frereD = cpyens(ens->frereD);
	if(cpy->frereD)
		cpy->frereD->frereG = cpy;
	return cpy;
}
Ensemble * invade(Ensemble * ens,Element * touselems,int depart,int taille_paquet,int periode)
{

	Ensemble * ens2 = NULL;
	Ensemble * tmp = NULL;
	
	//printf("ENTREE DANS LA FONCTION INVADE date %d-------------------------------\n",depart);
	//affiche_ensemble(ens);printf("\n");
	//on se place a droite de l'ensemble qui a crisis
	while(ens->frereD)
	{
		ens = ens->frereD;
	}
	Element * elems = init_element();
	Element * elemtmp=NULL;
	int date = depart;
	//on backtrack jusqu'a la fin
	while(ens)
	{
		if(ens->numero_element != -1) // si c'est un element
		{
			elemtmp = get_element_i(touselems,ens->numero_element);
			if(ens2 == NULL)//initialisation
			{
				ens2 = cree_ensemble(-2,-1);
			}
			else
			{
				tmp = ens2;
				ens2->frereG = cree_ensemble(-2,-1);
				ens2 = ens2->frereG;
				ens2->frereD = tmp;
			}
			elems=ajoute_elemt(elems,elemtmp->index,elemtmp->release,elemtmp->deadline);
			//printf("Ajout de %d dans elems \n",elemtmp->index);
			//affichejobs(elems);
		}

		else //si c'est un ensemble
		{
			if(ens2== NULL)
			{
				ens2 = cpyens(ens);
			}
			else
			{
				ens2->frereG = cpyens(ens);
				ens2->frereG->frereD = ens2;
				ens2 = ens2->frereG;
			}
		}
		if(ens->frereG)//on regarde si on est pas au debut de l'ensemble
		{
			
			//printf("On ajoute ");affiche_ensemble(ens);printf("au r.s\n");
			ens = ens->frereG;
			libereens(ens->frereD);
			ens->frereD = NULL;
		}
		else
		{
			//printf("On ajoute ");affiche_ensemble(ens);printf("au r.s\n");
			libereens(ens);
			ens = NULL;
		}
	}
	//affiche_ensemble(ens2);printf(" R.S.\n");
	//affichejobs(elems);
	
	Ensemble* ens3 = NULL;
	Ensemble* ens4 = NULL;
	int i;

	while(ens2)//parcours de ens2
	{
		if(ens2->numero_element == -2)//si on attend un element
		{
			
			i = eligible(elems,date);
			if(i==-1)
			{
				//printf("Failure 1\n");
				libereens(ens2);
			

				libereens(ens4);
				freeelems(elems);
				return NULL;
			}
			elemtmp = get_element_i(touselems,i);
			if(elemtmp == NULL)
			{
				//printf("Failure 0\n");
				libereens(ens2);
				libereens(ens4);
				freeelems(elems);
				return NULL;
			}
			
			date = max(date,elemtmp->release);
			if(date+taille_paquet > elemtmp->deadline)//CRISIS
			{
				//printf("Crisis(invade) sur la tache %d\n",elemtmp->index);
				Element * crisisrec = ajoute_elemt(NULL,elemtmp->index,elemtmp->release,elemtmp->deadline);
				elems=retire_element_i(elems,crisisrec->index);
				if(!elems)
					elems = ajoute_elemt(elems,99999,99999,999999);
				ens3 = crisis(ens3,crisisrec,elems,touselems,taille_paquet,periode);
				freeelems(crisisrec);
				//printf("-------------\n\n\n");
				//affichejobs(elems);
				//affiche_ensemble(ens2);		printf("\n");
				//affiche_ensemble(ens3);		
				//printf("-------------\n\n\n");		
				if(ens3 == NULL)
				{
					//printf(" NO PULL(%d) FOUND(invade)\n",elemtmp->index);
					libereens(ens2);
					freeelems(elems);
					return NULL;
				}
				
				while(ens3->frereD)
					ens3 = ens3->frereD;
				//affiche_ensemble(ensembletmp);printf("Elemenent le plus a droite\n");
				date = date_fin(ens3,taille_paquet);
				
				elems=tri_elems(elems);
				
				//rajouter un element pour rescheduler PULL(X) apres la crisis dans l'invasion
				if(ens2 == NULL)//initialisation
				{
					ens2 = cree_ensemble(-2,-1);
				}
				else
				{
					tmp = ens2;
					ens2->frereG = cree_ensemble(-2,-1);
					ens2 = ens2->frereG;
					ens2->frereD = tmp;
				}
			}
			else// PAS DE CRISE
			{
				
				if(ens3 == NULL)//premier element de ens3
				{
					ens3=cree_ensemble(elemtmp->index,date);
					ens4 = ens3;
					
				}
				else
				{
					
					ens3->frereD=cree_ensemble(elemtmp->index,date);
					ens3->frereD->frereG = ens3;
					ens3 = ens3->frereD;
				}
				elems= retire_element_i(elems,elemtmp->index);
				//printf("on Schedule %d a la date %d et on le retire de elem",elemtmp->index,date);
				date+=taille_paquet;
				
				if(ens2->frereD)//Si il y a un prochain element
				{
					if(ens2->frereD->numero_element == -1)//et que c'est un ensemble
					{
						if(date+taille_paquet>=ens2->frereD->temps_depart)//ET que ca INVADE
						{
							//printf("%d INVADE date %d\n",elemtmp->index,date);
							ens2->frereD->filsG = invade(ens2->frereD->filsG,touselems,date+taille_paquet,taille_paquet,periode);
							if(ens2->frereD->filsG)
								ens2->frereD->temps_depart = ens2->frereD->filsG->temps_depart;
							else
							{
								libereens(ens2);
								libereens(ens4);
								freeelems(elems);

								return NULL;
							}
							
						}
					}
				}
			}
			//affiche_ensemble(ens3);printf(" Element\n");
		}
		else//si c'est un ensemble
		{
			if(date+taille_paquet>=ens2->temps_depart)// INVADE Si besoin
			{
				//j'ai modifié ici

				ens2->filsG = invade(ens2->filsG,touselems,date+taille_paquet,taille_paquet,periode);
				if(ens2->filsG)
					ens2->temps_depart = ens2->filsG->temps_depart;
				else
				{
					libereens(ens2);
					libereens(ens4);
					freeelems(elems);

					return NULL;
				}
				
			}

			//On recopie ens2 dans ens3

			if(ens3== NULL)
			{
				
				ens3 =  cree_ensemble(-1,ens2->temps_depart);
				ens3->filsG = cpyens(ens2->filsG);
				ens4 = ens3;
				
			}
			else
			{
				ens3->frereD = cree_ensemble(-1,ens2->temps_depart);
				ens3->frereD->filsG = cpyens(ens2->filsG);
				ens3->frereD->frereG = ens3;
				ens3 = ens3->frereD;
			}
			//affiche_ensemble(ens2);
			date = date_fin(ens3,taille_paquet);//printf("fin = %d \n",date);

		}
		tmp = ens2;
		ens2 = ens2->frereD;
		if(ens2 != NULL)
			ens2->frereG = NULL;
		if(tmp)
			free(tmp);
		
	}

	//on remet ens3 a son debut
	
	while(ens3->frereG)
		ens3 = ens3->frereG;
	//affiche_ensemble(ens3);printf("RETOUR INVADE --------------------\n");
	return ens3;
		
}
Ensemble * crisis(Ensemble * ens,Element * crisise, Element * elemspere,Element * touselems, int taille_paquet, int periode)
{
	//printf("------------------ENTREE DANS LA FONCTION CRISIS-----------------------------\n");
	if(ens == NULL)
		return NULL;
	//printf("ens != NULL\n");
	//printf("Ensemble et elems pere a l'entree de crisis\n");
	//affiche_ensemble(ens);printf("\n");
	//affichejobs(elemspere);
	//printf("\n Et la crisis est :");affichejobs(crisise);printf("\n");
	//printf("fin\n");
	Ensemble * ens2 = NULL;
	Ensemble * tmp = NULL;
	Ensemble * debut = ens;
	//on se place a droite de l'ensemble qui a crisis
	while(ens->frereD)
	{
		ens = ens->frereD;
	}
	Element * elems = init_element();
	Element * elemtmp=NULL;
	int date = 0;
	elems = ajoute_elemt(elems,crisise->index,crisise->release,crisise->deadline);
	//on backtrack jusqu'a PULL de X ou a la fin
	while(ens)
	{
		
		if(ens->numero_element != -1) // si c'est un element
		{
			elemtmp = get_element_i(touselems,ens->numero_element);
			//printf("Elemtmp = %d\n",elemtmp->index);
			if(elemtmp->deadline > crisise->deadline)//si on est sur PULL(X)
			{	
				//printf("PULL X = %d\n",elemtmp->index);
				elemspere=ajoute_elemt_fin(elemspere,elemtmp->index,elemtmp->release,elemtmp->deadline);
				date = ens->temps_depart;
				//printf("PULL(%d) = %d\n",crisise->index,elemtmp->index);
				if(ens2 == NULL)
				{
					ens2 = cree_ensemble(-2,-1);
				}
				else
				{
					tmp = ens2;
					ens2->frereG = cree_ensemble(-2,-1);
					ens2 = ens2->frereG;
					ens2->frereD = tmp;
					
				}
				if(ens->frereG)//on regarde si on est pas au debut de l'ensemble
				{
					ens = ens->frereG;
					libereens(ens->frereD);
					ens->frereD = NULL;
				}
				else
				{
					libereens(ens);
					ens = NULL;
					debut = NULL;
				}
				break;
			}
			if(ens2 == NULL)//initialisation
			{
				ens2 = cree_ensemble(-2,-1);
			}
			else
			{
				tmp = ens2;
				ens2->frereG = cree_ensemble(-2,-1);
				ens2 = ens2->frereG;
				ens2->frereD = tmp;
			}
			
			elems=ajoute_elemt(elems,elemtmp->index,elemtmp->release,elemtmp->deadline);
		}
		
		else //si c'est un ensemble
		{
			if(ens2== NULL)
			{
				ens2 = cpyens(ens);
			}
			else
			{
				ens2->frereG = cpyens(ens);
				ens2->frereG->frereD = ens2;
				ens2 = ens2->frereG;
			}

		}
		
		if(ens->frereG)//on regarde si on est pas au debut de l'ensemble
		{
			
			//printf("On ajoute ");affiche_ensemble(ens);printf("au r.s\n");
			ens = ens->frereG;
			libereens(ens->frereD);
			ens->frereD = NULL;
		}
		else
		{
			//printf("On ajoute ");affiche_ensemble(ens);printf("au r.s\n");
			libereens(ens);
			ens = NULL;
			debut = NULL;
		}
		
		//si ens est null alors on n'a pas trouvé PULL(X) (si on a trouvé pull X on break)
		if(ens == NULL)

			{
				libereens(ens2);
				freeelems(elems);
				return NULL;
			}
	}


	
	Ensemble* ens3 = NULL;
	Ensemble * ens4 = NULL;
	int i;
	//printf("avant le rescheduling date = %d\n",date);
	//affiche_ensemble(ens2);
	//affichejobs(elems);
	//printf("\n");
	while(ens2)//parcours de ens2
	{
		if(ens2->numero_element == -2)//si on attend un element
		{
			
			i = eligible(elems,date);
			//printf("I = %d \n",i);
			if(i==-1)
			{
				//printf("Failure 1\n");
				libereens(ens2);
				libereens(debut);
				freeelems(elems);
				libereens(ens4);
				return NULL;
			}
			elemtmp = get_element_i(touselems,i);
			if(elemtmp == NULL)
			{
				//printf("Failure 0\n");
				libereens(ens2);
				libereens(debut);
				freeelems(elems);
				libereens(ens4);
				return NULL;
			}
			
			date = max(date,elemtmp->release);
			if(date+taille_paquet > elemtmp->deadline)//CRISIS
			{
				
				//printf("crisis(crisis) sur la tache %d\n",elemtmp->index);
				Element * crisisrec = ajoute_elemt(NULL,elemtmp->index,elemtmp->release,elemtmp->deadline);
				elems=retire_element_i(elems,crisisrec->index);
				if(!elems)
					elems = ajoute_elemt(elems,INT_MAX,INT_MAX,INT_MAX);
				//printf("Jobs avant crisis\n");affichejobs(elems);printf(" \n");
				ens4 = crisis(ens4,crisisrec,elems,touselems,taille_paquet,periode);
				freeelems(crisisrec);
				//printf("Ensemble apres crisis\n");affiche_ensemble(ens4);printf(" \n");
				//printf("jobs apres crisis\n");affichejobs(elems);printf(" \n");
				if(ens4 == NULL)
				{
					//printf(" NO PULL(%d) FOUND\n",elemtmp->index);
					libereens(ens2);
					libereens(debut);
					libereens(ens4);
					freeelems(elems);
					return NULL;
				}
				ens3 = ens4;
				while(ens3->frereD)
					ens3 = ens3->frereD;
				//affiche_ensemble(ensembletmp);printf("Elemenent le plus a droite\n");
				date = date_fin(ens3,taille_paquet);
				//recalculer_deadlines(elems,ens4->temps_depart+periode);
				elems = tri_elems(elems);
				//ajoute d'un elem -2 a ens 2

				Ensemble * temporaire = ens2->frereD;
				ens2->frereD = cree_ensemble(-2,-1);
				ens2->frereD->frereG = ens2;
				ens2->frereD->frereD = temporaire;
			}
			
			else// PAS DE CRISE
			{
				
				if(ens3 == NULL)//premier element de ens3
				{
					ens3=cree_ensemble(elemtmp->index,date);
					ens4 = ens3;
				}
				else
				{
					ens3->frereD=cree_ensemble(elemtmp->index,date);
					ens3->frereD->frereG = ens3;
					ens3 = ens3->frereD;
				}
				elems= retire_element_i(elems,elemtmp->index);
				date+=taille_paquet;
				if(ens2->frereD)//Si il y a un prochain element
				{
					if(ens2->frereD->numero_element == -1)//et que c'est un ensemble
					{
						if(date>=ens2->frereD->temps_depart)//ET que ca INVADE
						{
							//printf("%d INVADE date %d\n",elemtmp->index,date);
							
							ens2->frereD->filsG = invade(ens2->frereD->filsG,touselems,date,taille_paquet,periode);
							if(ens2->frereD->filsG)
								ens2->frereD->temps_depart = ens2->frereD->filsG->temps_depart;
							else
							{
								libereens(ens2);
								libereens(ens4);
								libereens(debut);
								freeelems(elems);
								return NULL;
							}
							
							
						}
					}
				}
			}
			
			//affiche_ensemble(ens3);printf(" Element\n");
		}
		else//si c'est un ensemble
		{
			
			if(ens3== NULL)
			{
				
				ens3 =  cree_ensemble(-1,ens2->temps_depart);
				ens3->filsG = cpyens(ens2->filsG);
				ens4= ens3;
			}
			else
			{
				ens3->frereD = cree_ensemble(-1,ens2->temps_depart);
				ens3->frereD->filsG = cpyens(ens2->filsG);
				ens3->frereD->frereG = ens3;
				ens3 = ens3->frereD;
			}
			//affiche_ensemble(ens2);
			date = date_fin(ens3,taille_paquet);//printf("fin = %d \n",date);
			if(ens2->frereD)//Si il y a un prochain element
			{
				if(ens2->frereD->numero_element == -1)//et que c'est un ensemble
				{
					if(date>=ens2->frereD->temps_depart)//ET que ca INVADE
					{
						//printf("%d INVADE date %d\n",elemtmp->index,date);
						
						ens2->frereD->filsG = invade(ens2->frereD->filsG,touselems,date,taille_paquet,periode);
						if(ens2->frereD->filsG)
							ens2->frereD->temps_depart = ens2->frereD->filsG->temps_depart;
						else
						{
							libereens(ens2);
							libereens(debut);
							freeelems(elems);
							libereens(ens4);
							return NULL;
						}
						
						
					}
				}
			}
		}
		
		tmp = ens2;
		ens2 = ens2->frereD;
		if(ens2 != NULL)
			ens2->frereG = NULL;
		if(tmp)
			free(tmp);
	}
	ens3 = ens4;

	//on remet ens3 a son debut
	//affiche_ensemble(ens);printf(" -----------------------------\n");
	while(ens3->frereG)
		ens3 = ens3->frereG;
	Ensemble * newfils = cree_ensemble(-1,ens3->temps_depart);
	newfils->filsG = ens3; 

	freeelems(elems);
	if(!ens)//si ens est vide, alors on est au tout debut 
	{
		//printf("RETOUR DE CRISIS--------------------------------------");
		//affiche_ensemble(newfils);printf("\n");
		return newfils;
	}
	ens->frereD = newfils;
	newfils->frereG = ens;
	//on donne a ens un element avec ens3 comme filsgauche
	//printf("RETOUR DE CRISIS--------------------------------------");
	//affiche_ensemble(debut);printf("\n");
	
	return debut;
	
}



void transforme_waiting(Ensemble * ens, int * wi)
{

	if(ens)
	{
		if(ens->numero_element <0)
		{
			transforme_waiting(ens->filsG,wi);
			transforme_waiting(ens->frereD,wi);
		}
		else
		{
			wi[ens->numero_element]=ens->temps_depart;
			transforme_waiting(ens->frereD,wi);
		}
	}
}




//Algo naif
int simons(Graphe g, int taille_paquet, int TMAX,int periode,int mode, int * offsets)
{
	
	Taskgroup tg;
	///////////////////////////////////////////////////////taille_paquet = 6;
	 if (!(g.N % 2))
    {
      printf ("G n'est peut être pas une étoile\n");
      exit (5);
    }
    int time_algo ;
	  int nbr_route = g.N / 2;
	  int Dl[nbr_route];
	  int m_i[nbr_route];
	  int w_i[nbr_route];
	  int deadline[nbr_route];
	int i;

	int permutation[nbr_route];
	int routes_retour[nbr_route];



	  for (i = 0; i < nbr_route; i++)
	    {
	    	permutation[i]=i;
	      Dl[i] = g.matrice[nbr_route][i]+g.matrice[nbr_route][i+nbr_route+1];

	      routes_retour[i]=g.matrice[nbr_route][i+nbr_route+1];
	    }



	int offset = 0;
	
	if(offsets)
	{
		for(int i=0;i<nbr_route;i++)
		{
			m_i[i] = offsets[i];
		}
	}
	else
	{

		switch(mode)
		{
			case 1:
				tri_bulles(Dl,permutation,nbr_route);
			break;
			case 2:
				tri_bulles_inverse(Dl,permutation,nbr_route);
			break;
			case 3:
				tri_bulles(routes_retour,permutation,nbr_route);
			break;
			case 4:
				tri_bulles_inverse(routes_retour,permutation,nbr_route);
			break;
			default:
				fisher_yates(permutation,nbr_route);
			break;
		}

		m_i[permutation[0]]=0;
		offset = taille_paquet+g.matrice[nbr_route][permutation[0]];
		for(int i=1;i<nbr_route;i++)
		{
			m_i[permutation[i]]=offset-g.matrice[nbr_route][permutation[i]];
			offset+=taille_paquet;
		}
	}





	
	//affiche_tab(m_i,nbr_route);

	//release times
	int arrivee[nbr_route];
	offset = 0;


	//Only usefull for synch
	int mini = m_i[0];
	if(SYNCH)
	for(int i=0;i<nbr_route;i++)
	{
		if(m_i[i]<mini)
			mini = m_i[i];
	}

	


	for(i=0;i<nbr_route;i++)
	{
		Dl[i] = g.matrice[nbr_route][i]+g.matrice[nbr_route][i+nbr_route+1];

		arrivee[i] = Dl[i]+m_i[i]+g.matrice[nbr_route][i+nbr_route+1];
		if(!SYNCH)
			deadline[i] =  TMAX+m_i[i]- g.matrice[nbr_route][i]+taille_paquet;
		else
			deadline[i] =  TMAX+mini- g.matrice[nbr_route][i]+taille_paquet;
	}
	
	i=lower(arrivee,nbr_route);
	

	/*decaler_release(arrivee,deadline, periode, i,nbr_route,taille_paquet);
	for(int k=0;k<nbr_route;k++)
	{
		if(arrivee[k] > (periode -taille_paquet))
		{
			arrivee[k]=0;
			deadline[k] -= periode;
		}
	}*/
	if(!ALGO_JOEL)
	{



		int	date=arrivee[i];
		//////////////////////////////////////////////////////////////////int date = 0;
		int j;

		//afficheTwoWayTrip(t);
		Element * elems = init_element();
		//int deadline_route;
		//int deadline_periode = date + periode;
		for(j=0;j<nbr_route;j++)
		{
			//deadline_route = TMAX+m_i[j]- g.matrice[nbr_route][j]+taille_paquet;
			elems = ajoute_elemt(elems,j,arrivee[j],deadline[j]);

		}
	/*
		elems= ajoute_elemt(elems,0,0,74);
		elems= ajoute_elemt(elems,1,21,46);
		elems= ajoute_elemt(elems,2,2,60);
		elems= ajoute_elemt(elems,3,50,68);
		elems= ajoute_elemt(elems,4,4,34);
		elems= ajoute_elemt(elems,5,10,36);
		elems= ajoute_elemt(elems,6,28,38);
		elems= ajoute_elemt(elems,7,54,62);
		elems= ajoute_elemt(elems,8,30,48);
		elems= ajoute_elemt(elems,9,52,68);
		elems= ajoute_elemt(elems,10,25,40);
		
		*/
		/*affiche_etoile(g);
		affiche_tab(m_i,nbr_route);
		printf("%d tmax \n", TMAX);
		*/
		
		//affiche_tab(m_i,nbr_route);
		//affichejobs(elems);
		Element *  elems2 = cpy_elems(elems);
		Element * tmp = elems2;
		
		int a_scheduler = nbr_route;
		/////////////////////////////////////////////////////int a_scheduler = 11;
		Ensemble * ens = NULL;
		Ensemble * ensembletmp;
		while(a_scheduler != 0)//tant qu'on n'a pas schedul tous les elements
		{
			//printf("test %p\n",elems2);
			i = eligible(elems2,date);
			//printf("i = %d\n",i);
			if(i==-1)
			{
				printf("Failure 1\n");
				return -1;
			}
			tmp = get_element_i(elems2,i);
			
			if(tmp == NULL)
			{
				printf("Failure 0\n");
				return -1;
			}
			
			date = max(date,tmp->release);
			if(date+taille_paquet > tmp->deadline)//CRISIS
			{
				
				//printf("Crisis(main) sur la tache %d\n",tmp->index);
				//affiche_ensemble(ens);printf(" Avant\n");
				Element * crisise = NULL;
				crisise = ajoute_elemt(crisise,tmp->index,tmp->release,tmp->deadline);

				elems2=retire_element_i(elems2,crisise->index);
				if(!elems2)
						elems2 = ajoute_elemt(elems2,INT_MAX,INT_MAX,INT_MAX);
				//printf("Taches avant crisis\n");affichejobs(elems2);printf("\n");
				//printf("ens avant crisis\n");affiche_ensemble(ens);printf("\n");
				ens = crisis(ens,crisise,elems2,elems,taille_paquet,periode);
					
				freeelems(crisise);
				//printf("Taches apres crisis\n");affichejobs(elems2);printf("\n");
				//printf("ens apres crisis\n");affiche_ensemble(ens);printf("\n");
				//affiche_ensemble(ens);printf(" Apres\n");
				if(ens == NULL)
				{
					//affichejobs(elems2);
					freeelems(elems);
					freeelems(elems2);
					return -1;
				}
				//on se remet a droite de l'ensemble
				ensembletmp = ens;
				while(ensembletmp->frereD)
					ensembletmp = ensembletmp->frereD;
				//affiche_ensemble(ensembletmp);printf("Elemenent le plus a droite\n");
				date = date_fin(ens, taille_paquet);
				
				/*printf("APRESCRISIS\n");
				printf("Date %d : ",date);*/
				//recalculer_deadlines(elems2,ens->temps_depart+periode);
				elems2 = tri_elems(elems2);
				/*affichejobs(elems2);
				affiche_ensemble(ens);printf("\n\n\n");*/
			}
			else // pas crisis
			{
				//printf("AVANT eligible = %d\n",i);
				//printf("Date %d : ",date);
				//affichejobs(elems2);
				//affiche_ensemble(ens);printf("\n");
				if(ens == NULL)//si c'est la premiere fois
				{
					ens = cree_ensemble(tmp->index,date);
					ensembletmp = ens;
				}
				else
				{
					ensembletmp->frereD = cree_ensemble(tmp->index,date);
					ensembletmp->frereD->frereG = ensembletmp;
					ensembletmp = ensembletmp->frereD;
				}
				//printf("APRES\n");
				elems2= retire_element_i(elems2,tmp->index);
				a_scheduler--;
				//printf("Date %d : ",date);
				//affichejobs(elems2);
				//affiche_ensemble(ens);printf("\n\n\n");
				date+=taille_paquet;
				
			}
			
			
		
		}
		/*printf("SOLUTION \n");
		affichejobs(elems);
		affiche_ensemble(ens);
		printf("--- \n");*/
		//ecriture des temps trouvés
		Ensemble * a_free = ens;

		transforme_waiting(ens,w_i);

	

		libereens(a_free);
		freeelems(elems);
		freeelems(elems2);

		//affiche_tab(m_i,nbr_route);
		//printf("simons wi\n");affiche_tab(w_i,nbr_route);
		//if(!is_ok(g,taille_paquet,m_i,w_i,periode)){printf("ERROR simons\n");}

		 
	}
	else
	{
		Task * task = (Task *)malloc(nbr_route * sizeof(Task));
		for(int j=0;j<nbr_route;j++)
		{
			task[j].release_time=arrivee[j];
			task[j].deadline=deadline[j];

		}
		
		tg.n = nbr_route;
		tg.tasks=task;
		//printf(" Les taches sont :");
		//show_tasks(tg);
		int * tabtmp = wrapper_fun(tg);
		if(tabtmp == NULL)
		{
			//printf("On est ici\n");
			return -1;
		}
		//show_schedule(tabtmp, tg.n);
		for(int j=0;j<nbr_route;j++)
		{ 
			w_i[j]=tabtmp[j];
		}
		time_algo = effective_time(tabtmp, tg.n);
		free(tabtmp);
		free(tg.tasks);
		
		
	}
	//printf("apres transforme wi\n");affiche_tab(w_i,nbr_route);
	for(i=0;i<nbr_route;i++)
	{
		w_i[i] -= arrivee[i];
	}
	int maximum ;
	if(ALGO_JOEL)
	{
		if(time_algo <= periode)
		{
			
			return 1;
		}
		else
		{
			return -1;
		}
	}

	//CALCUL TMAX
	if(!SYNCH)
	{
		maximum= w_i[0]+2*Dl[0];
		for(int i=0;i<nbr_route;i++)
		{
			if(w_i[i]+2*Dl[i] > maximum)
				maximum= w_i[i]+2*Dl[i];
		}
	}
	else
	{
		maximum= m_i[0]+w_i[0]+2*Dl[0];
		for(int i=0;i<nbr_route;i++)
		{
			if(m_i[i]+w_i[i]+2*Dl[i] > maximum)
				maximum= m_i[i]+w_i[i]+2*Dl[i];
		}
	}


	//TEST DEPASSEMENT PERIODE
	int retour[nbr_route];
	for(int i=0;i<nbr_route;i++)
	{
		retour[i] = m_i[i]+g.matrice[nbr_route][i]+2*g.matrice[nbr_route][nbr_route+i+1]+w_i[i];
	}
	int taille_periode_retour = retour[greater(retour,nbr_route)]-retour[lower(retour,nbr_route)]+taille_paquet;
	if(taille_periode_retour > periode)return -2;	
		//affiche_tab(retour,nbr_route);printf("maximum = %d\n",maximum);
	
		//affiche_solution(g,taille_paquet,m_i,w_i);
	return maximum;
}

int* retourne_offset(Graphe g, int taille_paquet, int* permutation,int mode,int periode)
{
	int nbr_route = g.N/2;
	int * offsets;
	assert(offsets=(int*)malloc(sizeof(int)*nbr_route));
	int offset = 0;
	int budget = periode - (nbr_route*taille_paquet);

	int inter_size = budget / nbr_route; 
	int * random;

	switch(mode)
	{
		case 0:
			for(int i=0;i<nbr_route;i++)
			{
					offsets[permutation[i]]=offset;
					offset+=taille_paquet;

			}
		break;
		case 1:
			for(int i=0;i<nbr_route;i++)
			{
					offsets[permutation[i]]=offset;
					offset+=taille_paquet+inter_size;

				
			}
		break;
		default:
			random = random_inters(budget, nbr_route);
			for(int i=0;i<nbr_route;i++)
			{
					
					offsets[permutation[i]]=offset;
					offset+=taille_paquet+random[i];
			}
			free(random);
			
		break;
	}
	
	return offsets;
}
int *retourne_departs(Graphe g, int * offsets)
{
	int nbr_route = g.N/2;
	int* m_i = (int*)malloc(sizeof(int)*nbr_route);
	int min = INT_MAX;
	for(int i=0;i<nbr_route;i++)
	{
		m_i[i] = offsets[i] - g.matrice[nbr_route][i];
		if(m_i[i]<min)
			min = m_i[i];
	}
	for(int i=0;i<nbr_route;i++)
	{
		m_i[i] += (-min);
	}
	return m_i;
}
Ensemble * algo_simons(Element * elems, int nbr_route, int taille_paquet, int date,int periode)
{
	int i;
	Element *  elems2 = cpy_elems(elems);
	Element * tmp = elems2;
	
	int a_scheduler = nbr_route;
	/////////////////////////////////////////////////////int a_scheduler = 11;
	Ensemble * ens = NULL;
	Ensemble * ensembletmp;
	while(a_scheduler != 0)//tant qu'on n'a pas schedul tous les elements
	{
		//printf("test %p\n",elems2);
		i = eligible(elems2,date);
		//printf("i = %d\n",i);
		if(i==-1)
		{
			printf("Failure 1\n");
			return NULL;
		}
		tmp = get_element_i(elems2,i);
		
		if(tmp == NULL)
		{
			printf("Failure 0\n");
			return NULL;
		}
		
		date = max(date,tmp->release);
		if(date+taille_paquet > tmp->deadline)//CRISIS
		{
			
			//printf("Crisis(main) sur la tache %d\n",tmp->index);
			//affiche_ensemble(ens);printf(" Avant\n");
			Element * crisise = NULL;
			crisise = ajoute_elemt(crisise,tmp->index,tmp->release,tmp->deadline);

			elems2=retire_element_i(elems2,crisise->index);
			if(!elems2)
					elems2 = ajoute_elemt(elems2,INT_MAX,INT_MAX,INT_MAX);
			//printf("Taches avant crisis\n");affichejobs(elems2);printf("\n");
			//printf("ens avant crisis\n");affiche_ensemble(ens);printf("\n");
			ens = crisis(ens,crisise,elems2,elems,taille_paquet,periode);
			freeelems(crisise);
			//printf("Taches apres crisis\n");affichejobs(elems2);printf("\n");
			//printf("ens apres crisis\n");affiche_ensemble(ens);printf("\n");
			//affiche_ensemble(ens);printf(" Apres\n");
			if(ens == NULL)
			{
				//affichejobs(elems2);
				//printf(" NO PULL(%d) FOUND\n",crisise->index);
				freeelems(elems);
				freeelems(elems2);
				return NULL;
			}
			//on se remet a droite de l'ensemble
			ensembletmp = ens;
			while(ensembletmp->frereD)
				ensembletmp = ensembletmp->frereD;
			//affiche_ensemble(ensembletmp);printf("Elemenent le plus a droite\n");
			date = date_fin(ens, taille_paquet);
			
			/*printf("APRESCRISIS\n");
			printf("Date %d : ",date);*/
			//recalculer_deadlines(elems2,ens->temps_depart+periode);
			elems2 = tri_elems(elems2);
			/*affichejobs(elems2);
			affiche_ensemble(ens);printf("\n\n\n");*/
		}
		else // pas crisis
		{
			//printf("AVANT eligible = %d\n",i);
			//printf("Date %d : ",date);
			//affichejobs(elems2);
			//affiche_ensemble(ens);printf("\n");
			if(ens == NULL)//si c'est la premiere fois
			{
				ens = cree_ensemble(tmp->index,date);
				ensembletmp = ens;
			}
			else
			{
				ensembletmp->frereD = cree_ensemble(tmp->index,date);
				ensembletmp->frereD->frereG = ensembletmp;
				ensembletmp = ensembletmp->frereD;
			}
			//printf("APRES\n");
			elems2= retire_element_i(elems2,tmp->index);
			a_scheduler--;
			//printf("Date %d : ",date);
			//affichejobs(elems2);
			//affiche_ensemble(ens);printf("\n\n\n");
			date+=taille_paquet;
			
		}
		
		
	
	}
	//affiche_ensemble(ens);
	//ecriture des temps trouvés


	freeelems(elems2);
	return ens;
	

}
 
int* normaliser_instance(Graphe g, int periode)
{
	int nbr_route = g.N/2;
	int * instance = (int *)malloc(sizeof(int)*nbr_route);
	for(int i=0;i<nbr_route;i++)
	{
		instance[i] = g.matrice[nbr_route][i+nbr_route+1]; // juste le retour
		while((2 * instance[i]) >= periode)
		{
			instance[i] -= periode/2;
		}
	}
	return instance;

}
int mod(int a, int b)
{
    int r = a % b;
    return (r < 0) ? r + b : r;
}
void decaler_release(int * release, int* deadline, int periode, int premier,int nbr_route,int taille_paquet)
{
	int releasepremier=release[premier];
	

	for(int i=0;i<nbr_route;i++)
	{
		//printf("Pour la route %d [%d;%d]\n",i,release[i],deadline[i]);
		release[i] -= releasepremier;
		deadline[i] -= releasepremier;
		//printf("apres : %d %d\n",release[i],deadline[i]);
		while(release[i] < 0)
		{
			//printf("Ajout de periode \n");
			release[i] += periode;
			deadline[i] += periode;
			//printf(" %d %d\n",release[i],deadline[i]);
		}
		//printf("Derniere modif \n");
		deadline[i] = deadline[i] - periode*(release[i]/periode);
		release[i] = mod(release[i],periode);
		
		//printf("apres2 : %d %d\n",release[i],deadline[i]);

		if(release[i] > (periode -taille_paquet))
		{
			release[i]=0;
			deadline[i] -= periode;
		}
		
		
	}
	
}
int check_Deadlines(int*tab,int size)
{
	for(int i=0;i<size;i++)
		{
			if(tab[i]<0)
				return 0;
		}
		return 1;
}
int simons_periodique(Graphe g, int taille_paquet,int TMAX, int periode, int * m_i)
{	
	
	
	///////////////////////////////////////////////////////taille_paquet = 6;
	 if (!(g.N % 2))
    {
      printf ("G n'est peut être pas une étoile\n");
      exit (5);
    }

	int nbr_route = g.N/2;
	
	int Dl[nbr_route];
	int w_i[nbr_route];
	int fin[nbr_route];
	int debut_periode_retour;
	int arrivee[nbr_route];
	int release[nbr_route];
	int deadline[nbr_route];
	int date;
	int deadline_periode;
	Element * elems;
	Ensemble * ens;
	Ensemble * a_free= NULL;
	int maximum;

	//printf("SIMONS PER \n");
	int mini = m_i[0];
	if(SYNCH)
		for(int i=0;i<nbr_route;i++)
		{
			if(m_i[i]<mini)
				mini = m_i[i];
		}

	for(int premier=0;premier<nbr_route;premier++)
	{
		//printf("Premier = %d \n",premier);
		for(int i=0;i<nbr_route;i++)
		{
			Dl[i] = g.matrice[nbr_route][i]+g.matrice[nbr_route][i+nbr_route+1];
			release[i] = Dl[i]+m_i[i]+g.matrice[nbr_route][i+nbr_route+1];
			arrivee[i] = Dl[i]+m_i[i]+g.matrice[nbr_route][i+nbr_route+1];
			if(!SYNCH)
				deadline[i] =  TMAX+m_i[i]- g.matrice[nbr_route][i]+taille_paquet;
			else
				deadline[i] =  TMAX+mini-g.matrice[nbr_route][i]+taille_paquet;
		}
		//printf("Release \n");affiche_tab(arrivee,nbr_route);printf("\n");
		debut_periode_retour = arrivee[premier];
		decaler_release(arrivee,deadline, periode, premier,nbr_route,taille_paquet);
		
		if(!check_Deadlines(deadline,nbr_route))continue;

		date=arrivee[premier];
		//printf("Release \n");affiche_tab(arrivee,nbr_route);printf("\n");
		//printf("Deadline \n");affiche_tab(deadline,nbr_route);printf("\n");
		elems = init_element();
		deadline_periode = arrivee[premier] + periode;
		
		//Test pour eviter que si le premier est trop petit en latence, ca lance simons quand même
		if(deadline[premier] < taille_paquet)
			continue;

		//	printf("date = %d, arrive premier = %d periode = %d, tmax = %d\n",date, arrivee[premier],periode,TMAX);
		if(!ALGO_JOEL)
		{
			for(int j=0;j<nbr_route;j++)
			{
				if(j != premier)
				{
					//elems = ajoute_elemt(elems,j,arrivee[j],deadline[j]);
					elems = ajoute_elemt(elems,j,arrivee[j],min(deadline_periode,deadline[j]));
					//printf("ajout de %d ( %d, min(%d %d) )\n",j,arrivee[j],deadline[j],deadline_periode);
				}
				else
				{
					elems = ajoute_elemt(elems,j,arrivee[j],arrivee[j]+taille_paquet);
				}


			}
			//printf("Per premier = %d\n",premier);
			//printf("Steeve\n");affichejobs(elems);
			ens = algo_simons(elems,nbr_route,taille_paquet,date,periode);
			if(ens == NULL)continue;
			/*ens->frereG = cree_ensemble(premier,arrivee[premier]);
			ens->frereG->frereD = ens;
			ens = ens->frereG;*/
			a_free = ens;
			
			//printf("Solution : \n");
			//affiche_ensemble(ens);printf("\n");
			//printf("Fin per \n");
			transforme_waiting(ens,fin);
		

			//affiche_tab(fin,nbr_route);
		
			libereens(a_free);
			freeelems(elems);
		}
		else
		{
			Task * task = (Task *)malloc(nbr_route * sizeof(Task));
			for(int j=0;j<nbr_route;j++)
			{ 
				if(j != premier)
				{
					task[j].release_time=arrivee[j];
					task[j].deadline=min(deadline_periode,deadline[j]);

				}
				else
				{
					task[j].release_time=arrivee[j];
					task[j].deadline=arrivee[j]+taille_paquet;
				}
			}
			

			Taskgroup tg;
			tg.n = nbr_route;
			tg.tasks=task;
			//show_tasks(tg);
			int * tabtmp = wrapper_fun(tg);
			if(tabtmp == NULL)
			{
				//printf("ici\n");
				continue;
			}
			else
				return 1;
			for(int j=0;j<nbr_route;j++)
			{
				//show_schedule(tabtmp, tg.n);
				fin[j]=tabtmp[j];
			}
			free(tabtmp);
			free(tg.tasks);
			//affiche_tab(fin,nbr_route);
		}
		//affiche_tab(fin,nbr_route);
		//printf("apres transforme wi\n");affiche_tab(w_i,nbr_route);
		for(int i=0;i<nbr_route;i++)
		{
			w_i[i] = fin[i] +debut_periode_retour - release[i] ; 
			//printf("[%d]%d = %d - %d\n",i,w_i[i],fin[i],arrivee[i]);
			while(w_i[i]<0)
			{
				w_i[i] += periode;
			}
			w_i[i] = w_i[i]%periode;
		}
		//affiche_tab(w_i,nbr_route);

		
		/*
		affiche_tab(m_i,nbr_route);
		affiche_tab(deadline,nbr_route);
		affiche_tab(arrivee,nbr_route);
		affiche_tab(fin,nbr_route);
		affiche_tab(Dl,nbr_route);
		affiche_tab(w_i,nbr_route);
		affiche_tab(release,nbr_route);
		printf("Debut%d\n",debut_periode_retour);
*/
		//printf("simons wi\n");affiche_tab(w_i,nbr_route);
		//affiche_solution(g,taille_paquet,m_i,w_i,periode);
		/*
			int retour[nbr_route];
			for(int i=0;i<nbr_route;i++)
			{
				retour[i] = m_i[i]+g.matrice[nbr_route][i]+2*g.matrice[nbr_route][nbr_route+i+1]+w_i[i];
			}
			int taille_periode_retour = retour[greater(retour,nbr_route)]-retour[lower(retour,nbr_route)]+taille_paquet;
			printf("Periode de taille %d \n\n",taille_periode_retour);
		*/

		if(!is_ok(g,taille_paquet,m_i,w_i,periode)){continue;}
		if(!SYNCH)
		{
			maximum= w_i[0]+2*Dl[0];
			for(int i=0;i<nbr_route;i++)
			{
				if(w_i[i]+2*Dl[i] > maximum)
				{
					maximum= w_i[i]+2*Dl[i];
					//printf("%d i\n",i);
				}
			}
		}
		else
		{
			maximum= m_i[0]+w_i[0]+2*Dl[0];
			for(int i=0;i<nbr_route;i++)
			{
				if(m_i[i]+w_i[i]+2*Dl[i] > maximum)
				{
					maximum= m_i[i]+w_i[i]+2*Dl[i];
					//printf("i = %d newmax = %d\n",i,maximum);
				}
			}
		}	
		//affiche_solution(g,taille_paquet,m_i,w_i,periode);
		if(maximum<=TMAX)
		{
			
			//printf("RETOUR \n");
			return maximum;
		}
		else
		{
			printf("ca arrive\n");
			//printf("%d %d",TMAX,maximum);	
			exit(34);
		}
		

	}

	return -1;
}


//int simons_FPT(Graphe g, int taille_paquet,int TMAX, int periode, int mode)
int simons_FPT(Graphe g, int taille_paquet,int TMAX, int periode, int * m_i, int premier,int * subset)
{	
	
	///////////////////////////////////////////////////////taille_paquet = 6;
	 if (!(g.N % 2))
    {
      printf ("G n'est peut être pas une étoile\n");
      exit (5);
    }

	int nbr_route = g.N/2;


	
	int Dl[nbr_route];
	int w_i[nbr_route];
	int fin[nbr_route];
	int debut_periode_retour;
	int arrivee[nbr_route];
	int release[nbr_route];
	int deadline[nbr_route];
	int date;
	int deadline_periode;
	Element * elems;
	Ensemble * ens;
	Ensemble * a_free;
	int maximum;
	
	for(int i=0;i<nbr_route;i++)
	{
		Dl[i] = g.matrice[nbr_route][i]+g.matrice[nbr_route][i+nbr_route+1];
		release[i] = Dl[i]+m_i[i]+g.matrice[nbr_route][i+nbr_route+1];
		arrivee[i] = Dl[i]+m_i[i]+g.matrice[nbr_route][i+nbr_route+1];
		deadline[i] =  TMAX+m_i[i]- g.matrice[nbr_route][i]+taille_paquet;
	}
	debut_periode_retour = arrivee[premier];
	decaler_release(arrivee,deadline, periode, premier,nbr_route,taille_paquet);
		
	
	/*printf("____ \n");
	affiche_tab(arrivee,nbr_route);
	affiche_tab(deadline,nbr_route);
	affiche_tab(subset,nbr_route);
	*/
	for(int i=0;i<nbr_route;i++)
	{
		if(subset[i])
		{
			arrivee[i] = 0;
			deadline[i] -= periode;
		}
	}
	/*
	printf("apres \n");
	affiche_tab(arrivee,nbr_route);
	affiche_tab(deadline,nbr_route);
	printf("\n");
	exit(32);
	*/

	date=arrivee[premier];
	deadline_periode = arrivee[premier] + periode;
	if(!ALGO_JOEL)
	{
		elems = init_element();
		
		//	printf("date = %d, arrive premier = %d periode = %d, tmax = %d\n",date, arrivee[premier],periode,TMAX);
		for(int j=0;j<nbr_route;j++)
		{
			if(j != premier)
			{
				//elems = ajoute_elemt(elems,j,arrivee[j],deadline[j]);
				elems = ajoute_elemt(elems,j,arrivee[j],min(deadline_periode,deadline[j]));
				//printf("ajout de %d ( %d, min(%d %d) )\n",j,arrivee[j],deadline[j],deadline_periode);
			}
			else
				elems = ajoute_elemt(elems,j,arrivee[j],arrivee[j]+taille_paquet);


		}
		//printf("FPT premier = %d\n",premier);
		//affichejobs(elems);
		ens = algo_simons(elems,nbr_route,taille_paquet,date,periode);
		if(ens == NULL)return -1;
		/*ens->frereG = cree_ensemble(premier,arrivee[premier]);
		ens->frereG->frereD = ens;
		ens = ens->frereG;*/
		a_free = ens;
		
		
		//affiche_ensemble(ens);printf("\n");
		//printf("Fin FPT \n");
		transforme_waiting(ens,fin);

		
			

		libereens(a_free);
		freeelems(elems);
	}
	else
	{
		Task * task = (Task *)malloc(nbr_route * sizeof(Task));
		for(int j=0;j<nbr_route;j++)
		{
			if(j != premier)
			{
				task[j].release_time=arrivee[j];
				task[j].deadline=min(deadline_periode,deadline[j]);

			}
			else
			{
				task[j].release_time=arrivee[j];
				task[j].deadline=arrivee[j]+taille_paquet;
			}


		}
	
		Taskgroup tg;
		tg.n = nbr_route;
		tg.tasks=task;
		int * tabtmp = wrapper_fun(tg);
		if(tabtmp == NULL)
			return -1;
		else
			return 1;
		for(int j=0;j<nbr_route;j++)
		{
			fin[j]=tabtmp[j];
		}
		free(tabtmp);
		free(tg.tasks);
	}
	//printf("apres transforme wi\n");affiche_tab(w_i,nbr_route);
	for(int i=0;i<nbr_route;i++)
	{

			w_i[i] = fin[i] +debut_periode_retour - release[i] ; 
		//printf("[%d]%d = %d - %d\n",i,w_i[i],fin[i],arrivee[i]);
		while(w_i[i]<0)
		{
			w_i[i] += periode;
		}
		w_i[i] = w_i[i]%periode;
	}
	//affiche_tab(w_i,nbr_route);
	//printf("simons wi\n");affiche_tab(w_i,nbr_route);
	//affiche_solution(g,taille_paquet,m_i,w_i);
	/*
		int retour[nbr_route];
		for(int i=0;i<nbr_route;i++)
		{
			retour[i] = m_i[i]+g.matrice[nbr_route][i]+2*g.matrice[nbr_route][nbr_route+i+1]+w_i[i];
		}
		int taille_periode_retour = retour[greater(retour,nbr_route)]-retour[lower(retour,nbr_route)]+taille_paquet;
		printf("Periode de taille %d \n\n",taille_periode_retour);
	*/

	if(!is_ok(g,taille_paquet,m_i,w_i,periode)){printf("ca arrive\n");return -1;}

	maximum= w_i[0]+2*Dl[0];
	for(int i=0;i<nbr_route;i++)
	{
		if(w_i[i]+2*Dl[i] > maximum)
			maximum= w_i[i]+2*Dl[i];
	}
	if(maximum<=TMAX)
		return maximum;
	else
		exit(50);
}

//Fait l'arbre recursif avec tous les sous ensemble de routes et appel sur MLS avec la premiere route fixée
int rec_FPT(Graphe g, int taille_paquet,int TMAX, int periode, int * m_i, int premier, int* subset,int * candidats, int profondeur, int nbr_candidats)
{
	
	int test;
	int val_G,val_D;
	if(profondeur == nbr_candidats)
	{
		//affiche_tab(subset,nbr_candidats);
		int routes[g.N /2];
		for(int i=0;i<g.N/2;i++)
			routes[i] = 0;
		for(int i=0;i<nbr_candidats;i++)
			if(subset[i])
				routes[candidats[i]]=1;
		test= simons_FPT(g,taille_paquet,TMAX,periode,m_i,premier,routes);
		return test;
	}
	else 
	{
		//printf("PArcours a droite, %d\n",profondeur);
		subset[profondeur] = 0;
		//affiche_tab(subset,nbr_candidats);
		val_D =  rec_FPT(g,taille_paquet,TMAX,periode,m_i,premier,subset,candidats,profondeur+1,nbr_candidats);
		
		if(val_D != -1)// SI on ne trouve pas on essaye la branche soeur
			return val_D;
		//printf("PArcours a gauche %d\n",profondeur);
		subset[profondeur]= 1;
		//affiche_tab(subset,nbr_candidats);
		val_G =  rec_FPT(g,taille_paquet,TMAX,periode,m_i,premier,subset,candidats,profondeur+1,nbr_candidats); 
		return val_G;

	}
}

int FPT_PALL(Graphe g, int taille_paquet,int TMAX, int periode, int * m_i)
{	
	if (!(g.N % 2))
    {
      printf ("G n'est peut être pas une étoile\n");
      exit (5);
    }

	int nbr_route = g.N/2;
	int res;

	

	// On cherche le nombre de routes avec la deadline qui sort de p
	int Dl[nbr_route];
	int arrivee[nbr_route];
	int deadline[nbr_route];

	

	int j = 0;
	int nbr_candidats;
	for(int premier=0;premier<nbr_route;premier++)
	{
		
		j=0;
		nbr_candidats = 0;
		for(int i=0;i<nbr_route;i++)
		{
			Dl[i] = g.matrice[nbr_route][i]+g.matrice[nbr_route][i+nbr_route+1];
			arrivee[i] = Dl[i]+m_i[i]+g.matrice[nbr_route][i+nbr_route+1];
			deadline[i] =  TMAX+m_i[i]- g.matrice[nbr_route][i]+taille_paquet;
		}
		decaler_release(arrivee,deadline, periode, premier,nbr_route,taille_paquet);
		/*for(int i=0;i<nbr_route;i++)
			printf("%d ",Dl[i]);
		printf("\n");
		for(int i=0;i<nbr_route;i++)
			printf("%d ",arrivee[i]);
		printf("\n");
		for(int i=0;i<nbr_route;i++)
			printf("%d ",deadline[i]);
		printf("\n\n\n");
		*/
		
		for(int i=0;i<nbr_route;i++)
		{
			if(i!= premier)
				if(deadline[i]>(periode+2*taille_paquet))
					nbr_candidats++;
		}
		int subset[nbr_candidats];
		int candidats[nbr_candidats];
		for(int i=0;i<nbr_route;i++)
		{
			if(i!= premier)
				if(deadline[i]>(periode+2*taille_paquet))
				{
					//printf(" i %d j %d\n",i,j);
					candidats[j] = i;
					j++;
				}
				
			
		}
		//printf("%d Candidats : \n",nbr_candidats);
		for(int i=0;i<nbr_candidats;i++)
		{
			subset[i]=0;
			//printf("%d -",candidats[i]);
		}
		//printf("\n");
		//printf("%d \n",nbr_candidats);
		/*
		int subset[nbr_route];
		int candidats[nbr_route];
		nbr_candidats= nbr_route;
		for(int i=0;i<nbr_route;i++)
		{
			subset[i]=0;
			candidats[i] = i;
				
			
		}*/

		res = rec_FPT(g,taille_paquet,TMAX,periode,m_i,premier,subset,candidats,0,nbr_candidats);
		


		if(res!= -1)
		{
			return res;
		}
		
				
	}


	return -1;
}
