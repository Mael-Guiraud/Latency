#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include "struct.h"
#include "operations.h"
#include "tests.h"
#include "simons.h"

int first_back(int * tab,int * deadline, int taille)
{
	int min = INT_MAX;
	int indice_min = -1;
	for(int i =0;i<taille;i++)
	{
		if(tab[i] != -1)
		{
			if(tab[i]<min)
			{
				min = tab[i];		
				indice_min = i;
			}
			if(tab[i]==min)
			{
				if(indice_min != -1)
					if(deadline[i]<deadline[indice_min])
					{
						min = tab[i];		
						indice_min = i;
					}
			}
		}
	}
	return indice_min;
}

int longest_etoile_2(Graphe g,int taille_paquets,int periode, int Tmax,int mode)
{
	if(!(g.N%2)){printf("Nombre de sommets impair, G n'est peut être pas une étoile\n");exit(5);}
	int nb_routes = g.N/2;
	int offset;
	int nb_assigned;
	int m_i[nb_routes];
	int w_i[nb_routes];

	int ordre[nb_routes];
	int routes[nb_routes];

	int noeud_retour[nb_routes];
	int date_limite[nb_routes];

	int routes_retour[nb_routes];

	//Initialisation des tableaux
	for(int i=0;i<nb_routes;i++){m_i[i]=0;w_i[i]=0;ordre[i]=i;noeud_retour[i]=0;}

	//Calcul de la taille des routes
	for(int i=0;i<nb_routes;i++)
	{
		routes[i]=g.matrice[nb_routes][i]+g.matrice[nb_routes][i+nb_routes+1];
		routes_retour[i]=g.matrice[nb_routes][i+nb_routes+1];
	}

	switch(mode)
	{
		case 1:
			tri_bulles(routes,ordre,nb_routes);
		break;
		case 2:
			tri_bulles_inverse(routes,ordre,nb_routes);
		break;
		case 3:
			tri_bulles(routes_retour,ordre,nb_routes);
		break;
		case 4:
			tri_bulles_inverse(routes_retour,ordre,nb_routes);
		break;
		default:
			fisher_yates(ordre,nb_routes);
		break;
	}

	m_i[ordre[0]]=0;
	offset = taille_paquets+g.matrice[nb_routes][ordre[0]];
	for(int i=1;i<nb_routes;i++)
	{
		m_i[ordre[i]]=offset-g.matrice[nb_routes][ordre[i]];
		offset+=taille_paquets;
	}


	//Calcul de la position potentielle dans ct
	for(int i=0;i<nb_routes;i++){
		noeud_retour[i]=m_i[i]+routes[i]+g.matrice[nb_routes][i+nb_routes+1];
		ordre[i]=i;
	}

	//affiche_tab(noeud_retour,nb_routes);

	for(int i=0;i<nb_routes;i++){date_limite[i] = m_i[i]+Tmax-g.matrice[nb_routes][i];}

	//Le premier a arriver passe direct
	int eligible = first_back(noeud_retour,date_limite,nb_routes);
	w_i[eligible]=0;
	offset = taille_paquets+noeud_retour[eligible];
	noeud_retour[eligible] = -1;
	nb_assigned = 1;
	
	

	//affiche_tab(date_limite,nb_routes);
	while(nb_assigned<nb_routes)
	{
		
		eligible = -1;
		for(int i=0;i<nb_routes;i++)
		{
			if((noeud_retour[i]!=-1)&&(noeud_retour[i]<=offset ))
			{
				
				if(eligible == -1)
					eligible = i;
				else
				{
					if(date_limite[i]<date_limite[eligible])
						eligible = i;
				}
			}
			
		}

		if(eligible == -1)
		{
			eligible = first_back(noeud_retour,date_limite,nb_routes);
		
			w_i[eligible]=0;
			offset=taille_paquets+noeud_retour[eligible];
		}
		else
		{
		
			w_i[eligible]=(offset-g.matrice[nb_routes][eligible+nb_routes+1])-(m_i[eligible]+routes[eligible] );			
			
			offset+=taille_paquets;

		}
	
		nb_assigned++;
		noeud_retour[eligible]=-1;

	}

	//affiche_tab(m_i,nb_routes);
	//affiche_tab(w_i,nb_routes);

	int retour[nb_routes];
	for(int i=0;i<nb_routes;i++)
	{
		retour[i] = m_i[i]+g.matrice[nb_routes][i]+2*g.matrice[nb_routes][nb_routes+i+1]+w_i[i];
	}
	int taille_periode_retour = retour[greater(retour,nb_routes)]-retour[lower(retour,nb_routes)]+taille_paquets;
	if(taille_periode_retour > periode)return -2;

	if(!is_ok(g,taille_paquets,m_i,w_i,periode))printf("ERROR !!\n");
	int max = w_i[0]+2*routes[0];
	for(int i=0;i<nb_routes;i++)
	{
		if(w_i[i]+2*routes[i] > Tmax)
			return -1;
		if(w_i[i]+2*routes[i] > max)
			max= w_i[i]+2*routes[i];
	}
	//affiche_solution(g,taille_paquets,m_i,w_i);
	return max;
}

//renvoire le debut du premier intervalle libre en donnant un i dans la periode, 
//0 si ca n'existe pas
int premier_libre(int *periode_retour, int i,int taille_paquets, int periode)
{
	if(i<0)i=0;
	if(i>=periode-taille_paquets)
			return -1;
	while( periode_retour[i] || periode_retour[i+taille_paquets-1])
	{
		i++;
		if(i>=periode-taille_paquets)
			return -1;
		//printf("i =%d , periode = %d\n",i,periode);
	}
	return i;
}

int longest_etoile_periodique(Graphe g,int taille_paquets,int periode, int Tmax,int mode)
{
	if(!(g.N%2)){printf("Nombre de sommets impair, G n'est peut être pas une étoile\n");exit(5);}
	int nb_routes = g.N/2;
	int offset;
	int nb_assigned;
	int m_i[nb_routes];
	int w_i[nb_routes];

	int ordre[nb_routes];
	int routes[nb_routes];
	int routes_retour[nb_routes];

	int noeud_retour[nb_routes];
	int date_limite[nb_routes];

	//Initialisation des tableaux
	for(int i=0;i<nb_routes;i++){m_i[i]=0;w_i[i]=0;ordre[i]=i;noeud_retour[i]=0;}

	//Calcul de la taille des routes
	for(int i=0;i<nb_routes;i++)
	{
		routes[i]=g.matrice[nb_routes][i]+g.matrice[nb_routes][i+nb_routes+1];
		routes_retour[i]=g.matrice[nb_routes][i+nb_routes+1];
	}



	switch(mode)
	{
		case 1:
			tri_bulles(routes,ordre,nb_routes);
		break;
		case 2:
			tri_bulles_inverse(routes,ordre,nb_routes);
		break;
		case 3:
			tri_bulles(routes_retour,ordre,nb_routes);
		break;
		case 4:
			tri_bulles_inverse(routes_retour,ordre,nb_routes);
		break;
		default:
			fisher_yates(ordre,nb_routes);
		break;
	}

	m_i[ordre[0]]=0;
	offset = taille_paquets+g.matrice[nb_routes][ordre[0]];
	for(int i=1;i<nb_routes;i++)
	{
		m_i[ordre[i]]=offset-g.matrice[nb_routes][ordre[i]];
		offset+=taille_paquets;
	}






	//Calcul de la position potentielle dans ct
	for(int i=0;i<nb_routes;i++){
		noeud_retour[i]=m_i[i]+routes[i]+g.matrice[nb_routes][i+nb_routes+1];
		//ordre[i]=i;
	}

	//affiche_tab(noeud_retour,nb_routes);

	for(int i=0;i<nb_routes;i++){
		date_limite[i] = m_i[i]+Tmax-g.matrice[nb_routes][i];
	}

	//Le premier a arriver passe direct
	int eligible = first_back(noeud_retour,date_limite,nb_routes);
	w_i[eligible]=0;
	offset = taille_paquets+noeud_retour[eligible];

	int debut_periode = noeud_retour[eligible];
	noeud_retour[eligible] = -1;
	nb_assigned = 1;
	
	
	int fin_periode = debut_periode + periode;
	int debut_seconde_periode = -1;

	int periode_retour[periode];

	for(int i=0;i<periode;i++)
		periode_retour[i]=0;

	for(int i=0;i<taille_paquets;i++)
		periode_retour[i]=1;

	//printf("Avant scheduling debut periode = %d, eligible = %d\n",debut_periode,eligible);
	//printf("Fin periode = %d \n",fin_periode);
	//affiche_tab(noeud_retour,nb_routes);
	//affiche_tab(date_limite,nb_routes);
	while(nb_assigned<nb_routes)
	{
		
		eligible = -1;
		for(int i=0;i<nb_routes;i++)
		{
			if((noeud_retour[i]!=-1)&&(noeud_retour[i]<=offset ))
			{
				
				if(eligible == -1)
					eligible = i;
				else
				{
					if(date_limite[i]<date_limite[eligible])
						eligible = i;
				}
			}
			
		}

		if(eligible == -1)
		{
			eligible = first_back(noeud_retour,date_limite,nb_routes);
			//printf("eligible etait -1, now %d\n",eligible);
			if((noeud_retour[eligible]+taille_paquets > fin_periode)||(debut_seconde_periode != -1))
			{
			//	printf("1 on essaye de lire premier libre (%d %d %d) \n",noeud_retour[eligible],periode,debut_periode);
				if(debut_seconde_periode == -1)
					debut_seconde_periode = eligible;
				offset = premier_libre(periode_retour,noeud_retour[eligible]-periode-debut_periode,taille_paquets,periode);
			//	printf("1 Premier libre(%d) = %d\n",noeud_retour[eligible]-periode-debut_periode,offset);
				
				if((offset == -1)||(offset+debut_periode+taille_paquets > fin_periode))
				{
					//printf("Periodicité impossible \n");
					return -2;
				}
				offset +=  debut_periode;
				w_i[eligible]= offset - (noeud_retour[eligible] - periode);
				offset += taille_paquets;
			}
			else
			{
				w_i[eligible]=0;
				offset=taille_paquets+noeud_retour[eligible];
			}
		}
		else
		{
			//printf("ELIGIBLE EST = %d\n",eligible);
			if((offset+taille_paquets > fin_periode)||(debut_seconde_periode != -1))
			{
			//	printf("2 on essaye de lire premier libre (%d %d %d) \n",offset,periode,debut_periode);
				if(debut_seconde_periode == -1)
					debut_seconde_periode = eligible;
					
				offset = premier_libre(periode_retour,offset-periode-debut_periode,taille_paquets,periode);
			//	printf("2 Premier libre(%d) = %d\n",offset-periode-debut_periode,offset);
				
				if((offset == -1)||(offset+debut_periode+taille_paquets > fin_periode))
				{
					return -2;
				}
				offset +=  debut_periode;
				w_i[eligible]= offset - (noeud_retour[eligible] - periode);
				offset += taille_paquets;
			}
			else
			{
				w_i[eligible]=(offset-g.matrice[nb_routes][eligible+nb_routes+1])-(m_i[eligible]+routes[eligible] );			
				
				offset+=taille_paquets;
			}

		}

		//printf(" Eligible = %d , offset = %d, wi = %d, et on rempli la periode de %d à %d\n",eligible,offset,w_i[eligible],offset-taille_paquets-debut_periode,offset-debut_periode);
		for(int i=offset-debut_periode-taille_paquets;i<offset-debut_periode;i++)
			periode_retour[i]=1;
		nb_assigned++;

		//printf("ELIGIBLE = %d \n",eligible);
		noeud_retour[eligible]=-1;

	}

	//affiche_tab(m_i,nb_routes);
	//affiche_tab(w_i,nb_routes);

	if(!is_ok(g,taille_paquets,m_i,w_i,periode)){printf("ERROR 2!!\n");exit(16);}
	//printf("-------------------------\n\n");
	int max = w_i[0]+2*routes[0];
	for(int i=0;i<nb_routes;i++)
	{
		if(w_i[i]+2*routes[i] > Tmax)
		{
			/*affiche_matrice(g);
			

			printf("Tmax depassé (%d = %d + %d + %d)\n\n",w_i[i]+2*routes[i]+taille_paquets,w_i[i],2*routes[i],taille_paquets);*/
			return -1;
		}
		if(w_i[i]+2*routes[i] > max)
			max= w_i[i]+2*routes[i];
	}
	//printf("Tmax = %d \n",max);
	//affiche_etoile(g);
	// affiche_solution(g,taille_paquets,m_i, w_i);
	return max;
}
/*
int stochastic(Graphe g, int taille_paquets, int periode, int tmax)
{
	if(!(g.N%2)){printf("Nombre de sommets impair, G n'est peut être pas une étoile\n");exit(5);}
	int nb_routes = g.N/2;
	int offset=0;
	int m_i[nb_routes];
	int aller[nb_routes];
	int aller_recales[nb_routes];
	int attente_aller[nb_routes];
	int w_i[nb_routes];
	int arrivees[nb_routes];
	int arrivees_recales[nb_routes];


	//Tirage au hasard de temps d'arrivées dans cs des messages
	for(int i=0;i<nb_routes;i++)aller_recales[i]=aller[i]=rand()%(periode-taille_paquets);

		
	int eligible;
	for(int i=0;i<nb_routes;i++)//calcul des buffers et temps aller
	{
		eligible= lower(aller,nb_routes);
		
		if(offset < aller[eligible])//pas de buffer
		{
			attente_aller[eligible] = 0;
			m_i[eligible]= aller[eligible] - g.matrice[nb_routes][eligible];
			offset = aller[eligible] + taille_paquets;

		}
		else//bufferistation
		{
			m_i[eligible]= offset - g.matrice[nb_routes][eligible];
			attente_aller[eligible] = offset - m_i[eligible];
			aller_recales[eligible] = offset;
			
			offset += taille_paquets;
		}
		
		aller[eligible] = INT_MAX;
	}
	
	offset = 0;
	for(int i=0;i<nb_routes;i++)arrivees_recales[i] = arrivees[i]=aller_recales[i]+2*g.matrice[nb_routes][nb_routes+i+1];

	for(int i=0;i<nb_routes;i++)//on rajoute les décalages 
	{
		eligible = lower(arrivees,nb_routes);
	
		if(offset < arrivees[eligible])//pas de buffer
		{
			offset = arrivees[eligible] + taille_paquets;
			w_i[eligible] = 0;
		}
		else//bufferistation
		{
			w_i[eligible] = offset - arrivees[eligible];
			arrivees_recales[eligible] = offset;
			offset += taille_paquets;
		}

		arrivees[eligible] = INT_MAX;
	}
	
	

	int taille_periode_aller = aller_recales[greater(aller_recales,nb_routes)]-aller_recales[lower(aller_recales,nb_routes)]+taille_paquets;
	if(taille_periode_aller > periode)return -1;
	int taille_periode_retour  = arrivees_recales[greater(arrivees_recales,nb_routes)]-arrivees_recales[lower(arrivees_recales,nb_routes)]+taille_paquets;
	if(taille_periode_retour > periode)return -1;
	if(!is_ok(g,taille_paquets,m_i,w_i,periode)){printf("ERROR randoms!!\n");exit(12);}

	int routes[nb_routes];
	for(int i=0;i<nb_routes;i++)routes[i]=g.matrice[nb_routes][i]+g.matrice[nb_routes][i+nb_routes+1];


	int max = attente_aller[0]+w_i[0]+2*routes[0];
	for(int i=0;i<nb_routes;i++)
	{
		if( attente_aller[i]+w_i[i]+2*routes[i] > tmax)
		{
			//affiche_tab(attente_aller,nb_routes);
			//printf("%d %d (=%d+%d+%d)\n",tmax ,attente_aller[i]+w_i[i]+2*routes[i],attente_aller[i],w_i[i],2*routes[i]  );
			return -1;
			
		}
		if( attente_aller[i] +w_i[i]+2*routes[i] > max)
		{
			max=  attente_aller[i]+ w_i[i]+2*routes[i];
		}
	}

	return max;	

}
*/





/**********
Recherches Linéaires *****/



int linear_simons_per(Graphe g,int periode,int taille_message,int nb_rand)
{
	int result;
	int tmax;
	int marge =0;
	int longest = longest_route(g);
	do{
		tmax =longest + marge;
		for(int compteur_rand = 0;compteur_rand<nb_rand;compteur_rand++)
			{
				result = simons_periodique(g,taille_message,tmax,periode,0);
				if(result != -2)
				{	
					if(result != -1)
					{
						
						return marge;

					}
					
				}
			}

		marge++;

	}while( (result == -1) && (result != -2 ));
	return marge;
}
/*
int linear_stochastic(Graphe g,int periode,int taille_message)
{
	int result;
	int tmax;
	int marge =0;

	int longest = longest_route(g);
	do
	{
		tmax =longest + marge;
		result = stochastic(g, taille_message, periode,tmax);

		marge++;
		//affiche_etoile(g);
		

	}while((result == -1 )||(result == -2));
	return marge;
}*/

