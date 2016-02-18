/******************************************************

Copyright 2004, 2010 Fabien de Montgolfier
fm@liafa.jussieu.fr

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

**********************************************************/

#include <time.h>
#include "dm_english.h"

#define NIV 20
#define VERBEUX 1
 
//extern Node *decomposition_modulaire(int *,int);
extern void printarbre(Node *N, labelFunction label);
/* ppm est la part par million d'arretes voulues dans le graphe */

void compte(Node *N, int level, int *C)
{
  Child *F;
  switch(N->type)
    {
    case SERIES: C[4 * level]++; break;
    case PARALLEL: C[4 * level + 1]++; break;
    case PRIME: C[4 * level + 2]++; break;
    case LEAF: C[4 * level + 3]++; break;
    }
  if(N->type != LEAF)  
    for(F=N->fils;F!=NULL;F=F->suiv)
      compte(F->pointe, level+1, C);
}

void test(int n, long int ppm, int *C)
{
  /*genere un graphe aleatoire, l'affiche, le decompose*/
  Graph G;

  int i,j;
  Adj *a;
  Node *R;


  srandom((unsigned)time(NULL));

  G.n= (long) n;
  G.G=(Adj **)malloc(n * sizeof(Adj *));

  for(i=0;i<n;i++)
    G.G[i]=NULL;

  printf("Programme test : generation d'un graphe aleatoire de %i sommets\n",n);
  for(i=0; i<n; i++)
    {
      if(VERBEUX)
	{
	  printf("Ajacence de %i: ",i);
	  for(a=G.G[i]; a!=NULL; a=a->suiv) 
	    printf("%ld ",a->s);
	}
      for(j=i+1;j<n;j++)
	{
	if( (random()%1000000L) < ppm )
	  {	
	    // ajoute j a l'adjacence de i
	    a=(Adj *)malloc(sizeof(Adj));
	    a->s= (VertexIdType) j;
	    a->suiv=G.G[i];
	    G.G[i]=a;
	    // et reciproquement
	    a=(Adj *)malloc(sizeof(Adj));
	    a->s= (VertexIdType) i;
	    a->suiv=G.G[j];
	    G.G[j]=a;
	    if(VERBEUX)
	      printf("%i ",j);
	  }
	}
      if(VERBEUX)
	printf("\n");
    }

  // appel de la fonction de decomposition
  R = decomposition_modulaire(G);

  // affichage de l'arbre
  if(VERBEUX)
    printarbre(R, G.labels);

  compte(R,0,C);
  printf("Statistiques sur l'arbre de decomposition:\n");
  if(C[0])
    printf("La racine est Serie\n");
  else if(C[1])
    printf("La racine est Parrallele\n");
  else
    printf("La racine est Premier \n");
  for(i=1 ; i<NIV ; i++)
    {
      printf("Niveau %i: %i modules (S-P-Pr= %i - %i - %i) et %i feuilles\n",i,
       C[4*i]+C[4*i+1]+C[4*i+2], C[4*i], C[4*i+1], C[4*i+2],C[4*i+3]);  
      if(i<NIV-1 && C[4*i+4]+C[4*i+5]+C[4*i+6]+C[4*i+7]==0)
	break;
    }
  printf("\n");
}

int main(int narg, char **arg)
{
  int n;
  long ppm;
  int C[4*NIV];int i;

  if(narg!=3)
    {
      printf("Decomposition modulaire de graphes \"aleatoires\" \n"
	     "Donnez en argument:\n"
	     "le nombre de sommets du graphe\n"
	     "puis la proportion d'aretes en en millioniemes \n"
	     "(generateur aleatoire tres primaire)\n"
	     "Exemple : %s 100 20000\n",arg[0]);
      exit(0);
    }
  n=atoi(arg[1]);
  ppm=atol(arg[2]);
  for(i=0;i<4*NIV;i++) C[i]=0;

  test(n, ppm, C);
  
  return 0;
}





