#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "graph.h"	/* for taboo list */


#define MAXSIZE (512)

#define TABOOSIZE (500)
#define BIGCOUNT (9999999)
int *g0;
int
taboo_search_D(int gsize,void (*writefile)(char*,int*,int),char* filename,
	void (*writesocket)(int*,int,char*,int),char* host,int port)
{
	int *g;
	int *new_g;
	int count;
	int i;
	int j;
	int best_count;
	int best_i;
	int best_j;
	void *taboo_list;

	/*
	 * start with graph of size 8
	 */
	//g = (int *)malloc(gsize*gsize*sizeof(int));
	g = RandomGraphGenerator(gsize);
	//for test
	//g0 = (int*)malloc(gsize*gsize*sizeof(int));
	//memcpy(g0,g,gsize*gsize*sizeof(int));
	//end for test
	if(g == NULL) {
		exit(1);
	}

	/*
	 * make a fifo to use as the taboo list
	 */
	taboo_list = FIFOInitEdge(TABOOSIZE);
	if(taboo_list == NULL) {
		exit(1);
	}

	/*
	 * start out with all zeros
	 */
	//memset(g,0,gsize*gsize*sizeof(int));

	/*
	 * while we do not have a publishable result
	 */
	//while(gsize < 44)
	count = CliqueCount(g,gsize);
	while(1)
	{
		/*
		 * find out how we are doing
		 */
		//count = CliqueCount(g,gsize);

		/*
		 * if we have a counter example
		 */
		if(count == 0)
		{
			printf("Eureka!  Counter-example found!\n");
			PrintGraph(g,gsize);
			
			//return;//for test
			writefile(filename,g,gsize);
			writesocket(g,gsize,host,port);
			/*
			 * make a new graph one size bigger
			 */
			free(g);
			g = NULL;
			g = RandomGraphGenerator(gsize);
			PrintGraph(g,gsize);
			if(g == NULL)
				exit(1);
			/*
			 * copy the old graph into the new graph leaving the
			 * last row and last column alone
			 */
			//CopyGraph(g,gsize,new_g,gsize+1);

			/*
			 * zero out the last column and last row
			 */
			//for(i=0; i < (gsize+1); i++)
			//{
			//	new_g[i*(gsize+1) + gsize] = 0; // last column
			//	new_g[gsize*(gsize+1) + i] = 0; // last row
			//}

			/*
			 * throw away the old graph and make new one the
			 * graph
			 */
			//gsize = gsize+1;

			/*
			 * reset the taboo list for the new graph
			 */
			taboo_list = FIFOResetEdge(taboo_list);

			/*
			 * keep going
			 */
			continue;
		}

		/*
		 * otherwise, we need to consider flipping an edge
		 *
		 * let's speculative flip each edge, record the new count,
		 * and unflip the edge.  We'll then remember the best flip and
		 * keep it next time around
		 *
		 * only need to work with upper triangle of matrix =>
		 * notice the indices
		 */
		best_count = BIGCOUNT;
		for(i=0; i < gsize; i++)
		{
			for(j=i+1; j < gsize; j++)
			{
				/*
				 * flip it
				 */
				int old_clique = CliqueCount_D(g,gsize,i,j);
				g[i*gsize+j] = 1 - g[i*gsize+j];
				int new_clique = CliqueCount_D(g,gsize,i,j);

				int temp_count = count + new_clique - old_clique;

				if(temp_count<0)
				{
					temp_count = 0;
				}
				/*
				 * is it better and the i,j,count not taboo?
				 */
				if((temp_count < best_count) && 
					!FIFOFindEdge(taboo_list,i,j))
//					!FIFOFindEdgeCount(taboo_list,i,j,count))
				{
					best_count = temp_count;
					best_i = i;
					best_j = j;
				}

				/*
				 * flip it back
				 */
				g[i*gsize+j] = 1 - g[i*gsize+j];
			}
		}

		if(best_count == BIGCOUNT) {
			printf("no best edge found, restart\n");
			free(g);
			g = RandomGraphGenerator(gsize); 
		}
		
		/*
		 * keep the best flip we saw
		 */
		g[best_i*gsize+best_j] = 1 - g[best_i*gsize+best_j];
		count = best_count;
		/*
		 * taboo this graph configuration so that we don't visit
		 * it again
		 */
		//count = CliqueCount(g,gsize);
		FIFOInsertEdge(taboo_list,best_i,best_j);
//		FIFOInsertEdgeCount(taboo_list,best_i,best_j,count);

		printf("ce size: %d, best_count: %d, best edge: (%d,%d), new color: %d\n",
			gsize,
			best_count,
			best_i,
			best_j,
			g[best_i*gsize+best_j]);

		/*
		 * rinse and repeat
		 */
	}

	FIFODeleteGraph(taboo_list);


	return(0);

}

int
taboo_search_D_openmp(int gsize,void (*writefile)(char*,int*,int),char* filename,
	void (*writesocket)(int*,int,char*,int),char* host,int port)
{
	int *g;
	int *new_g;
	int count;
	int i;
	int j;
	int best_count;
	int best_i;
	int best_j;
	void *taboo_list;
	int threads = omp_get_max_threads();
	int **g_temp = (int**)malloc(threads*sizeof(int*));
	for(i=0;i<threads;i++)
	{
		g_temp[i] = (int*)malloc(gsize*gsize*sizeof(int));
	}
	/*
	 * start with graph of size 8
	 */
	//g = (int *)malloc(gsize*gsize*sizeof(int));
	g = RandomGraphGenerator(gsize);
	//g = g0;
	if(g == NULL) {
		exit(1);
	}

	/*
	 * make a fifo to use as the taboo list
	 */
	taboo_list = FIFOInitEdge(TABOOSIZE);
	if(taboo_list == NULL) {
		exit(1);
	}

	/*
	 * start out with all zeros
	 */
	//memset(g,0,gsize*gsize*sizeof(int));

	/*
	 * while we do not have a publishable result
	 */
	//while(gsize < 44)
	count = CliqueCount(g,gsize);
	int* cliques = (int*)malloc(gsize*gsize*sizeof(int));
	while(1)
	{
		/*
		 * find out how we are doing
		 */
		//count = CliqueCount(g,gsize);

		/*
		 * if we have a counter example
		 */
		if(count == 0)
		{
			
			printf("Eureka!  Counter-example found!\n");
			PrintGraph(g,gsize);
			//return;//for test
			writefile(filename,g,gsize);
			writesocket(g,gsize,host,port);
			/*
			 * make a new graph one size bigger
			 */
			free(g);
			g = NULL;
			g = RandomGraphGenerator(gsize);
			PrintGraph(g,gsize);
			if(g == NULL)
				exit(1);
		
			taboo_list = FIFOResetEdge(taboo_list);

			/*
			 * keep going
			 */
			continue;
		}

		/*
		 * otherwise, we need to consider flipping an edge
		 *
		 * let's speculative flip each edge, record the new count,
		 * and unflip the edge.  We'll then remember the best flip and
		 * keep it next time around
		 *
		 * only need to work with upper triangle of matrix =>
		 * notice the indices
		 */
		best_count = BIGCOUNT;

		for(i=0;i<threads;i++)
			memcpy(g_temp[i],g,gsize*gsize*sizeof(int));
		
		#pragma omp parallel for private(i,j) shared(g_temp)
		for(i=0; i < gsize; i++)
		{

			int tid = omp_get_thread_num();
			/*
			char testtt[] = "000";
			testtt[0] = '0' + tid;
			write(1,testtt,3);//test
			*/
			for(j=i+1; j < gsize; j++)
			{
				/*
				 * flip it
				 */
				int old_clique = CliqueCount_D(g_temp[tid],gsize,i,j);
				//g[i*gsize+j] = 1 - g[i*gsize+j];
				int new_clique = CliqueCount_D_with_flip(g_temp[tid],gsize,i,j);

				cliques[i*gsize+j] = count + new_clique - old_clique;

				/*
				if(temp_count<0)
				{
					temp_count = 0;
				}
				*/
				/*
				 * is it better and the i,j,count not taboo?
				 */
				 /*
				if((temp_count < best_count) && 
					!FIFOFindEdge(taboo_list,i,j))
//					!FIFOFindEdgeCount(taboo_list,i,j,count))
				{
					best_count = temp_count;
					best_i = i;
					best_j = j;
				}
				*/

				/*
				 * flip it back
				 */
				//g[i*gsize+j] = 1 - g[i*gsize+j];
			}
		}
		for(i=0; i < gsize; i++)
		{
			for(j=i+1; j < gsize; j++)
			{
				if((cliques[i*gsize+j] < best_count) && 
					!FIFOFindEdge(taboo_list,i,j))
//					!FIFOFindEdgeCount(taboo_list,i,j,count))
				{
					best_count = cliques[i*gsize+j];
					best_i = i;
					best_j = j;
				}
			}
		}
		
		if(best_count == BIGCOUNT) {
			printf("no best edge found, restart\n");
			free(g);
			g = RandomGraphGenerator(gsize); 
		}
		
		/*
		 * keep the best flip we saw
		 */
		g[best_i*gsize+best_j] = 1 - g[best_i*gsize+best_j];
		count = best_count;
		/*
		 * taboo this graph configuration so that we don't visit
		 * it again
		 */
		//count = CliqueCount(g,gsize);
		FIFOInsertEdge(taboo_list,best_i,best_j);
//		FIFOInsertEdgeCount(taboo_list,best_i,best_j,count);

		printf("ce size: %d, best_count: %d, best edge: (%d,%d), new color: %d\n",
			gsize,
			best_count,
			best_i,
			best_j,
			g[best_i*gsize+best_j]);

		/*
		 * rinse and repeat
		 */
	}

	FIFODeleteGraph(taboo_list);
	free(cliques);

	return(0);

}
