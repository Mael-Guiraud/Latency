#include "structs.h"
#include "treatment.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
//return 1 if the message can reach the destination without collision, 0 otherwise
int message_no_collisions(Graph g,int route,int offset,int message_size,Period_kind kind,int P)
{
	if(kind == FORWARD)
	{
		//For each arcs
		for(int i=0;i<g.size_routes[route];i++)
		{
			
			//This is a contention point
			if(g.routes[route][i]->period_f != NULL)
			{
				
				if(g.routes[route][i]->period_f[offset%P] || g.routes[route][i]->period_f[(offset+message_size-1)%P] )
				{
					
					return 0;
				}
				
			}
			offset += g.routes[route][i]->length; 
		}
	}
	else
	{
		//For each arcs
		for(int i=g.size_routes[route]-1;i>=0;i--)
		{
			
			//This is a contention point
			if(g.routes[route][i]->period_b != NULL)
			{
				if(g.routes[route][i]->period_b[offset%P] || g.routes[route][i]->period_b[(offset+message_size-1)%P] )
				{
					//printf("[%d %d %d %d ]",offset,offset%P,g.routes[route][i]->period_b[offset%P],g.routes[route][i]->period_b[(offset+message_size-1)%P]);
					return 0;
				}
				
			}
			offset += g.routes[route][i]->length; 
		}
	}
	return 1;

				
}

void fill_period(Graph g,int route,int offset,int message_size,Period_kind kind,int P)
{

	if(kind == FORWARD)
	{
		//For each arcs
		for(int i=0;i<g.size_routes[route];i++)
		{
			
			//This is a contention point
			if(g.routes[route][i]->period_f != NULL)
			{
				for(int j=0;j<message_size;j++)
				{
					if(route == 0)
						g.routes[route][i]->period_f[(offset+j)%P]=-1;
					else
						g.routes[route][i]->period_f[(offset+j)%P]=route;
				}
					
				
			}
			offset += g.routes[route][i]->length;
		}
	}
	else
	{
		//For each arcs
		for(int i=g.size_routes[route]-1;i>=0;i--)
		{
			
			//This is a contention point
			if(g.routes[route][i]->period_f != NULL)
			{
				for(int j=0;j<message_size;j++)
				{
					if(route ==0)
						g.routes[route][i]->period_b[(offset+j)%P]=-1;
					else
						g.routes[route][i]->period_b[(offset+j)%P]=route;
				}
					
				
			}
			offset += g.routes[route][i]->length;
		}
	}
}
int route_length(Graph g,int route)
{
	int length = 0;
	//printf("%d \n",g.size_routes[route]);
	//For each arcs
	for(int i=0;i<g.size_routes[route];i++)
	{
		length += g.routes[route][i]->length;
	}
	return length;
}
int insert_if_not_seen(int * tab, int sizetab, int e)
{
	int i;
	for(i=0;(i<sizetab && tab[i] != -1);i++)
	{
		if(tab[i] == e)
			return 0; 
	}
	if(i== sizetab){printf("Error, tab too short (insert_if_not_seen() )\n");exit(96);}
	tab[i] = e;
	return 1;
}
int nb_collisions_route(Graph g, int route)
{
	int seen[128];
	int cols = 0;
	for(int i= 0;i<128;i++)
	{
		seen[i] = -1;
	}
	for(int i=0;i<g.size_routes[route];i++)
	{
		for(int j=0;j<g.routes[route][i]->nb_routes;j++)
		{
			cols += insert_if_not_seen(seen,128,g.routes[route][i]->routes_id[j]);
		}
	}
	return cols-1; //-1 bcause we dont count the route itself
}
int * routes_by_id(Arc a)
{
	int * id = (int*)malloc(sizeof(int)*a.nb_routes);
	for(int i=0;i<a.nb_routes;i++)
	{
		id[i]=a.routes_id[i];
	}
	return id;
}


void moove_elems(int* tab, int debut, int fin)
{
	for(int i=fin-1;i>=debut;i--)
	{
		tab[i+1] = tab[i];
	}
	tab[debut] = 0;
}
int * routes_sorted_lenght_arcs_bbu(Graph g)
{
	int * id = (int*)malloc(sizeof(int)*g.nb_routes);
	int route_lenghts[g.nb_routes];
	for(int i=0;i<g.nb_routes;i++)
		route_lenghts[i] = route_length(g,i);
	int id_fin =1;
	int id_tmp;
	id[0] = 0;
	for(int i=1;i<g.nb_routes;i++)
	{
	
		for(id_tmp = 0;id_tmp<id_fin;id_tmp++)
		{
			if(route_lenghts[i] > route_lenghts[id_tmp])
			{
				moove_elems(id,id_tmp,id_fin);
				id[id_tmp] = i;
				break;
				
			}
			else
			{
				if(route_lenghts[i] == route_lenghts[id_tmp])
				{
					if(g.size_routes[i] > g.size_routes[id_tmp])
					{
						moove_elems(id,id_tmp,id_fin);
						id[id_tmp] = i;
						break;
					}
					else
					{
						if(g.size_routes[i] == g.size_routes[id_tmp])
						{
							if(g.routes[i][0]->bbu_dest > g.routes[id_tmp][0]->bbu_dest)
							{
								moove_elems(id,id_tmp,id_fin);
								id[id_tmp] = i;
								break;
							}
						}
					}
				}
			}
		}
		if(id_tmp == id_fin)
		{
		
			id[id_tmp] = i;
		}
		id_fin++;

	}
	return id;

}
int * sort_longest_routes_on_arc(Graph g, Arc a)
{
	int * id = (int*)malloc(sizeof(int)*a.nb_routes);
	int routes_length[a.nb_routes];
	for(int i=0;i<a.nb_routes;i++)
	{
		id[i]=a.routes_id[i];
		routes_length[i]=route_length(g,a.routes_id[i]);
	}
	tri_bulles(routes_length,id,a.nb_routes);
	return id;
}
int * sort_routes_by_collisions(Graph g, Arc a)
{
	int * id = (int*)malloc(sizeof(int)*a.nb_routes);
	int routes_colls[a.nb_routes];
	for(int i=0;i<a.nb_routes;i++)
	{
		id[i]=a.routes_id[i];
		routes_colls[i]=nb_collisions_route(g,a.routes_id[i]);
	}
	tri_bulles(routes_colls,id,a.nb_routes);
	return id;
}
int longest_route(Graph g)
{
	int max = 0;
	int tmp;
	for(int i=0;i<g.nb_routes;i++)
	{
		tmp = route_length(g,i);
		max = (max>tmp)?max:tmp;
	}
	return max;
}
void reset_periods(Graph g, int P)
{
	for(int i=0;i<g.arc_pool_size;i++)
	{
		if(g.arc_pool[i].period_f)
		{
			for(int j=0;j<P;j++)
			{
				g.arc_pool[i].period_f[j]=0;
				g.arc_pool[i].period_b[j]=0;
			}
		}
	}
}
int route_length_untill_arc(Graph g,int route, Arc * a,Period_kind kind)
{
	int length = 0;
	
	if(kind == FORWARD)
	{
		for(int i=0;i<g.size_routes[route];i++)
		{

			
			if(a == g.routes[route][i])
				return length;
			length += g.routes[route][i]->length;
		}
		
	}
	else
	{
		for(int i=g.size_routes[route]-1;i>=0;i--)
		{

			if(a == g.routes[route][i])
				return length;
			length += g.routes[route][i]->length;
		}
		
	}	
	return length;		
	
}



void free_assignment(Assignment a)
{
	free(a->offset_forward);
	free(a->offset_backward);
	free(a->waiting_time);
	free(a);
}
void free_graph(Graph g)
{
	
	for(int i=0;i<g.nb_routes;i++)
	{
		for(int j=0;j<g.size_routes[i];j++)
		{
			if(g.routes[i][j]->nb_routes >= 1)
			{
				free(g.routes[i][j]->period_f);
				free(g.routes[i][j]->period_b);
				g.routes[i][j]->nb_routes = 0;
				
			}
		}
	}

	free(g.arc_pool);
	for(int i=0;i<g.nb_routes;i++)
		free(g.routes[i]);
	free(g.routes);
	free(g.size_routes);
}

//trie le tableau "odre"
//Ordre contient les indices des elements, mais il est trié en fonction des valeurs de "tab"
// Du plus grand au plus petit

void tri_bulles(int* tab,int* ordre,int taille)
{
	int sorted;
	int tmp;
	int tmp_ordre;

	int tabcpy[taille];
	for(int i=0;i<taille;i++)tabcpy[i]=tab[i];

	for(int i=taille-1;i>=1;i--)
	{
		sorted = 1;
		for(int j = 0;j<=i-1;j++)
		{

			if(tabcpy[j+1]>tabcpy[j])
			{
				tmp_ordre = ordre[j+1];
				ordre[j+1]=ordre[j];
				ordre[j]=tmp_ordre;
				tmp = tabcpy[j+1];
				tabcpy[j+1]= tabcpy[j];
				tabcpy[j]= tmp;
				sorted = 0;
			}
		}
		if(sorted){return;}
	}

}


//Pareil mais trié du plus petit au plus grand
void tri_bulles_inverse(int* tab,int* ordre,int taille)
{
	int sorted;
	int tmp;
	int tmp_ordre;

	int tabcpy[taille];
	for(int i=0;i<taille;i++)tabcpy[i]=tab[i];

	for(int i=taille-1;i>=1;i--)
	{
		sorted = 1;
		for(int j = 0;j<=i-1;j++)
		{

			if(tabcpy[j+1]<tabcpy[j])
			{
				tmp_ordre = ordre[j+1];
				ordre[j+1]=ordre[j];
				ordre[j]=tmp_ordre;
				tmp = tabcpy[j+1];
				tabcpy[j+1]= tabcpy[j];
				tabcpy[j]= tmp;
				sorted = 0;
			}
		}
		if(sorted){return;}
	}

}


int travel_time_max(Graph g, int tmax, Assignment a)
{
	int first_forward =  a->offset_forward[0];
	for(int i=0;i<g.nb_routes;i++)
	{
		if(a->offset_forward[i] < first_forward)
		{
			first_forward = a->offset_forward[i];
		}
	}
	
	int max;
	if(SYNCH)
	{
		max = a->offset_forward[0]-first_forward + 2*route_length( g,0) + a->waiting_time[0];
	}
	else
	{
		max = 2*route_length( g,0) + a->waiting_time[0];
	}
	for(int i=0;i<g.nb_routes;i++)
	{
		if(SYNCH)
		{
			if( (a->offset_forward[i]-first_forward + 2*route_length( g,i) + a->waiting_time[i] ) > tmax )
				return -1;
			if((a->offset_forward[i]-first_forward + 2*route_length( g,i) + a->waiting_time[i] ) > max )
			{
				max = a->offset_forward[i]-first_forward + 2*route_length( g,i) + a->waiting_time[i] ;
			}

		}
		else
		{
			if( (2*route_length( g,i) + a->waiting_time[i] ) > tmax )
				return -1;
			if((2*route_length( g,i) + a->waiting_time[i] ) > max )
			{
				max = + 2*route_length( g,i) + a->waiting_time[i] ;
			}
		}
	}
	return max;
}

int * load_links(Graph g)
{
	int load[g.arc_pool_size];
	int * id = malloc(sizeof(int)*g.arc_pool_size);
	for(int i=0;i<g.arc_pool_size;i++)
	{
		load[i] = g.arc_pool[i].nb_routes;
		id[i] = i;
	}
	tri_bulles(load,id,g.arc_pool_size);
	return id;
}

int  load_max(Graph g)
{
	int loadmax=0;
	for(int i=0;i<g.arc_pool_size;i++)
	{
	
		if(g.arc_pool[i].nb_routes > loadmax)
		loadmax = g.arc_pool[i].nb_routes;

	}
	
	return loadmax;
}