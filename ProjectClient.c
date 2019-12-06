// Client Side Socket Program
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

// Set Port
#define PORT 2000 

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
    printf("%s",buffer);

    while (runClient == 1)
    {
        char input[30];
        char *conData;
        fgets(input, 30, stdin);
        conData = strtok(input, "\n");

        // Exit From Client Loop
        if (strcmp(conData,"exit") == 0)
        {
            runClient = 0;
        }
        // ------------------
        // FOR TESTING ONLY
        // Exit Thread
        else if (strcmp(conData,"end") == 0)
        {
            send(sock, conData, strlen(conData), 0);
            runClient = 0;
        }
        // FOR TESTING ONLY
        // ------------------

        // Send Message To Server
        else
        {
            send(sock, conData, strlen(conData), 0);


            // Read Server Message To Buffer
            char buffer[1024] = {0};
            read(sock, buffer, 1024);
            printf("%s",buffer);
        }
    }

    return 0;
}