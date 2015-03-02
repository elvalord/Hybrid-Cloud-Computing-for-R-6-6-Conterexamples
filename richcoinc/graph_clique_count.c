#include "graph.h"

#define sgsize (6)

/*
 ***
 *** returns the number of monochromatic cliques in the graph presented to
 *** it
 ***
 *** graph is stored in row-major order
 *** only checks values above diagonal
 */

int CliqueCount(int *g,
	     int gsize)
{
    int i;
    int j;
    int k;
    int l;
    int m;
    int n;
    int count=0;
    
    for(i=0;i < gsize-sgsize+1; i++)
    {
	for(j=i+1;j < gsize-sgsize+2; j++)
        {
	    for(k=j+1;k < gsize-sgsize+3; k++) 
            { 
		if((g[i*gsize+j] == g[i*gsize+k]) && 
		   (g[i*gsize+j] == g[j*gsize+k]))
		{
		    for(l=k+1;l < gsize-sgsize+4; l++) 
		    { 
			if((g[i*gsize+j] == g[i*gsize+l]) && 
			   (g[i*gsize+j] == g[j*gsize+l]) && 
			   (g[i*gsize+j] == g[k*gsize+l]))
			{
			    for(m=l+1;m < gsize-sgsize+5; m++) 
			    {
				if((g[i*gsize+j] == g[i*gsize+m]) && 
				   (g[i*gsize+j] == g[j*gsize+m]) &&
				   (g[i*gsize+j] == g[k*gsize+m]) && 
				   (g[i*gsize+j] == g[l*gsize+m]))
				{
				  for(n=m+1;n<gsize-sgsize+6;n++)
					{
					  if
				   ((g[i*gsize+j] == g[i*gsize+n]) && 
				   (g[i*gsize+j] == g[j*gsize+n]) &&
				   (g[i*gsize+j] == g[k*gsize+n]) && 
				   (g[i*gsize+j] == g[l*gsize+n]) &&
				   (g[i*gsize+j] == g[m*gsize+n]))
			    {
			      count++;
			    }
					}
	
				}
			    }
			}
		    }
		}
	    }
         }
     }
    return(count);
}
int CliqueCountN(int *g,int gsize,int clique)
{
	if(clique>gsize)
		return 0;
	//init
	int *toTest = (int*)malloc(clique*sizeof(int));
	for(int i=0;i<clique;i++)
		toTest[i] = i;

	int count = 0;
	while(1)
	{
		int hasNClique = 1;
		int color = g[toTest[0]*gsize+toTest[1]];
		for(int i=0;i<clique;i++)
		{
			for(int j=i+1;j<clique;j++)
			{
				if(g[toTest[i]*gsize+toTest[j]]!=color)
				{
					hasNClique = 0;
					break;
				}
			}
			if(hasNClique==0)
				break;
		}

		if(hasNClique==1)
			count++;

		//next
		toTest[clique-1]++;
		int i = clique-1;

		//for(int j=0;j<clique;j++)
		//	printf("%d ",toTest[j]);
		//printf("\n");
		//printf("%d ",count);
		while(1)
		{
			if(toTest[i]>gsize-1-(clique-1-i))
			{
				if(i==0)
					return count;

				toTest[i-1]++;
				i--;
			}
			else
				break;

		}
		
		for(int j=i+1;j<clique;j++)
			toTest[j] = toTest[i] + (j-i);

	}
	return count;

}
double CliqueCountG(int *g, int gsize, double *countg)
{
    int i;
    int j;
    int k;
    int l;
    int m;
    int n;
    double count=0.0;

    /*
     * zero out countg
     */
    if(countg != NULL)
    {
	    memset(countg,0,gsize*gsize*sizeof(double));
    }
    
    for(i=0;i < gsize-sgsize+1; i++)
    {
	for(j=i+1;j < gsize-sgsize+2; j++)
        {
	    for(k=j+1;k < gsize-sgsize+3; k++) 
            { 
		if((g[i*gsize+j] == g[i*gsize+k]) && 
		   (g[i*gsize+j] == g[j*gsize+k]))
		{
		    for(l=k+1;l < gsize-sgsize+4; l++) 
		    { 
			if((g[i*gsize+j] == g[i*gsize+l]) && 
			   (g[i*gsize+j] == g[j*gsize+l]) && 
			   (g[i*gsize+j] == g[k*gsize+l]))
			{
			    for(m=l+1;m < gsize-sgsize+5; m++) 
			    {
				if((g[i*gsize+j] == g[i*gsize+m]) && 
				   (g[i*gsize+j] == g[j*gsize+m]) &&
				   (g[i*gsize+j] == g[k*gsize+m]) && 
				   (g[i*gsize+j] == g[l*gsize+m]))
				{
				  for(n=m+1;n<gsize-sgsize+6;n++)
					{
					  if
				   ((g[i*gsize+j] == g[i*gsize+n]) && 
				   (g[i*gsize+j] == g[j*gsize+n]) &&
				   (g[i*gsize+j] == g[k*gsize+n]) && 
				   (g[i*gsize+j] == g[l*gsize+n]) &&
				   (g[i*gsize+j] == g[m*gsize+n]))
			    {
			      count++;
			      if(countg != NULL)
			      {
				      countg[i*gsize+j]++;  countg[i*gsize+k]++;
				      countg[i*gsize+l]++;  countg[i*gsize+m]++;
				      countg[i*gsize+n]++;  countg[j*gsize+k]++;
				      countg[j*gsize+l]++;  countg[j*gsize+m]++;
				      countg[j*gsize+n]++;  countg[k*gsize+l]++;
				      countg[k*gsize+m]++;  countg[k*gsize+n]++;
				      countg[l*gsize+m]++;  countg[l*gsize+n]++;
				      countg[m*gsize+n]++;
			      }
			    }
					}
	
				}
			    }
			}
		    }
		}
	    }
         }
     }
    return(count);
}








