#include "graph.h"
void afficheTwoWayTrip(TwoWayTrip t)
{
	int i;
	for(i=0;i<t.taille;i++)
	{
		printf("Route %d : m = %d, w = %d\n",i,t.M[i],t.W[i]);
	}
	
}
void affiche_graphe(Graphe g){
  printf("Graphe avec %d sommets\n",g.vertex_number);
  int i,j;
  for(i=0; i <g.vertex_number;i++){
    printf("Vertex %d(%d), degree %d: ",g.vertices[i].index,i,g.vertices[i].edge_number);
    for(j=0; j < g.vertices[i].edge_number;j++){
      printf("(%d,%d)  ",g.vertices[i].neighboors[j],g.vertices[i].weight[j]);
    }
    printf("\n");
  }
  printf("Routes : \n");
  for(i=0;i<g.sources;i++)
  {
	  printf("Route %d (taille %d) : ",i,distance(g.routes[i],g.routes[i].route_lenght));
	  for(j=0;j<g.routes[i].route_lenght;j++)
	  {
		  printf("%d, ",g.routes[i].vertices[j].index);
	  }
	  printf("\n");
  }
  printf("La route la plus longue est la route %d\n",longest(g.routes,g.sources));
}
void afficheMatrice(int** m, int taille1, int taille2)
{
	printf("Début de la matrice\n");
	int i,j;
	for(i=0;i<taille1;i++)
	{
		for(j=0;j<taille2;j++)
		{
			printf("%d ",m[i][j]);
		}
		printf("\n");
	}
}
int available(int** m, int taille1, int taille2)
{
	int i,j;
	int err = 0;
	for(j=1;j<taille2-1;j++)
	{
		for(i=0;i<taille1-1;i++)
		{
			if(fabs((double)m[i][j] - (double)m[i+1][j]) <2500)
			{
				printf("Collision entre (%d,%d):%d et (%d,%d)%d\n",i,j,m[i][j],i+1,j,m[i+1][j]);
				err ++;
			}
		}
	}
	return err;
}
//return number collsions
int test_collisions(int * tab, int taille)
{
	int i,j;
	int err = 0;
	for(i=0;i<taille-1;i++)
	{
		for(j=i+1;j<taille;j++)
		{
			if(fabs((double)tab[i] - (double)tab[j]) < 2500)
			{
				//printf("Collision entre %d:%d et %d:%d\n",i,tab[i],j,tab[j]);
				err ++;
			}
				
		}
	}
	return err;
}

void creationfichierWindow(int* tab, int taille,char * nom){

	FILE *f;
	f=fopen(nom,"w+");

	int i;
	for(i=0; i <taille;i++){
		fprintf(f,"| %d -(ROUTE %d)- %d |",tab[i],i,tab[i]+2500);
		
	}
	fclose(f);
	
}
void creationfichierResultats(int i, int tmax,int window,char * nom){

	FILE *f;
	f=fopen(nom,"a");
	fprintf(f,"%d %d %d\n",i,tmax,window);
	fclose(f);
	
}

void simulation(int mode)
{
	Graphe g = topologie1(7,7,mode);
	ecrire_fichierGraph(g);
	//printf("-------------------G-------------\n");
	affiche_graphe(g);
	
	//printf("-------------------Gr-------------\n");
	Graphe gr = renverse(g);
	//affiche_graphe(gr);
	TwoWayTrip t = heuristique_top_1(g,30000);
	//printf("----------2way------\n");
	afficheTwoWayTrip(t);
	int i;
	
	int arrivee[gr.sources];
	int collisions[g.sources];
	for(i=0;i<g.sources;i++)
	{
		collisions[i] = t.M[i]+distance(g.routes[i],1);
		arrivee[i] = t.M[i]+distance(g.routes[i],g.routes[i].route_lenght);
	}
	creationfichierWindow(collisions, g.sources,"Aller.txt");
	//printf("ARRIVEE\n");
	//affichetab(arrivee,g.sources);
	printf("il y a %d collisions a l'aller\n",test_collisions(collisions,g.sources));
	printf("taille de la fenetre = %d\n",taille_fenetre(collisions,g.sources));
	for(i=0;i<g.sources;i++)
	{
		collisions[i] = arrivee[i]+t.W[i]+distance(gr.routes[i],1);
	}
	
	creationfichierWindow(collisions, g.sources,"Retour.txt");
	printf("il y a %d collisions au retour\n",test_collisions(collisions,g.sources));
	printf("Tmax = %d(route %d) (longest *2 = %d)\n",tMax(g,t),indiceTMax(g,t),2*distance(g.routes[longest(g.routes,g.sources)],g.routes[longest(g.routes,g.sources)].route_lenght));
	printf("taille de la fenetre = %d\n",taille_fenetre(collisions,g.sources));
}
void  simulations()
{
	int i,j,k,l,m;
	Graphe g;
	TwoWayTrip t;
	int nb_simul = 1000;
	int moyenne_window = 0;
	int taillewindowmax;
	int moyenne_Tmax=0;
	char nom[64];
	for(i=0;i<8;i++)//taille route
	{
		int collisions[i];
		int collisionsr[i];
		ecrire_bornes(i,2500);
		for(j=0;j<3;j++)//algo
		{
			for(k=0;k<3;k++)//mode
			{
				moyenne_Tmax= 0;
				moyenne_window=0;
				for(l=0;l<nb_simul;l++)//nb_simul tirates
				{
					g=topologie1(i,i,k);
					if(j == 0)
					{
						t = heuristique_top_1(g,i*4*2500);
						strcpy(nom,"results/heuristique_longest-shortest");
					}
					else if(j == 1)
					{
						t = algo_yann(g,i*4*2500);
						strcpy(nom,"results/greedy_star_assignment");
					}
					else if(j == 2)
					{
						t = shortest_to_longest(g,i*4*2500);
						strcpy(nom,"results/shortest-longest");
					}
					moyenne_Tmax+=tMax(g,t);
					for(m=0;m<g.sources;m++)
					{
						collisions[m] = t.M[m]+distance(g.routes[m],1);
						collisionsr[m] = t.M[m]+distance(g.routes[m],g.routes[m].route_lenght)+t.W[m]+(distance(g.routes[m],g.routes[m].route_lenght)-distance(g.routes[m],2));
					}
					taillewindowmax = max(taille_fenetre(collisions,g.sources),taille_fenetre(collisionsr,g.sources));
					moyenne_window += taillewindowmax;
				}
				moyenne_Tmax /= nb_simul;
				moyenne_window /= nb_simul;
			
						
				if(k==0)
				{
					strcat(nom,"_mode0.txt");
					creationfichierResultats(i,moyenne_Tmax,moyenne_window,nom);
					printf("écriture dans %s\n",nom);
				}
				else if(k==1)
				{
					strcat(nom,"_mode1.txt");
					creationfichierResultats(i,moyenne_Tmax,moyenne_window,nom);
					printf("écriture dans %s\n",nom);
				}
				else if(k==2)
				{
					strcat(nom,"_mode2.txt");
					creationfichierResultats(i,moyenne_Tmax,moyenne_window,nom);
					printf("écriture dans %s\n",nom);
				}
			}
		}
	}
}
void ecrire_bornes(int l, int t)
{
	FILE *f;
	f=fopen("results/bornes.txt","a");
	fprintf(f,"%d %d %d \n",l,l*t,2*l*t);
	fclose(f);
}
void resultats(Graphe g, int taille,int mode, int algo){
	int moyenne_w=0;
	int moyenne_Tmax=0;
	int TmaxMax = 0;
	int nbTmax = 0;
	int moyenne_window = 0;
	int collisions[taille];
	int collisionsr[taille];
	int taillewindowmax;
	char nom[64];
	TwoWayTrip t;
	int i,j,k;
	if(algo == 0)
	{
		strcpy(nom,"results/heuristique_longest-shortest");
	}
	else if(algo == 1)
	{
		strcpy(nom,"results/greedy_star_assignment");

	}
	else if(algo == 2)
	{
		strcpy(nom,"results/shortest-longest");

	}
	//printf("Moyennes pour 1000 simulations avec:\n");
	for(j=0;j<1000;j++)
	{
		
		if(algo == 1)
		{
			t = algo_yann(g,taille*4*2500);
		}
		else if(algo == 0)
		{
			t = heuristique_top_1(g,taille*4*2500);
		}
		else if(algo == 2)
		{
			t = shortest_to_longest(g,taille*4*2500);
		}
		
		for(i=0;i<g.sources;i++)
		{
			collisions[i] = t.M[i]+distance(g.routes[i],1);
			collisionsr[i] = t.M[i]+distance(g.routes[i],g.routes[i].route_lenght)+t.W[i]+(distance(g.routes[i],g.routes[i].route_lenght)-distance(g.routes[i],2));
		}
		taillewindowmax = max(taille_fenetre(collisions,g.sources),taille_fenetre(collisionsr,g.sources));
		moyenne_window += taillewindowmax;

		for(k=0;k<t.taille;k++)
		{
			moyenne_w += t.W[k];
		}
		moyenne_Tmax+=tMax(g,t);
		if(tMax(g,t) > TmaxMax)
		{
			TmaxMax = tMax(g,t);
		}
		//si tmax = route la plus longue
		if(tMax(g,t) == 2*distance(g.routes[longest(g.routes,g.sources)],g.routes[longest(g.routes,g.sources)].route_lenght))
		{
			nbTmax ++;
		}
	}
	moyenne_w /=6000;
	moyenne_Tmax /= 1000;
	moyenne_window /= 1000;
	if(mode==0)
	{
		strcat(nom,"_mode0.txt");
		creationfichierResultats(taille,moyenne_Tmax,moyenne_window,nom);
		printf("écriture dans %s\n",nom);
		printf("- valeurs aléatoires partout :nbTmax = %d, Tmax = %d(max = %d), w = %d\n",nbTmax,moyenne_Tmax,TmaxMax,moyenne_w);
	}
	else if(mode==1)
	{
		strcat(nom,"_mode1.txt");
		creationfichierResultats(taille,moyenne_Tmax,moyenne_window,nom);
		
		printf("écriture dans %s\n",nom);
		printf("- 0 vers les feuilles :nbTmax = %d Tmax = %d(max = %d), w = %d\n",nbTmax,moyenne_Tmax,TmaxMax,moyenne_w);
	}
	else if(mode==2)
	{
		strcat(nom,"_mode2.txt");
		creationfichierResultats(taille,moyenne_Tmax,moyenne_window,nom);
		printf("écriture dans %s\n",nom);
		printf("- 0 vers les sources :nbTmax = %d Tmax = %d(max = %d), w = %d\n",nbTmax,moyenne_Tmax,TmaxMax,moyenne_w);
	}
	
}
//renvoie l'indice du ieme element pas a -1 d'un tableau
int i_dispo(int * tab, int taille)
{
	int i;
	for(i=0;i<taille;i++)
	{
		if(tab[i] != -1)
		{
			return i;
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
//renvoie la premiere ligne de la matrice dont les indices sont a -1
int permier_dispo_matrice(int ** matrice, int taille)
{
	int i;
	int f = factorielle(taille);
	for(i=0;i<f;i++){
		if(matrice[i][0] == -1)
			return i;
	}
	return -1;
}
/*
void bruteforce(int ** permutations,int * tab, int * dispo, int taille, int nb_dispo)
{
	if(nb_dispo ==0)
	{
		//cpy_tab(tab,permutations[permier_dispo_matrice(permutations,taille)],taille);
		affichetab(tab,taille);
	}
	else
	{
		int i;
		int indice;
		for(i=0;i<nb_dispo;i++)
		{
			indice = i_dispo(dispo,taille,i);
			tab[taille-nb_dispo] = dispo [indice];
			int tab2[taille];
			int dispo2[taille];
			cpy_tab(tab, tab2, taille);
			cpy_tab(dispo, dispo2, taille);
			dispo2[indice] = -1;
			bruteforce(permutations,tab2,dispo2,taille,nb_dispo-1);
		}
	}
}*/

