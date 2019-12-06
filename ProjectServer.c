// Server Side Socket Program
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

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
char buffer[1024] = {0};


// Mutex Lock
pthread_mutex_t lock;

// ===========================================================================
// function to call when customer makes reservation for multiple people
// ===========================================================================
void client_append_receipt(char name[50], char dob[15], char gender[7], char id[15], char dof[15], int seat_num, int ticket_num) {
    FILE *fp;
    char filename[15];

    sprintf(filename, "%d.txt", ticket_num);
    fp = fopen(filename, "ab+");

    if(fp == NULL) {
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "Name: %s\nDate of birth: %s\nGender: %s\nGovernment ID#: %s\nDate of Flight: %s\nSeat number: %d\n\n", name, dob, gender, id, dof, seat_num);
    fclose(fp);
}

// ===========================================================================
// creates a new text file based on the date of flight
// creates 150 "vacant" seats in the file
// ===========================================================================
void date_file(char dot[15]) {
    FILE *fp;
    char filename[20];

    sprintf(filename, "%s.txt", dot);
    if(!(fp = fopen(filename, "rb"))) {
        fp = fopen(filename, "wb+");
    }
    else {
        printf("date of flight file exists.\n");
        fclose(fp);
    }

    for(int i=0; i<SEATS; i++)
        fprintf(fp, "%03d vacant\n", i+1);

    fclose(fp);
}

// ===========================================================================
// function to call whenever client enters date of travel
// also keeps track of vacant or occupied seats
// ===========================================================================
void seat_manager(char name[50], char gender[7], char dot[15], char choice[5]) {
    FILE *fp;
    FILE *ftemp;
    char filename[20], filetmp[20], newline[75], cmp[20];
    char file_error_msg[20] = "ERROR";
    char *choose_msg = "Type any number with a vacant label.\n\n";
    int new_seat = 0;

    sprintf(filename, "%s.txt", dot);
    sprintf(filetmp, "%s.tmp", dot);
    fp = fopen(filename, "r");
    ftemp = fopen(filetmp, "w");
    if(fp == NULL) {
        send(new_socket, file_error_msg, strlen(file_error_msg), 0);
        exit(EXIT_FAILURE);
    }

    fclose(fp);
}

// ===========================================================================
// section handles making reservation and ask for relevant info:
// name, email, phone #, birth date, gender, government ID number, and flight date
// ===========================================================================
void make_res(int new_socket) {
    int ticket_num = rand();
    int add_people = 1;
    int seat_num;
    char name[50], dob[12], gender[7], id[15], dot[12];
    char email[100], phone[15];
    char choice[5];
    char *invalid_msg = "Invalid answer. Please answer yes or no.\n";

    while(add_people == 1) {
        // asks for full name of customer
        char *name_msg = "\nPlease enter full name: ";
        send(new_socket, name_msg, strlen(name_msg), 0);
        valread = read(new_socket, buffer, 1024);
        strcpy(name, buffer);
        memset(buffer, 0, sizeof(buffer));
                        
        // asks for birthdate of customer
        char *birthdate_msg = "Please enter date of birth (01-01-0001): ";
        send(new_socket, birthdate_msg, strlen(birthdate_msg), 0);
        valread = read(new_socket, buffer, 1024);
        strcpy(dob, buffer);
        memset(buffer, 0, sizeof(buffer));
                    
        // asks for gender of customer
        char *gender_msg = "Please enter gender (male or female): ";
        send(new_socket, gender_msg, strlen(gender_msg), 0);
        valread = read(new_socket, buffer, 1024);
        strcpy(gender, buffer);
        memset(buffer, 0, sizeof(buffer));

        // asks for government ID of customer    
        char *id_msg = "Please enter government ID number: ";
        send(new_socket, id_msg, strlen(id_msg), 0);
        valread = read(new_socket, buffer, 1024);
        strcpy(id, buffer);
        memset(buffer, 0, sizeof(buffer));

        // asks for date of travel/flight
        char *dot_msg = "Please enter the date you want to travel (01-01-0001): ";
        send(new_socket, dot_msg, strlen(dot_msg), 0);
        valread = read(new_socket, buffer, 1024);
        strcpy(dot, buffer);
        memset(buffer, 0, sizeof(buffer));
        date_file(dot);

        // asks if customer would like to reserve a seat_num
        char *seat_msg = "Would you like to reserve a seat? [yes/no] ";
        while(1) {
            send(new_socket, seat_msg, strlen(seat_msg), 0);
            valread = read(new_socket, buffer, 1024);
            strcpy(choice, buffer);
            memset(buffer, 0, sizeof(buffer));

            if ((strcmp(choice, "yes") == 0) || (strcmp(choice, "no") == 0)) {
                printf("Wants to pick out seat_num.\n");
                seat_manager(name, gender, dot, choice);
                break;
            }
            else
                send(new_socket, invalid_msg, strlen(invalid_msg), 0);
        }

        // calls function to create and write info to text file
        client_append_receipt(name, dob, gender, id, dot, seat_num, ticket_num);

        // asks if customer would like to make a reservation for another person
        char *people_msg = "Would you like to add more people? [yes/no] ";
                
        // exit loop after done adding all relevant info for reservation
        while(1) {
            send(new_socket, people_msg, strlen(people_msg), 0);
            valread = read(new_socket, buffer, 1024);
            strcpy(choice, buffer);
            memset(buffer, 0, sizeof(buffer));

            if (strcmp(choice, "yes") == 0)
                break;
            else if (strcmp(choice, "no") == 0) {
                add_people = 0;
                break;
            }
            else
                send(new_socket, invalid_msg, strlen(invalid_msg), 0);
        }
    }

    // asks for email
    char *email_msg = "\nYou're almost done!!\n\nPlease enter your email: ";
    send(new_socket, email_msg, strlen(email_msg), 0);
    valread = read(new_socket, buffer, 1024);
    strcpy(email, buffer);
    memset(buffer, 0, sizeof(buffer));
            
    // asks for phone
    char *phone_msg = "Please enter your phone number: ";
    send(new_socket, phone_msg, strlen(phone_msg), 0);
    valread = read(new_socket, buffer, 1024);
    strcpy(phone, buffer);
    memset(buffer, 0, sizeof(buffer));

    // thank you message
    char thank_msg[100] = "\nThank you for making reservations with us! Here is your ticket number: ";
    char ending_msg[130];
    sprintf(ending_msg, "%s %d", thank_msg, ticket_num);
    send(new_socket, ending_msg, strlen(ending_msg), 0);
}

// ===========================================================================
// thread function for client to inquire about reservation
// ===========================================================================
void inquire_res(int new_socket) {
    FILE *fp;
    char filename[15];
    char ticket_num[15];

    char *ticket_msg = "\nPlease enter ticket number: ";
    send(new_socket, ticket_msg, strlen(ticket_msg), 0);
    valread = read(new_socket, buffer, 1024);
    strcpy(ticket_num, buffer);
    memset(buffer, 0, sizeof(buffer));

    sprintf(filename, "%s.txt", ticket_num);
    fp = fopen(filename, "r");
    if(fp == NULL) {
        char *error_msg = "Unable to find receipt.\n";
        send(new_socket, error_msg, strlen(ticket_msg), 0);
    }

    while(fp != NULL) {
        unsigned char buff[256] = {0};
        int nread = fread(buff, 1, 256, fp);
        printf("Bytes read %d\n", nread);

        if(nread > 0) {
            printf("Sending \n");
            write(new_socket, buff, nread);
        }

        if(nread < 256) {
            if(feof(fp))
                printf("End of file.\n");
            else if(feof(fp))
                printf("Error reading.\n");
            break;
        }
    }

    fclose(fp);
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
void cancel_res() {
    printf("\n\nCancel reservation\n\n");
}

void *establishCon(void *threadID)
{
    // Thread ID
    int *id = (int *)threadID;

    int new_socket;

    // Thread Running Boolean
    int keepRunning = 1;
    int end = 0;

    // Message Handling
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
        // Accept Connection And Create New Socket For It
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("TID: %d\n",*id);
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
                //
                // Create Reservations
                else if (strcmp(buffer, "1") == 0)
                {
                    printf("SELECTED 1...\n");
                    pthread_mutex_lock(&lock);
                    make_res(new_socket);
                    pthread_mutex_unlock(&lock);

                    // Send Default Message Again
                    send(new_socket, conMessage, strlen(conMessage), 0);
                }
                // Inquiries
                else if (strcmp(buffer, "2") == 0)
                {
                    printf("SELECTED 2...\n");
                    inquire_res(new_socket);
                }
                // Modify Reservations
                else if (strcmp(buffer, "3") == 0)
                {
                    printf("SELECTED 3...\n");
                    pthread_mutex_lock(&lock); 
                    send(new_socket, conMessage, strlen(conMessage), 0);
                    pthread_mutex_unlock(&lock);
                }
                // Cancel Reservations
                else if (strcmp(buffer, "4") == 0)
                {
                    printf("SELECTED 4...\n");
                    send(new_socket, conMessage, strlen(conMessage), 0);
                }
                // Send Default Message
                else
                    send(new_socket, conMessage, strlen(conMessage), 0);
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
    int MAX_CLIENTS = 3;

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
    pthread_t clientThread[300];

    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    
    for (int i = 0; i < MAX_CLIENTS; i++)
        pthread_create(&clientThread[i], NULL, establishCon, &clientThread[i]);

    for (int i = 0; i < MAX_CLIENTS; i++)
        pthread_join(clientThread[i], NULL);

    return 0;
}