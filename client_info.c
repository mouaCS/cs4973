#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SEATS 150
#define TICKET_PRICE 450

// ===========================================================================
// function to call when customer makes reservation for multiple people
// ===========================================================================
void client_append_receipt(char name[50], char dof[15], int seat_num, int ticket_num) {
    FILE *fp;
    char filename[15];

    sprintf(filename, "%d.txt", ticket_num);
    fp = fopen(filename, "ab+");

    if(fp == NULL) {
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "Name: %s\nDate of Flight: %s\nSeat number: %d\n\n", name, dof, seat_num);
    fclose(fp);
}

// ===========================================================================
// function to call whenever client enters date of travel
// also keeps track of vacant or occupied seats
// ===========================================================================
void flight_manager(char travel_date[12]) {
	FILE *fp;
	char filename[sizeof "00-00-0000.txt"];
	char seat_available[SEATS] = {"vacant"};
	char choice[5];

	sprintf(filename, "%02d-%02d-%04d.txt", travel_date);
	fp = fopen(filename, "ab+");

	if(fp == NULL) {
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

/*    while(strcmp(seat_available[seat_num], "vacant") != 0) {
        if(strcmp(seat_available[seat_num], "vacant") == 0)
        	seat_available[seat_num] = "occupied";
        else {
        	printf("The seat you have chosen is not available, we will randomly select a seat for you.");
        	seat_num = 1 + rand() / (RAND_MAX / (150 - 1 + 1) + 1);
        }
    }*/

    fclose(fp);
}

// ===========================================================================
// thread function for client to make a reservation and ask for relevant info:
// name, email, phone #, birth date, gender, government ID number, and flight date
// ===========================================================================
void *make_res(void *arg) {
    int ticket_num = rand();
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

        // calls function to create and write info to text file
        client_append_receipt(name, dot, seat, ticket_num);

        // asks if customer would like to make a reservation for another person
        printf("Would you like to add more people? [yes/no] ");
        scanf(" %[^\n]", choice);

        // exit loop after done adding all relevant info for reservation
        if (strcmp(choice, "no") == 0)
            add_people = 0;
    }

    // asks for email
    printf("\nYou're almost done!!\n\nPlease enter your email: ");
    scanf(" %[^\n]", email);
    
    // asks for phone
    printf("Please enter your phone number: ");
    scanf(" %[^\n]", phone);

    printf("\nThank you for making reservations with us! Here is your ticket number: %d\n", ticket);

    pthread_exit(NULL);
}

// ===========================================================================
// thread function for client to inquire about reservation
// ===========================================================================
void *inquire_res(void *arg) {
	printf("\nInquiry reservation\n\n");
	printf("Please enter ticket number: ");
	pthread_exit(NULL);
}

// ===========================================================================
// thread function for client to modify about reservation
// ===========================================================================
void *modify_res(void *arg) {
	printf("\nModify reservation\n\n");
	pthread_exit(NULL);
}

// ===========================================================================
// thread function for client to cancel reservation
// ===========================================================================
void *cancel_res(void *arg) {
	printf("\nCancel reservation\n\n");
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
	scanf(" %[^\n]", client_ans);
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