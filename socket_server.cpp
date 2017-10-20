#include<stdio.h>
#include<winsock2.h>
#include<malloc.h>
#include<string.h>
#include<conio.h>
#include<windows.h>
#include<pthread.h>
#pragma comment(lib,"libws2_32.a") //Winsock Library
#pragma comment (lib, "libMswsock.a")
#pragma comment (lib, "libAdvApi32.a")
#define MAX_CON 10

typedef struct {
        int c_id;
        char name[20];
        SOCKET cs;
        struct sockaddr_in c_addr;
    }client_data;
pthread_t tid[MAX_CON];
pthread_mutex_t c_mutex;

void *handle_client(void *args) {
    client_data *ctd=(client_data *)args;
    char c_ip[32];
    strcpy(c_ip,inet_ntoa(ctd->c_addr.sin_addr));
    if(ctd->c_id==1) {
        system("cls");
    }
    printf("Connection accepted with IP %s,(Client %d)\n",c_ip,ctd->c_id);
    printf("Now start chatting\n");
    char msg[100];
    SOCKET new_socket=ctd->cs;
    while(1) {

        char msg_reply[100];
        int msg_len;
        if((msg_len=recv(new_socket,msg_reply,100,0))==SOCKET_ERROR) {
            printf("Connection terminated with client %d\n",ctd->c_id);
            pthread_exit(NULL);
        }
        pthread_mutex_lock(&c_mutex);
        printf("Message reccieved from client %d: ",ctd->c_id);
        msg_reply[msg_len]='\0';
        puts(msg_reply);
        printf("Want to reply (y/n): ");
        char c;
        do {
            fflush(stdin);
            c=getchar();
        }
        while(c!='y' && c!='n');
        fflush(stdin);
        if(c=='y') {

            char message[100];
            printf("Message: ");
            fgets(message,100,stdin);
            if(send(new_socket,message,strlen(message),0)<0) {
                printf("Connection terminated with client %d\n",ctd->c_id);
                pthread_mutex_unlock(&c_mutex);
                pthread_exit(NULL);
            }
            puts("Message sent");
        }
        else {
            if(send(new_socket,"n",1,0)<0) {
                printf("Connection terminated with client %d\n",ctd->c_id);
                pthread_mutex_unlock(&c_mutex);
                pthread_exit(NULL);
            }
        }
        pthread_mutex_unlock(&c_mutex);

    }
}

int main(int argc , char *argv[])
{
    WSADATA wsa;
    SOCKET s;
    printf("Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
    printf("Initialised.\n");
    printf("Creating Socket\n");
    s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(s==INVALID_SOCKET) {
        printf("Failed to create socket\n");
    }
    else {
        printf("Socket created successfully\n");
    }
    struct sockaddr_in *server=(struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    server->sin_family=AF_INET;
    server->sin_addr.S_un.S_addr=INADDR_ANY;
    server->sin_port=htons(8585);
    if(bind(s,(struct sockaddr *)server,sizeof(*server))==SOCKET_ERROR) {
        printf("Failed to bind socket\n");
        return 1;
    }
    printf("Binded successsfully\n");
    listen(s,3);
    printf("Waiting for incoming connection...\n");
    struct sockaddr_in client[MAX_CON];
    int c_size=sizeof(struct sockaddr_in);
    SOCKET new_socket[MAX_CON];
    int n_con=0;
    pthread_mutex_init(&c_mutex,NULL);
    while(1) {
        new_socket[n_con]=accept(s,(struct sockaddr *)&client[n_con],&c_size);
        if(new_socket[n_con]==INVALID_SOCKET) {
            printf("Found one invalid socket\n");
            continue;
        }
        else {
            client_data *clt=(client_data *)malloc(sizeof(client_data));
            clt->c_id=1+n_con;
            clt->cs=new_socket[n_con];
            clt->c_addr=client[n_con];
            pthread_create(&tid[n_con],NULL,handle_client,(void *)clt);
            n_con++;
        }
    }
    closesocket(s);
    WSACleanup();
    pthread_mutex_destroy(&c_mutex);
    getch();
    return 0;
}

