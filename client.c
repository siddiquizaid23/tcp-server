#define _POSIX_C_SOURCE 200112L

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <errno.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>

#include<arpa/inet.h>

#define PORT "3490"
#define MAXDATASIZE 100 
void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);

    
}
int main(int argc , char *argv[]){
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints ,*servinfo , *p;
    int rv ;
    char s[INET6_ADDRSTRLEN];

    if(argc != 2){
        fprintf(stderr,"usage: client hostnames\n");
        exit(1);
    }
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(rv));
        return 1 ;
    }
    for(p = servinfo; p != NULL; p = p->ai_next)
{
    if((sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1){
        perror("client : socket");
        continue;
    }
    inet_ntop(p->ai_family,get_in_addr((struct sockaddr *)p->ai_addr),s,sizeof s);
    if(connect(sockfd,p->ai_addr,p->ai_addrlen) == -1){
        perror("client: connect");
        close(sockfd);
        continue;
    }
    break;
}

if(p == NULL){
    fprintf(stderr,"client: failed to connect\n");
    return 2;
}
inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s,sizeof s);
printf("client : connected to %s\n",s);
freeaddrinfo(servinfo);
while (1)
{
    printf("client :");
    if(fgets(buf,sizeof(buf),stdin)== NULL){
        break;
    }
    buf[strcspn(buf,"\n")] = '\0';
    if(send(sockfd,buf,strlen(buf),0)== -1){
        perror("send");
        break;
    }
    if(strcmp(buf,"/quit") == 0){
        printf("closing connection...\n");
        break;
    }
    memset(buf,0,sizeof(buf));
    numbytes = recv(sockfd,buf,MAXDATASIZE - 1 , 0);


if (numbytes == -1)
{
perror("recv");
break;
}
if(numbytes == 0){
    printf("server disconnectd\n");
    break;
}
buf[numbytes] = '\0';
printf("server: %s\n",buf);
if(strcmp(buf,"/quit") == 0){
    printf("server closed the chat\n");
    break;
}

}
close(sockfd);
return 0;
}