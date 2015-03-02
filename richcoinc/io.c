
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2012, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
#include <curl/curl.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <search.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
/*
 * This example shows an FTP upload, with a rename of the file just after
 * a successful upload.
 *
 * Example based on source code provided by Erick Nuwendam. Thanks!
 */



/* NOTE: if you want this example to work on Windows with libcurl as a
   DLL, you MUST also provide a read callback with CURLOPT_READFUNCTION.
   Failing to do so will give you a crash since a DLL may not use the
   variable's memory when passed in to it from an app like this. */
size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
  curl_off_t nread;
  /* in real-world cases, this would probably get this data differently
     as this fread() stuff is exactly what the library already would do
     by default internally */
  size_t retcode = fread(ptr, size, nmemb, stream);

  nread = (curl_off_t)retcode;

  fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
          " bytes from file\n", nread);
  return retcode;
}
int ftp_upload(char *filename)
{
  char *LOCAL_FILE;     //"./ftp.c";
  char *UPLOAD_FILE_AS; // "ftp.c";
  char *REMOTE_URL;     // "ftp://209.141.56.115/"  UPLOAD_FILE_AS;
  char *RENAME_FILE_TO; // "ftp.c";
  LOCAL_FILE = filename;
  UPLOAD_FILE_AS = filename;
  RENAME_FILE_TO = filename;

  char URL[] = "ftp://209.141.56.115/";
  REMOTE_URL = (char*)malloc(strlen(LOCAL_FILE)+strlen(URL)+1);REMOTE_URL[0]='\0';
  REMOTE_URL = strcat(REMOTE_URL,URL);
  REMOTE_URL = strcat(REMOTE_URL,LOCAL_FILE);

  CURL *curl;
  CURLcode res;
  FILE *hd_src;
  struct stat file_info;
  curl_off_t fsize;

  struct curl_slist *headerlist=NULL;
  char *buf_1; //= "RNFR " UPLOAD_FILE_AS;
  char *buf_2; //= "RNTO " RENAME_FILE_TO;

  buf_1 = (char*)malloc(6+strlen(UPLOAD_FILE_AS));
  buf_2 = (char*)malloc(6+strlen(RENAME_FILE_TO));

  buf_1 = strcat(buf_1,"RNFR ");
  buf_2 = strcat(buf_2,"RNTO ");

  buf_1 = strcat(buf_1,UPLOAD_FILE_AS);
  buf_2 = strcat(buf_2,RENAME_FILE_TO);


  /* get the file size of the local file */
  if(stat(LOCAL_FILE, &file_info)) {
    printf("Couldnt open '%s': %s\n", LOCAL_FILE, strerror(errno));
    return 1;
  }
  fsize = (curl_off_t)file_info.st_size;

  printf("Local file size: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", fsize);

  /* get a FILE * of the same file */
  hd_src = fopen(LOCAL_FILE, "rb");

  /* In windows, this will init the winsock stuff */
  curl_global_init(CURL_GLOBAL_ALL);

  /* get a curl handle */
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl,CURLOPT_FRESH_CONNECT,1);
    /* build a list of commands to pass to libcurl */
    //headerlist = curl_slist_append(headerlist, buf_1);
    //headerlist = curl_slist_append(headerlist, buf_2);

    /* we want to use our own read function */
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    
    /* password and username */
    curl_easy_setopt( curl, CURLOPT_USERPWD, "yanbo:19900328" );

    /* enable uploading */
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    /* specify target */
    curl_easy_setopt(curl,CURLOPT_URL, REMOTE_URL);

    /* pass in that last of FTP commands to run after the transfer */
    curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

    /* now specify which file to upload */
    curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

    /* Set the size of the file to upload (optional).  If you give a *_LARGE
       option you MUST make sure that the type of the passed-in argument is a
       curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
       make sure that to pass in a type 'long' argument. */
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t)fsize);

    /* Now run off and do what you've been told! */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* clean up the FTP commands list CURLOPT_FRESH_CONNECT*/
    //curl_slist_free_all (headerlist);

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  fclose(hd_src); /* close the local file */

  curl_global_cleanup();
  return 0;
}



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
    //write(pFile, "\n",1);
  }
  toWrite[n*n] = '\n';
  toWrite[n*n+1] = '\0';
  write(pFile,toWrite,n*n+1);
  free(toWrite);
  close(pFile);
}

void socket_upload(int *g,int gsize,char *host,int port)
{
  int sock;
  int *gCopy = (int*)malloc(gsize*gsize*sizeof(int));
  memcpy(gCopy,g,gsize*gsize*sizeof(int));
  sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  //PrintGraph(gCopy,gsize);
  struct sockaddr_in sockInfo;
  memset(&sockInfo,0,sizeof(sockInfo));
  sockInfo.sin_family = AF_INET;
  sockInfo.sin_addr.s_addr = inet_addr(host);
  sockInfo.sin_port = htons(port);
  connect(sock,&sockInfo,sizeof(sockInfo));

  char *toSend = (char*)malloc(255+(gsize*gsize+3)*sizeof(char));
  int i;
  toSend[0] = gsize;
  for(i=1;i<gsize*gsize+1;i++)
  {
    toSend[i] = '0' + gCopy[i];
  }
  toSend[i] = '\n';
  toSend[i+1] = '\0';
  //printf("%s\n",toSend);

  while(1)
  {
    //printf("total blocks: %d\n", (gsize*gsize+2)/255); 
    for(int j=0;j<(gsize*gsize+2)/255+1;j++)
    {
      //printf("printing block: %d\n",j);
      if(send(sock,toSend+j*255,255,0)<0)
      {
        printf("fail, retrying at package %d...\n",j);
        j--;
        continue;
      }
      
    }
    printf("wait.\n");
    char buf[256];
    buf[0] = 0;
    int received = recv(sock,buf,1,0);
    if(received==1)
       break;
    
  }
  printf("sent.\n");
  free(gCopy);
  free(toSend);
  close(sock);
}