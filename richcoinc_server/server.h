#include "jval.h"
#include "jrb.h"
#include "dllist.h"
#include "pthread.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int IsIsomorph(int *g1, int *g2, int s1);
typedef struct
{
	int gsize;
	int *g;
}graph;

typedef struct 
{
	pthread_mutex_t lock;
	pthread_mutex_t cond;
}ThreadInfo;