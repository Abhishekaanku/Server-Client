#include<stdio.h>
#include<winsock2.h>
#include<malloc.h>
#include<conio.h>
#pragma comment(lib,"libws2_32.a") //Winsock Library
#pragma comment (lib, "libMswsock.a")
#pragma comment (lib, "libAdvApi32.a")



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
    char ip_addr[20];
    int port;
    printf("Enter IP address of server: ");
    scanf("%s",ip_addr);
    printf("Enter port: ");
    scanf("%d",&port);
    server->sin_addr.S_un.S_addr=inet_addr(ip_addr);
    server->sin_family=AF_INET;
    server->sin_port=htons(port);
    if (connect(s , (struct sockaddr *)server , sizeof(*server)) < 0)
    {
        puts("Connection error");
        getch();
        return 1;
    }

    system("cls");
    puts("Connected");
    char message[100];
    printf("Start chatting now\n");
    fflush(stdin);
    while(1) {
        fgets(message,50,stdin);
        if(send(s,message,strlen(message),0)<0) {
            printf("Message sending failed\n");
            break;
        }
        printf("Sent Successfully\n");
        char server_reply[500];
        int rec_size;
        if((rec_size=recv(s,server_reply,500,0))==SOCKET_ERROR) {
            printf("Error..connection with server terminated\n");
            break;
        }
        server_reply[rec_size]='\0';
        if(strcmp(server_reply,"n")==0) {
            printf("Server didn't reply\n");
        }
        else {
            fputs("Message recieved: ",stdout);
            puts(server_reply);
        }
    }
    closesocket(s);
    WSACleanup();
    getch();
    return 0;
}

