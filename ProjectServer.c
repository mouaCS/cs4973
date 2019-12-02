// Server Side Socket Program
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

// Use Port 8080
#define PORT 8080

int main(int argc, char const *argv[])
{
    int keepRunning = 1;
    int server_fd;
    int new_socket;
    int valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *conMessage = "Connected";

    // Create Socket File Descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach Socket To Port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    // Send Connection Message
    send(new_socket, conMessage, strlen(conMessage), 0);

    while (keepRunning == 1)
    {

        // Read Client Message To Buffer
        char buffer[1024] = {0};
        valread = read(new_socket, buffer, 1024);
        if (buffer[0] != 0)
        {
            printf("%s\n",buffer);
        }
        else
        {
            keepRunning = 0;
        }

    }
    return 0;
}