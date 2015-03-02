#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "graph.h"	/* for taboo list */


#define MAXSIZE (512)

#define TABOOSIZE (2000)
#define BIGCOUNT (9999999)

int fint_next_prime(int p)
{
	const int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 
						  71, 73, 79, 83, 89, 97, 101, 103};
	int i;
	for(i=0;i<27;i++)
		if(primes[i]>p)
			return primes[i];
	return -1;
}
int* next_graph(int *g,int gsize_old)
{
	int gsize_new = fint_next_prime(gsize_old);
	int *color_distance = (int*)malloc(sizeof(int)*(1+gsize_new/2));
	int i,j;
	for(i=0;i<=gsize_old/2;i++)
		color_distance[i] = g[i];
	for(;i<=gsize_new/2;i++)
		color_distance[i] = rand()%2;
	free(g);
	g = (int*)malloc(gsize_new*gsize_new*sizeof(int));
	memset(g,0,gsize_new*gsize_new*sizeof(int));
	for(i=0;i<gsize_new;i++)
	{
		for(j=1;j<=gsize_new/2 && i+j<gsize_new;j++)
			g[i*gsize_new+i+j] = color_distance[j];

		for(;j<gsize_new && i+j<gsize_new;j++)
			g[i*gsize_new+i+j] = color_distance[gsize_new - j];
	}
	free(color_distance);
	PrintGraph(g,gsize_new);
	return g;

}

int
taboo_search_cygraph(int gsize,void (*writefile)(char*,int*,int),char* filename,
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
	int best_dist;
	void *taboo_list;
	
		/*
	 * start with graph of size 8
	 */
	g = (int *)malloc(gsize*gsize*sizeof(int));
	memset(g,0,gsize*gsize*sizeof(int));
	//generate sigraph color
	int *color_distance = (int*)malloc((gsize/2+1)*sizeof(int));
	for(i=0;i<=gsize/2;i++)
		color_distance[i] = rand()%2;

	for(i=0;i<gsize;i++)
	{
		for(j=1;j<=gsize/2 && i+j<gsize;j++)
			g[i*gsize+i+j] = color_distance[j];
		for(;j<gsize && i+j<gsize;j++)
			g[i*gsize+i+j] = color_distance[gsize-j];
	}
	free(color_distance);

	//PrintGraph(g,gsize);
	
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
	PrintGraph(g,gsize);
	count = CliqueCount(g,gsize);
	while(gsize<101)
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
			
			//printf("Eureka!  Counter-example found!\n");
			PrintGraph(g,gsize);
			//return;//for test
			writefile(filename,g,gsize);
			//writesocket(g,gsize,host,port);
			/*
			 * make a new graph one size bigger
			 */
			g = next_graph(g,gsize);
			gsize = fint_next_prime(gsize);
			printf("moving on to %d\n",gsize);
			//free(g);
			//g = NULL;
			//g = RandomGraphGenerator(gsize);
			PrintGraph(g,gsize);
			if(g == NULL)
				exit(1);
		
			taboo_list = FIFOResetEdge(taboo_list);

			/*
			 * keep going
			 */
			count = CliqueCount(g,gsize);
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

		for(i=1;i<gsize/2;i++)
		{	
			if(FIFOFindEdge(taboo_list,0,i))
				continue;
			//flip
			for(j=0;j<gsize && j+i<gsize;j++)
				g[j*gsize+j+i]=1-g[j*gsize+j+i];
			count = CliqueCount(g,gsize);
			if(best_count>count)
			{
				best_count = count;
				best_dist  = i;
			}

			//flip back
			for(j=0;j<gsize && j+i<gsize;j++)
				g[j*gsize+j+i]=1-g[j*gsize+j+i];

		}
		

		/*
		 * keep the best flip we saw
		 */
		for(j=0;j<gsize && j+best_dist<gsize;j++)
			g[j*gsize+j+best_dist]=1-g[j*gsize+j+best_dist];

		//random change
		if(rand()%2==0)
		{
			int rand_dist = rand()%(gsize/2)+1;
			for(j=0;j<gsize && j+rand_dist<gsize;j++)
				g[j*gsize+j+rand_dist]=1-g[j*gsize+j+rand_dist];
			count = CliqueCount(g,gsize);
			if(count>=3*best_count/2)
			{
				for(j=0;j<gsize && j+rand_dist<gsize;j++)
					g[j*gsize+j+rand_dist]=1-g[j*gsize+j+rand_dist];
			}
			else
			{
				best_count = count;
				best_dist  = rand_dist;
			}
		}

		/*
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
		*/
		if(best_count == BIGCOUNT) {
			printf("no best edge found, restart\n");
			free(g);
			g = (int *)malloc(gsize*gsize*sizeof(int));
			memset(g,0,gsize*gsize*sizeof(int));
			//generate sigraph color
			int *color_distance = (int*)malloc((gsize/2+1)*sizeof(int));
			for(i=0;i<=gsize/2;i++)
				color_distance[i] = rand()%2;

			for(i=0;i<gsize;i++)
			{
				for(j=1;j<=gsize/2 && i+j<gsize;j++)
					g[i*gsize+i+j] = color_distance[j];
				for(;j<gsize && i+j<gsize;j++)
					g[i*gsize+i+j] = color_distance[gsize-j];
			}
			free(color_distance);
		}
		count = best_count;
		
		//g[best_i*gsize+best_j] = 1 - g[best_i*gsize+best_j];
		//count = best_count;
		/*
		 * taboo this graph configuration so that we don't visit
		 * it again
		 */
		//count = CliqueCount(g,gsize);
		FIFOInsertEdge(taboo_list,0,best_dist);
//		FIFOInsertEdgeCount(taboo_list,best_i,best_j,count);

		printf("ce size: %d, best_count: %d, best distance: %d.\n",
			gsize,
			best_count,
			best_dist);

		/*
		 * rinse and repeat
		 */
	}

	FIFODeleteGraph(taboo_list);


	return(0);

}
int size_isequal;
int isequal(Jval a,Jval b)
{
	int vertex_num=size_isequal;
	int *a_int = (int*)a.v;
	int *b_int = (int*)b.v;
	if(a_int==NULL || b_int==NULL)
		return 0;
	int i;
	for(i=0;i<vertex_num;i++)
	{
		if(a_int[i]!=b_int[i])
		{
			//printf("%d %d",a_int[i],b_int[i]);
			return 0;
		}
	}
	return 1;
}/*
int test_mfifo()
{
	MFIFO mfifo_test = make_mfifo(10);
	int test1[]={1,2,3,4,5,6,7,8,9,0};
	int test2[]={1,2,3,4,5,6,7,8,9,0};
	int test3[]={1,2,3,4,5,6,7,8,8,0};
	insert_mfifo(mfifo_test,new_jval_v((void*)test1));
	insert_mfifo(mfifo_test,new_jval_v((void*)test2));
	printf("%d\n",find_mfifo(mfifo_test,isequal,new_jval_v((void*)test3)));
}*/
typedef struct count_edge_pair{
	int edge;
	int count;
}CE_pair;

int
taboo_search_cygraph2(int gsize,void (*writefile)(char*,int*,int),char* filename,
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
	int best_dist;
	MFIFO taboo_list;
	int candidate_size = gsize/10 + 1;
		/*
	 * start with graph of size 8
	 */
	g = (int *)malloc(gsize*gsize*sizeof(int));
	memset(g,0,gsize*gsize*sizeof(int));
	//generate sigraph color
	int *color_distance = (int*)malloc((gsize/2+1)*sizeof(int));
	for(i=0;i<=gsize/2;i++)
		color_distance[i] = rand()%2;

	for(i=0;i<gsize;i++)
	{
		for(j=1;j<=gsize/2 && i+j<gsize;j++)
			g[i*gsize+i+j] = color_distance[j];
		for(;j<gsize && i+j<gsize;j++)
			g[i*gsize+i+j] = color_distance[gsize-j];
	}
	

	//PrintGraph(g,gsize);
	
	/*
	 * make a fifo to use as the taboo list
	 */
	taboo_list = make_mfifo(TABOOSIZE);
	if(taboo_list == NULL) {
		exit(1);
	}
	insert_mfifo(taboo_list,new_jval_v((void*)color_distance));
	/*
	 * start out with all zeros
	 */
	//memset(g,0,gsize*gsize*sizeof(int));

	/*
	 * while we do not have a publishable result
	 */
	//while(gsize < 44)
	//PrintGraph(g,gsize);
	count = CliqueCount(g,gsize);
	best_count = count;
	int *tempColoring = (int*)malloc((gsize/2+1)*sizeof(int));
	CE_pair* candidate_edge = (CE_pair*)malloc(candidate_size*sizeof(CE_pair));
	memset(candidate_edge,0,candidate_size*sizeof(CE_pair));
	while(gsize<101)
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
			
			//printf("Eureka!  Counter-example found!\n");
			PrintGraph(g,gsize);
			//return;//for test
			writefile(filename,g,gsize);
			//writesocket(g,gsize,host,port);
			/*
			 * make a new graph one size bigger
			 */
			g = next_graph(g,gsize);
			gsize = fint_next_prime(gsize);
			printf("moving on to %d\n",gsize);
			//free(g);
			//g = NULL;
			//g = RandomGraphGenerator(gsize);
			PrintGraph(g,gsize);
			if(g == NULL)
				exit(1);
		
			//taboo_list = FIFOResetEdge(taboo_list);
			
			for(i=0;i<TABOOSIZE;i++)
			{
				if(taboo_list->mfifo_data[i].v!=0)
				{
					free((int*)taboo_list->mfifo_data[i].v);
					taboo_list->mfifo_data[i].v=0;
				}
						
			}
			free(tempColoring);
			tempColoring = (int*)malloc((gsize/2+1)*sizeof(int));

			free(candidate_edge);
			candidate_size = gsize/10+1;
			candidate_edge = (CE_pair*)malloc(candidate_size*sizeof(CE_pair));
			memset(candidate_edge,0,candidate_size*sizeof(CE_pair));

			/*
			 * keep going
			 */
			count = CliqueCount(g,gsize);
			best_count = count;
			continue;
		}
		//size_isequal = gsize/2;
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
		//best_count = BIGCOUNT;

		memcpy(tempColoring,g,(gsize/2+1)*sizeof(int));
		for(i=1;i<=gsize/2;i++)
		{	
			tempColoring[i] = 1 - tempColoring[i];
			size_isequal = gsize/2 + 1;
			if(find_mfifo(taboo_list,isequal,new_jval_v((void*)tempColoring)))
			{
				tempColoring[i] = 1 - tempColoring[i];
				continue;
			}
			tempColoring[i] = 1 - tempColoring[i];
			//flip
			/*
			for(j=0;j<gsize && j+i<gsize;j++)
				g[j*gsize+j+i]=1-g[j*gsize+j+i];

			count = CliqueCount(g,gsize);
			*/
			//flip and count
			count = best_count;
			for(j=0;j<gsize && j+i<gsize;j++)
			{
				count += CliqueCount_D_with_flip(g,gsize,j,j+i) - CliqueCount_D(g,gsize,j,j+i);
				g[j*gsize+j+i]=1-g[j*gsize+j+i];
				if(j+gsize-i<gsize)
				{
					count += CliqueCount_D_with_flip(g,gsize,j,j+gsize-i) - CliqueCount_D(g,gsize,j,j+gsize-i);
					g[j*gsize+j+gsize-i]=1-g[j*gsize+j+gsize-i];
				}
			}
			//printf("count:%d<---------->%d\n",count,CliqueCount(g,gsize));
			/*
			if(best_count>count)
			{
				best_count = count;
				best_dist  = i;
			}
			*/
			//add to candidate list
			int largest_candidate_index = 0;
			int largest_candidate_count = 0;
			for(j=0;j<candidate_size;j++)
			{
				if(candidate_edge[j].count==0)
				{
					largest_candidate_index = j;
					candidate_edge[j].count = count;
					candidate_edge[j].edge = i;
					break;
				}
				if(candidate_edge[j].count>largest_candidate_count)
				{
					largest_candidate_count = candidate_edge[j].count;
					largest_candidate_index = j;
				}
								
			}
			if(j==candidate_size && count<largest_candidate_count)
			{
				candidate_edge[largest_candidate_index].edge = i;
				candidate_edge[largest_candidate_index].count= count;
			}
			//flip back
			for(j=0;j<gsize && j+i<gsize;j++)
			{
				g[j*gsize+j+i]=1-g[j*gsize+j+i];
				if(j+gsize-i<gsize)
					g[j*gsize+j+gsize-i]=1-g[j*gsize+j+gsize-i];
			}

		}
		

		/*
		 * keep the best flip we saw
		 */
		


		//random change
		/*
		if(rand()%2==0)
		{
			int rand_dist = rand()%(gsize/2)+1;
			for(j=0;j<gsize && j+rand_dist<gsize;j++)
				g[j*gsize+j+rand_dist]=1-g[j*gsize+j+rand_dist];
			count = CliqueCount(g,gsize);
			if(count>=3*best_count/2)
			{
				for(j=0;j<gsize && j+rand_dist<gsize;j++)
					g[j*gsize+j+rand_dist]=1-g[j*gsize+j+rand_dist];
			}
			else
			{
				best_count = count;
				best_dist  = rand_dist;
			}
		}
		*/
		int next_edge_idx = rand()%(candidate_size);
		best_dist = candidate_edge[next_edge_idx].edge;
		best_count= candidate_edge[next_edge_idx].count;

		for(j=0;j<gsize && j+best_dist<gsize;j++)
		{
			g[j*gsize+j+best_dist]=1-g[j*gsize+j+best_dist];
			if(j+gsize-best_dist<gsize)
					g[j*gsize+j+gsize-best_dist]=1-g[j*gsize+j+gsize-best_dist];
		}
		if(rand()%2==0)
		{
			candidate_size--;
			if(candidate_size==0)
			{
				candidate_size = gsize/10+1;
			}

		}
		memset(candidate_edge,0,candidate_size*sizeof(CE_pair));
		
		tempColoring[best_dist] = 1 - tempColoring[best_dist];
		int* tabooColoring = (int*)malloc((gsize/2+1)*sizeof(int));
		memcpy(tabooColoring,tempColoring,(gsize/2+1)*sizeof(int));
		

		int *to_detaboo = (int*)find_next_mfifo(taboo_list).v;
		if(to_detaboo!=0)
			free(to_detaboo);
		insert_mfifo(taboo_list,new_jval_v((void*)tabooColoring));

		count = best_count;
		

		printf("ce size: %d, best_count: %d, best distance: %d.\n",
			gsize,
			best_count,
			best_dist);

	}

	//FIFODeleteGraph(taboo_list);


	return(0);

}