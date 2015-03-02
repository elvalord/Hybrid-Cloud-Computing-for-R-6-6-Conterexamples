#define TABOOSIZE (500)
#define THRESHOLD 50000 //temperature threshold
#define INITEM 100000//initial temperature
#define DTEM 100//temperature decrease interval
#define INICLI 9999999//initial best clinique value
#include "graph.h"

int inte_al(int gsize,void (*writefile)(char*,int*,int),char* filename,
	void (*writesocket)(int*,int,char*,int),char* host,int port){
	int *g;
	int gt[2]={0,0};
	int *new_g;
	int count;
	int i;
	int j;
	int best_count;
	int best_i;
	int best_j;
	void *taboo_list;

	int t=INITEM;
	
	int choice=0;
	/*
	 * start with graph of size 8
	 */
	//g = (int *)malloc(gsize*gsize*sizeof(int));
	g = RandomGraphGenerator(gsize);
/*	if(g == NULL) {
		exit(1);
	}
*/
		/*
	 * make a fifo to use as the taboo list
	 */
	taboo_list = FIFOInitEdge(TABOOSIZE);
/*	if(taboo_list == NULL) {
		exit(1);
	}
	*/
	
	
	//new_g=(int*) malloc(gsize*gsize*sizeof(int));
	int best_clique=INICLI;
	int flag=0;
	int* cliques = (int*)malloc(gsize*gsize*sizeof(int));
	while(1)
	{
		count = CliqueCount(g,gsize);
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
			printf("Clique Number:%d\n",CliqueCount(g,gsize));
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
		best_clique=INICLI;
		choice=rand()%10;
		if (choice==0){
			i=rand()%gsize;
			j=rand()%gsize;
			while (FIFOFindEdge(taboo_list,i,j)){
				i=rand()%gsize;
				j=rand()%gsize;
			}
			gt[0]=i;
			gt[1]=j;
			g[gt[0]*gsize+gt[1]] = 1 - g[gt[0]*gsize+gt[1]];
			best_clique=CliqueCount_D(g,gsize,gt[0],gt[1]);
			g[gt[0]*gsize+gt[1]] = 1 - g[gt[0]*gsize+gt[1]];
		}
		else{
			#pragma omp parallel for private(i,j) shared(g)
			for(i=0; i < gsize; i++)
			{
				for(j=i+1; j < gsize; j++)
				{
					if(!FIFOFindEdge(taboo_list,i,j)){
						int old_clique = CliqueCount_D(g,gsize,i,j);
						//g[i*gsize+j] = 1 - g[i*gsize+j];
						int new_clique = CliqueCount_D_with_flip(g,gsize,i,j);
						cliques[i*gsize+j] = count + new_clique - old_clique;
						
					}
					
				}
			}

			for(i=0; i < gsize; i++)
			{
				for(j=i+1; j < gsize; j++)
				{
					if((cliques[i*gsize+j] < best_clique) && 
						!FIFOFindEdge(taboo_list,i,j))
	//					!FIFOFindEdgeCount(taboo_list,i,j,count))
					{
						best_clique = cliques[i*gsize+j];
						gt[0] = i;
						gt[1] = j;
					}
				}
			}
		}
	if (best_clique<=count){
		g[gt[0]*gsize+gt[1]] = 1 - g[gt[0]*gsize+gt[1]];
	//	flag=1;
	//	break;
	}
	else{
		if (t>THRESHOLD){
			g[gt[0]*gsize+gt[1]] = 1 - g[gt[0]*gsize+gt[1]];
		//	flag=1;
		//	break;
		}	
	}
	FIFOInsertEdge(taboo_list,gt[0],gt[1]);
	t-=DTEM;	
	if (t==0){
		t=INITEM;
	}

	}//end of while
	free(cliques);
	//if (flag!=1 && )


}