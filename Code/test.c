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
	int taille = 4;
	Graphe g = topologie1(taille,taille,mode);
	ecrire_fichierGraph(g);
	printf("-------------------G-------------\n");
	affiche_graphe(g);
	int P = 4*taille*taille_paquet;

	//printf("-------------------Gr-------------\n");
	Graphe gr = renverse(g);
	//	P = 2*distance(gr.routes[longest_on_sources(gr.routes,g.sources)],1)+g.sources*taille_paquet-2*distance(gr.routes[shortest_on_sources(gr.routes,g.sources)],1);

	//affiche_graphe(gr);
	
	//TwoWayTrip t = bruteforceiter(g,taille_paquet,P,taille,temps_retour);
	//TwoWayTrip t = greedy_prime(g,P);
	//TwoWayTrip t = algo_bruteforce(g,P);
	//TwoWayTrip t = shortest_to_longest(g);
	TwoWayTrip t= simons(g);
	TwoWayTrip t2= longest_shortest(g);
	//TwoWayTrip t = recherche_lineaire_star(g,P);
	if(t.window_size == -1)
	{
		printf("Pas de solutions\n");
		return;
	}
	printf("----------2way------\n");
	afficheTwoWayTrip(t);
	afficheTwoWayTrip(t2);
	int i;
	
	int arrivee[gr.sources];
	int collisions[g.sources];
	for(i=0;i<g.sources;i++)
	{
		//printf("route %d : %d\n",i,t.buffer[i]);
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
	printf("taille theorique (uniquement pour shortest longest) de la fenetre = %d\n",2*distance(gr.routes[longest_on_sources(gr.routes,g.sources)],1)+g.sources*taille_paquet-2*distance(gr.routes[shortest_on_sources(gr.routes,g.sources)],1));

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
	int nb_simul = 10000;
	int moyenne_Tmax[4];
	int pire_Tmax[4];
	int tmaxtmp = 0;
	int longesttmp = 0;
	long long int ecart_type[4];
	int slotsperdus[4];
	for(i=0;i<4;i++)
	{
		moyenne_Tmax[i] = 0;
		pire_Tmax[i] =0;
		ecart_type[i] = 0;
		slotsperdus[i] = 0;
	}
	char nom[64];
	ecrire_bornesTMax(0);
	for(i=1;i<8;i++)//taille route
	{
		ecrire_bornesTMax(i);
			for(k=0;k<1;k++)//mode
			{
				for(l=0;l<nb_simul;l++)
				{
				g=topologie1(i,i,k);
				//affiche_graphe(g);
				for(j=0;j<4;j++)
				{
					
						
				longesttmp = 2*distance(g.routes[longest(g.routes,g.sources)],g.routes[longest(g.routes,g.sources)].route_lenght);
					if(j == 0)
					{
						t = simons(g);
					}
					else if(j==1)
						t = longest_shortest(g);
					else if(j==3)
					{
						t = random_sending(g);
					}
					
					if(j==2)
					{ 
						moyenne_Tmax[j] += longesttmp;
						pire_Tmax[j] = max(pire_Tmax[j],longesttmp);
					}

					
					else{
						if(j == 3)
						{
							tmaxtmp = tMax_random(g,t);
						}
						else
						{
							tmaxtmp = tMax(g,t);
						}
						moyenne_Tmax[j] += tmaxtmp;
						ecart_type[j] = (long long)ecart_type[j] + (long long)(longesttmp * longesttmp);
						slotsperdus[j] += tmaxtmp - longesttmp;
						pire_Tmax[j] = max(pire_Tmax[j],tmaxtmp);
					}
					
					
					
					
				}
			}
			for(j=0;j<4;j++)
			{
				moyenne_Tmax[j] /= nb_simul;
				ecart_type[j] /= (long long)nb_simul;
				ecart_type[j] -= (long long)(moyenne_Tmax[j] * moyenne_Tmax[j]);
				ecart_type[j] = (long long)sqrt((double)ecart_type[j]);
				slotsperdus[j] /= nb_simul;
				//printf("%d\n", (int)ecart_type);
					if(j == 0)
					{
						sprintf(nom,"results/Tmax_heuristique_longest-shortest_mode%d.txt",k);
						printf("%d routes , écriture dans %s\n",i,nom);
						creationfichierTmax(i,moyenne_Tmax[j],pire_Tmax[j],slotsperdus[j],nom);
					}
					else if(j == 1)
					{
						sprintf(nom,"results/Tmax_simons_mode%d.txt",k);
						printf("%d routes , écriture dans %s\n",i,nom);
						creationfichierTmax(i,moyenne_Tmax[j],pire_Tmax[j],slotsperdus[j],nom);
					}
					else if(j == 2)
					{
						sprintf(nom,"results/bornestmax_mode%d.txt",k);
						printf("%d routes , écriture dans %s\n",i,nom);
						creationfichier(i,moyenne_Tmax[j],pire_Tmax[j],nom);
					}
					else if(j == 3)
					{
						sprintf(nom,"results/Tmax_random_mode%d.txt",k);
						printf("%d routes , écriture dans %s\n",i,nom);
						creationfichierTmax(i,moyenne_Tmax[j],pire_Tmax[j],slotsperdus[j],nom);
					}
				moyenne_Tmax[j] = 0;
				pire_Tmax[j] = 0;
				ecart_type[j] = 0;
				slotsperdus[j] = 0;		
			}
			
		}
	}
	ecrire_bornesTMax(i+1);
}
	
void simulation_brute_shortest()
{
	Graphe g;
	TwoWayTrip t;
	int j;
	int tmp;
	for(int i = 9000;i<9001;i++)
	{
		//printf("%d i \n",i);
		 g = topologie1_variation(2,i);
		 affiche_graphe(g);
		 //affiche_graphe(g);
		 //printf("1\n");
		 t = shortest_to_longest(g);
		 tmp = t.window_size;
		 afficheTwoWayTrip(t);
		 //printf("2\n");
		 j = recherche_lineaire_brute(g);
		 int * temps_retour= graphe_to_temps_retour(g);
		 t = bruteforceiter(g,taille_paquet,j,g.sources,temps_retour);
		 afficheTwoWayTrip(t);
		printf("%d %d (%d)\n",tmp,j,tmp-j);
		freeTwoWayTrip(t);
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
	
	
	int i,j,k,l;
	Graphe g;
	TwoWayTrip t;
	int nb_simul = 100;
	int moyenne_window[4];
	int taillewindowmax=0;
	int piretaille[4];
	char nom[64];
	for(i=0;i<4;i++)
	{
		moyenne_window[i] = 0;
		piretaille[i] = 0;
	}
	for(i=1;i<8;i++)//taille route
	{
		ecrire_bornesWindow(i,taille_paquet);
			for(k=0;k<1;k++)//mode
			{
				for(l=0;l<nb_simul;l++)
				{ 
					if(l%100 == 0)
					{
						printf("%d \n",l/10);
					}
					g=topologie1(i,i,k);
					for(j=0;j<4;j++)//algo
					{
						if(j == 0)
						{
					
							taillewindowmax = recherche_lineaire_prime(g);

						}
						else if(j == 1)
						{
						
							taillewindowmax = recherche_lineaire_star(g);
							
						}
						else if(j == 2)
						{
						
							t = shortest_to_longest(g);
							taillewindowmax = t.window_size;
							
						}
						else if(j == 3)
						{
							taillewindowmax = recherche_lineaire_brute(g);
							
						}
						
						//printf("Calcul window\n");
						moyenne_window[j]  += taillewindowmax;
						piretaille[j] = max(piretaille[j],taillewindowmax);
						//printf("%d %d\n",j,tmpwindow[j]);
						
					}
										
				}
			
				
				for(j=0;j<4;j++)//algo
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
					else if(j == 3)
					{
						sprintf(nom,"results/Window_bruteforce_mode%d.txt",k);
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



void etude_exp_bruteforce()
{
	
	TwoWayTrip t;
	int i,j;
	int reussite;
	Graphe g;
	FILE *f;
	remove ("./results/expbruteforce.txt");
	f=fopen("results/expbruteforce.txt","a");
	int * temps_retour;
	for(i=1;i<8;i++)
	{
		printf("%d \n",i);
		reussite = 0;
		for(j=0;j<100;j++)
		{
			if(j%10 ==0)
				printf("%d %d\n",j/10,reussite);
			g = topologie1(i,i,0);
			temps_retour = graphe_to_temps_retour(g);
			t = bruteforceiter(g,taille_paquet,19500,g.sources,temps_retour);
			if(t.window_size==1)
				reussite++;
			freeTwoWayTrip(t);
			//freeGraphe(g);
		}
		//reussite /=100;
		fprintf(f,"%i %d \n",i, reussite);
	}
	fclose(f);
}



void genere_distrib()
{
	
	DIR * rep = opendir ("./results/");
  
   if (rep != NULL)
   {
      struct dirent * ent;
  
      while ((ent = readdir (rep)) != NULL)
      {
          if(strstr(ent->d_name, "distrib") != NULL) // si on a distrib dans le nom du fichier
		{ 
		 char nom[64];
		strcpy(nom,"./results/");
 		 remove (strcat(nom,ent->d_name));
		printf("deletion of %s\n",nom);
	}
      }
       
      closedir (rep);
   }
	
	FILE *f;
	Graphe g;
	TwoWayTrip t;
	int taille;
	int j;
	int T_max ;
	char nom[64];
	double compteur = 0;
	int distribs[150];

	for(taille = 1;taille < 8; taille ++)
	{
		for(j=0;j<150;j++)
		{
			distribs[j] = 0;
		}
		sprintf(nom,"results/distribution_random_%d.txt",taille);
		f=fopen(nom,"a");
		for(j=0;j<100000;j++)
		{
			
			g = topologie1(taille,taille,0);
			t = longest_shortest(g);
			T_max = tMax(g,t);
			T_max /= 100;
			if(T_max < 150)
			{
				distribs[T_max]++;
			}
			else
				printf("plus loins\n");
			//freeGraphe(g);
			freeTwoWayTrip(t);
		}	
		
		for(j=0;j<150;j++)	
		{
			compteur+=(double)((double)distribs[j]/1000);
			fprintf(f,"%d %f\n",j*100,(double)((double)distribs[j]/1000));
		}
		
		printf("counter = %f\n",compteur);
		compteur = 0;
		fclose(f);

	}

}




void genere_distrib_cumulee()
{
	
	DIR * rep = opendir ("./results/");
  
   if (rep != NULL)
   {
      struct dirent * ent;
  
      while ((ent = readdir (rep)) != NULL)
      {
          if(strstr(ent->d_name, "distribution_cumulee_") != NULL) // si on a distrib dans le nom du fichier
		{ 
		 char nom[64];
		strcpy(nom,"./results/");
 		 remove (strcat(nom,ent->d_name));
		printf("deletion of %s\n",nom);
	}
      }
       
      closedir (rep);
   }
	
	FILE *f;
	Graphe g;
	TwoWayTrip t;
	int taille;
	int j;
	int T_max ;
	char nom[64];
	int distribs[150];

	for(taille = 1;taille < 8; taille ++)
	{
		for(j=0;j<150;j++)
		{
			distribs[j] = 0;
		}
		sprintf(nom,"results/distribution_cumulee_%d.txt",taille);
		f=fopen(nom,"a");
		for(j=0;j<100000;j++)
		{
			
			g = topologie1(taille,taille,0);
			t = random_sending(g);
			T_max = tMax_random(g,t);

			T_max /= 100;
			if(T_max < 150)
			{
				distribs[T_max]++;
			}
			else
				printf("plus loins\n");
			//freeGraphe(g);
			freeTwoWayTrip(t);
		}	
		
		for(j=0;j<150;j++)	
		{
			if(j)
			distribs[j] += distribs[j-1];
			fprintf(f,"%d %f\n",j*100,(double)((double)distribs[j]/1000));
		}
		
		

		fclose(f);

	}

}


void simulataions_petits_paquets()
{
	char nom[64];
	int a;
	for(a=0;a<3;a++)
	{
		sprintf(nom,"results/petits_paquets%d.txt",a);
		int nbr_paquets = pow(10,a);
		//printf("%d\n",nbr_paquets);
		FILE *f;
		Graphe g;
		f=fopen(nom,"w+");
		int i,j;
		int somme = 0;
		int pire_cas = 0;
		int tmp;
		for(i=1;i<8;i++)
		{
			for(j=0;j<10000;j++)
			{
				g = topologie1(i,i,0);
				tmp = random_petits_paquets(g,nbr_paquets);
				somme += tmp;
				if(tmp > pire_cas)
				{
					pire_cas = tmp;
				}
			}
			//printf("somme = %d\n",somme);
			somme /= 1000;
			fprintf(f,"%d %d %d \n",i,somme,pire_cas);
			somme = 0;
			pire_cas = 0;
		}
		fclose(f);
	}
}



void genere_distrib_cumulee_petits_paquets()
{
	char nom[64];
	int a;
	int distribs[150];
	for(a=0;a<3;a++)
	{
		
		int nbr_paquets = pow(10,a);
		//printf("%d\n",nbr_paquets);
		FILE *f;
		Graphe g;
		
		int i,j,k;
		int tmp;
		for(i=1;i<8;i++)
		{
			sprintf(nom,"results/petits_paquets_distrib%d_%d.txt",a,i);
			f=fopen(nom,"w+");
			for(j=0;j<150;j++)
			{	
				distribs[j] = 0;
			}
			for(j=0;j<10000;j++)
			{
				g = topologie1(i,i,0);
				tmp = random_petits_paquets(g,nbr_paquets);
				freeGraphe(g);
				tmp /= 100 ;
				if(tmp < 150)
				{
					distribs[tmp]++;
				}
			}
			for(k=0;k<150;k++)	
			{
				distribs[k] += distribs[k-1];
				fprintf(f,"%d %f\n",k*100,(double)((double)distribs[k]/100));
			}
		}
		fclose(f);
	}
}







