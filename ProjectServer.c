// Server Side Socket Program
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

// Set Port
#define PORT 2000

// Set Plane Info
#define SEATS 150
#define TICKET_PRICE 450


// Global Server Socket Variables
int server_fd;
int new_socket;
int valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);


// ===========================================================================
// thread function for client to make a reservation and ask for relevant info:
// name, email, phone #, birth date, gender, government ID number, and flight date
// ===========================================================================
void *make_res(void *arg) {
    char name[50];
    char dob[15];
    char gender[10];
    char id[15];

    printf("\nPlease enter your full name: ");
    fgets(name, sizeof(name), stdin);

    printf("Please enter your date of birth (01/01/0001): ");
    fgets(dob, sizeof(dob), stdin);

    printf("Please enter your gender (male or female): ");
    fgets(gender, sizeof(gender), stdin);

    printf("Please enter your government ID number: ");
    fgets(id, sizeof(id), stdin);

    pthread_exit(NULL);
}


// ===========================================================================
// thread function for client to inquire about reservation
// ===========================================================================
void *inquire_res(void *arg) {
    printf("\n\nInquiry reservation\n\n");
    pthread_exit(NULL);
}

// ===========================================================================
// thread function for client to modify about reservation
// ===========================================================================
void *modify_res(void *arg) {
    printf("\n\nModify reservation\n\n");
    pthread_exit(NULL);
}

// ===========================================================================
// thread function for client to cancel reservation
// ===========================================================================
void *cancel_res(void *arg) {
    printf("\n\nCancel reservation\n\n");
    pthread_exit(NULL);
}

void *establishCon(void *threadID)
{
    int *id = (int *)threadID;

    // Thread Running Boolean
    int keepRunning = 1;
    int end = 0;

    // Message Handling
    char buffer[1024] = {0};
    char *conMessage = "\
    Hello dear customer, what can we do for you today?\n\n\
    [1] MAKE A RESERVATION\n\
    [2] RESERVATION INQUIRY\n\
    [3] MODIFY RESERVATION\n\
    [4] CANCEL RESERVATION\n\n\
    Please Select (Type exit to leave): ";

    // Encapsulating Thread Loop
    while (end != 1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // Send Connection Message
        send(new_socket, conMessage, strlen(conMessage), 0);

        // Loop For Single Client
        while (keepRunning == 1)
        {

            // Read Client Message To Buffer
            char buffer[1024] = {0};
            valread = read(new_socket, buffer, 1024);
            if (buffer[0] != 0)
            {
                // -----------------------------
                // FOR TESTING ONLY
                // End Thread Completely
                if (strcmp(buffer, "end") == 0)
                {
                    keepRunning = 0;
                    end = 1;
                    printf("ENDING THREAD\n");
                }
                // FOR TESTING ONLY
                // -----------------------------

                // Selection Handling
                else if (strcmp(buffer, "1") == 0)
                {
                    printf("SELECTED 1\n");
                    send(new_socket, conMessage, strlen(conMessage), 0);
                }
                else if (strcmp(buffer, "2") == 0)
                {
                    printf("SELECTED 2\n");
                    send(new_socket, conMessage, strlen(conMessage), 0);
                }
                else if (strcmp(buffer, "3") == 0)
                {
                    printf("SELECTED 3\n");
                    send(new_socket, conMessage, strlen(conMessage), 0);
                }
                else if (strcmp(buffer, "4") == 0)
                {
                    printf("SELECTED 4\n");
                    send(new_socket, conMessage, strlen(conMessage), 0);
                }
                else
                {
                    send(new_socket, conMessage, strlen(conMessage), 0);
                }
            }
            else
            {
                // End Thread
                keepRunning = 0;
            }

        }
    }
    return NULL;
}



//----------------
//----------------
// MAIN EXECUTION
//----------------
//----------------
int main(int argc, char const *argv[])
{
    int MAX_CLIENTS = 100;

    // Create Socket File Descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach Socket To Port
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


    // Thread Handling
    pthread_t clientThread;
    
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        pthread_create(&clientThread, NULL, establishCon, &clientThread);
    }
    pthread_join(clientThread, NULL); 
}