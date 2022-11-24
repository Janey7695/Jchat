#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include "cJSON.h"
using namespace std;


#define DEFAULT_SERVER_PORT 6666
#define BUFFER_MAXSIZE 1024
const char* input_sign = "|/-\\";
int input_over_flag = 0;
void show_inputsign();
void send_data(int &connfd);
void recv_data(int &connfd);
void get_stdin(char *sendbuf,int &connfd);
char sendbuf[BUFFER_MAXSIZE];
char receivebuf[BUFFER_MAXSIZE];
char userename[20];
int progress_over_flag = 0;

int main(int argc,char* argv[]){

    //客户端只需要一个套接字文件描述符，用于和服务器通信
    int clientSocket;

    //描述服务器的socket 结构体
    struct sockaddr_in serverAddr;
    
    // //发送缓冲
    // char sendbuf[BUFFER_MAXSIZE];
    // char receivebuf[BUFFER_MAXSIZE];
    
    int len;
    int current_input_sign = 2;

    if(argc < 2 ){
        printf("please input your Name.\r\n");
        printf("usage: client [your name] [port(default to 6666)]\r\n");
        return 1;
    }

    strcpy(userename,argv[1]);
    
    if((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket open error");
        exit(1);
    }
    
    serverAddr.sin_family = AF_INET;

    //若提供端口则使用提供的端口，否则使用默认端口
    if(argc == 3)   serverAddr.sin_port = htons(atoi(argv[2]));
    else serverAddr.sin_port = htons(DEFAULT_SERVER_PORT);
    
    //指定服务器端的ip
    //inet_addr()函数，将点分十进制IP转换成网络字节序IP
    serverAddr.sin_addr.s_addr = inet_addr("114.132.245.124");
    //serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect error");
        exit(1);
    }

    printf("Welcome %s ,let's see what people talk about. Enjoy!\r\n",argv[1]);

    thread thread_showsign(show_inputsign);
    thread thread_readstdin(get_stdin,ref(sendbuf),ref(clientSocket));
    thread thread_senddata(send_data,ref(clientSocket));
    thread thread_receivedata(recv_data,ref(clientSocket));
    thread_showsign.join();
    thread_senddata.join();
    thread_readstdin.join();
    thread_receivedata.join();

    close(clientSocket);
    
    return 0;
 
}

cJSON* create_json_message(){
    cJSON* cjson_message = NULL;
    cjson_message = cJSON_CreateObject();
    cJSON_AddStringToObject(cjson_message,"content",sendbuf);
    cJSON_AddStringToObject(cjson_message,"sender",userename);
    return cjson_message;
}


void show_inputsign(){
    int current_input_sign = 2;
    while(!progress_over_flag){
        printf("\r%c",input_sign[current_input_sign--]);
        if(current_input_sign < 0) current_input_sign = 2;
        fflush(stdout);
        usleep(500000);
    }
}

void get_stdin(char *sendbuf,int &connfd){
    cJSON* cjson;
    char* str;
    while(!progress_over_flag){
        //printf("??\n");
        fgets(sendbuf,1024,stdin);
        if(sendbuf[0]=='.' && sendbuf[1] == 'Q'){
            progress_over_flag = 1;
            //close(connfd);
            printf("Byee~\r\n");
        }
        cjson = create_json_message();
        strcpy(sendbuf,cJSON_Print(cjson));
        cJSON_Delete(cjson);
        input_over_flag = 1;
        //printf("??\n");
        
    }
}

void send_data(int &connfd){
    int len=0;
    char *eod = "";
    while(!progress_over_flag){
        if(input_over_flag){
            //printf("your send %d bytes\n",len);
            len = strlen(sendbuf);
            sendbuf[len] = '\n';
            len = send(connfd,sendbuf,len+1,0);
            input_over_flag = 0;
            //printf("your send %d bytes\n",len);
        }
        usleep(1000000);
    }
}

void recv_data(int &connfd){
    char buff[BUFFER_MAXSIZE] = {0};
    while(1){
        size_t n = recv(connfd, buff, 1024, 0);
        if(n == 0){
            close(connfd);
            break;
        }
        printf("%s\n",buff);
    }
}