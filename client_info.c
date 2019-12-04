#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SEATS 150
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

// ===========================================================================
// main method
// ===========================================================================
int main() {
	char client_ans[3];
	int option;

	// declares thread for clients
	pthread_t clientThread;
	
	// creates threads for N clients at random intervals of time
	printf("\nHello dear customer, what can we do for you today?\n\n");
	printf("[1] Would you like to make a reservation?\n");
	printf("[2] Would you like to inquire about your reservation?\n");
	printf("[3] Would you like to modify your reservation?\n");
	printf("[4] Would you like to cancel your reservation?\n\n");
	printf("Please select any of the options above by typing the number next to the suggestions! ");
	fgets(client_ans, sizeof(client_ans), stdin);
	sscanf(client_ans, "%d", &option);

	switch(option) {
		case 1:
			pthread_create(&clientThread, NULL, make_res, NULL);
			break;
		case 2:
			pthread_create(&clientThread, NULL, inquire_res, NULL);
			break;
		case 3:
			pthread_create(&clientThread, NULL, modify_res, NULL);
			break;
		case 4:
			pthread_create(&clientThread, NULL, cancel_res, NULL);
			break;
		default:
			printf("You have made an invalid request.\n");
			break;
	}

	// makes sure for all threads to finish
	pthread_join(clientThread, NULL);

	return 0;
}