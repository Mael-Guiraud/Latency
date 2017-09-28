// TOUTES LES PETITES FONCTIONS DE CALCULS DIVERSES
#include "graph.h"

//renvoie un entier aléatoire entre 0 et n
long int rand_entier(int n){
	
	return random()/(double) RAND_MAX *(n+1);
}

//renvoie la distance du début de la route jusqu'au sommet numero v
int distance(Route r, int v)
{
	//petit correctif
	if(v == r.route_lenght)
		v--;
	int i,j;
	int sum=0;
	for(i=v;i>0;i--)
	{
		for(j=0;j<r.vertices[i].edge_number;j++)
		{
			if(r.vertices[i-1].index==r.vertices[i].neighboors[j])
			{
				sum += r.vertices[i].weight[j];
			}
		}
	}
return sum;
}

//Retourne le numero du sommet sur la route, -1 si le sommet n'est pas sur la route
int vertex_index(Route r, Vertex v)
{
	int i;
	for(i=0;i<r.route_lenght;i++)
	{
		if(r.vertices[i].index == v.index)
			return i;
	}
	return -1;
}

//Retourne la route la plus longue dans une liste de route
int longest(Route * r,int taille)
{
	int i;
	int max = distance(r[0],r[0].route_lenght);
	int indice_max = 0;
	for(i=1;i<taille;i++)
	{
		if(distance(r[i],r[i].route_lenght) > max)
		{
			max = distance(r[i],r[i].route_lenght);
			indice_max = i;
		}
		
	}
	return indice_max;
}

//Retourne la route qui a le chemin le plus long du switch vers les sources
int longest_on_sources(Route * r,int taille)
{
	int i;
	int max = distance(r[0],1);
	int indice_max = 0;
	for(i=1;i<taille;i++)
	{
		if(distance(r[i],1) > max)
		{
			max = distance(r[i],1);
			indice_max = i;
		}
		
	}
	return indice_max;
}

//Retourne la route qui a le chemin le plus court du switch vers les sources
int shortest_on_sources(Route * r,int taille)
{
	int i;
	int min = distance(r[0],1);
	int indice_min = 0;
	for(i=1;i<taille;i++)
	{
		if(distance(r[i],1) < min)
		{
			min = distance(r[i],1);
			indice_min = i;
		}
		
	}
	return indice_min;
}

//Retourne le max tableau d'int
int greater(int * tab,int taille)
{
	int i;
	int max = tab[0];
	int indice_max = 0;
	for(i=1;i<taille;i++)
	{
		if(tab[i] > max)
		{
			max = tab[i];
			indice_max = i;
		}
		
	}
	return indice_max;
}

//Retourne l'indice du min d'un tableau d'int
int lower(int * tab,int taille)
{
	int i;
	int min = tab[0];
	int indice_min = 0;
	for(i=1;i<taille;i++)
	{
		if(tab[i] < min)
		{
			min = tab[i];
			indice_min = i;
		}
		
	}
	return indice_min;
}

//test si un tableau de bool est rempli de 1 (retourne 1 si oui, 0 sinon)
int full(int * tab, int taille)
{
	int i;
	int ack=1;
	for(i=0;i<taille;i++)
	{
		if(tab[i] == 0)
			ack = 0;
	}
	return ack;
}

//return factorielle a
int factorielle(int a)
{
	if(a==1)
		return a;
	else
		return (a*factorielle(a-1));
}

int max(int a, int b)
{
	if(a>b)
		return a;
	return b;
}
int min(int a,int b)
{
	if(a<b)
		return a;
	return b;
}
//additionne les valeurs de deux tableaux de meme taille
void ajoutetab(int * t1, int * t2, int taille)
{
	int i;
	for(i=0;i<taille;i++)
	{
		t1[i]+=t2[i];
	}
}

//renvoie la taille d'une fenetre a partir du tableau avec les arrivées de chacuns.
int taille_fenetre(int * tab, int taille)
{
	
	int min = tab[0];
	int max = tab[0];
	int i;
	for(i=1;i<taille;i++)
	{
		if(tab[i] > max)
		{
			max = tab[i];
		}
		
		if(tab[i] < min)
		{
			min = tab[i];
		}
	}
	return (max-min+taille_paquet);
}

//Renvoie le Tmax dans une graphe g avec un twowaytrip assigné t
int tMax(Graphe g,TwoWayTrip t)
{
	int i;
	//afficheTwoWayTrip(t);
	//affiche_graphe(g);
	int max = 2*distance(g.routes[0],g.routes[0].route_lenght)+t.W[0];
	for(i=0;i<t.taille;i++)
	{
		if(2*distance(g.routes[i],g.routes[i].route_lenght)+t.W[i] > max)
			max = 2*distance(g.routes[i],g.routes[i].route_lenght)+t.W[i];
	}
	return max;
}


//Renvoie le Tmax dans une graphe g avec un twowaytrip assigné t avec les buffers
int tMax_random(Graphe g,TwoWayTrip t)
{
	int i;
	//afficheTwoWayTrip(t);
	//affiche_graphe(g);
	if(!t.buffer)return-1;
	
	int max = 2*distance(g.routes[0],g.routes[0].route_lenght)+t.W[0]+t.buffer[0];
	for(i=0;i<t.taille;i++)
	{
		
		if(2*distance(g.routes[i],g.routes[i].route_lenght)+t.W[i]+t.buffer[i] > max)
			max = 2*distance(g.routes[i],g.routes[i].route_lenght)+t.W[i]+t.buffer[i];
	}
	return max;
}

int indiceTMax(Graphe g,TwoWayTrip t)
{
	int i;
	int max = 2*distance(g.routes[0],g.routes[0].route_lenght)+t.W[0];
	int indicemax = 0 ;
	for(i=1;i<t.taille;i++)
	{
		//printf("%d i= %d %d \n",max,i,2*distance(g.routes[i],g.routes[i].route_lenght)+t.W[i]);
		if(2*distance(g.routes[i],g.routes[i].route_lenght)+t.W[i] > max)
		{
			max = 2*distance(g.routes[i],g.routes[i].route_lenght)+t.W[i];
			indicemax = i;
			
		}
	}
	return indicemax;
}


//retourne la route la plus longue parmis celles qui peuvent partir a un offset donnée
int longerOfeEligible(int * Dl, int * lambda, int * arrivee,int offset, int taille)
{
	int i;
	int max;
	int indice_max;

	//boucle pour trouver la premire valeure eligible
	for(i=0;i<taille;i++)
	{
		if(arrivee[i]+lambda[i] <= offset)
		{
			if(Dl[i] !=0)
			{
				max = Dl[i];
				indice_max = i;
				break;
			}
		}
		max = -1;
		indice_max = -1;
	}

	for(i=0;i<taille;i++)
	{
		if(arrivee[i]+lambda[i] < offset)
		{
			if(Dl[i]>max)
			{
				max = Dl[i];
				indice_max=i;
			}
		}
	}
	return indice_max;
}

//retourne 1 si il y a collision dans un neoud entre deux paquets,
// dont les offsets d'arrivé dans le noeud sont a et b, 0 sinon
int collision_a_b(int a, int b, int P)
{
	int bfin = (b+taille_paquet)%P;
	int afin = (a+taille_paquet)%P;
	//si erreur
	if(abs(a - b) < taille_paquet)
	{
		//printf("1 %d %d \n",a,b);
		return 1;
	}
	if((a<b) && (afin>bfin) && (a<bfin))
	{
		//printf("2 %d %d %d %d\n",a,b,afin,bfin);
		return 1;
	}	
	if((a>b) && (bfin>afin) && (b<afin))
	{
		
		//printf("3 %d %d %d %d\n",a,b,afin,bfin);
		return 1;
	}
	//printf("OK\n");
	return 0;
}


//retourne 1 si l'offset de depart j collisionne soit a l'aller soit au retour avec les autres routes
int test_collisions_offset(int * retour, RouteStar r, int indice, int j,int P)
{
	int i;
	for(i=0;i<r.nb_routes;i++)
	{
		if((retour[i] != -1)&&(indice != i)) //test a n'effectuer que si l'offset existe, sinon ca n'a pas de sens
		{
			if(collision_a_b(retour[i],(j+2*r.y[indice])%P,P))
			{
				return 1;
			}
		}
		
	}
	//printf("----------------------------------------------------------------------\n");
	return 0;
}

//return the number of collisions in a table representing the window P in a node
int test_collisions(int * tab, int taille, int P)
{
	int i,j;
	int err = 0;
	for(i=0;i<taille-1;i++)
	{
		for(j=i+1;j<taille;j++)
		{
			if(collision_a_b(tab[i],tab[j],P))
			{
				//printf("collision entre %d(%d) et %d(%d)\n",i,tab[i],j,tab[j]);
				err++;
			}
		}
	
	}
	return err;
}

//renvoie l'indice du ieme element pas a -1 d'un tableau
int i_dispo(int * tab, int taille, int i)
{
	int j;
	int compteur = 0;
	for(j=0;j<taille;j++)
	{
		if(tab[j] != -1)
		{
			compteur++;
			if(i == compteur)
				return j;
		}
	}
	return -1;
}

//copie tab 1 dans tab 2
int * cpy_tab(int * tab1, int* tab2, int taille)
{
	int i;
	for(i=0;i<taille;i++)
	{
		tab2[i] = tab1[i];
	}
	return tab2;
}

RouteStar cree_routestar(Graphe g)
{
	RouteStar r;
	Graphe gr = renverse(g);
	r.nb_routes = g.sources;
	r.x = malloc(sizeof(int)*g.sources);
	r.y = malloc(sizeof(int)*g.sources);
	int i;
	for(i=0;i<g.sources;i++)
	{
		r.x[i] = distance(g.routes[i],1);
		r.y[i] = distance(gr.routes[i],1);
	}
	
	freeGraphe(gr);
	return r;
}

//return 1 if [a;b] is free in tab
int rentre_dans(int * tab, int a, int b)
{
	if((tab[a] == 0) && (tab[b] == 0))
	{
		return 1;
	}
	return 0;
	
}

int decaler_a(int a)
{
	int i;
	for(i=0;i<taille_paquet;i++)
	{
		if(a%taille_paquet != 0)
			a++;
		else
			return i;
	}
	return i;
}

//met des 1 de a à b
void occuper_p(int * tab, int a, int b)
{
	int i;
	for(i=a;i<b+1;i++)
		tab[i] = 1;
		
}

void freeGraphe(Graphe g)
{
	int i;
	for(i=0;i<g.vertex_number;i++)
	{
		if(g.vertices[i].weight != NULL)
		{
			//printf("free de %p\n",g.vertices[i].weight);
			free(g.vertices[i].weight);
		}
		
		if(g.vertices[i].neighboors != NULL)
		{
			
			//printf("free de %p\n",g.vertices[i].neighboors);
			free(g.vertices[i].neighboors);
		}

	}
	for(i=0;i<g.sources;i++)
	{
		free(g.routes[i].vertices);
		g.routes[i].vertices = NULL;
	}
	free(g.vertices);
	g.vertices = NULL;
	free(g.routes);
	g.routes = NULL;
}
void freeTwoWayTrip(TwoWayTrip t)
{
	free(t.M);
	t.M = NULL;
	free(t.W);
	t.W = NULL;
}
void freeRouteStar(RouteStar r)
{
	free(r.x);
	r.x = NULL;
	free(r.y);
	r.y = NULL;
}
//transforme un graphe en un tableau de temps de retour
int * graphe_to_temps_retour(Graphe g)
{
	int * tab = malloc(sizeof(int)*g.sources);
	int i;
	Graphe gr = renverse(g);
	for(i=0;i<g.sources;i++)
	{
		tab[i]=2*distance(gr.routes[i],1);
	}
	freeGraphe(gr);
	return tab;
}












