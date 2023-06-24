#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h> 
#define MAX 800
#define PORT 8080
#define SA struct sockaddr
    
struct dirent** listArq(int *n){

  struct dirent **namelist; 
  int nomeArq=0;

  *n = scandir("./nuvem", &namelist, 0, alphasort);
  if (n < 0)
  perror("scandir");
  else 
  {
	while(nomeArq<*n) 
	{	
		
		if(namelist[nomeArq]->d_name[0]!='.'){
			//printf("%s\n", namelist[nomeArq]->d_name);	
		}
		
		nomeArq++;
	}
  }

  return namelist;
}






int main()
{
 	
    int sockfd, connfd, len,on=1,tamStr;
    struct sockaddr_in servaddr, cli;
    char buff[MAX];

  while(1)
  {	
 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
        
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    printf("setsockopt(SO_REUSEADDR) failed");    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0)
    printf("setsockopt(SO_REUSEADDR) failed");    
        
    bzero(&servaddr, sizeof(servaddr));
   

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
  
   
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
  

    if ((listen(sockfd, 1)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
   
    
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
        printf("server acccept the client...\n");
   
    
	
		
	read(connfd, buff, sizeof(buff));
	//printf("buffer: %s\n",buff);
    
    if(strstr(buff,"/ ")!=NULL)
    {
        char response[10000];
        int qntArq,i;
        struct dirent **namelist = listArq(&qntArq);
        //printf("qntArq: %d\n",qntArq);
        sprintf(response,"HTTP/1.0 200 OK\nContent-Type: text/html;\nAccess-Control-Allow-Origin: *\ncharset=ISO-8859-1\r\n\r\n<!DOCTYPE html>\n<html>\n<body>\n<h1>Index of Tp1</h1>\n");
        
        
        
        for(i=2;i<qntArq;i++){
           char aux[1000];
           sprintf(aux,"<a href=\"http://192.168.0.108:8080/nuvem/%s\">%s</a>\n<br>",namelist[i]->d_name,namelist[i]->d_name);
           strcat(response,aux);

        }

        strcat(response,"</body>\n</html>");
        //printf("resposta: %s\n",response);
        write(connfd,response,strlen(response));

    }
    
    else if(strstr(buff,"/header")!=NULL){
        char response[10000];
        sprintf(response,"HTTP/1.0 200 OK\nContent-Type: text/html; charset=ISO-8859-1\r\n\r\n<!DOCTYPE html><html><h1>%s</h1></html>",buff);
	    write(connfd,response,strlen(response));

        bzero(response,10000);
        bzero(buff,800);
       
    }
    else if(strstr(buff,"/nuvem")!=NULL){
        
       char *p = strstr(buff,"/nuvem");
       p = p+7;
       char *q = strstr(p," ");
       *q = '\0';
       //printf("P:%s\n",p);
       char header[10000],caminho[100],*content;
       sprintf(caminho,"/home/diego/Área de Trabalho/tp1/nuvem/%s",p); // Altere aqui para o diretório onde foram baixados os arquivos
       //printf("%s\n",caminho);
       char *r = strstr(p,".");
       r++;
       if(*r=='h') content = "text/html";
       else if(*r=='p'&&*(r+1)=='n') content = "image/png";
       else if(*r=='p'&&*(r+1)=='d') content = "application/pdf";
       else if(*r=='j') content = "image/jpg";
       else if(*r=='t') content = "text/plain"; 

      FILE * arq = fopen(caminho,"rb");
      if(arq == NULL){ printf ("arquivo não abriu\n");}
      else{

       
      //bzero(header,sizeof(header));
        
      sprintf(header,"HTTP/1.0 200 OK\nContent-Type: %s;\nAccess-Control-Allow-Origin: *\ncharset=ISO-8859-1\r\n\r\n",content);
      write(connfd,header,strlen(header));
      //printf("HEADER: %s\n\n",header);  
      bzero(buff,sizeof(buff));

     
        int bytes;  
        while(1) 
        {    
                bytes = fread(buff,1,MAX,arq);
                write(connfd,buff,bytes);
               
                bzero(buff,sizeof(buff));
                if(bytes<MAX)break;


         }
         
     }

    }
    else if(strstr(buff,"/info")!=NULL){
       
        system("hostnamectl >> info.txt");
        FILE *arq = fopen("/home/diego/Área de Trabalho/tp1/info.txt","rb+"); // Altere aqui para o diretório onde foram baixados os arquivos
        fseek(arq,0,SEEK_END);
        char vet[2];
        vet[0] = '\n';
        vet[1] = '\n';
        int tamStr = fwrite(vet,sizeof(char),2,arq);
        fseek(arq,0,SEEK_END);
        //printf("TamStr: %d\n\n",tamStr);
        system("date --utc >> info.txt");
        fseek(arq,0,SEEK_END);
        fwrite(vet,sizeof(char),2,arq);
        fseek(arq,0,SEEK_END);
        system("who >> info.txt");
        fseek(arq,0,SEEK_SET);
        char header[10000];
        if(arq == NULL){ printf ("arquivo não abriu\n");}
        else
        {

       
        //bzero(header,sizeof(header));
        
            sprintf(header,"HTTP/1.0 200 OK\nContent-Type: text/plain;\nAccess-Control-Allow-Origin: *\ncharset=ISO-8859-1\r\n\r\n");
            write(connfd,header,strlen(header));
            printf("HEADER: %s\n\n",header);  
            bzero(buff,sizeof(buff));

     
            int bytes;  
            while(1) 
            {    
                bytes = fread(buff,1,MAX,arq);
                write(connfd,buff,bytes);
                bzero(buff,sizeof(buff));
                if(bytes<MAX)break;


            }   
          system(" > info.txt");       
        }
    }   
    else if(strstr(buff,"/hello")!=NULL){
        
        FILE *arq = fopen("/home/diego/Área de Trabalho/tp1/hello.html","rb"); // Altere aqui para o diretório onde foram baixados os arquivos
        char header[10000];
        if(arq == NULL){ printf ("arquivo não abriu\n");}
        else
        {
            sprintf(header,"HTTP/1.0 200 OK\nContent-Type: text/html;\nAccess-Control-Allow-Origin: *\ncharset=ISO-8859-1\r\n\r\n");
            write(connfd,header,strlen(header));
            //printf("HEADER: %s\n\n",header);  
            bzero(buff,sizeof(buff));

     
            int bytes;  
            while(1) 
            {    
                bytes = fread(buff,1,MAX,arq);
                write(connfd,buff,bytes);
                bzero(buff,sizeof(buff));
                if(bytes<MAX)break;


            }   

        }





    }
    else {
        
        write(connfd,"HTTP/1.0 200 OK\nContent-Type: text/plain; charset=ISO-8859-1\r\n\r\nNao entendi!",strlen("HTTP/1.0 200 OK\nContent-Type: text/plain; charset=ISO-8859-1\r\n\r\nNao entendi!"));
        
    }
    //getchar();
    close(connfd);
    close(sockfd);
   }
  } 
