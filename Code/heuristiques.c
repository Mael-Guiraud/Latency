#include "graph.h"

//retourne le twowaytrip de facon a ce que toutes les routes soient collées a l'aller de la plus courte a la plus longe
//donne une taille de fenetre que l'on test
TwoWayTrip shortest_to_longest(Graphe g)
{
	TwoWayTrip t ;
	Graphe gr = renverse(g);
	t.taille=g.sources;
	//offsets
	t.M = malloc(sizeof(int)*g.sources);
	t.W = malloc(sizeof(int)*g.sources);
	int Dl[g.sources];
	int collisionsr[g.sources];
	int i;
	int indice;
	int offset = 0;
	//init
	for(i=0;i<g.sources;i++)
	{	
		Dl[i] = distance(gr.routes[i],1);
	}
	for(i=0;i<g.sources;i++)
	{
		indice = lower(Dl,g.sources);
		Dl[indice] = 9999999;
		if(offset == 0)
		{
			offset= taille_paquet+ distance(g.routes[indice],1);
			t.M[indice] = 0;
			collisionsr[indice] = offset+2*distance(gr.routes[indice],1);
		}
		else
		{
			t.M[indice] = offset - distance(g.routes[indice],1);
			offset += taille_paquet;
			collisionsr[indice] = offset+2*distance(gr.routes[indice],1);
		}
		t.W[i] = 0;
	}
	t.window_size = taille_fenetre(collisionsr,t.taille);
	
	freeGraphe(gr);
	return t;
}


TwoWayTrip bruteforce(int * tab,RouteStar r,int * dispo,int * offsets,int * offsetsr, int taille, int nb_dispo, int budget, int offset, int P)
{
	/*printf("\n\n");
	printf("dispo ");affichetab(dispo,taille);
	printf("offsets ");affichetab(offsets,taille);
	printf("nb_dispo %d budget = %d \n",nb_dispo, budget);
	printf("offset %d P = %d \n",offset,P);*/
	//printf("nb_dispo %d budget = %d \n",nb_dispo, budget);
	TwoWayTrip t;
	t.taille = 0;
	if(nb_dispo ==0)
	{
		/*FILE *f;
		f=fopen("results/permutations.txt","a");
		int i;
		for(i=0;i<taille;i++)
		{
			fprintf(f,"%d %d ",i,offsets[i]);
			//printf("route %d offset %d ",tab[i],offsets[i]);
		}
		fprintf(f,"\n");
		//printf("\n");
		fclose(f);
		int taillewindowmax = max(taille_fenetre(offsets,taille),taille_fenetre(offsetsr,taille));
		if(taillewindowmax < best_window)
		{
			best_window = taillewindowmax;
		}
		solutions++;*/
		t.taille = taille;
		t.M = malloc(sizeof(int)*taille);
		t.W = malloc(sizeof(int)*taille);
		int i;
		for(i=0;i<taille;i++)
		{
			t.M[i] = offsets[i]-r.x[i]; 
			//securité pour eviter d'avoir des offsets de départ négatifs
			if(t.M[i] < 0)
			{
				t.M[i] = P + t.M[i];
			}
			t.W[i] = 0;
		}
		return t;
		
	}
	else
	{
		int first = 0;
		int i;
		int indice;
		int j;
		int budget2;
		for(i=1;i<nb_dispo+1;i++)
		{
			indice = i_dispo(dispo,taille,i);
			//affichetab(dispo,taille);
			//printf("i=%d indice =%d\n",i,indice);
			//afficheRouteStar(r);
			//pour chaque offset
			for(j=offset;j<offset+budget;j++)
			{
				
				//printf("j = %d\n offsetr \n",j);
				//affichetab(offsetsr,taille);
				//si pas de collisions
				//printf("test collisions = %d\n",test_collisions_offset(offsetsr,r,indice, j,P));
				//si ca rentre  ET que c'est le plus petit offset pour lequel ca rentre ou que l'offset retour est >= T
				if((!test_collisions_offset(offsetsr,r,indice, j,P))&&((first == 0) || ((j+(r.y[indice]*2)-(tab[taille-nb_dispo-1]+taille_paquet))%P >= taille_paquet )))
				{
					//printf("avant %d\n",budget);
					budget2 = budget-(j-offset);
					//printf("apres %d (j = %d) offset = %d i = %d\n", budget2,j, offset,i);
					int dispo2[taille];
					int offsets2[taille];
					int offsetsr2[taille];
					int tab2[taille];
					cpy_tab(dispo, dispo2, taille);
					cpy_tab(tab, tab2, taille);
					cpy_tab(offsets, offsets2, taille);
					cpy_tab(offsetsr, offsetsr2, taille);
					dispo2[indice] = -1;
					offsets2[indice] = j;
					tab2[taille-nb_dispo] = indice;
					offsetsr2[indice] = (j+2*r.y[indice])%P;
					t = bruteforce(tab2,r,dispo2,offsets2,offsetsr2,taille,nb_dispo-1,budget2,j+taille_paquet,P);
					if(t.taille != 0)
					{
						return t;
					}
					first=1;
				}
			}

			//condition
			
		}
	}
	//si on ne trouve rien
	return t;
}


TwoWayTrip algo_bruteforce(Graphe g, int P)
{
	int taille = g.sources;
	RouteStar r = cree_routestar(g);
	TwoWayTrip t;
	t.taille=g.sources;
	//offsets
	t.M = malloc(sizeof(int)*g.sources);
	t.W = malloc(sizeof(int)*g.sources);
	//afficheRouteStar(r);
	int dispo[taille];
	int offsets[taille];
	int offsetsr[taille];
	int tab[taille];
	int i;
	for(i=0;i<taille;i++)
	{
		offsets[i] = -1;
		offsetsr[i] = -1;
		dispo[i] = i;
		tab[i] = -1;
		
	}

	offsets[0] = 0;
	offsetsr[0] = r.y[0]*2;
	dispo[0] = -1;
	tab[0] = 0;
	int budget = P - taille * taille_paquet;
	t = bruteforce(tab,r,dispo,offsets,offsetsr , taille, taille-1,budget,taille_paquet, P);




	freeRouteStar(r);
	return t;
	
}

//si Ca rentre pas dans P renvoie t.windowsize = -1
TwoWayTrip greedy_prime(Graphe g, int P)
{
	TwoWayTrip t;
	if(P < g.sources * taille_paquet)
	{
		t.window_size = -1;
		return t;
	}
	else
		t.window_size = 0;
	Graphe gr;
	gr = renverse(g);
	//affiche_graphe(gr);
	t.taille=g.sources;
	//offsets
	t.M = malloc(sizeof(int)*g.sources);
	t.W = malloc(sizeof(int)*g.sources);
	int periode_retour[P];
	int i,j;
	int a,b;
	int offset = 0;
	for(i=0;i<P;i++)
	{
		periode_retour[i] = 0;
	}
	//on prend les routes une par une
	for(i=0;i<g.sources;i++)
	{
		for(j=offset;j<P;j++)
		{
			
			if(offset == 0)
				a = (j+distance(g.routes[i],4)+distance(gr.routes[i],1))%P;
			else
				a = (j+distance(g.routes[i],4)-distance(g.routes[i],1)+distance(gr.routes[i],1))%P;
			b = (a+taille_paquet)%P;			
			if(rentre_dans(periode_retour,a,b))
			{		
				//printf("i %5d j,%5d offset %5d %5d %5d\n",i,j,offset,a,b);
				occuper_p(periode_retour,a,b);
				if(offset == 0)
				{
					t.M[i]= 0;
					t.W[i] = 0;
					offset = (taille_paquet+distance(g.routes[i],1))%P;
				}
				else
				{
					t.M[i]= (j - distance(g.routes[i],1))%P;
					t.W[i] = 0;
					offset = j +taille_paquet;
				}
				break;
			}
			//si affecté, on ne reviens pas ici car break, si pas de place on met a -1 pour indiquer que ca ne rentre pas
			t.M[i] = -1;
			t.W[i] = -1;
		}
		//des qu'on ne peux plus affecter une route, on sort de la fonction et on décrit le twowaytrip comme invalide
		if(t.M[i] == -1)
		{
			t.window_size = -1;
			break;
		}
	}
	
	freeGraphe(gr);
	return t;
}


TwoWayTrip longest_shortest(Graphe g)
{
	TwoWayTrip t ;
	t.taille=g.sources;
	t.window_size = t.taille * taille_paquet;
	//offsets
	t.M = malloc(sizeof(int)*g.sources);
	t.W = malloc(sizeof(int)*g.sources);
	int  Dl[g.sources];
	int  lambdaV[g.sources]; 
	//booleans 1= route affecté 0=route non traitée
	int aff[g.sources];
	int i;
	int offset = 0;
	
	//init
	for(i=0;i<g.sources;i++)
	{
		aff[i] = 0;
		Dl[i] = distance(g.routes[i],g.routes[i].route_lenght);
		lambdaV[i] = distance(g.routes[i],1);
	}
	
	//tant qu'on n'a pas affecté toutes les routes
	while(!full(aff,g.sources))
	{
		i=greater(Dl,g.sources);
		//Si on est le premier
		if(offset==0)
		{
			t.M[i] = 0;
			offset=taille_paquet+lambdaV[i];
		}
		else
		{
			t.M[i] = offset-lambdaV[i];
			offset += taille_paquet;
		}
		aff[i] = 1;
		Dl[i]=0;
	}
	
	//init second offset : waiting times
	Graphe gr = renverse(g);
	int arrivee[gr.sources];
	int arriveeandlambda[gr.sources];
	offset = 0;
	for(i=0;i<gr.sources;i++)
	{
		aff[i] = 0;
		Dl[i] = distance(gr.routes[i],gr.routes[i].route_lenght);
		lambdaV[i] = distance(gr.routes[i],1);
		arrivee[i] = Dl[i]+t.M[i];
		arriveeandlambda[i] = arrivee[i];
	}
	ajoutetab(arriveeandlambda,lambdaV,g.sources);
	
	//tant qu'on n'a pas affecté toutes les routes
	while(!full(aff,gr.sources))
	{
		
		//Si on est le premier
		if(offset==0)
		{
			i=lower(arriveeandlambda,gr.sources);
			
			t.W[i] = 0;
			offset=taille_paquet+lambdaV[i]+arrivee[i];
		}
		else
		{
			
			i=longerOfeEligible(Dl, lambdaV,arrivee,offset,gr.sources);
			//dans le cas ou i est deja traité, on ajoute 100 a l'offset et on retrouve un autre i.
			//printf("i=%d offset = %d , lambdaV = %d arrive = %d\n",i, offset, lambdaV[i],arrivee[i]);
			
			if(Dl[i] ==0)
			{
				offset+=1;
			}
			else
			{
			t.W[i] = offset-lambdaV[i]-arrivee[i];
			offset += taille_paquet;
			}
		}
		aff[i] = 1;
		Dl[i]=0;
		
	}
	freeGraphe(gr);
return t;
	
}

//si Ca rentre pas dans P renvoie t.windowsize = -1
TwoWayTrip greedy_star(Graphe g, int P)
{
	TwoWayTrip t;
	t.window_size = 1;
	//si trop petit, on ne calcul meme pas
	if(P < g.sources * taille_paquet)
	{
		t.window_size = -1;
		return t;
	}
	Graphe gr;
	gr = renverse(g);
	//affiche_graphe(gr);
	t.taille=g.sources;
	//offsets
	t.M = malloc(sizeof(int)*g.sources);
	t.W = malloc(sizeof(int)*g.sources);
	int nombre_slots_aller = P/(taille_paquet*2);
	int periode_aller[nombre_slots_aller];
	int periode_retour[P];
	int a,b,decalage;
	int i,j;
	for(i=0;i<nombre_slots_aller;i++)
	{
		periode_aller[i] = 0;
	}
	for(i=0;i<P;i++)
	{
		periode_retour[i] = 0;
	}
	//on prend les routes une par une
	for(i=0;i<g.sources;i++)
	{
		for(j=0;j<nombre_slots_aller;j++)
		{
			if(periode_aller[j] == 0)
			{
				
				a = (2*distance(gr.routes[i],1))%P;
				decalage = decaler_a(a);
				a += decalage+(j*(taille_paquet*2));
				b = (a+taille_paquet)%P;
				//printf("a = %d, b=%d, tab[a] = %d tab[b] = %d\n",a,b,periode_retour[a],periode_retour[b]);
				
				
				if(rentre_dans(periode_retour,a,b))
				{
					//printf("retour libre\n");
					periode_aller[j] = 1;
					occuper_p(periode_retour,a,b);
					t.M[i]= (j*(taille_paquet*2)+decalage - distance(g.routes[i],2)) %P;
					//securité pour eviter d'avoir des offsets de départ négatifs
					if(t.M[i] < 0)
					{
						t.M[i] = P + t.M[i];
					}
					t.W[i] = 0;

					break;
				}
				
				//printf("retour pas libre\n");
			}
			//si affecté, on ne reviens pas ici car break, si pas de place on met a -1 pour indiquer que ca ne rentre pas
			t.M[i] = -1;
			t.W[i] = -1;
		}
		//des qu'on ne peux plus affecter une route, on sort de la fonction et on décrit le twowaytrip comme invalide
		if(t.M[i] == -1)
		{
			t.window_size = -1;
			break;
		}
	}
	freeGraphe(gr);
	return t;
}

//effectue une recherche dychotomique dans l'intervalle 0;P pour la plus petite taille de fenetre pour g
TwoWayTrip dichotomique(Graphe g,int P, int mode)
{
	TwoWayTrip t ;
	int id=0;
	int ifin = P;
	int im;
	int continuer = 1;
	while(continuer && ((ifin-id) > 1))
	{
		im = (int)ceil((double)(id+ifin)/(double)2);
		//printf("%d\n",im);
		if(mode == 0)
			t = greedy_star(g,im);
		else
			t = greedy_prime(g,im);
		//printf("TAILLE %d\n",t.window_size);
		if(t.window_size==-1)
		{
			continuer = 1;
			id = im;
		}
		else
		{
			continuer = 0;
			ifin = im;
		}
		
	//printf("debut %d milieu %d fin %d\n",id,im,ifin);
	}
	t.window_size = ifin;
	//printf("retour \n");
	
	//printf("debut %d milieu %d fin %d window size %d\n",id,im,ifin,t.window_size);
	return t;
}
TwoWayTrip recherche_lineaire_star(Graphe g, int P)
{

	TwoWayTrip t;
	int i;
	for(i=g.sources*taille_paquet;i<P;i++)
	{
		t = greedy_star(g,i);
		if(valide(g,t,i))
		{
			return t;
			
		}
		freeTwoWayTrip(t);
	}
	t.window_size = -1;
	return t;
}

TwoWayTrip recherche_lineaire_prime(Graphe g, int P)
{

	TwoWayTrip t;
	int i;
	for(i=g.sources*taille_paquet;i<P;i++)
	{
		t = greedy_prime(g,i);
		if(valide(g,t,i))
		{
			return t;
			
		}
		freeTwoWayTrip(t);
	}
	t.window_size = -1;
	return t;
}


TwoWayTrip recherche_lineaire_brute(Graphe g, int P)
{
	int * temps_retour= graphe_to_temps_retour(g);;
	int i = g.sources*taille_paquet;
	TwoWayTrip t;
	for(i;i<P+1;i++)
	{
		t = bruteforceiter(g,taille_paquet,i,g.sources,temps_retour);
		if(t.window_size == 1)
		{
			return t;
		}
	}
	
	t.window_size = -1;
	return t;
}

TwoWayTrip random_sending(Graphe g)
{
	TwoWayTrip t;
	t.taille = g.sources;
	t.M = malloc(sizeof(int)*g.sources);
	t.buffer = malloc(sizeof(int)*g.sources);
	t.W = malloc(sizeof(int)*g.sources);
	Graphe gr = renverse(g);
	int i;
	int departs[g.sources];
	int arrivees[g.sources];
	for(i=0;i<g.sources;i++)
	{
		departs[i] = rand_entier(16942);
		arrivees[i] = 0;
		t.M[i] = 0;
		t.W[i] = 0;
		t.buffer[i] = 0;
	}
	int j;
	int date = 0;
	//affichetab(departs,g.sources);
	for(i=0;i<g.sources;i++)//calcul des buffers et temps allers
	{
		j= lower(departs,g.sources);
		
		if(date < departs[j])//pas de buffer
		{
			t.buffer[j] = 0;
			date = departs[j] + taille_paquet;
			t.M[j] = departs[j] -distance(g.routes[j],1);
			arrivees[j] = t.M[j] + distance(g.routes[j],3) + distance(gr.routes[j],1);
		}
		else//bufferistation
		{
			t.buffer[j] = date - departs[j];
			t.M[j] = date - distance(g.routes[j],1);
			date += taille_paquet;
			arrivees[j] = t.M[j] + distance(g.routes[j],3) + distance(gr.routes[j],1);
		}
		departs[j] = 99999;
	}
	
	date = 0;
	for(i=0;i<g.sources;i++)//on rajoute les décalages 
	{
		j = lower(arrivees,g.sources);
		
		if(date < arrivees[j])//pas de buffer
		{
			date = arrivees[j] + taille_paquet;
			t.W[j] = 0;
		}
		else//bufferistation
		{
			t.W[j] = date - arrivees[j];
			date += taille_paquet;
		}
		arrivees[j] = 99999;
	}
	freeGraphe(gr);
	return t;
}
					
