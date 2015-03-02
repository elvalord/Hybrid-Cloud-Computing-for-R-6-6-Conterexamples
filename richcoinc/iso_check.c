#include "search.h"
#include <time.h>
void writeSolutions()
{
	int num = 3000;
	int *g0 = PaleyGenerator(101);
	for(int i=0;i<num;i++)
	{
		int newSize;
		int *g = RandomSubgraph(g0,101,79,&newSize);
		
		write_to_file("solutions.txt",g,newSize);
	}
}
void writeSolutions2()
{
	int** gs = DegradeFrom101Paley();
	printf("generation complete!\n");
	filterIsomorphismAndWrite(gs,101,100,&write_to_file,"solutions.txt");
	filterIsomorphismAndWrite(gs+101,101*50,99,&write_to_file,"solutions.txt");
	int i;
	for(i=0;i<101+101*50;i++)
	{
		if(gs[i]!=NULL)
			free(gs[i]);
	}

}
int isequal2(Jval a,Jval b)
{
	int vertex_num=10;
	int *a_int = (int*)a.v;
	int *b_int = (int*)b.v;
	if(a_int==NULL || b_int==NULL)
		return 1;
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
}
int test_mfifo()
{
	MFIFO mfifo_test = make_mfifo(10);
	int test1[]={1,2,3,4,5,6,7,8,9,0};
	int test3[]={1,2,3,4,5,6,7,8,9,0};
	int i=0;
	for(i=0;i<20;i++)
	{
		int* to_add = (int*)malloc(10*sizeof(int));
		memcpy(to_add,test1,40);
		int *next_in_list = (int*)find_next_mfifo(mfifo_test).v;
		if(next_in_list!=0)
			free(next_in_list);
		insert_mfifo(mfifo_test,new_jval_v((void*)to_add));
	}
	printf("%d\n",find_mfifo(mfifo_test,isequal2,new_jval_v((void*)test3)));
}
int
main(int argc,char *argv[])
{

	srand(time(0));
	//test_mfifo();
	taboo_search_cygraph2(99,&write_to_file,"solution100.txt",&socket_upload,argv[2],atoi(argv[3]));
	//taboo_search_D_openmp(99,&write_to_file,"solution100.txt",&socket_upload,argv[2],atoi(argv[3]));
	/*
	int a[]={1,0,0,0,1,0,0,0,1};
	write_to_file("aaa.txt",a,3);
	ftp_upload("aaa.txt");

	write_to_file("aab.txt",a,3);
	ftp_upload("aab.txt");

	write_to_file("aac.txt",a,3);
	ftp_upload("aac.txt");
	*/
	/*
	int *g = RandomGraphGenerator(6);
	for(int i=0;i<6;i++)
		for(int j=i+1;j<6;j++)
			g[i*6+j] = 1;
	printf("%d\n",CliqueCount(g,6));
	printf("%d\n",CliqueCount_D(g,6,6));
	*/
	/*
	int *g = PaleyGenerator(101);
	g = RandomSubgraph(g,101,79);
	printf("%d\n",CliqueCountN(g,6,6));
	*/
	/*
	int ggsize = 50;
	int *g = RandomGraphGenerator(ggsize);
	int ii = 5, jj = 6;
	printf("%d\n",CliqueCount_D_with_flip(g,ggsize,ii,jj));
	g[ii*ggsize+jj] = 1-g[ii*ggsize+jj];
	printf("%d\n",CliqueCount_D(g,ggsize,ii,jj));
	*/
	int i,j;
	int gsize=99;
	if(argc<4)
	{
		printf("the format is <mode> <host> <port>.\n");
		exit(1);
	}
	//int *g = RandomGraphGenerator(gsize);

	//int result = taboo_search_D(gsize,&write_to_file,"solution_test.txt");

	//writeSolutions2();
	if(strcmp(argv[1],"mine99")==0)
	{
		
		int begin,end;
		/*
		gettimeofday(&begin,NULL);
		taboo_search_D(30,&write_to_file,"solution99.txt",&socket_upload,argv[2],atoi(argv[3]));
		gettimeofday(&end,NULL);
		printf("===================%d\n",end-begin);
		*/
		//gettimeofday(&begin,NULL);
		//inte_al(50,&write_to_file,"solution99.txt",&socket_upload,argv[2],atoi(argv[3]));
		//inte_al(60,&write_to_file,"solution99.txt",&socket_upload,argv[2],atoi(argv[3]));
		taboo_search_D_openmp(99,&write_to_file,"solution100.txt",&socket_upload,argv[2],atoi(argv[3]));
		//gettimeofday(&end,NULL);
		//printf("===================%d\n",end-begin);
		//taboo_search(30,&write_to_file,"solution99.txt");
	}
	else if(strcmp(argv[1],"mine100")==0)
	{
		taboo_search_D_openmp(100,&write_to_file,"solution100.txt",&socket_upload,argv[2],atoi(argv[3]));
	}
}

