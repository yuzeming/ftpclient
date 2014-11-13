//
//  main.c
//  ftpClient
//
//  Created by 俞则明 on 14/11/13.
//  Copyright (c) 2014年 ZemingYU. All rights reserved.
//

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <assert.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/ftp.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>


#define CHECK(b,s) CHECK_LINE_FILE_(b,s,__LINE__,__FILE__)
#define CHECKRET(b,s)CHECK_LINE_FILE_( (b) == 0,s,__LINE__,__FILE__)

#define CHECK_LINE_FILE_(b,s,l,f)  \
do{ \
if (!(b)) \
{ \
fprintf(stderr,"%s:%d %s\n err:%d:%s\n",f,l,s,errno,strerror(errno)); \
exit(EXIT_FAILURE); \
} \
} while(0)

#define SendMsg(fd,str) send(fd,str,strlen(str),0)

int usePASV = 1;

struct sockaddr_in addr;
int port=21;
int sockfd;

char user[BUFSIZ],pass[BUFSIZ];

int ReadRespose(int sock,char *ret,size_t ret_sz)
{
    int code = -1;
    size_t len = 0;
    char buff[BUFSIZ];

    while (1)
    {
        while (len < BUFSIZ && (buff[len-2]!='\r' || buff[len-1] != '\n'))
        {
            size_t sz;
            while ( (sz = recv(sock,buff+len,1,0)) !=1 )
                if (sz == -1)
                    return -1;
            
            if (buff[len]=='\\')
            {
                recv(sock,buff+len,3,0);
                buff[len+3]='\0';
                sscanf(buff+len,"%d",&code);
                if (code!=377)
                {
                    buff[len] = (unsigned char)code;
                    ++len;
                }
            }
            else
                ++len;
        }
        len -= 2;
        buff[len]='\0';
        printf("%s\n",buff);
        if (isnumber(buff[0])&&buff[3]==' ')
        {
            buff[3]='\0';
            sscanf(buff,"%d", &code);
            if (buff!=NULL)
            {
                strncpy(ret, buff+4, ret_sz);
                ret[MIN(len-4,ret_sz)]='\0';
            }
            return code;
        }
    }
    return -1;
}


void send_command_TYPE(int sock,char *param)
{
    
}

int main(int argc,char * argv[]) {
    
    if (argc==1)
    {
        printf("usage:%s [-p] ip [port]\n",argv[0]);
        return 0;
    }
    if (strcmp(argv[1],"-p")==0) {
        printf("Use PORT.\n");
        usePASV = 0;
        ++argv;
        --argc;
    }
    if (argc == 3)
        sscanf(argv[2], "%d",&port);
    
    inet_pton(AF_INET,argv[1],&(addr.sin_addr));
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    sockfd = socket(PF_INET,SOCK_STREAM,0);
    char buff[BUFSIZ];
    
    CHECK(connect(sockfd,(struct sockaddr*)&addr ,sizeof(addr))== 0,"can't connect server.");
    int code;
    
    // Welcome Msg
    code = ReadRespose(sockfd,buff,sizeof(buff));
    CHECK(code == 220,"err:Welcome Msg");
    
    while (1)
    {
    
        printf("USER (anonymous):");
        gets(user);
        if (strlen(user)==0)
            strcpy(user, "anonymous");
    
        sprintf(buff,"USER %s\r\n",user);
        SendMsg(sockfd,buff);
    
        code = ReadRespose(sockfd,buff,sizeof(buff));
        CHECK(code ==331,"USER");
        
        char *tmppass=getpass("PASS:");
        strcpy(pass, tmppass);
        free(tmppass);
        
        sprintf(buff, "PASS %s\r\n",pass);
        SendMsg(sockfd,buff);

        code = ReadRespose(sockfd,buff,sizeof(buff));
        CHECK(code != -1,"PASS");
        
        if (code==230)
            break;
        if (code==530)
            printf("Try Again\n");
    }
    
    send_command_TYPE(sockfd,"I");
    CHECK(ReadRespose(sockfd,NULL,0) == 200,"set type");
    if (usePASV)
    {
        send_command_PASV(sock,);
    }
    
    
    return 0;
}
