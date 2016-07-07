#include "graph.h"


void afficheTwoWayTrip(TwoWayTrip t)
{
	if(t.taille == 0)
	{
		printf("twowaytrip vide\n");
		return;
	}
	int i;
	printf("Taille %d\n",t.window_size);
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
	int taille = 6;
	Graphe g = topologie1(taille,taille,mode);
	ecrire_fichierGraph(g);
	printf("-------------------G-------------\n");
	affiche_graphe(g);
	int P = taille*taille_paquet*4;
	//printf("-------------------Gr-------------\n");
	Graphe gr = renverse(g);
	//affiche_graphe(gr);

	TwoWayTrip t = greedy_star(g,P);

	printf("----------2way------\n");
	afficheTwoWayTrip(t);
	int i;
	
	int arrivee[gr.sources];
	int collisions[g.sources];
	for(i=0;i<g.sources;i++)
	{
		
		collisions[i] = (t.M[i]+distance(g.routes[i],1))%P;
	
		arrivee[i] = (t.M[i]+distance(g.routes[i],g.routes[i].route_lenght))%P;
	
	}
	

	creationfichierWindow(collisions, g.sources,"results/Aller.txt");
	//printf("ARRIVEE\n");
	//affichetab(arrivee,g.sources);
	printf("il y a %d collisions a l'aller\n",test_collisions(collisions,g.sources,P));
	printf("taille de la fenetre = %d\n",taille_fenetre(collisions,g.sources));
	for(i=0;i<g.sources;i++)
	{
		collisions[i] = (arrivee[i]+t.W[i]+distance(gr.routes[i],1))%P;
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
	
	DIR * rep = opendir ("./results/");
  
   if (rep != NULL)
   {
      struct dirent * ent;
  
      while ((ent = readdir (rep)) != NULL)
      {
          if((strstr(ent->d_name, "Tmax") != NULL)||(strstr(ent->d_name, "tmax") != NULL)) // si on a tmax ou Tmax dans le nom du fichier
		{ 
		 char nom[64];
		strcpy(nom,"./results/");
 		 remove (strcat(nom,ent->d_name));
		printf("deletion of %s\n",nom);
	}
      }
       
      closedir (rep);
   }
   
   
	int i,j,k,l;
	Graphe g;
	TwoWayTrip t;
	int nb_simul = 1000;
	int moyenne_Tmax=0;
	int pire_Tmax=0;
	int moyenne_Tmaxopti = 0;
	int pire_Tmaxopti = 0;
	int longesttmp = 0;
	int tmaxtmp = 0;
	long long int ecart_type=0;
	char nom[64];
	
	for(i=1;i<8;i++)//taille route
	{
		ecrire_bornesTMax(i);
		for(j=0;j<1;j++)//algo
		{
			for(k=0;k<3;k++)//mode
			{
				moyenne_Tmax= 0;
				moyenne_Tmaxopti = 0;
				pire_Tmax = 0;
				pire_Tmaxopti = 0;
				ecart_type = 0;
				if(j == 0)
				{
					strcpy(nom,"results/Tmax_heuristique_longest-shortest");
				}
				for(l=0;l<nb_simul;l++)
				{
					g=topologie1(i,i,k);
					if(j == 0)
					{
						t = longest_shortest(g);
					}
					longesttmp = 2*distance(g.routes[longest(g.routes,g.sources)],g.routes[longest(g.routes,g.sources)].route_lenght);
					tmaxtmp = tMax(g,t);
					moyenne_Tmaxopti+=longesttmp;
					moyenne_Tmax += tmaxtmp;
					ecart_type = (long long)ecart_type + (long long)(longesttmp * longesttmp);
					
					/*if((tMax(g,t)> pire_Tmax )&&(j==0))
					{
						printf("%d routes----------------------------\n",i);
						affiche_graphe(g);
						afficheTwoWayTrip(t);
						printf("Tmax = %d(route %d) (longest *2 = %d)\n",tMax(g,t),indiceTMax(g,t),2*distance(g.routes[longest(g.routes,g.sources)],g.routes[longest(g.routes,g.sources)].route_lenght));
	
					}*/
					pire_Tmax = max(pire_Tmax,tmaxtmp);
					pire_Tmaxopti = max(pire_Tmaxopti,longesttmp);
					
				}
				//moyenne_Tmax /= nb_simul;
				moyenne_Tmaxopti /= nb_simul;
				moyenne_Tmax /= nb_simul;
				ecart_type /= (long long)nb_simul;
				ecart_type -= (long long)(moyenne_Tmax * moyenne_Tmax);
				ecart_type = (long long)sqrt((double)ecart_type);
				//printf("%d\n", (int)ecart_type);
				if(k==0)
				{
					strcat(nom,"_mode0.txt");
					creationfichierTmax(i,moyenne_Tmax,pire_Tmax,(int)ecart_type,nom);
					creationfichier(i,moyenne_Tmaxopti,pire_Tmaxopti,"results/bornestmax_mode0.txt");
					
					//printf("écriture dans %s\n",nom);
				}
				else if(k==1)
				{
					strcat(nom,"_mode1.txt");
					creationfichierTmax(i,moyenne_Tmax,pire_Tmax,(int)ecart_type,nom);
					creationfichier(i,moyenne_Tmaxopti,pire_Tmaxopti,"results/bornestmax_mode1.txt");
					//printf("écriture dans %s\n",nom);
				}
				else if(k==2)
				{
					strcat(nom,"_mode2.txt");
					creationfichierTmax(i,moyenne_Tmax,pire_Tmax,(int)ecart_type,nom);
					creationfichier(i,moyenne_Tmaxopti,pire_Tmaxopti,"results/bornestmax_mode2.txt");
					//printf("écriture dans %s\n",nom);
				}
			}
		}
	}
}
	
void  simulationsWindow()
{
	
	
	DIR * rep = opendir ("./results/");
  
   if (rep != NULL)
   {
      struct dirent * ent;
  
      while ((ent = readdir (rep)) != NULL)
      {
          if((strstr(ent->d_name, "Window") != NULL)||(strstr(ent->d_name, "bornes.txt") != NULL)) 
		{ 
		 char nom[64];
		strcpy(nom,"./results/");
 		 remove (strcat(nom,ent->d_name));
		printf("deletion of %s\n",nom);
	}
      }
       
      closedir (rep);
   }
	
	
	int i,j,k,l,m;
	Graphe g;
	TwoWayTrip t;
	int nb_simul = 1000;
	int moyenne_window[3];
	int taillewindowmax;
	int piretaille[3];
	char nom[64];
	for(i=0;i<3;i++)
	{
		moyenne_window[i] = 0;
		piretaille[i] = 0;
	}
	for(i=1;i<8;i++)//taille route
	{
		int collisions[i];
		int collisionsr[i];
		ecrire_bornesWindow(i,taille_paquet);
			for(k=0;k<3;k++)//mode
			{
				for(l=0;l<nb_simul;l++)
				{
					g=topologie1(i,i,k);
					for(j=0;j<3;j++)//algo
					{
						if(j == 0)
						{
							t = recherche_lineaire_prime(g,4*i*taille_paquet);
						}
						else if(j == 1)
						{
							t = recherche_lineaire_star(g,4*i*taille_paquet);
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
						moyenne_window[j] += taillewindowmax;
						piretaille[j] = max(piretaille[j],taillewindowmax);
					}
				}
				
				for(j=0;j<3;j++)//algo
				{
					moyenne_window[j] /= nb_simul;
					if(j == 0)
					{
						sprintf(nom,"results/Window_greedy_prime_mode%d.txt",k);
						printf("%d routes , écriture dans %s\n",i,nom);
					}
					else if(j == 1)
					{
						sprintf(nom,"results/Window_greedy_star_assignment_mode%d.txt",k);
						printf("%d routes , écriture dans %s\n",i,nom);
					}
					else if(j == 2)
					{
						sprintf(nom,"results/Window_shortest_longest_mode%d.txt",k);
						printf("%d routes , écriture dans %s\n",i,nom);
					}
					creationfichier(i,moyenne_window[j],piretaille[j],nom);
					moyenne_window[j]=0;
					piretaille[j] = 0;	
				}
					
				
			}
	}
}



//renvoie  0 si le twowaytrip n'est pas valide, 1 sinon
int valide(Graphe g, TwoWayTrip t, int P)
{
	Graphe gr = renverse(g);
	int i;
	int arrivee[gr.sources];
	int collisions[g.sources];
	for(i=0;i<g.sources;i++)
	{
		collisions[i] = (t.M[i]+distance(g.routes[i],1))%P;
		arrivee[i] = (t.M[i]+distance(g.routes[i],g.routes[i].route_lenght))%P;
	}
	if(test_collisions(collisions,g.sources,P))
	{
		freeGraphe(gr);
		return 0;
	}
	for(i=0;i<g.sources;i++)
	{
		collisions[i] = (arrivee[i]+t.W[i]+distance(gr.routes[i],1))%P;
	}
	if(test_collisions(collisions,g.sources,P))
	{
		freeGraphe(gr);
		return 0;
	}
	freeGraphe(gr);
	return 1;

}




