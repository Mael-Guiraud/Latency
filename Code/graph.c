// FONCTIONS SUR LES GRAPHES
#include "graph.h"

void ecrire_fichierGraph(Graphe g)
{
	Graphe gr = renverse(g);
	creationfichierGraph(g,"g.dot");
	creationfichierGraph(gr,"gr.dot");
	
}
void creationfichierGraph(Graphe g,char * nom){

	FILE *f;
	f=fopen(nom,"w+");
	fprintf(f,"graph noeud_%d {\n", g.vertex_number);
	fprintf(f,"node [fixedsize=true width=0.4 height=0.25];\n");
	int i,j;
	for(i=0; i <g.vertex_number;i++){
		for(j=0; j < g.vertices[i].edge_number;j++){
			fprintf(f,"%d -- %d [dir=back, label=\"%d\"]; \n",g.vertices[i].index,g.vertices[i].neighboors[j],g.vertices[i].weight[j]);
		}
	}
	fprintf(f,"}");
	fclose(f);
	
}

Graphe renverse(Graphe g)
{
	Graphe gr;
	gr.sources = g.sources;
	gr.vertex_number = g.vertex_number;
	gr.vertices = malloc((gr.vertex_number)*sizeof(Vertex));
	
	int i,j,k;
	int count = 0;
	for(i=0;i<gr.vertex_number;i++)
	{
		count =0;
		gr.vertices[i].index=i;
		for(j=0;j<g.vertex_number;j++)
		{
			for(k=0;k<g.vertices[j].edge_number;k++)
			{
				if(g.vertices[j].neighboors[k] == i)
				{
					count++;
				}
			}
		}
		gr.vertices[i].edge_number = count;
		gr.vertices[i].neighboors= malloc(count*sizeof(int));
		gr.vertices[i].weight= calloc(count,sizeof(int));
		count = 0;
		for(j=0;j<gr.vertex_number;j++)
		{
			for(k=0;k<g.vertices[j].edge_number;k++)
			{
				if(g.vertices[j].neighboors[k] == i)
				{
					gr.vertices[i].neighboors[count] = g.vertices[j].index;
					gr.vertices[i].weight[count] = g.vertices[j].weight[k];
					count++;
				}
			}
		}
	}
	
	gr.routes = malloc(sizeof(Route)*gr.sources);
	for(i=0;i<gr.sources;i++)
	{
		gr.routes[i].route_lenght = g.routes[i].route_lenght ;
		gr.routes[i].vertices= malloc(sizeof(Vertex)*gr.routes[i].route_lenght);
		for(j=0;j<gr.routes[i].route_lenght;j++)
		{
			gr.routes[i].vertices[j] = gr.vertices[g.routes[i].vertices[g.routes[i].route_lenght-1-j].index];
		}
	}
	return gr;
}

TwoWayTrip heuristique_top_1(Graphe g, int P)
{
	TwoWayTrip t ;
	t.taille=g.sources;
	//offsets
	t.M = malloc(sizeof(int)*g.sources);
	t.W = malloc(sizeof(int)*g.sources);
	int * Dl = malloc(sizeof(int)*g.sources);
	int * lambdaV = malloc(sizeof(int)*g.sources);
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
			offset=2500+lambdaV[i];
		}
		else
		{
			t.M[i] = offset-lambdaV[i];
			offset += 2500;
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
			offset=2500+lambdaV[i]+arrivee[i];
		}
		else
		{
			
			i=longerOfeEligible(Dl, lambdaV,arrivee,offset,gr.sources);
			//dans le cas ou i est deja traité, on ajoute 100 a l'offset et on retrouve un autre i.
			//printf("i=%d offset = %d , lambdaV = %d arrive = %d\n",i, offset, lambdaV[i],arrivee[i]);
			
			if(Dl[i] ==0)
			{
				offset+=100;
			}
			else
			{
			t.W[i] = offset-lambdaV[i]-arrivee[i];
			offset += 2500;
			}
		}
		aff[i] = 1;
		Dl[i]=0;
		
	}
	
return t;
	
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
	for(i=0;i<2500;i++)
	{
		if(a%2500 != 0)
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
TwoWayTrip algo_yann(Graphe g, int P)
{
	TwoWayTrip t;
	Graphe gr;
	gr = renverse(g);
	//affiche_graphe(gr);
	t.taille=g.sources;
	//offsets
	t.M = malloc(sizeof(int)*g.sources);
	t.W = malloc(sizeof(int)*g.sources);
	int nombre_slots_aller = P/5000;
	int periode_aller[nombre_slots_aller];
	int periode_retour[P];
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
				int a,b,decalage;
				a = (2*distance(gr.routes[i],1))%P;
				decalage = decaler_a(a);
				a += decalage+(j*5000);
				b = (a+2500)%P;
				//printf("a = %d, b=%d, tab[a] = %d tab[b] = %d\n",a,b,periode_retour[a],periode_retour[b]);
				
				
				if(rentre_dans(periode_retour,a,b))
				{
					//printf("retour libre\n");
					periode_aller[j] = 1;
					occuper_p(periode_retour,a,b);
					t.M[i]= (j*5000+decalage - distance(g.routes[i],2)) %P;
					t.W[i] = 0;
					break;
				}
				
				//printf("retour pas libre\n");
			}
		}
	}
	
	return t;
}

TwoWayTrip shortest_to_longest(Graphe g, int P)
{
	TwoWayTrip t ;
	t.taille=g.sources;
	//offsets
	t.M = malloc(sizeof(int)*g.sources);
	t.W = malloc(sizeof(int)*g.sources);
	int * Dl = malloc(sizeof(int)*g.sources);
	int i;
	int indice;
	int offset = 0;
	//init
	for(i=0;i<g.sources;i++)
	{	
		Dl[i] = distance(g.routes[i],g.routes[i].route_lenght);
	}
	for(i=0;i<g.sources;i++)
	{
		indice = lower(Dl,g.sources);
		Dl[indice] = 9999999;
		if(offset == 0)
		{
			offset= 2500+ distance(g.routes[i],1);
			t.M[i] = 0;
		}
		else
		{
			t.M[i] = offset - distance(g.routes[i],1);
			offset+=2500;
		}
		t.W[i] = 0;
	}
	return t;
}
//renvoie 1 si il y a des collisions, 0 sinon
int test_collisions_brute(int * tab, int taille)
{
	int i,j;
	for(i=0;i<taille;i++)
	{
		for(j=i+1;j<taille;j++)
		{
			if((tab[i] != -1) && (tab[j] != -1))
			{
				if(fabs((double)tab[i] - (double)tab[j]) < 2500)
				{
					return 1;
				}
			} 
		}
	}
	return 0;
}
void bruteforce(int * tab, int * dispo,int * offsets, int taille, int nb_dispo, int budget, int offset, int P)
{
	if(nb_dispo ==0)
	{
		FILE *f;
		f=fopen("results/permutations.txt","a");
		int i;
		for(i=0;i<taille;i++)
			fprintf(f,"%d ",tab[i]);
		fprintf(f,"\n");
		fclose(f);
		
	}
	else
	{
		int i;
		int indice;
		int j;
		for(i=0;i<nb_dispo;i++)
		{
			indice = i_dispo(dispo,taille,i);
			tab[taille-nb_dispo] = dispo[indice];
			int tab2[taille];
			int dispo2[taille];
			int offsets2[taille];
			for(j=0;j<taille;j++)
			{
				offsets2[j] = -1;
			}
			cpy_tab(tab, tab2, taille);
			cpy_tab(dispo, dispo2, taille);
			cpy_tab(offsets, offsets2, taille);
			dispo2[indice] = -1;
			
			//pour chaque offset
			for(j=offset;j<P;j++)
			{
				offsets2[indice] = j;
				//si pas de collisions
				if(!test_collisions_brute(offsets,taille))
				{
					budget -= j-offset;
					offset = j+2500;
					
					if(budget > 0)
					{
						/*printf("\n\n");
						printf("tab   ");affichetab(tab,taille);
						printf("dispo ");affichetab(dispo,taille);
						printf("tab2   ");affichetab(tab,taille);
						printf("dispo2 ");affichetab(dispo,taille);
						printf("budget = %d \n",budget);
						printf("offset %d j %d\n",offset,j);*/
						bruteforce(tab,dispo2,offsets2,taille,nb_dispo-1,budget,offset,P);
					}
				}
			}

			//condition
			
		}
	}
}
TwoWayTrip algo_bruteforce(int P)
{
	int taille = 5;
	Graphe g = topologie1(taille,taille,0);
	TwoWayTrip t;
	t.taille=g.sources;
	//offsets
	t.M = malloc(sizeof(int)*g.sources);
	t.W = malloc(sizeof(int)*g.sources);
	
	int taillefactorielle = factorielle(taille);
	int tab[taille];
	int dispo[taille];
	int offsets[taille];
	int i,j;
	for(i=0;i<taille;i++)
	{
		offsets[i] = -1;
		tab[i] = -1;
		dispo[i] = i;
		
	}
	int budget = P - taille * 2500;
	bruteforce(tab, dispo,offsets, taille, taille,budget,0, P);

	int k;


	
	return t;
	
}

