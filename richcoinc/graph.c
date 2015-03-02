#include "graph.h"

/*
 * PrintGraph
 *
 * prints in the right format for the read routine
 */
void PrintGraph(int *g, int gsize)
{
	int i;
	int j;

	fprintf(stdout,"%d\n",gsize);

	for(i=0; i < gsize; i++)
	{
		for(j=0; j < gsize; j++)
		{
			fprintf(stdout,"%d ",g[i*gsize+j]);
		}
		fprintf(stdout,"\n");
	}

	return;
}

/* additional graph functions comes here */

int *PaleyGenerator(int n)//n should be prime
{
	JRB dists = make_jrb();
	int i=1;
	for(;i<n;i++)
	{
		int m = i*i%n;
		if(jrb_find_int(dists,m)!=NULL) continue;
		jrb_insert_int(dists,m,new_jval_i(m));
	}

	

	//graph
	int *g = (int*)malloc(n*n*sizeof(int));

	for(i=0;i<n;i++)
	{
		JRB temp;
		jrb_rtraverse(temp,dists)
		{
			int v1 = i;
			int v2 = (i + temp->val.i)%n;
			if(v1>v2)//make sure v1<v2
			{
				int j=v1;
				v1=v2;
				v2=j;
			}
			g[v1*n+v2] = 1;

		}
	}

	return g;
}
int *RandomGraphGenerator(int n)
{
	int *g = (int*)malloc(n*n*sizeof(int));
	int i=0,j=0;
	//srand(time(0));
	for(i=0;i<n;i++)
	{
		for(j=i+1;j<n;j++)
			g[i*n+j] = rand()%2;
	}
	return g;
}
int *RandomSubgraph(int *g, int gsize,int min,int *newSize)
{
	int gsizeNew = min + rand()%(gsize-min);
	*newSize = gsizeNew;
	JRB toDel = make_jrb();
	int toDelCount = 0;
	while(toDelCount<gsize-gsizeNew)
	{
		int toDelNext = rand()%gsizeNew;
		if(jrb_find_int(toDel,toDelNext)==NULL)
		{

			toDelCount++;
			jrb_insert_int(toDel,toDelNext,new_jval_i(toDelNext));
		}
	}

	int *gNewFromOld = (int*)malloc(gsizeNew*sizeof(int));
	int i,j=0;
	for(i=0;i<gsize;i++)
	{
		if(jrb_find_int(toDel,i)==NULL)
		{
			gNewFromOld[j] = i;
			j++;
		}
	}
	//for(i=0;i<gsizeNew;i++) printf("%d ",gNewFromOld[i]);
	//printf("\n");
	int *gNew = (int*)malloc(gsizeNew*gsizeNew*sizeof(int));
	for(i=0;i<gsizeNew;i++)
		for(j=0;j<gsizeNew;j++)
			gNew[i*gsizeNew+j] = g[gNewFromOld[i]*gsize+gNewFromOld[j]];

	return gNew;

}
int** DegradeFrom101Paley()
{
	//generate all 100 and 99 from 101 Paley
	int **g = (int**)malloc(sizeof(int*)*(101+101*50));
	int *g101= PaleyGenerator(101);
	//all 100
	int i=0;
	int p,q;
	for(i=0;i<101;i++)
	{
		g[i] = (int*)malloc(sizeof(int)*100*100);
		for(p=0;p<101;p++)
		{
			for(q=0;q<101;q++)
			{
				int a,b;
				if(p<i)
					a=p;
				else if(p>i)
					a=p-1;
				else
					continue;

				if(q<i)
					b=q;
				else if(q>i)
					b=q-1;
				else
					continue;

				g[i][a*100+b] = g101[p*101+q];
			}
		}
	}
	int j = 0;
	int counter = 0;
	for(i=0;i<100;i++)
	{
		for(j=i+1;j<101;j++)
		{
			int n = 101 + counter;
			counter++;
			g[n] = (int*)malloc(sizeof(int)*99*99);
			for(p=0;p<101;p++)
			{
				for(q=0;q<101;q++)
				{
					int a,b;
					if(p<i)
						a=p;
					else if(p>i && p<j)
						a=p-1;
					else if(p>j)
						a=p-2;
					else
						continue;

					if(q<i)
						b=q;
					else if(q>i && q<j)
						b=q-1;
					else if(q>j)
						b=q-2;
					else
						continue;

					g[n][a*99+b] = g101[p*101+q];
				}
			}
		}
	}
	return g;
}

void filterIsomorphism(int **g,int n,int graphSize)
{
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<i;j++)
			if(g[j]!=NULL)
				if(IsIsomorph(g[i],g[j],graphSize)==1)
					break;
		if(j<i)
		{
			if(g[i]!=NULL)
			{
				free(g[i]);
				g[i] = NULL;
			}
		}
	}

}
void filterIsomorphismAndWrite(int **g,int n,int graphSize,void (*writefile)(char*,int*,int),char* filename)
{
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<i;j++)
			if(g[j]!=NULL)
				if(IsIsomorph(g[i],g[j],graphSize)==1)
					break;
		if(j<i)
		{
			if(g[i]!=NULL)
			{
				free(g[i]);
				g[i] = NULL;
			}
		}
		else
		{
			(*writefile)(filename,g[i],graphSize);
		}
	}

}


