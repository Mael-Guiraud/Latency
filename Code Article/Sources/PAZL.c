#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>


#include "struct.h"
#include "tests.h"
#include "operations.h"
#define DEBUG 0

/* Structs For PAZL ------
------------------------*/


//Pour le new bruteforce
typedef struct{
	int *next;
	int margin;
}stack;


// Pour le old bruteforce
typedef struct
{
  int debut;
  int fin;
  int suivant;
} intervalle_liste;



/* ALGOS GREEDY POLYNOMIAUX
----------------------*/



//met toutes les cases du message a 1 dans la periode
void ecrire_message_periode(int * p,int periode, int debut, int fin)
{
	if(debut > fin)
	{
		for(int i=0;i<=fin;i++)p[i]=1;
		for(int i=debut;i<periode;i++)p[i]=1;
	}
	else
	{
		for(int i=debut;i<=fin;i++)p[i]=1;
	}
}


//retourne -1 si la periode donnée ne donne pas de solutions, la taille de la periode sinon
int algo_shortest_longest(Graphe g,int periode,int taille_message)
{
	if(!(g.N%2)){printf("G n'est peut être pas une étoile\n");exit(5);}
	int nb_routes = g.N/2;
	int min = g.matrice[nb_routes][nb_routes+1];
	int max = min;
	for(int i=1;i<nb_routes;i++)
	{
		if(g.matrice[nb_routes][i+nb_routes+1] < min)
			min = g.matrice[nb_routes][i+nb_routes+1];
		if(g.matrice[nb_routes][i+nb_routes+1] > max)
			max = g.matrice[nb_routes][i+nb_routes+1];
	}
	if(periode >= (taille_message * nb_routes + 2*(max-min) ) ) 
		return taille_message * nb_routes + 2*(max-min);
	 
	return -1;
}

//retourne -1 si la periode donnée ne donne pas de solutions, la taille de la periode sinon
int algo_3NT(Graphe g,int periode,int taille_message)
{
	if(!(g.N%2)){printf("G n'est peut être pas une étoile\n");exit(5);}
	int nb_routes = g.N/2;
	int routes[nb_routes];
	int nb_slots = periode / taille_message;
	int assigned = 0;
	for(int i=0;i<nb_routes;i++)
	{
		routes[i]=2*g.matrice[nb_routes][i+nb_routes+1];
	}

	int aller[nb_slots], retour[periode];
	for(int i=0;i<nb_slots;i++){aller[i]=0;}
	for(int i=0;i<periode;i++){retour[i]=0;}


	//Pour chaque route
	for(int i=0;i<nb_routes;i++)
	{
		//On recherche le depart le plus petit
		for(int j=0;j<nb_slots;j++)
		{
			if(!aller[j] && !retour[(j*taille_message+routes[i])%periode] && !retour[(j*taille_message+routes[i]+taille_message-1)%periode])
			{

				ecrire_message_periode(aller,nb_routes,j,j);
				ecrire_message_periode(retour,periode,(j*taille_message+routes[i])%periode,(j*taille_message+routes[i]+taille_message-1)%periode);
				assigned = 1;
				break;
			}
		}
		if(!assigned)//Si une route n'a pas trouvé de départ possible, c'est que la fenètre est trop petite.
			return -1;
		assigned = 0;
	}
	return periode;
}




//retourne -1 si la periode donnée ne donne pas de solutions, la taille de la periode sinon
int algo_prime(Graphe g,int periode,int taille_message)
{
	if(!(g.N%2)){printf("G n'est peut être pas une étoile\n");exit(5);}
	int nb_routes = g.N/2;
	int routes[nb_routes];
	int assigned = 0;
	for(int i=0;i<nb_routes;i++)
	{
		routes[i]=2*g.matrice[nb_routes][i+nb_routes+1];
	}

	int aller[periode], retour[periode];
	for(int i=0;i<periode;i++){aller[i]=0;retour[i]=0;}


	//Pour chaque route
	for(int i=0;i<nb_routes;i++)
	{
		//On recherche le depart le plus petit
		for(int j=0;j<periode;j++)
		{
			if(!aller[j] && !aller[(j+taille_message)%periode] && !retour[(j+routes[i])%periode] && !retour[(j+routes[i]+taille_message-1)%periode])
			{

				ecrire_message_periode(aller,periode,j,(j+taille_message-1)%periode);
				ecrire_message_periode(retour,periode,(j+routes[i])%periode,(j+routes[i]+taille_message-1)%periode);
				assigned = 1;
				break;
			}
		}
		if(!assigned)//Si une route n'a pas trouvé de départ possible, c'est que la fenètre est trop petite.
			return -1;
		assigned = 0;
	}
	//affiche_periode(aller,periode);
	//affiche_periode(retour,periode);
//	printf("%d \n",periode);
	return periode;
}




 /* Brute Force Rapide ----
 ------------------------*/
void graphe_to_temps_retour(Graphe g, int * tab,int periode)
{
	int nbr_route = g.N / 2;

	for (int i = 0; i < nbr_route; i++)
	  {
	    tab[i] = (2 * g.matrice[nbr_route][i + nbr_route + 1])%periode;
	  }

}


void print_solution(int *id, int *start_slot, int *return_slot,  int size, stack *fw, stack *bw){
	printf("Partial solution of size %d:\n", size);
	printf("Forward windows  (forward margin : %d): ",fw[size].margin);
	for(int i = 0; i <= size; i++){
		printf("[%d :(%d,%d)] ",id[i], start_slot[i],fw[size].next[i]);
	}
	printf("\n");
	printf("Backward windows  (backward margin : %d): ",bw[size].margin);
	for(int i = 0; i <= size; i++){
		printf("[%d :(%d,%d)] ",id[i], return_slot[i],bw[size].next[i]);
	}
	printf("\n");
}

int collision(int message_size, int period, int slot, int *messages, int level){ //slot is the number 
	//of the slot in the period in which the message is not stacked against another one
	if(slot > period - message_size) return 1; //special case of the first message
	int i;
	for(i = 0; i < level 
		&& (slot <= messages[i] - message_size 
		|| slot >= messages[i] + message_size ); i++){}
	return (i != level);
}

int research_interval(int slot, int *messages, int level, int *min, int *max){
	//return the index of the element before the one which is inserted
	int previous = 0;
	for(int i = 1; i < level; i++){
		if(slot > messages[i] && messages[i] > *min) {
			*min = messages[i];
			previous = i;
		}
		if(messages[i] > slot && messages[i] < *max) *max = messages[i];
	}
	return previous;
}


void update_solution(int *id, int* start_slot, int *return_slot,stack *fw, stack *bw,
 int added_route, int previous_route, int slot, int route_number, int period, int message_size, 
 int min, int max, int level, int previous_index){
	//update the solution
	id[level] = added_route;
	start_slot[level] = start_slot[previous_route] + message_size;
	return_slot[level] = slot;
	//update the two stacks
	for(int i = 0; i < level; i++){
		fw[level].next[i] = (fw[level-1].next[i] <= added_route) ? added_route + 1 :  fw[level-1].next[i];
		bw[level].next[i] = (bw[level-1].next[i] <= added_route) ? added_route + 1 :  bw[level-1].next[i];
	}
	//one cannot use previous route anymore
	fw[level].next[previous_route] = route_number;

	//compute whether we can put something before or after route_added
	bw[level].next[level] = (return_slot[level] + 2*message_size > max ) ? route_number : 1;
	if(min + 2*message_size > slot) bw[level].next[previous_index] = route_number;

	//find the message after the one we are inserting in the fw_period
	max = period;
	for(int i = 1; i < level; i++){
		if(start_slot[i] > start_slot[level] && start_slot[i] < max) max = start_slot[i];
	}
	// compute if there is some room after the message we have just inserted
	fw[level].next[level] = (max - start_slot[level] < 2*message_size) ? route_number : 1;
}

int recursive_search(int *id, int*start_slot, int *return_slot, stack *fw, stack* bw, int *unused_route, int level,
 int* return_time, int route_number, int message_size, int period){ //renvoie 1 si une solution a été trouvée, 0 sinon
	//level is the current depth in the search tree, it corresponds to solutions with level+1 elements 
	//print_solution(id, start_slot, return_slot,level-1, fw, bw);
	if(level == route_number ) return 1; //we have placed all messages
	//place a message next to another one in the forward windows
	int slot, min, max, previous_index;
	for(int i = 0; i < level; i++){//go through the  messages already placed 
		for(int j = fw[level-1].next[i]; j < route_number; j++){//add a message next to the message number i in the forward windows
			if(unused_route[j]){
				slot = start_slot[i] + message_size + return_time[j];
				if (slot > period) slot -= period; //put retour in the interval modulo period
				//test whether there is a collision
				if(collision(message_size, period, slot, return_slot, level)) continue;
				//compute the min and max
				min = 0; max = period;
				previous_index = research_interval(slot,return_slot,level,&min,&max);
				//update the bw_margin and skip this partial solution if it is smaller than 0	
				bw[level].margin = bw[level -1].margin + (max - slot)/message_size 
				+ (slot - min)/message_size - (max - min)/message_size;
				if(bw[level].margin < 0) continue;
				fw[level].margin = fw[level-1].margin; // this margin does not change
				update_solution(id, start_slot, return_slot,fw, bw, j, i, slot,route_number, period, message_size, min, max, level, previous_index); //update all informations in the stacks and solutions
				unused_route[j] = 0;
				if(recursive_search(id,start_slot,return_slot,fw,bw,unused_route,level+1,return_time,route_number, message_size,period)) return 1; // we have found a solution, exit
					//we have not found a solution go one level back in the tree
				unused_route[j] = 1;
			}
		}
		//place a message next to another one in the backward windows
		//same code but the  role of fw and bw variables are exchanged
		for(int j = bw[level-1].next[i]; j < route_number; j++){//add a message next to the message number i in the forward windows
			if(unused_route[j]){
				slot = return_slot[i] + message_size - return_time[j];
				if (slot < 0) slot += period; //put retour in the interval modulo period
				//test whether there is a collision
				if(collision(message_size, period, slot, start_slot,level)) continue;
				//compute the min and max
				min = 0; max = period;
				previous_index = research_interval(slot,start_slot,level,&min,&max);
				//update the bw_margin and skip this partial solution if it is smaller than 0	
				fw[level].margin = fw[level -1].margin + (max - slot)/message_size 
				+ (slot - min)/message_size - (max - min)/message_size;
				if(fw[level].margin < 0) continue;
				bw[level].margin = bw[level-1].margin; // this margin does not change
				update_solution(id, return_slot, start_slot,bw, fw, j, i, slot,route_number, period, message_size, min, max, level, previous_index); //update all informations in the stacks and solutions
				unused_route[j] = 0;
				if(recursive_search(id,start_slot,return_slot,fw,bw,unused_route,level+1,return_time,route_number, message_size,period)) return 1; // we have found a solution, exit
					//we have not found a solution go one level back in the tree
				unused_route[j] = 1;
			}
		}
	}
	return 0;
}


int search(Graphe g,int message_size, int period){
	

	int route_number = g.N/2;
	int return_time[route_number];

	graphe_to_temps_retour(g,return_time,period);
	if(DEBUG)printf("Instance aléatoire :\n");//we assume that the value in return time are in [0,period[
	if(DEBUG)for(int i = 0; i < route_number; i++) printf("%d  ",return_time[i]);
	int shift = return_time[0];
	if(DEBUG)printf("\nInstance aléatoire décalée :\n");
	for (int i = 0; i < route_number; i++){//we shift the values so that the first route has return time 0
		return_time[i] -= shift;
		if (return_time[i] < 0) return_time[i] += period;
		if(DEBUG)printf("%d  ",return_time[i]);
	}
	if(DEBUG)printf("\n");
    /* Memory allocation */		
	stack *fw = malloc(sizeof(stack)*route_number);
	stack *bw = malloc(sizeof(stack)*route_number);
	int *id = malloc(route_number*sizeof(int));
  	int *start_slot = malloc(route_number*sizeof(int));
  	int *return_slot = malloc(route_number*sizeof(int));
  	int *unused_route = malloc(route_number*sizeof(int));

  	for(int i = 0; i < route_number; i++){ 
  		fw[i].next = malloc(sizeof(int)*(i+1));
  		bw[i].next = malloc(sizeof(int)*(i+1));
  		unused_route[i] = 1;
  	}
  	/* Initialization, the first route is fixed */
  	
  	fw[0].next[0] = 1;
  	bw[0].next[0] = 1;
  	fw[0].margin = period /message_size - route_number;
  	bw[0].margin = fw[0].margin;
  	id[0] = 0;
  	start_slot[0] = 0;
  	return_slot[0] = 0;
  	
  	/* Call the recursive part with the proper algorithm */
  	int return_value = recursive_search(id, start_slot, return_slot, fw, bw, unused_route, 1, return_time, route_number, message_size, period);
  		//printf("Solution trouvée \n");
  		//print_solution(id, start_slot, return_slot,route_number-1, fw, bw);
  	/* Free the memory */

  	int mi[route_number];
  	int wi[route_number];
  	if(return_value)
  	{
 	if(DEBUG)print_solution(id, start_slot, return_slot, route_number-1, fw,bw);
  		//printf("\n");
  	//for (int i = 0; i < route_number; i++){printf("%d %d \n",id[i],start_slot[i]);}
  	//	affiche_etoile(g);
  	for (int i = 0; i < route_number; i++){wi[i]=0;mi[id[i]]=start_slot[i]-g.matrice[id[i]][route_number];}
  	 if(!is_ok( g,  message_size, mi,  wi, period))exit(16);
  }

  	free(id);
  	free(start_slot);
  	free(return_slot);
  	free(unused_route);
  	for(int i = 0; i < route_number ; i++) {
  		free(fw[i].next);
  		free(bw[i].next);
  	}
  	free(fw);
  	free(bw);
  	return return_value?1:-1;
}





/* Ancien BruteForce


-----------------------*/




//intervalle fermé à gauche et ouvert à droite
// le codage debut, taille serait sans doute légèrement mieux

intervalle_liste *initialise (int nbr_route, int taille_paquets, int periode)
{       //initialise les intervalles de temps restants pour le retour
  intervalle_liste *l = malloc (sizeof (intervalle_liste) * nbr_route);
  l[0].suivant = -1;
  l[0].debut = taille_paquets;
  l[0].fin = periode;
  return l;
}

void affiche_intervalle (intervalle_liste * liste)
{
  int pos = 0;
  while (pos != -1)
    {
      printf ("[%d,%d[  ", liste[pos].debut, liste[pos].fin);
      pos = liste[pos].suivant;
    }
  printf ("\n");
}


int ajoute_element (intervalle_liste * liste, int debut, int taille,
    int taille_paquets, int *nombre_slot, int nbr_route)
{       //renvoie 0 si l'insertion n'est pas possible, 1 sinon et modifie alors liste
  //pour couper en deux l'intervalle approprié
  int pos = 0;
  while (pos != -1)
    {       //détecte l'intervalle à découper si il existe
      if (liste[pos].debut <= debut
    && liste[pos].fin - debut >= taille_paquets)
    {     //l'intervalle existe, on le découpe
    //on calcule le nombre de slot après découpage (en prenant en compte les paquets déjà placés)
    *nombre_slot +=
      (debut - liste[pos].debut) / taille_paquets + (liste[pos].fin -
                 debut) /
      taille_paquets - (liste[pos].fin -
            liste[pos].debut) / taille_paquets;
    if (*nombre_slot >= nbr_route)
      {     // si le nombre de slots retour est insuffisant, on ne permet pas l'ajout de l'élément à cet endroit     
        liste[taille].debut = debut + taille_paquets;
        liste[taille].fin = liste[pos].fin;
        liste[taille].suivant = liste[pos].suivant;
        liste[pos].fin = debut;
        liste[pos].suivant = taille;
        return 1;
      }
    else
      {
        *nombre_slot += 1;
        return 0;
      }

    }
      pos = liste[pos].suivant;
    }
  return 0;
}

void
retire_element (intervalle_liste * liste, int debut, int taille,
    int *nombre_slot, int taille_paquets)
{       //enlève une route et fusionne les intervalles correspondants
  int i;
  for (i = 0; i < taille; i++)
    {
      if (liste[i].fin == debut)
  {
    break;
  }
    }       //cherche l'intervalle à fusionner, l'autre est en dernière position par construction
  *nombre_slot +=
    -(debut - liste[i].debut) / taille_paquets - (liste[taille].fin -
              debut) / taille_paquets +
    (liste[taille].fin - liste[i].debut) / taille_paquets;
  liste[i].suivant = liste[taille].suivant;
  liste[i].fin = liste[taille].fin;
}

int prochain_debut (intervalle_liste * liste, int debut, int taille, int taille_paquets)
{       //renvoie la prochaine position possible pour la route (décalage à la position actuelle donnée par debut)
  int pos = 0;
  while (liste[pos].suivant != -1){
    if (debut < liste[pos].fin){
      break;
    }
    pos = liste[pos].suivant;
  }       //trouve le premier intervalle qui peut contenir la route
  if (debut < liste[pos].debut)
    {       //debut avant l'intervalle
      if (liste[pos].fin - liste[pos].debut >= taille_paquets)
  {     //on met la route au début de l'intervalle si assez de place
    return liste[pos].debut - debut;
  }
    }
  else
    {
      if (debut < liste[pos].debut + taille_paquets)
  {     //debut dans une partie proche du début de l'intervalle 
    if (liste[pos].fin - liste[pos].debut >= 2 * taille_paquets)
      {     // Evite les intervalles inférieur à taille_paquet qui sont inutiles
        return liste[pos].debut + taille_paquets - debut;
      }
  }
      else
  {     //si il y a la place on décale juste de 1
    if (liste[pos].fin - debut >= taille_paquets)
      {
        return 1;
      }
  }
    }
  //dans les cas restants on renvoie le début du premier intervalle qui peut contenir une route

  while (liste[pos].suivant != -1)
    {
      pos = liste[pos].suivant;
      if (liste[pos].fin - liste[pos].debut >= taille_paquets)
  {
    return liste[pos].debut - debut;
  }
    }
  return liste[pos].fin - debut + taille_paquets; //on a pas trouvé de bon emplacement, on retourne au début des temps retour 
}


void
print_sol (int *solution_pos, int *solution_num, int nbr_route, int budget)
{
  printf ("Budget restant: %d \n", budget);
  for (int i = 0; i < nbr_route; i++)
    {
      printf ("(%d,%d) ", solution_num[i], solution_pos[i]);
    }
  printf ("\n");
}


//mettre debut_courant à la place de décalage_courant partout ? Ca pourrait simplifier un peu
//on pourrait utiliser un entier dont les bits représente les routes restantes et trouver la première route libre avec une instruction processeur spéciale

int bruteforceiter (Graphe g, int periode, int taille_paquets)
{
  long long int compteur = 0;

  if (!(g.N % 2))
    {
      printf ("G n'est peut être pas une étoile\n");
      exit (5);
    }

  int nbr_route = g.N / 2;

 

  int *solution_pos, *solution_num, *temps_retour, *route_restante;
  assert (solution_pos = malloc (sizeof (int) * nbr_route));
  assert (solution_num = malloc (sizeof (int) * nbr_route));
  assert (temps_retour = malloc (sizeof (int) * nbr_route));
  assert (route_restante = malloc (sizeof (int) * nbr_route));
  //affiche les données sur lesquelles on lance le bruteforce

    graphe_to_temps_retour(g,temps_retour,periode);

  for (int j = 1; j < nbr_route; j++)
    {
      route_restante[j] = 1;  //on pourrait juste virer la première route ?
 

    }


  /*
     for(int j = 0; j < nbr_route; j++){
     printf("%d(%d)",temps_retour[j],temps_retour_param[j]);
     }
     printf("\n"); */
  route_restante[0] = 0;
  solution_pos[0] = 0;
  solution_num[0] = 0;
  int solution_taille = 1;  //première route fixée
  int budget = periode - nbr_route * taille_paquets;
  int num_courant = 1, decalage_courant = 0;
  intervalle_liste *retour = initialise (nbr_route, taille_paquets, periode);
  int debut_retour, i;
  int nombre_slot = periode / taille_paquets;

  //normalise les temps retour en mettant le premier à 0
  for (int j = nbr_route - 1; j >= 0; j--)
    temps_retour[j] = (temps_retour[j] - temps_retour[0] + periode) % periode;
  //affiche les données sur lesquelles on lance le bruteforce
if(DEBUG)
{
 printf("\n");
  for(int j = 0; j < nbr_route; j++){
    printf("%d",temps_retour[j]);
  }
  printf("\n");
}
  /////////////////// Début de l'arbre de recherche ////////////////////////////


  while (compteur < 1000000000)
    {
      compteur++;
      if (solution_taille == nbr_route)
      {
       if(DEBUG) print_sol(solution_pos,solution_num,nbr_route,budget);
        //printf("PERIODE %d \n",periode);

       
        int mi[nbr_route];
        int wi[nbr_route];
          for (int i = 0; i < nbr_route; i++){wi[i]=0;mi[solution_num[i]]=solution_pos[i]-g.matrice[solution_num[i]][nbr_route];}
  		 if(!is_ok( g,  taille_paquets, mi,  wi, periode))exit(17);

  		 free (solution_num);
        free (solution_pos);
        free (route_restante);
        free (temps_retour);

        return periode;
      }     //sortir la solution et arrêt
      if (num_courant == nbr_route)
      {     //plus de route à utiliser, on revient en arrière et on utilise un début de route plus élevé
        solution_taille--;
        if (solution_taille == 0)
          {
            free (solution_num);
            free (solution_pos);
            free (route_restante);
            free (temps_retour);

            return -1;
          }
        num_courant = solution_num[solution_taille];
        route_restante[num_courant] = 1;
        debut_retour = solution_pos[solution_taille] + temps_retour[num_courant];
        if (debut_retour >= periode)
          {
            debut_retour -= periode;  // valeur au plus deux fois la période
          }
        //printf("Taille %d, debut %d \n",solution_taille, debut_retour);
        //affiche_intervalle(retour);
        retire_element (retour, debut_retour, solution_taille, &nombre_slot,
            taille_paquets);
        //affiche_intervalle(retour);
        decalage_courant = solution_pos[solution_taille] - solution_pos[solution_taille - 1] - taille_paquets;  //decalage de la route qu'on enlève
        //de la solution avec la précédente
        budget += decalage_courant;
        decalage_courant += prochain_debut (retour, debut_retour, solution_taille, taille_paquets); //calcul le prochain point ou on va placer le paquet de num_courant 
      }
      else
      {
        if (budget - decalage_courant < 0)
          {     // plus de budget pour la route, on essaye de mettre la route suivante à la place
            for (i = num_courant + 1; i < nbr_route && !route_restante[i];
           i++){}
            num_courant = i;
            decalage_courant = 0;
          }
        else
          {
                debut_retour =solution_pos[solution_taille - 1] + taille_paquets +  decalage_courant + temps_retour[num_courant];
                if (debut_retour >= periode)
              {
                debut_retour -= periode;  // valeur au plus deux fois la période
              }
                if (ajoute_element(retour, debut_retour, solution_taille, taille_paquets,&nombre_slot, nbr_route))
                { //on agrandit ici la solution partielle d'une nouvelle route
                  solution_pos[solution_taille] = solution_pos[solution_taille - 1] + decalage_courant + taille_paquets;
                  solution_num[solution_taille] = num_courant;
                  budget -= decalage_courant;
                  solution_taille++;
                  route_restante[num_courant] = 0;
                  for (i = 0; i < nbr_route && !route_restante[i]; i++)
                    {
                    }
                  num_courant = i;  //on utilise la première route libre comme nouveau candidat à placer
                  decalage_courant = 0; //avec un décalage de 0
                  //print_sol(solution_pos,solution_num,solution_taille,budget);
                  //affiche_intervalle(retour);
                }
                else
                {   //pas de place pour le retour, on essaye le décalage suivant.
                  decalage_courant += prochain_debut (retour, debut_retour, solution_taille,taille_paquets);
                }
          }
      }
    }
  //printf("Pas de solution \n");
  free (solution_num);
  free (solution_pos);
  free (route_restante);
  free (temps_retour);

  return -2;
}


/*RECHERCHES LINEAIRES 
TODO : pointeurs de fonctions ?
-----------------*/




int linear_3NT(Graphe g,int taille_message)
{
	int result;
	int nb_routes = g.N/2;
	for(int i =nb_routes*taille_message;i<taille_message*(nb_routes)*3;i+=2500)
	{
		result = algo_3NT(g,i,taille_message);
		if(result != -1)
		{
			return result;
		}
	}
	return -1;
}

int linear_prime(Graphe g,int taille_message)
{
	int result;
	int nb_routes = g.N/2;
	for(int i =nb_routes*taille_message;i<taille_message*(nb_routes)*3;i++)
	{
		result = algo_prime(g,i,taille_message);
		if(result != -1)
		{
			return result;
		}
	}
	return -1;
}



int linear_brute(Graphe g,int taille_message)
{
	int result;
	int nb_routes = g.N/2;
	for(int i =nb_routes*taille_message;i<taille_message*(nb_routes)*3;i++)
	{
		result = bruteforceiter(g,i,taille_message);
		if((result != -1)&&(result != -2))
		{
			return result;
		}
	}
	return -1;
}

int linear_search(Graphe g,int taille_message)
{
	int result;
	int nb_routes = g.N/2;
	for(int i =nb_routes*taille_message;i<taille_message*(nb_routes)*3;i++)
	{
		//printf(" search for %d (%d) \n",i,result);
		result = search(g, taille_message,i);
		if(result != -1)
		{
			return i;
		}
	}

	return -1;
}
