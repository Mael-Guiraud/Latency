#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NBR_ROUTE 7
#define TAILLE_PAQUET 2558
#define PERIODE 13000
#define TAILLE_ROUTE 2000


typedef struct{
  int debut;
  int fin;
  int suivant;
}intervalle_liste;
//intervalle fermé à gauche et ouvert à droite
// le codage debut, taille serait sans doute légèrement mieux

intervalle_liste *initialise(int nbr_route,int taille_paquet, int periode){//initialise les intervalles de temps restants pour le retour
  intervalle_liste* l= malloc(sizeof(intervalle_liste)*nbr_route);
  l[0].suivant = -1;
  l[0].debut = taille_paquet;
  l[0].fin = periode;
  return l;
} 

void affiche_intervalle(intervalle_liste *liste){
  int pos = 0;
  while(pos != -1 ){
    printf("[%d,%d[  ",liste[pos].debut,liste[pos].fin);
    pos = liste[pos].suivant;
  }
  printf("\n");
}


int ajoute_element(intervalle_liste *liste, int debut, int taille, int taille_paquet, int *nombre_slot, int nbr_route){//renvoie 0 si l'insertion n'est pas possible, 1 sinon et modifie alors liste
  //pour couper en deux l'intervalle approprié
  int pos=0;
  while(pos != -1){//détecte l'intervalle à découper si il existe
    if(liste[pos].debut <= debut && liste[pos].fin - debut >=  taille_paquet){//l'intervalle existe, on le découpe
      //on calcule le nombre de slot après découpage (en prenant en compte les paquets déjà placés)
      *nombre_slot+= (debut - liste[pos].debut)/taille_paquet +  (liste[pos].fin - debut)/taille_paquet -  (liste[pos].fin - liste[pos].debut)/taille_paquet;
      //if((debut - liste[pos].debut)/taille_paquet +  (liste[pos].fin - debut)/taille_paquet -  (liste[pos].fin - liste[pos].debut)/taille_paquet){printf("Décrémente: %d\n",*nombre_slot);}
      //else{printf("Décrémente pas %d\n",*nombre_slot);}
      if(*nombre_slot >= nbr_route){// si le nombre de slots retour est insuffisant, on ne permet pas l'ajout de l'élément à cet endroit     
      liste[taille].debut = debut + taille_paquet;
      liste[taille].fin = liste[pos].fin;
      liste[taille].suivant = liste[pos].suivant;
      liste[pos].fin = debut;
      liste[pos].suivant = taille;
      return 1;
      }
      else{
	*nombre_slot+=1;
	return 0;}
    }
    pos = liste[pos].suivant;
  }
  return 0;
}

void retire_element(intervalle_liste *liste, int debut, int taille, int *nombre_slot,int taille_paquet){//enlève une route et fusionne les intervalles correspondants
  int i;
  for (i = 0; i< taille; i++){
    if(liste[i].fin == debut){break;}
  }//cherche l'intervalle à fusionner, l'autre est en dernière position par construction

 *nombre_slot+= - (debut - liste[i].debut)/taille_paquet - (liste[taille].fin - debut)/taille_paquet + (liste[taille].fin - liste[i].debut)/taille_paquet; 

  liste[i].suivant = liste[taille].suivant;
  liste[i].fin = liste[taille].fin;

}
  
int prochain_debut(intervalle_liste *liste, int debut, int taille, int taille_paquet){//renvoie la prochaine position possible pour la route (décalage à la position actuelle donnée par debut)
  int pos=0;
  while(liste[pos].suivant != -1){
    if(debut < liste[pos].fin){break;}
    pos = liste[pos].suivant;
  }//trouve le premier intervalle qui peut contenir la route
  if(debut < liste[pos].debut){ //debut avant l'intervalle
    if(liste[pos].fin - liste[pos].debut >= taille_paquet){//on met la route au début de l'intervalle si assez de place
      return liste[pos].debut - debut; 
    }
  }
  else{
    if(debut < liste[pos].debut + taille_paquet){//debut dans une partie proche du début de l'intervalle 
      if(liste[pos].fin - liste[pos].debut >= 2*taille_paquet){// Evite les intervalles inférieur à taille_paquet qui sont inutiles
	return liste[pos].debut + taille_paquet - debut;
      }
    }
    else{ //si il y a la place on décale juste de 1
      if(liste[pos].fin - debut >= taille_paquet) {return 1;}
    }
  }
  //dans les cas restants on renvoie le début du premier intervalle qui peut contenir une route
  
  while(liste[pos].suivant != -1){
    pos = liste[pos].suivant;
    if(liste[pos].fin -liste[pos].debut >= taille_paquet) {return liste[pos].debut - debut; }
  }
  return liste[pos].fin - debut + taille_paquet;//on a pas trouvé de bon emplacement, on retourne au début des temps retour 
}


int* genere_reseau(int nbr_route, int taille_route){
  int *temps_retour = malloc(sizeof(int)*nbr_route);
  for(int i=0; i<nbr_route; i++){
    temps_retour[i] = rand()%taille_route;
  }
  return temps_retour;
}

typedef struct stack_route{int numero_route, position_route, budget, prochain_retour;} stack_route;


void print_sol(stack_route *stack, int nbr_route){
  printf("Budget restant: %d \n",stack[nbr_route-1].budget);
  for(int i = 0; i < nbr_route;i++){
    printf("(%d,%d) ",stack[i].numero_route,stack[i].position_route);
  }
  printf("\n");
}


//mettre debut_courant à la place de décalage_courant partout ? Ca pourrait simplifier un peu
//on pourrait utiliser un entier dont les bits représente les routes restantes et trouver la première route libre avec une instruction processeur spéciale

int bruteforce(int taille_paquet, int periode, int nbr_route, int* temps_retour){

  int *route_restante = malloc(sizeof(int)*nbr_route);
  stack_route *stack =  malloc(sizeof(stack_route)*nbr_route);
  intervalle_liste *retour = initialise(nbr_route,taille_paquet,periode);
  
  for(int j=1; j <nbr_route;j++) route_restante[j]=1; //on pourrait juste virer la première route ?
  route_restante[0] = 0;
  stack[0].numero_route = 0;
  stack[0].position_route = 0;
  stack[0].budget = periode - nbr_route*taille_paquet;
  stack[0].prochaine_route = taille_paquet;

  
  int solution_taille = 1;//première route fixée
  int num_courant = 1, decalage_courant = 0; //à commenter
  int debut_retour,i;
  int nombre_slot = periode/taille_paquet;

  /*  for(int j = 0; j < nbr_route; j++){
    printf("%d ",temps_retour[j]);
  }
  printf("\n");
  */
  //normalise les temps retour en mettant le premier à 0
  for(int j=nbr_route-1; j>=0;j--) temps_retour[j]= (temps_retour[j] - temps_retour[0] + periode)%periode;
  //affiche les données sur lesquelles on lance le bruteforce
  /*printf("\n");
  for(int j = 0; j < nbr_route; j++){
    printf("%d ",temps_retour[j]);
  }
  printf("\n");*/
  

  /////////////////// Début de l'arbre de recherche ////////////////////////////

  while(solution_taille > 0){
    if(solution_taille == nbr_route) {
      print_sol(stack);
      affiche_intervalle(retour);
      return 1;} //sortir la solution et arrêt
    if(num_courant == nbr_route){//plus de route à utiliser, on revient en arrière et on utilise un début de route plus élevé
      solution_taille--;
      num_courant = stack[solution_taille].numero_route;
      route_restante[num_courant] = 1;
      debut_retour = stack[solution_taille].prochain_retour;
      if(debut_retour >= periode) {
	debut_retour -= periode;// valeur au plus deux fois la période
      }
      //printf("Taille %d, debut %d \n",solution_taille, debut_retour);
      //affiche_intervalle(retour);
      retire_element(retour,debut_retour,solution_taille,&nombre_slot,taille_paquet);
      //affiche_intervalle(retour);
      decalage_courant = stack[solution_taille].prochain_retour; //decalage de la route qu'on enlève
      //de la solution avec la précédente
      budget = stack[solution_taille].budget;
    }
    else{
      if(budget - decalage_courant < 0){// plus de budget pour la route, on essaye de mettre la route suivante à la place
	for(i = num_courant+1; i<nbr_route && !route_restante[i];i++){}
	num_courant = i;
	decalage_courant = 0;
      }
      else{
	debut_retour = 


	  /*solution_pos[solution_taille-1] + taille_paquet + decalage_courant + temps_retour[num_courant];
	if(debut_retour >= periode) {
	  debut_retour -= periode; // valeur au plus deux fois la période
	  }*/
	if(ajoute_element(retour,debut_retour,solution_taille,taille_paquet,&nombre_slot,nbr_route)){//on agrandit ici la solution partielle d'une nouvelle route
	  solution_pos[solution_taille] = solution_pos[solution_taille-1] + decalage_courant + taille_paquet;
	  solution_num[solution_taille] = num_courant;
	  budget -= decalage_courant;
	  solution_taille++;
	  route_restante[num_courant] = 0;
	  for(i=0; i < nbr_route && !route_restante[i];i++){}
	  num_courant = i; //on utilise la première route libre comme nouveau candidat à placer
	  decalage_courant = 0;//avec un décalage de 0
	  //print_sol(solution_pos,solution_num,solution_taille,budget);
	  //affiche_intervalle(retour);
	}
	else{ //pas de place pour le retour, on essaye le décalage suivant.
	  decalage_courant+= prochain_debut(retour,debut_retour,solution_taille,taille_paquet);
	}
      }
    }
  }

  printf("Pas de solution \n");
  return 0;
}


int main(){
  srand(time(NULL));
  //int *temps_retour = genere_reseau(NBR_ROUTE,TAILLE_ROUTE);

  /*intervalle_liste *liste = initialise(6,2000,16000);
  int nbr_slot = 8;
  ajoute_element(liste, 5000, 1, 2000,&nbr_slot,6);
  affiche_intervalle(liste);
  printf("Nbr slot libre %d \n",nbr_slot);
  ajoute_element(liste, 8900, 2, 2000,&nbr_slot,6);
  affiche_intervalle(liste);
  printf("Nbr slot libre %d \n",nbr_slot);
  ajoute_element(liste, 13800, 3, 2000,&nbr_slot,6);
  affiche_intervalle(liste);
  printf("Nbr slot libre %d \n",nbr_slot);
  ajoute_element(liste, 7300, 4, 2000,&nbr_slot,6);
  affiche_intervalle(liste);
  printf("Nbr slot libre %d \n",nbr_slot);
  retire_element(liste,13800,3,&nbr_slot,2000);
  affiche_intervalle(liste);
  printf("Nbr slot libre %d \n",nbr_slot);
  retire_element(liste,8900,2,&nbr_slot,2000);
  affiche_intervalle(liste);
  printf("Nbr slot libre %d \n",nbr_slot);
  printf("Prochain %d \n",prochain_debut(liste, 9200, 4,2000));*/

  int temps_retour[6] = {178,344,456,872,924,1272};
  for(int i = 6*2558; i < 12*2558;i++){
    
    temps_retour[3]= 178;
    temps_retour[4]= 344;
    temps_retour[1]= 456;
    temps_retour[5]= 872;
    temps_retour[2]= 924;
    temps_retour[0]= 1272;
    if(bruteforce(2558,i,6,temps_retour)) {printf("Taille de la fenêtre %d \n",i); break;}
  }
  //bruteforce(TAILLE_PAQUET,PERIODE,NBR_ROUTE,temps_retour);


}
