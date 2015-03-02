#include "server.h"

int pFile;
Dllist checkingQueue;
Dllist existingSolutions;
void write_to_file(char *filename, int *example, int n)
{
	int pFile = open(filename,O_WRONLY | O_APPEND | O_SYNC | O_CREAT,0666);
  	char *toWrite = (char*)malloc(n*n+2);
  	int i,j;
  	for(i=0; i < n; i++)
  	{
  	  	for(j=0; j < n; j++)
  	  	{
  	  	  	if(example[i*n+j]==0)
  	  	  	  toWrite[i*n+j] = '0';
  	  	  	else
  	  	  	  toWrite[i*n+j] = '1';
  	  	  	//write(pFile, " ", 1);
  	  	}
  	  	//write(pFile,e "\n",1);
  	}
  	toWrite[n*n] = '\n';
  	toWrite[n*n+1] = '\0';
  	write(pFile,toWrite,n*n+1);
  	free(toWrite);
  	close(pFile);
  	
}
void log_received(graph *g)
{
	char filename[256];
	char prefix[] = "candidate";
	char buf[5];
	//itoa(g->gsize,buf,10);
	sprintf(buf,"%d",g->gsize);
	strcpy(filename,prefix);
	strcat(filename,buf);

	write_to_file(filename,g->g,g->gsize);
}
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
void doIsoCheck(graph *g)
{
	Dllist curr;
	int existing = 0;
	int isIso = 0;
	dll_traverse(curr, existingSolutions)
	{
		graph *g0 = (graph*)curr->val.v;
		if(g0->gsize==g->gsize)
		{
			existing = 1;
			if(IsIsomorph(g0->g,g->g,g->gsize)==1)
			{
				isIso = 1;
				break;
			}
		}
	}
	if(isIso==1)
	{
		free(g->g);
		free(g);
		return;
	}

	char filename[256];
	char prefix[] = "solutions";
	char buf[5];
	//itoa(g->gsize,buf,10);
	sprintf(buf,"%d",g->gsize);
	strcpy(filename,prefix);
	strcat(filename,buf);

	int gsize = g->gsize;
	if(existing==0)
	{
		//add to lib
		int pFile = open(filename,O_WRONLY | O_APPEND | O_SYNC | O_CREAT,0666);
 		char *line = (char*)malloc(gsize*gsize+1);
 		int readLength = 0;
 		while((readLength=read(pFile,line,g->gsize*g->gsize+1))>0)
 		{
 			graph *gRec = (graph*)malloc(sizeof(graph));
 			gRec->gsize = g->gsize;
 			gRec->g     = (int*)malloc(sizeof(int)*gsize*gsize);
 			for(int i=0;i<g->gsize*g->gsize;i++)
 				 gRec->g[i] = line[i] - '0';

 			dll_prepend(existingSolutions,new_jval_v((void*)gRec));

 			if(IsIsomorph(gRec->g,g->g,g->gsize)==1)
				isIso = 1;

 		}
 		close(pFile);
	}

	if(isIso==0)
	{
		//add to iso
		dll_prepend(existingSolutions,new_jval_v((void*)g));
		write_to_file(filename,g->g,g->gsize);

	}
	else
	{
		free(g->g);
		free(g);
	}
}
void isoCheck(void *thread_info)
{
	graph *g;
	ThreadInfo *ti = (ThreadInfo*)thread_info;
	printf("isomorphism checking thread is working.\n");
	while(1)
	{
		pthread_mutex_lock(&(ti->lock));
		if(dll_empty(checkingQueue)==1)
		{
			pthread_cond_wait(&(ti->cond),&(ti->lock));
		}
		else
		{
			g = (graph*)checkingQueue->blink->val.v;
			dll_delete_node(checkingQueue->blink);
			//printf("checking isomorphism.\n");
			doIsoCheck(g);
			
		}
		pthread_mutex_unlock(&(ti->lock));

	}

}
#define MAXPENDING 10
#define BUFFSIZE 255
int main(int argc,int *argv[])
{
	ThreadInfo *t = (ThreadInfo*)malloc(sizeof(ThreadInfo));
	pthread_mutex_init(&(t->lock),NULL);
	pthread_cond_init(&(t->cond),NULL);

	checkingQueue = new_dllist();
	existingSolutions = new_dllist();


	//fork checking thread
	pthread_attr_t *attr = (pthread_attr_t*)malloc(sizeof(pthread_attr_t));
	pthread_t *checkIsoThread = (pthread_t)malloc(sizeof(pthread_t));
	pthread_attr_init(attr);
	pthread_attr_setscope(attr,PTHREAD_SCOPE_SYSTEM);
	pthread_create(checkIsoThread,attr,isoCheck,t);

	int sockServer,sockClient;
	struct sockaddr_in sockServerInfo,sockClientInfo;
	sockServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockServer>0) printf("socket init successful.\n");

	memset(&sockServerInfo,0,sizeof(sockServerInfo));
	sockServerInfo.sin_family = AF_INET;
	sockServerInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	sockServerInfo.sin_port = htons(atoi(argv[1]));
	if(bind(sockServer,&sockServerInfo,sizeof(sockServerInfo))>=0) printf("bind successful.\n");
	if(listen(sockServer,MAXPENDING)>=0)printf("set listening successful.\n");
	while(1)
	{
		int clientInfoLength = sizeof(sockClientInfo);
		sockClient = accept(sockServer,&sockClientInfo,&clientInfoLength);
		if(sockClient>0) printf("get client socket!\n");
		char buff[BUFFSIZE+1];
		int received = recv(sockClient,buff,BUFFSIZE,0);
		char sigSucc[2];
		sigSucc[0] = 1;
		send(sockClient,sigSucc,1,0);

		printf("start receiving.\n");
		graph *g = (graph*)malloc(sizeof(graph));
		g->gsize = buff[0];
		printf("graph size: %d \n",g->gsize);
		g->g = (int*)malloc(buff[0]*buff[0]*sizeof(int));
		int saved = 0;
		int i=1;
		int finished = 0;
		for(;i<received;i++)
		{
			if(buff[i]=='\n')
			{

				finished = 1;
				break;
			}
			g->g[i-1] = buff[i]-'0';
		}
		saved += received-1;

		while((received = recv(sockClient,buff,BUFFSIZE,0))>0)
		{
			//printf("received: %d\n",received);
			
			for(i=0;i<received;i++)
			{
				if(buff[i]!='\n' && finished==0)
				{
					g->g[i+saved] = buff[i] - '0';
				}
				else if(buff[i]=='\n')
				{
					//char sigSucc[2];
					//sigSucc[0] = 1;
					//send(sockClient,sigSucc,1,0);

					finished=1;
				}
				
			}
			saved += received;
		}
		//printf("sending sigSucc\n");
		send(sockClient,sigSucc,1,0);
		log_received(g);
		//sleep(1);
		pthread_mutex_lock(&(t->lock));
		dll_prepend(checkingQueue,new_jval_v((void*)g));
		pthread_cond_signal(&(t->cond));
		pthread_mutex_unlock(&(t->lock));
		
		
		/*
		char filename[256];
		int filenameLength = 0;
		char filenamePrefix[] = "solutionBackup";
		*/
		
		//PrintGraph(g->g,g->gsize);
		printf("finish receiving.\n");
		close(sockClient);
	}
	close(sockServer);
	//close(pFile);
}