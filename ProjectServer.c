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
#define SEATS 150 // seats per flight
#define TICKET_PRICE 450

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
// summary of flights based on dates
// ===========================================================================
void flight_seat_manager(void) {
    File *fp;

    fp = fopen("date.txt", "ab+");

    if(fp == NULL) {
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }
}

// ===========================================================================
// thread function for client to make a reservation and ask for relevant info:
// name, email, phone #, birth date, gender, government ID number, and flight date
// ===========================================================================
void *make_res(void *arg) {
    int add_people = 1;
    int seat number = 0;
    char name[50]; // full name of client
    char dob[15]; // date of birth
    char gender[10];
    char id[15]; // government id #
    char dot[15]; // date of travel
    char email[100];
    char phone[15];
    char choice[5];

    while(add_people == 1) {
        printf("\nPlease enter full name: ");
        fgets(name, sizeof(name), stdin);
    
        printf("Please enter date of birth (01/01/0001): ");
        fgets(dob, sizeof(dob), stdin);
    
        printf("Please enter gender (male or female): ");
        fgets(gender, sizeof(gender), stdin);
    
        printf("Please enter government ID number: ");
        fgets(id, sizeof(id), stdin);
    
        printf("Would you like to reserve a seat?");
        fgets(choice, sizeof(choice), stdin);
    
        if (strcmp(choice, "yes") == 0)
            printf("show flight summary for client to choose seat");
        else if (strcmp(choice, "no") == 0)
            printf("randomly choose seat for client");

        client_receipt(name, gender, seat);

        printf("Would you like to add more people?");

        if (strcmp(choice, "yes") == 0)
            continue;
        else if (strcmp(choice, "no") == 0) {
            printf("\nYou're almost done!!\n\nPlease enter your email: ");
            fgets(email, sizeof(email), stdin);
            printf("Please enter your phone number: ");
            fgets(email, sizeof(email), stdin);
        }
    }

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
    char *conMessage = "\
    Hello dear customer, what can we do for you today?\n\n\
    [1] MAKE A RESERVATION\n\
    [2] RESERVATION INQUIRY\n\
    [3] MODIFY RESERVATION\n\
    [4] CANCEL RESERVATION\n\n\
    Please Select (Type exit to leave): ";

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

            // Selection Handling
            if (strcmp(buffer, "1") == 0)
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
            keepRunning = 0;
        }

    }

    file_manager();
    return 0;
}