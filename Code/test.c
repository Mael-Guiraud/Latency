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


void affichetab(int* tab, int taille)
{
	//printf("TABLEAU de taille %d\n",taille);
	int i;
	for(i=0;i<taille;i++)
	{
		printf("%d ",tab[i]);
	}
	printf("\n");
}

void afficheRouteStar(RouteStar r)
{
	int i;
	for(i=0;i<r.nb_routes;i++)
		printf("x = %d y = %d \n",r.x[i],r.y[i]);
}


void simulation(int mode)
{
	
	Graphe g = topologie1(3,3,mode);
	ecrire_fichierGraph(g);
	printf("-------------------G-------------\n");
	affiche_graphe(g);
	int P = 30000;
	//printf("-------------------Gr-------------\n");
	Graphe gr = renverse(g);
	//affiche_graphe(gr);

	TwoWayTrip t = shortest_to_longest(g);
	if(t.window_size == -1)
	{
		printf("NE RENTRE PAS %d\n",t.window_size);
		return;
	}
	printf("----------2way------\n");
	afficheTwoWayTrip(t);
	int i;
	
	int arrivee[gr.sources];
	int collisions[g.sources];
	for(i=0;i<g.sources;i++)
	{
		collisions[i] = t.M[i]+distance(g.routes[i],1);
		arrivee[i] = t.M[i]+distance(g.routes[i],g.routes[i].route_lenght);
	}
	creationfichierWindow(collisions, g.sources,"results/Aller.txt");
	//printf("ARRIVEE\n");
	//affichetab(arrivee,g.sources);
	printf("il y a %d collisions a l'aller\n",test_collisions(collisions,g.sources,P));
	printf("taille de la fenetre = %d\n",taille_fenetre(collisions,g.sources));
	for(i=0;i<g.sources;i++)
	{
		collisions[i] = arrivee[i]+t.W[i]+distance(gr.routes[i],1);
	}
	
	creationfichierWindow(collisions, g.sources,"results/Retour.txt");
	printf("il y a %d collisions au retour\n",test_collisions(collisions,g.sources,P));
	printf("Tmax = %d(route %d) (longest *2 = %d)\n",tMax(g,t),indiceTMax(g,t),2*distance(g.routes[longest(g.routes,g.sources)],g.routes[longest(g.routes,g.sources)].route_lenght));
	printf("taille de la fenetre = %d\n",taille_fenetre(collisions,g.sources));
	//printf("taille theorique (uniquement pour shortest longest) de la fenetre = %d\n",2*distance(gr.routes[longest_on_sources(gr.routes,g.sources)],1)+g.sources*taille_paquet-2*distance(gr.routes[shortest_on_sources(gr.routes,g.sources)],1));

	//freeGraphe(g);
	//freeGraphe(gr);
	//freeTwoWayTrip(t);

}

void  simulationsTmax()
{
	int i,j,k,l;
	Graphe g;
	TwoWayTrip t;
	int nb_simul = 1000;
	int moyenne_Tmax=0;
	int pire_Tmax=0;
	char nom[64];
	for(i=1;i<4;i++)//taille route
	{
		ecrire_bornesTMax(i);
		for(j=0;j<1;j++)//algo
		{
			for(k=0;k<1;k++)//mode
			{
				moyenne_Tmax= 0;
				pire_Tmax = 0;
				if(j == 0)
				{
					strcpy(nom,"results/Tmax_heuristique_longest-shortest");
				}
				else if(j == 1)
				{
					strcpy(nom,"results/Tmax_greedy_star_assignment");
				}
				for(l=0;l<nb_simul;l++)
				{
					g=topologie1(i,i,k);
					if(j == 0)
					{
						t = longest_shortest(g);
					}
					else if(j == 1)
					{
						t = greedy_star(g,i*4*taille_paquet);
					}
					else if(j == 2)
					{
						t = shortest_to_longest(g);
					}
					else if(j == 3)
					{
						t = greedy_prime(g,4*i*taille_paquet);
					}					
					moyenne_Tmax+=tMax(g,t);
					if((tMax(g,t)> pire_Tmax )&&(j==0))
					{
						printf("%d routes----------------------------\n",i);
						affiche_graphe(g);
						afficheTwoWayTrip(t);
						printf("Tmax = %d(route %d) (longest *2 = %d)\n",tMax(g,t),indiceTMax(g,t),2*distance(g.routes[longest(g.routes,g.sources)],g.routes[longest(g.routes,g.sources)].route_lenght));
	
					}
					pire_Tmax = max(pire_Tmax,tMax(g,t));
				}
				moyenne_Tmax /= nb_simul;
				if(k==0)
				{
					strcat(nom,"_mode0.txt");
					creationfichier(i,moyenne_Tmax,pire_Tmax,nom);
					printf("écriture dans %s\n",nom);
				}
				else if(k==1)
				{
					strcat(nom,"_mode1.txt");
					creationfichier(i,moyenne_Tmax,pire_Tmax,nom);
					printf("écriture dans %s\n",nom);
				}
				else if(k==2)
				{
					strcat(nom,"_mode2.txt");
					creationfichier(i,moyenne_Tmax,pire_Tmax,nom);
					printf("écriture dans %s\n",nom);
				}
			}
		}
	}
}
	
void  simulationsWindow()
{
	int i,j,k,l,m;
	Graphe g;
	TwoWayTrip t;
	int nb_simul = 1000;
	int moyenne_window = 0;
	int taillewindowmax;
	int piretaille = 0;
	char nom[64];
	for(i=1;i<8;i++)//taille route
	{
		int collisions[i];
		int collisionsr[i];
		ecrire_bornesWindow(i,taille_paquet);
		for(j=0;j<3;j++)//algo
		{
			for(k=0;k<3;k++)//mode
			{
				moyenne_window=0;
				piretaille = 0;
				if(j == 0)
				{
					strcpy(nom,"results/Window_greedy_prime");
				}
				else if(j == 1)
				{
					strcpy(nom,"results/Window_greedy_star_assignment");
				}
				else if(j == 2)
				{
					strcpy(nom,"results/Window_shortest_longest");
				}
				for(l=0;l<nb_simul;l++)
				{
					g=topologie1(i,i,k);
					if(j == 0)
					{
						t = dichotomique(g,4*i*taille_paquet,1);
					}
					else if(j == 1)
					{
						t = dichotomique(g,4*i*taille_paquet,0);
					}
					else if(j == 2)
					{
						t = shortest_to_longest(g);
					}
					for(m=0;m<g.sources;m++)
					{
						collisions[m] = t.M[m]+distance(g.routes[m],1);
						collisionsr[m] = t.M[m]+distance(g.routes[m],g.routes[m].route_lenght)+t.W[m]+(distance(g.routes[m],g.routes[m].route_lenght)-distance(g.routes[m],2));
					}
					//printf("Calcul window\n");
					taillewindowmax = max(taille_fenetre(collisions,g.sources),taille_fenetre(collisionsr,g.sources));
					moyenne_window += taillewindowmax;
					piretaille = max(piretaille,taillewindowmax);
				}
				moyenne_window /= nb_simul;
						
				if(k==0)
				{
					strcat(nom,"_mode0.txt");
					creationfichier(i,moyenne_window,piretaille,nom);
					printf("écriture dans %s\n",nom);
				}
				else if(k==1)
				{
					strcat(nom,"_mode1.txt");
					creationfichier(i,moyenne_window,piretaille,nom);
					printf("écriture dans %s\n",nom);
				}
				else if(k==2)
				{
					strcat(nom,"_mode2.txt");
					creationfichier(i,moyenne_window,piretaille,nom);
					printf("écriture dans %s\n",nom);
				}
			}
		}
	}
}

void  simulationsSuccess()
{
	int periode = 35000 ;
	int i,j,k,l;
	Graphe g;
	TwoWayTrip t;
	int nb_simul = 1000;
	int success = 0;
	char nom[64];
	for(i=1;i<8;i++)//taille route
	{
		for(j=0;j<2;j++)//algo
		{
			for(k=0;k<3;k++)//mode
			{
				success=0;
				if(j == 0)
				{
					strcpy(nom,"results/Success_greedy_prime");
				}
				else if(j == 1)
				{
					strcpy(nom,"results/Success_greedy_star_assignment");
				}
				for(l=0;l<nb_simul;l++)
				{
					g=topologie1(i,i,k);
					if(j == 0)
					{
						t = greedy_prime(g,periode);
						if(t.window_size != -1)
						{
							success ++;
						}
					}
					else if(j == 1)
					{
						t = greedy_star(g,periode);
						if(t.window_size != -1)
						{
							success ++;
						}
					}

				}
				success *= 100;
				success /= nb_simul;
						
				if(k==0)
				{
					strcat(nom,"_mode0.txt");
					creationfichier(i,success,0,nom);
					printf("écriture dans %s\n",nom);
				}
				else if(k==1)
				{
					strcat(nom,"_mode1.txt");
					creationfichier(i,success,0,nom);
					printf("écriture dans %s\n",nom);
				}
				else if(k==2)
				{
					strcat(nom,"_mode2.txt");
					creationfichier(i,success,0,nom);
					printf("écriture dans %s\n",nom);
				}
			}
		}
	}
}



