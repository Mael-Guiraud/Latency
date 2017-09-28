#include<stdio.h>
#include<stdlib.h>
#include <string.h>

//Liste d'adjacence pour les graphes
//les sommets ont comme voisinage les sommets voisins qui ont un indice plus petit
// arête orientée du sommet de plus grand indice vers le plus petit indice avec un poids entre 0 et windows-1
//pour plus d'efficacité l'arête dans l'autre sens devrait être présente avec un poids négatif 

typedef struct vertex{
  int edge_number;
  int *weight;
  int *neighboors;
} Vertex;
  
typedef struct graphe{
  Vertex* vertices;
  int vertex_number;
} Graphe;


void affiche_graphe(Graphe g){
  printf("Graphe avec %d sommets\n",g.vertex_number);
  for(int i=0; i <g.vertex_number;i++){
    printf("Vertex %d, degree %d: ",i,g.vertices[i].edge_number);
    for(int j=0; j < g.vertices[i].edge_number;j++){
      printf("(%d,%d)  ",g.vertices[i].neighboors[j],g.vertices[i].weight[j]);
    }
    printf("\n");
  }
}

Graphe complete_bipartite(int k,int l){//génère un K_{k,l}
  Graphe g;
  g.vertex_number = k + l;
  g.vertices = malloc((k+l)*sizeof(Vertex));
  for(int i=0;i<k;i++){
      g.vertices[i].edge_number = 0;
  }
  for(int i=k;i<k+l;i++){
      g.vertices[i].edge_number = k;
      g.vertices[i].neighboors= malloc(k*sizeof(int));
      g.vertices[i].weight= calloc(k,sizeof(int));
      for(int j=0;j<k;j++){
	g.vertices[i].neighboors[j]=j;
      }
  }
  return g;
}

int solve(Graphe g,int windows, int* sol){//optimisation, premier élément toujours à 0, car les solutions sont invariantes par addition
  int number = 1;//nombre d'élément fixés dans la solution
  int consistant;
  sol[1]=0;
  while(1){
    if(sol[number] == windows){//enlève un élément quand c'est nécessaire (plus d'élément à la position number possible)
      number --;
      if(number == 0){
	return 0;} //if we arrive here we have tried every possibility without success
      sol[number]++;
    }
    else{
      //ajoute un élément si possible
      while(1){
	consistant=1;
	for(int i=0; i< g.vertices[number].edge_number ; i++){//on teste si l'élément à la position number est consistant
	  //on sait que sol[g.vertices[number].edges[i]] est défini
	  if((sol[number]  + sol[g.vertices[number].neighboors[i]] == g.vertices[number].weight[i] + windows) ||(sol[number]  + sol[g.vertices[number].neighboors[i]] == g.vertices[number].weight[i])){
	    consistant = 0;
	    break;
	  }
	}
	if(consistant){
	  //si consistant on  valide l'élément et on incrémente number et on sort de la boucle
	  number++;
	  if(number == g.vertex_number){return 1;}//on a trouvé une solution,
	  sol[number] = 0;
	  break;
	}
	else{
	  sol[number]++;//si non consistant on incrémente l'élément
	  if(sol[number] == windows){	//si aucun élément ne convient on sort de la boucle 
	    break;
	  }
	}
      }
    }
  }
}


Graphe* copy_graphe(Graphe *g){
  return NULL;
}

//problème avec les arêtes dans un seul sens
int* kernel(Graphe *g_in, Graphe *g_out,int degree){ //kernelise en enlevant les sommets de petit degré et renvoie la correspondance entre g_in et g_out
  g_out = copy_graphe(g_in);
  while(1)//remove vertices while they are of degree less than degree
    {
      for(int i = 0; i< g_out->vertex_number;i++){
	if(g_out->vertices[i].edge_number < degree && g_out->vertices[i].edge_number !=0){
	  for(int j = 0; j < g_out->vertices[i].edge_number;j++){
	    
	  }
	  g_out->vertices[i].edge_number = 0;
	}
      }
      break;//no more vertex to remove
    }
  return NULL;
  
}

int* degree_order(Graphe g_in, Graphe g_out){ //ordre d'élimination à degré croissant, renvoie la correspondance entre g_in et g_out 
  return NULL;
  
}

int worst_constraints(Graphe g, int windows){
  //énumère tous les poids possibles entre 0 et windows-1 pour les arêtes du graphe
  //pour améliorer ça il faudrait ne pas faire des coloriages isomorphes et invariants par somme de la même constante partout
  //on peut fixer la valeur d'une arête par sommet sans perte de généralité
  long long int compteur =1;
  int i=0,j=0;//i : current vertex number, j: current edge number
  int* buffer= calloc(g.vertex_number,sizeof(int));//utilisé pour stocker les solutions dans le solveur
  if(!solve(g,windows,buffer)){
    affiche_graphe(g);
    printf("Pas de solution avec une fenêtre de %d",windows);
    free(buffer);
    return 0;
  }
  while(i < g.vertex_number){
    j=1;//the first edge of a vertex has always weight 0
    while(j < g.vertices[i].edge_number){
      if(g.vertices[i].weight[j] == windows-1){
	g.vertices[i].weight[j] = 0;
	j++;
      }
      else{
	g.vertices[i].weight[j]++;
	compteur++;
	if(compteur%1000000 == 0) printf("%lld graphes testés\n",compteur);
	if(!solve(g,windows,buffer)){
	  affiche_graphe(g);
	  printf("Pas de solution avec une fenêtre de %d\n",windows);
	  free(buffer);
	  return 0;
	}
	i=0;
      }
    }
    i++;
  }
  free(buffer);
  return 1;
}

int worst_bipartite(int k, int l,int windows){//copie de l'algo précédent avec un peu plus de symétries
  Graphe g = complete_bipartite(k,l);
  //affiche_graphe(g);
  long long int compteur =1;
  int i,j,edge,temp;//i : current vertex number, j: current edge number
  int* buffer= calloc(g.vertex_number,sizeof(int));//utilisé pour stocker les solutions dans le solveur
  while(g.vertices[k].weight[g.vertices[k].edge_number-1]< windows-1){//on peut numéroter abritairement les éléments connectés au premier sommet -> poids croissants sur les arêtes
    edge = 1;
    while(g.vertices[k].weight[edge] == windows-1){
      edge++;
    }
    g.vertices[k].weight[edge]++;
    temp = g.vertices[k].weight[edge];
    for(int cmpt = edge-1; cmpt > 0; cmpt--){
      g.vertices[k].weight[cmpt]= temp;
    }
    /*for(int cmpt =0; cmpt < g.vertices[k].edge_number; cmpt++){
      printf("%d ",g.vertices[k].weight[cmpt]);
    }
    printf("\n");*/
    i = k+1;
    if(!solve(g,windows,buffer)){
      affiche_graphe(g);
      printf("Pas de solution avec une fenêtre de %d",windows);
      free(buffer);
      return 0;
    }
    while(i < g.vertex_number){ 
      j=1;//the first edge of a vertex has always weight 0
      while(j < g.vertices[i].edge_number){
	if(g.vertices[i].weight[j] == windows-1){
	  g.vertices[i].weight[j] = 0;
	  j++;
	}
	else{
	  g.vertices[i].weight[j]++;
	  compteur++;
	  if(compteur%100000 == 0) printf("%lld graphes testés\n",compteur);
	  if(!solve(g,windows,buffer)){
	    affiche_graphe(g);
	    printf("Pas de solution avec une fenêtre de %d\n",windows);
	    free(buffer);
	    return 0;
	  }
	  i=k+1;
	}
      }
      i++;
    }
  }
  free(buffer);
  return 1;
 
}

//todo générateur de graphe aléatoire
//todo vérifier correction de génération des poids + option pour poids binaire uniquement

int main(){
  //Graphe g = complete_bipartite(3,6);
  //affiche_graphe(g);
  //worst_constraints(g,4);
  worst_bipartite(4,6,4);
}

