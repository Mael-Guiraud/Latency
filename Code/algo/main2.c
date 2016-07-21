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
Ensemble * crisis(Ensemble * ens,Element * crisise, Element * elemspere,Element * touselems);
Ensemble * init_ensemble(){return NULL;}
Element * init_element(){return NULL;}
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

Element * ajoute_element(Element * elem,int index, int release, int deadline)
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
	if(elem->deadline > deadline)//inser avant
	{
		new->next = elem;
		return new;
	}

	while(elem->next)
	{
		
		if(elem->next->deadline > deadline)//inser avant
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
/*
Element * ajoute_element(Element * elem,int index, int release, int deadline)
{
	Element * tmp = elem;
	Element * new = malloc(sizeof(Element));
	new->index = index;
	new->release = release;
	new->deadline = deadline;
	new->next = NULL;
	if(elem == NULL)
	{
		return new;
	}
	while(elem->next)
	{
		elem = elem->next;
	}
	elem->next = new;
	return tmp;
}*/
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
		free(tmp);
		return debut;
	}
	while(elem->next)
	{
		if(elem->next->index == index)
		{
			tmp = elem->next;
			elem->next = elem->next->next;
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
		new = ajoute_element(new,elems->index, elems->release,elems->deadline);
		elems = elems->next;
	}
	return new;
}
int date_fin(Ensemble * ens)
{
	while(ens->frereD)
	{
		ens = ens->frereD;
	}
	//affiche_ensemble_unique(ens);
	if(ens->numero_element == -1)
	{
		return date_fin(ens->filsG);
	}
	else
	{
		return ens->temps_depart+TEMPS_TACHE;
	}
}
//renvoie l'element eligible
int eligible(Element * elems, int time)
{
	Element * debut =elems;
	if(elems == NULL)
		return -1;
	//printf("pointeur envoyé = %p\n",elems);
	int deadline_min=9999;
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
		return eligible(debut,time+1);
	}
}
void libereens(Ensemble * ens)
{
	if(ens)
	{
		if(ens->numero_element == -1)
		{
			libereens(ens->filsG);
		}
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
Ensemble * invade(Ensemble * ens,Element * touselems,int depart)
{

	Ensemble * ens2 = NULL;
	Ensemble * tmp = NULL;
	Ensemble * debut = ens;
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
			elems=ajoute_element(elems,elemtmp->index,elemtmp->release,elemtmp->deadline);
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
	int i;

	while(ens2)//parcours de ens2
	{
		if(ens2->numero_element == -2)//si on attend un element
		{
			
			i = eligible(elems,date);
			if(i==-1)
			{
				//printf("Failure 1\n");
				return NULL;
			}
			elemtmp = get_element_i(touselems,i);
			if(elemtmp == NULL)
			{
				//printf("Failure 0\n");
				return NULL;
			}
			
			date = max(date,elemtmp->release);
			if(date+TEMPS_TACHE > elemtmp->deadline)//CRISIS
			{
				//printf("Crisis(invade) sur la tache %d\n",elemtmp->index);
				Element * crisisrec = ajoute_element(NULL,elemtmp->index,elemtmp->release,elemtmp->deadline);
				elems=retire_element_i(elems,crisisrec->index);
				if(!elems)
					elems = ajoute_element(elems,99999,99999,999999);
				ens3 = crisis(ens3,crisisrec,elems,touselems);
				//printf("-------------\n\n\n");
				//affichejobs(elems);
				//affiche_ensemble(ens2);		printf("\n");
				//affiche_ensemble(ens3);		
				//printf("-------------\n\n\n");		
				if(ens3 == NULL)
				{
					//printf(" NO PULL(%d) FOUND(invade)\n",elemtmp->index);
					return NULL;
				}
				while(ens3->frereD)
					ens3 = ens3->frereD;
				//affiche_ensemble(ensembletmp);printf("Elemenent le plus a droite\n");
				date = date_fin(ens3);
				
				
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
					
				}
				else
				{
					
					ens3->frereD=cree_ensemble(elemtmp->index,date);
					ens3->frereD->frereG = ens3;
					ens3 = ens3->frereD;
				}
				elems= retire_element_i(elems,elemtmp->index);
				//printf("on Schedule %d a la date %d et on le retire de elem",elemtmp->index,date);
				date+=TEMPS_TACHE;
				
				if(ens2->frereD)//Si il y a un prochain element
				{
					if(ens2->frereD->numero_element == -1)//et que c'est un ensemble
					{
						if(date+TEMPS_TACHE>=ens2->frereD->temps_depart)//ET que ca INVADE
						{
							//printf("%d INVADE date %d\n",elemtmp->index,date);
							ens2->frereD->filsG = invade(ens2->frereD->filsG,touselems,date+TEMPS_TACHE);
							ens2->frereD->temps_depart = ens->frereD->filsG->temps_depart;
							
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
				
			}
			else
			{
				ens3->frereD = cree_ensemble(-1,ens2->temps_depart);
				ens3->frereD->filsG = cpyens(ens2->filsG);
				ens3->frereD->frereG = ens3;
				ens3 = ens3->frereD;
			}
			//affiche_ensemble(ens2);
			date = date_fin(ens3);//printf("fin = %d \n",date);

		}
		tmp = ens2;
		ens2 = ens2->frereD;
		free(tmp);
		
	}

	//on remet ens3 a son debut
	
	while(ens3->frereG)
		ens3 = ens3->frereG;
	//affiche_ensemble(ens3);printf("RETOUR INVADE --------------------\n");
	return ens3;
		
}
Ensemble * crisis(Ensemble * ens,Element * crisise, Element * elemspere,Element * touselems)
{
	if(ens == NULL)
		return NULL;
	//printf("------------------ENTREE DANS LA FONCTION CRISIS-----------------------------\n");
	//affiche_ensemble(ens);printf("\n");
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
	elems = ajoute_element(elems,crisise->index,crisise->release,crisise->deadline);
	//on backtrack jusqu'a PULL de X ou a la fin
	while(ens)
	{
		
		if(ens->numero_element != -1) // si c'est un element
		{
			elemtmp = get_element_i(touselems,ens->numero_element);
			if(elemtmp->deadline > crisise->deadline)//si on est sur PULL(X)
			{	
				ajoute_element(elemspere,elemtmp->index,elemtmp->release,elemtmp->deadline);
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
			
			elems=ajoute_element(elems,elemtmp->index,elemtmp->release,elemtmp->deadline);
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
		
		//si ens est null alors on n'a pas trouvé PULL(X) (si on a trouvé pull X on break)
		if(ens == NULL)return NULL;
	}
	
	//affiche_ensemble(ens2);printf(" R.S.\n");
	//affichejobs(elems);
	
	Ensemble* ens3 = NULL;
	int i;
	while(ens2)//parcours de ens2
	{
		if(ens2->numero_element == -2)//si on attend un element
		{
			
			i = eligible(elems,date);
			if(i==-1)
			{
				//printf("Failure 1\n");
				return NULL;
			}
			elemtmp = get_element_i(touselems,i);
			if(elemtmp == NULL)
			{
				//printf("Failure 0\n");
				return NULL;
			}
			
			date = max(date,elemtmp->release);
			if(date+TEMPS_TACHE > elemtmp->deadline)//CRISIS
			{
				
				//printf("crisis(crisis) sur la tache %d\n",elemtmp->index);
				Element * crisisrec = ajoute_element(NULL,elemtmp->index,elemtmp->release,elemtmp->deadline);
				elems=retire_element_i(elems,crisisrec->index);
				if(!elems)
					elems = ajoute_element(elems,99999,99999,999999);
				ens3 = crisis(ens3,crisisrec,elems,touselems);
				if(ens3 == NULL)
				{
					//printf(" NO PULL(%d) FOUND\n",elemtmp->index);
					return NULL;
				}
				while(ens3->frereD)
					ens3 = ens3->frereD;
				//affiche_ensemble(ensembletmp);printf("Elemenent le plus a droite\n");
				date = date_fin(ens3);
				
			}
			
			else// PAS DE CRISE
			{
				
				if(ens3 == NULL)//premier element de ens3
				{
					ens3=cree_ensemble(elemtmp->index,date);
				}
				else
				{
					ens3->frereD=cree_ensemble(elemtmp->index,date);
					ens3->frereD->frereG = ens3;
					ens3 = ens3->frereD;
				}
				elems= retire_element_i(elems,elemtmp->index);
				date+=TEMPS_TACHE;
				if(ens2->frereD)//Si il y a un prochain element
				{
					if(ens2->frereD->numero_element == -1)//et que c'est un ensemble
					{
						if(date>=ens2->frereD->temps_depart)//ET que ca INVADE
						{
							//printf("%d INVADE date %d\n",elemtmp->index,date);
							
							ens2->frereD->filsG = invade(ens2->frereD->filsG,touselems,date);
							
							ens2->frereD->temps_depart = ens2->frereD->filsG->temps_depart;
							
							
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
				
			}
			else
			{
				ens3->frereD = cree_ensemble(-1,ens2->temps_depart);
				ens3->frereD->filsG = cpyens(ens2->filsG);
				ens3->frereD->frereG = ens3;
				ens3 = ens3->frereD;
			}
			//affiche_ensemble(ens2);
			date = date_fin(ens3);//printf("fin = %d \n",date);

		}
		
		tmp = ens2;
		ens2 = ens2->frereD;
		free(tmp);
	}

	//on remet ens3 a son debut
	//affiche_ensemble(ens);printf(" -----------------------------\n");
	while(ens3->frereG)
		ens3 = ens3->frereG;
	Ensemble * newfils = cree_ensemble(-1,ens3->temps_depart);
	newfils->filsG = ens3; 
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

//Algo naif
int main()
{
	Element * elems = init_element();
	elems = ajoute_element(elems,0,15,74);
	elems = ajoute_element(elems,1,21,46);
	elems = ajoute_element(elems,2,2,60);
	elems = ajoute_element(elems,3,13,68);
	elems = ajoute_element(elems,4,4,34);

	
	
	Element *  elems2 = cpy_elems(elems);
	Element * tmp = elems2;
	affichejobs(elems2);
	int i;
	int date = 0;
	int a_scheduler = 5;
	Ensemble * ens = NULL;
	Ensemble * ensembletmp;
	while(a_scheduler != 0)//tant qu'on n'a pas schedul tous les elements
	{
		//printf("test %p\n",elems2);
		affichejobs(elems2);
		i = eligible(elems2,date);
		printf("choisi  = %d\n",i);
		if(i==-1)
		{
			printf("Failure 1\n");
			return 0;
		}
		tmp = get_element_i(elems2,i);
		if(tmp == NULL)
		{
			printf("Failure 0\n");
			return 0;
		}
		
		date = max(date,tmp->release);
		if(date+TEMPS_TACHE > tmp->deadline)//CRISIS
		{
			
			//printf("Crisis(main) sur la tache %d\n",tmp->index);
			Element * crisise = NULL;
			crisise = ajoute_element(crisise,tmp->index,tmp->release,tmp->deadline);
			elems2=retire_element_i(elems2,crisise->index);
			if(!elems2)
					elems2 = ajoute_element(elems2,99999,99999,999999);
			//affiche_ensemble(ens);printf(" Avant\n");
			ens = crisis(ens,crisise,elems2,elems);
			
			//affiche_ensemble(ens);printf(" Apres\n");
			if(ens == NULL)
			{
				
				printf(" NO PULL(%d) FOUND\n",tmp->index);
				return 0;
			}
			//on se remet a droite de l'ensemble
			ensembletmp = ens;
			while(ensembletmp->frereD)
				ensembletmp = ensembletmp->frereD;
			//affiche_ensemble(ensembletmp);printf("Elemenent le plus a droite\n");
			date = date_fin(ens);
			
			//printf("APRESCRISIS\n");
			//printf("Date %d : ",date);
			//affichejobs(elems2);
			//affiche_ensemble(ens);printf("\n\n\n");
		}
		else // pas crisis
		{
			//printf("AVANT eligible = %d\n",i);
			printf("Date %d : ",date);
			affichejobs(elems2);
			affiche_ensemble(ens);printf("\n");
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
			date+=TEMPS_TACHE;
		}
		
		
	
	}
	affiche_ensemble(ens);
	libereens(ens);
	freeelems(elems);
	freeelems(elems2);
	
	return 0;
}
