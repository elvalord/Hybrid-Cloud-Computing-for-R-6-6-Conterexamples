#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <ctype.h>
#include <omp.h>
#include <stddef.h>



#include "jval.h"
#include "jrb.h"
#include "fifo.h"
#include "dllist.h"
#include "mfifo.h"

int CliqueCount(int *g,int gsize);
int IsIsomorph(int *g1, int *g2, int s1);
/* iso = IsIsomorph(Config1.config,Config2.config,Config1.size); */
double CliqueCountG(int *g, int gsize, double *countg);
void PrintGraph(int *g, int gsize);

int *PaleyGenerator(int n);
int *RandomGraphGenerator(int n);
int *RandomSubgraph(int *g, int gsize,int min,int *newSize);
int** DegradeFrom101Paley();
void filterIsomorphism(int **g,int n,int graphSize);
void filterIsomorphismAndWrite(int **g,int n,int graphSize,void (*writefile)(char*,int*,int),char* filename);

//Add from simple_taboo_search.c
void CopyGraph(int *old_g, int o_gsize, int *new_g, int n_gsize);

int CliqueCount_D(int *g, int gsize, int i, int j);
int CliqueCount_D_with_flip(int *g, int gsize, int i, int j);
