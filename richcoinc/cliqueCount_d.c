#include "graph.h"

#define sgsize (6)

int CliqueCount_D(int *g, int gsize, int i, int j) //i*gsize+j
{
    //int i;
    //int j;
    int k;
    int l;
    int m;
    int n;
    int count=0;
    
    //for(i=0;i < gsize-sgsize+1; i++)
    //{
	//for(j=i+1;j < gsize-sgsize+2; j++)
        //{
    for(k=0;k < gsize-sgsize+3; k++) 
    { 
    	if(k==i || k==j) continue;
    	
    	int k_array[2];
    	if(i<k) k_array[0]=i*gsize+k;
    	else 	k_array[0]=k*gsize+i;
    	if(j<k) k_array[1]=j*gsize+k;
    	else 	k_array[1]=k*gsize+j;

		if((g[i*gsize+j] == g[k_array[0]]) && 
		   (g[i*gsize+j] == g[k_array[1]]))
		{
		    for(l=k+1;l < gsize-sgsize+4; l++) 
		    {
		    	if(l==i || l==j) continue;

		    	int l_array[2];
		    	if(i<l) l_array[0]=i*gsize+l;
		    	else	l_array[0]=l*gsize+i;
		    	if(j<l) l_array[1]=j*gsize+l;
		    	else	l_array[1]=l*gsize+j;

				if((g[i*gsize+j] == g[l_array[0]]) && 
				   (g[i*gsize+j] == g[l_array[1]]) && 
				   (g[i*gsize+j] == g[k*gsize+l]))
				{
				    for(m=l+1;m < gsize-sgsize+5; m++) 
				    {
				    	if(m==i || m==j) continue;

				    	int m_array[2];
				    	if(i<m) m_array[0]=i*gsize+m;
				    	else	m_array[0]=m*gsize+i;
				    	if(j<m) m_array[1]=j*gsize+m;
				    	else	m_array[1]=m*gsize+j;


						if((g[i*gsize+j] == g[m_array[0]]) && 
						   (g[i*gsize+j] == g[m_array[1]]) &&
						   (g[i*gsize+j] == g[k*gsize+m]) && 
						   (g[i*gsize+j] == g[l*gsize+m]))
						{
						  for(n=m+1;n<gsize-sgsize+6;n++)
							{
								if(n==i || n==j) continue;

								int n_array[2];
				    			if(i<n) n_array[0]=i*gsize+n;
				    			else	n_array[0]=n*gsize+i;
				    			if(j<n) n_array[1]=j*gsize+n;
				    			else	n_array[1]=n*gsize+j;

							  if((g[i*gsize+j] == g[n_array[0]]) && 
							   (g[i*gsize+j] == g[n_array[1]]) &&
							   (g[i*gsize+j] == g[k*gsize+n]) && 
							   (g[i*gsize+j] == g[l*gsize+n]) &&
							   (g[i*gsize+j] == g[m*gsize+n]))
							    {
							    	//printf("%d,%d,%d,%d,%d,%d.\n",i,j,k,l,m,n);
							      	count++;
							    }
							}

						}
				    }
				}
		    }
		}
    }
         //}
     //}
    return(count);
}

int CliqueCount_D_with_flip(int *g, int gsize, int i, int j) //i*gsize+j
{//parallel safe
    //int i;
    //int j;
    int k;
    int l;
    int m;
    int n;
    int count=0;
    
    //for(i=0;i < gsize-sgsize+1; i++)
    //{
	//for(j=i+1;j < gsize-sgsize+2; j++)
        //{
    int color = 1 - g[i*gsize+j];
    for(k=0;k < gsize-sgsize+3; k++) 
    { 
    	if(k==i || k==j) continue;
    	
    	int k_array[2];
    	if(i<k) k_array[0]=i*gsize+k;
    	else 	k_array[0]=k*gsize+i;
    	if(j<k) k_array[1]=j*gsize+k;
    	else 	k_array[1]=k*gsize+j;

		if((color == g[k_array[0]]) && 
		   (color == g[k_array[1]]))
		{
		    for(l=k+1;l < gsize-sgsize+4; l++) 
		    {
		    	if(l==i || l==j) continue;

		    	int l_array[2];
		    	if(i<l) l_array[0]=i*gsize+l;
		    	else	l_array[0]=l*gsize+i;
		    	if(j<l) l_array[1]=j*gsize+l;
		    	else	l_array[1]=l*gsize+j;

				if((color == g[l_array[0]]) && 
				   (color == g[l_array[1]]) && 
				   (color == g[k*gsize+l]))
				{
				    for(m=l+1;m < gsize-sgsize+5; m++) 
				    {
				    	if(m==i || m==j) continue;

				    	int m_array[2];
				    	if(i<m) m_array[0]=i*gsize+m;
				    	else	m_array[0]=m*gsize+i;
				    	if(j<m) m_array[1]=j*gsize+m;
				    	else	m_array[1]=m*gsize+j;


						if((color == g[m_array[0]]) && 
						   (color == g[m_array[1]]) &&
						   (color == g[k*gsize+m]) && 
						   (color == g[l*gsize+m]))
						{
						  for(n=m+1;n<gsize-sgsize+6;n++)
							{
								if(n==i || n==j) continue;

								int n_array[2];
				    			if(i<n) n_array[0]=i*gsize+n;
				    			else	n_array[0]=n*gsize+i;
				    			if(j<n) n_array[1]=j*gsize+n;
				    			else	n_array[1]=n*gsize+j;

							  if((color == g[n_array[0]]) && 
							   (color == g[n_array[1]]) &&
							   (color == g[k*gsize+n]) && 
							   (color == g[l*gsize+n]) &&
							   (color == g[m*gsize+n]))
							    {
							    	//printf("%d,%d,%d,%d,%d,%d.\n",i,j,k,l,m,n);
							      	count++;
							    }
							}

						}
				    }
				}
		    }
		}
    }
         //}
     //}
    return(count);
}