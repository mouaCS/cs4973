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
// function to create client receipts
// ===========================================================================
void client_receipt(char name[50], char dof[15], int seat_num) {
    FILE *fp;

    fp = fopen("receipt.txt", "ab+");

    if(fp == NULL) {
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "Name: %s\nDate of Flight: %s\nSeat number: %d\n\n", name, dof, seat_num);
    fclose(fp);
}

// ===========================================================================
// thread function for client to make a reservation and ask for relevant info:
// name, email, phone #, birth date, gender, government ID number, and flight date
// ===========================================================================
void *make_res(void *arg) {
    int add_people = 1;
    int seat = 0;
    int count = 1;
    char name[50], dob[12], gender[7], id[15], dot[12];
    char email[100], phone[15];
    char choice[5];

    while(add_people == 1) {
        // asks for full name of customer
        printf("\nPlease enter full name: ");
        scanf(" %[^\n]", name);
    
        // asks for birthdate of customer
        printf("Please enter date of birth (01/01/0001): ");
        scanf(" %[^\n]", dob);
    
        // asks for gender of customer
        printf("Please enter gender (male or female): ");
        scanf(" %[^\n]", gender);

        // asks for government ID of customer    
        printf("Please enter government ID number: ");
        scanf(" %[^\n]", id);

        // asks for date of travel/flight
        printf("Please enter the date you want to travel (01/01/0001): ");
        scanf(" %[^\n]", dot);

        // asks if customer would like to reserve a seat
        printf("Would you like to reserve a seat? [yes/no] ");
        scanf(" %[^\n]", choice);
    
        if (strcmp(choice, "yes") == 0) {
            printf("show flight summary for client to choose seat\n\n");
        }
        else if (strcmp(choice, "no") == 0)
            seat = 1 + rand() / (RAND_MAX / (150 - 1 + 1) + 1);

        client_receipt(name, gender, seat);

        // asks if customer would like to make a reservation for another person
        printf("Would you like to add more people? [yes/no] ");
        scanf(" %[^\n]", choice);

        if (strcmp(choice, "no") == 0)
            add_people = 0;
    }

    printf("\nYou're almost done!!\n\nPlease enter your email: ");
    scanf(" %[^\n]", email);
    
    printf("Please enter your phone number: ");
    scanf(" %[^\n]", phone);

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