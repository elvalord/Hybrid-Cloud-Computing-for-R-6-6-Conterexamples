#include "graph.h"

/* I/O functions */
void write_to_file(char *filename, int *example,int n);
int ftp_upload(char *filename);
void socket_upload(int *g,int gsize,char *host,int port);

/* search algorithms */
int taboo_search(int gsize,void (*writefile)(char*,int*,int),char* filename);
int taboo_search_D(int gsize,void (*writefile)(char*,int*,int),char* filename,
	void (*writesocket)(int*,int,char*,int),char* host,int port);
int taboo_search_D_openmp(int gsize,void (*writefile)(char*,int*,int),char* filename,
	void (*writesocket)(int*,int,char*,int),char* host,int port);
int taboo_search_cygraph(int gsize,void (*writefile)(char*,int*,int),char* filename,
	void (*writesocket)(int*,int,char*,int),char* host,int port);
int taboo_search_cygraph2(int gsize,void (*writefile)(char*,int*,int),char* filename,
	void (*writesocket)(int*,int,char*,int),char* host,int port);
int inte_al(int gsize,void (*writefile)(char*,int*,int),char* filename,
	void (*writesocket)(int*,int,char*,int),char* host,int port);
