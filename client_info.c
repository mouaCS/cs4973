#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SEATS 150
#define TICKET_PRICE 450

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

// ===========================================================================
// main method
// ===========================================================================
int main() {
	char client_ans[3];
	int option;

	// declares thread for clients
	pthread_t clientThread;
	
	// asks client what they want to do for reservation
	printf("\nHello dear customer, what can we do for you today?\n\n");
	printf("[1] Would you like to make a reservation?\n");
	printf("[2] Would you like to inquire about your reservation?\n");
	printf("[3] Would you like to modify your reservation?\n");
	printf("[4] Would you like to cancel your reservation?\n\n");
	printf("Please select any of the options above by typing the number next to the suggestions! ");
	fgets(client_ans, sizeof(client_ans), stdin);
	sscanf(client_ans, "%d", &option);

	// based on user input, will send them to appropriate thread function
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