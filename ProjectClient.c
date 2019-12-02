// Client Side Socket Program
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

// Use Port 8080
#define PORT 8080 

int main(int argc, char const *argv[])
{
    int runClient = 1;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *conMessage = "Connected";
    char buffer[1024] = {0};

    // Attempt To Create Socket connection
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation failed\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // IPv4/6 To Binary
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nFailed to connect\n");
        return -1;
    }

    // Read From Socket To Buffer
    valread = read(sock, buffer, 1024);

    while (runClient == 1)
    {
        connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        printf("ENTER: ");
        char conData[30];
        scanf("%s", conData);

        if (strcmp(conData,"exit") == 0)
        {
            runClient = 0;
        }
        else
        {
            send(sock, conData, strlen(conData), 0);
        }
    }

    return 0;
}