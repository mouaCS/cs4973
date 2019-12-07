// Server Side Socket Program
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <memory.h>
#include <time.h>
#include <fcntl.h>

// Set Port
#define PORT 2000

// Set Plane Info
#define SEATS 150
#define TICKET_PRICE 450


// Global Server Socket Variables
int server_fd;
int valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char buffer[1024] = {0};
char read_only_message[10] = "READ ONLY";


// Mutex Lock
pthread_mutex_t lock;

typedef struct CSeat
{
	char m_Name[50];
	char m_DoB[10];
	char m_Gender[10];
	char m_IDn[15];
}CSeat;

int * getAvailableSeatsList(void * p)
{
	static int seatsAvail[150];
	memset(seatsAvail,0,sizeof(seatsAvail));
	struct CSeat * theSeats = (struct CSeat *) p; 
	int i = 1, j = 0;
	while(i < 151)
	{
		if(strlen(theSeats[i].m_Name) < 2)
		{
			seatsAvail[j] = i;
			j++;
		}
		i++;
	}
	return seatsAvail;
}

char * getAvailableSeats(void * p, int cases, char fileName[])
{
	char * seatsAvail = (char *)malloc(750);
	memset(seatsAvail,0,sizeof(seatsAvail));
	char temp[3];
	struct CSeat * theSeats = (struct CSeat *) p; 
	int i = 1;
	while(i < 151)
	{
		if(strlen(theSeats[i].m_Name) < 2)
		{
			sprintf(temp,"%d",i);
			strcat(seatsAvail,temp);
			strcat(seatsAvail," ");
		}
		i++;
	}
	if(cases == 11) // For modification
	{
		FILE * fd=fopen(fileName,"rb");
		char * pch;
		char readline[100];
		int counter = 0;
		while(counter < 5)
		{
			fgets(readline, 100, fd);
			counter++;
		}
		readline[strlen(readline)-1]=0; // Remove "\n"
		pch = strtok(readline, ", ");
		while(pch != NULL)
		{
			strcat(seatsAvail,pch);
			strcat(seatsAvail," ");
			pch = strtok(NULL, ", ");
		}
		fclose(fd);
	}
	return seatsAvail;
}

struct CSeat * readData(char fileName[], int cases)
{
	struct CSeat * theSeats = (struct CSeat *)malloc(151*sizeof(theSeats[1]));
	if(cases == 10) // Exist
	{
		FILE * fd=fopen(fileName,"rb");
		fread(theSeats,1,sizeof(theSeats[0])*151,fd);
		fclose(fd);
	}
	else // Or not
	{
		strcpy(&(theSeats->m_Name[0]), "0");
	}
	return theSeats;
}

/*
	Write a struct as seat list, named DoT.DAT
*/
void writeDate(char fileName[], void * p)
{
	struct CSeat * theSeats = (struct CSeat *) p; 
	FILE * fd=fopen(fileName,"wb");
	fwrite(theSeats,1,sizeof(theSeats[0])*151,fd);
	fclose(fd);
}

/*
	Write a new receipt, called in reserve(). Use all personal infomation, wanted seats, contact information
	and dateofTravel.
*/
void writeReceipt(char info[160][80], char seats[150][2], char ci1[], char ci2[], char dateofTravel[], int ticketNumber)
{
	int i = 0, j = 0;
	char fileName[15];
	sprintf(fileName, "%d.txt", ticketNumber);
	FILE * fd=fopen(fileName,"wb");
	fprintf(fd, "Ticket Number: %d\n", ticketNumber);
	fprintf(fd, "E-mail: %s, Phone number: %s\n", ci1, ci2);
	fprintf(fd, "%s\n", dateofTravel);
	fprintf(fd, "Reserved Seats: \n");
	while(strlen(seats[j]) > 0)
	{
		fprintf(fd, "%s, ", seats[j]);
		j++;
	}
	fprintf(fd, "\n");
	while(strlen(info[i]) > 0)
	{
		fprintf(fd, "Name: %s, DoB: %s, Gender: %s, ID: %s\n", info[i], info[i+1], info[i+2], info[i+3]);
		i = i + 4;
	}
	fclose(fd);
}

// Messages Requesting Client Response
char *client_req(int new_socket, char *msg)
{
    char tBuffer[1024] = {0};
    char response[80];

    send(new_socket, msg, strlen(msg), 0);
    valread = read(new_socket, tBuffer, 1024);
    strcpy(response, tBuffer);
    memset(tBuffer, 0, sizeof(tBuffer));

    char *returnString = response;
    return returnString;
}

int client_req_and_recv_response(int new_socket, char * msg, int msgLen, char * response, int resLimit)
{
    send(new_socket, msg, msgLen, 0);
    return read(new_socket, response, resLimit);
}

// ===========================================================================
// section handles making reservation and ask for relevant info:
// name, email, phone #, birth date, gender, government ID number, and flight date
// ===========================================================================
void make_res(int new_socket) {
	struct CSeat seatsNew[151]; 
	CSeat * p;
	char infoSplit[160][80];
	memset(infoSplit,0,sizeof(infoSplit));
	int passagersNum = 0;
	char dateofTravel[12];
	char seatNum[150][2]; // Save seats wanted
	memset(seatNum,0,sizeof(seatNum));
	char seats[150]; // Seats chosen
	
	srand((unsigned int)time(NULL));
	int ticketNumber = rand();
    int add_people = 1;
    int seat_num;
    char name[50], dob[12], gender[7], id[15], dot[12];
	char choice[5];
    char choiceA[5];
	char choiceB[5];
    char *invalid_msg = "Invalid answer. Please answer yes or no.\n";


    char *name_msg = "\nPlease enter full name: ";
    char *birthdate_msg = "Please enter date of birth (01-01-0001): ";
    char *gender_msg = "Please enter gender (male or female): ";
    char *id_msg = "Please enter government ID number: ";
    char *dot_msg = "Please enter the date you want to travel (01-01-0001): ";
    char *seat_msg = "Would you like to reserve a seat? [yes/no] ";
    char *people_msg = "Would you like to add more people? [yes/no] ";
	char *seatsavail_msg = "Please enter the seats you want, separated by space (1 2 11): ";
    char *email_msg = "\nYou're almost done!!\n\nPlease enter your email: ";
    char *phone_msg = "Please enter your phone number: ";
	char *enoughseats_msg = "Enough seats remains.\n";
	char *noseats_msg = "Sorry, we don't have enough seats.";
    char thank_msg[100] = "\nThank you for making reservations with us!\n Here is your ticket number: ";
    while(add_people == 1) { 
	
		int n=client_req_and_recv_response(new_socket,name_msg,strlen(name_msg),
		  infoSplit[4*passagersNum],sizeof(infoSplit[0]));
		infoSplit[4*passagersNum][n]=0;

		n=client_req_and_recv_response(new_socket,birthdate_msg,strlen(birthdate_msg),
		  infoSplit[4*passagersNum+1],sizeof(infoSplit[0]));
		infoSplit[4*passagersNum+1][n]=0;

		n=client_req_and_recv_response(new_socket,gender_msg,strlen(gender_msg),
		  infoSplit[4*passagersNum+2],sizeof(infoSplit[0]));
		infoSplit[4*passagersNum+2][n]=0;

		n=client_req_and_recv_response(new_socket,id_msg,strlen(id_msg),
		  infoSplit[4*passagersNum+3],sizeof(infoSplit[0]));
		infoSplit[4*passagersNum+3][n]=0;

		if(passagersNum == 0)
		{
			n=client_req_and_recv_response(new_socket,dot_msg,strlen(dot_msg),
				dateofTravel,sizeof(dateofTravel));
			dateofTravel[n] = 0;
		}
		passagersNum++;
        while(1)
        {
			n=client_req_and_recv_response(new_socket,people_msg,strlen(people_msg),
				choiceA,sizeof(choiceA));
			choiceA[n] = 0;

            if (strcmp(choiceA, "yes") == 0)
            {
                break;
            }
            else if (strcmp(choiceA, "no") == 0)
            {
                add_people = 0;
                break;
            }
            else
            {
                // send read only message
                send(new_socket, read_only_message, strlen(read_only_message), 0);
                read(new_socket, buffer, 1024);
                memset(buffer, 0, sizeof(buffer));
                send(new_socket, invalid_msg, strlen(invalid_msg), 0);
            }
        }
    }
	
	char fileName[20];
    strcpy(fileName, dateofTravel);
	strcat(fileName,".DAT");
	if((access(fileName, F_OK)) != -1) // Exist
	{
		p = readData(fileName, 10); //----------------------------
		int seatRem = 150 - (atoi)(p[0].m_Name);
		if (seatRem >=  passagersNum) // If have enough seats.
		{
			send(new_socket, enoughseats_msg, strlen(enoughseats_msg), 0);
			printf("Sure.\n");
		}
		else // Or not.
		{
			send(new_socket, noseats_msg, strlen(noseats_msg), 0);
			return;
			// Refuse request and exit. //
		}
	}
	else // Not exist
	{
		send(new_socket, enoughseats_msg, strlen(enoughseats_msg), 0);
		p = readData(fileName, 11); //----------------------------
	}
	char * seatsAvail;
	while(1)
    {
		seatsAvail = getAvailableSeats(p, 10, "NULLL");
		int n=client_req_and_recv_response(new_socket,seat_msg,strlen(seat_msg),
			choiceB,sizeof(choiceB));
		choiceB[n] = 0;
        if (strcmp(choiceB, "yes") == 0)
        {
			send(new_socket, seatsAvail, strlen(seatsAvail), 0);
			n=client_req_and_recv_response(new_socket,seatsavail_msg,strlen(seatsavail_msg),
				seats,sizeof(seats));
			seats[n] = 0;
			char * pch;
			int i = 0;
			
			pch = strtok(seats, " ");
			while(pch != NULL)
			{
				strcpy(seatNum[i], pch);
				pch = strtok(NULL, ", ");
				i++;
			}
            break;
        }
		else if(strcmp(choiceB, "no") == 0)
		{
			char * pch;
			int i = 0;
			
			pch = strtok(seatsAvail, " ");
			while(pch != NULL && i < passagersNum)
			{
				strcpy(seatNum[i], pch);
				pch = strtok(NULL, ", ");
				i++;
			}
			break;
		}
        else
        {
            // send read only message
            send(new_socket, read_only_message, strlen(read_only_message), 0);
            read(new_socket, buffer, 1024);
            memset(buffer, 0, sizeof(buffer));
            send(new_socket, invalid_msg, strlen(invalid_msg), 0);
        }
		free(seatsAvail);
    }
	
	char email[25], phone[15];
	int n=client_req_and_recv_response(new_socket,email_msg,strlen(email_msg),
		email,sizeof(email));
	email[n] = 0;
	n=client_req_and_recv_response(new_socket,phone_msg,strlen(phone_msg),
		phone,sizeof(phone));
	phone[n] = 0;
	
	writeReceipt(infoSplit, seatNum, email, phone, dateofTravel, ticketNumber);
	int i = 0;
	while(i < passagersNum)
	{
		strcpy(p[(atoi)(seatNum[i])].m_Name, infoSplit[4*i]);
		strcpy(p[(atoi)(seatNum[i])].m_DoB, infoSplit[4*i+1]);
		strcpy(p[(atoi)(seatNum[i])].m_Gender, infoSplit[4*i+2]);
		strcpy(p[(atoi)(seatNum[i])].m_IDn, infoSplit[4*i+3]);
		i++;
	}
	int passagerNumUpdate = (atoi)(p[0].m_Name);
	passagerNumUpdate = passagerNumUpdate + passagersNum;
	char temp[25];
	sprintf(temp,"%d",passagerNumUpdate);
	strcpy(p[0].m_Name, temp);
	writeDate(fileName, p);
	free(p);
	
    // send read only message
    send(new_socket, read_only_message, strlen(read_only_message), 0);
    read(new_socket, buffer, 1024);
    memset(buffer, 0, sizeof(buffer));
    // thank you message
    char ending_msg[130];
    sprintf(ending_msg, "%s %d\n", thank_msg, ticketNumber);
    send(new_socket, ending_msg, strlen(ending_msg), 0);
}

void changeSeats(int new_socket)
{
	char *seatsavail_msg = "Please enter the seats you want, separated by space, end by comma (1,2,11,): ";
	char fileName[20];
	char *ticket_msg = "\nPlease enter ticket number: ";
	int n=client_req_and_recv_response(new_socket,ticket_msg,strlen(ticket_msg),
		fileName,sizeof(fileName));
	fileName[n] = 0;
	strcat(fileName, ".txt");
	FILE * fd=fopen(fileName,"r+");
	if(fd == NULL)
	{
        char *error_msg = "Unable to find receipt.\n";
        send(new_socket, error_msg, strlen(error_msg), 0);
		return;
    }
	char readline[100], seatsFileName[15];
	int oldSeats[150], newSeats[150];
	int counter = 0, counterSaved = 0, line_len = 0, len = 0; 
	char * pch;
	CSeat empty = {'\0', '\0', '\0', '\0'};
	while(counter < 3)
	{
		fgets(readline, 100, fd);
		line_len = strlen(readline);
		len += line_len;
		counter++;
	}
	strcpy(seatsFileName, readline); // Find filename saved in receipt
	seatsFileName[strlen(seatsFileName)-1]=0; // Remove "\n"
	strcat(seatsFileName, ".DAT");

	char * seatsAvail;
	char seatsChosen[150];
	char seatsTemp[100];
	CSeat * p, * temp;
	p = readData(seatsFileName, 10);
	temp = readData(seatsFileName, 10);
	seatsAvail = getAvailableSeats(p, 11, fileName);
	// Send seatsAvail to client and wait for choice //
	send(new_socket, seatsAvail, strlen(seatsAvail), 0);
	n=client_req_and_recv_response(new_socket,seatsavail_msg,strlen(seatsavail_msg),
		seatsChosen,sizeof(seatsChosen));
	seatsChosen[n] = 0;
			
	strcpy(seatsTemp, seatsChosen);
	
	fgets(readline, 100, fd);
	line_len = strlen(readline);
	len += line_len;

	counter = 0;
	fgets(readline, 100, fd);
	readline[strlen(readline)-1]=0; // Remove "\n"
	pch = strtok(readline, ", ");
	while(pch != NULL)
	{
		oldSeats[counter] = (atoi)(pch);
		pch = strtok(NULL, ", ");
		counter++;
	}
	counterSaved = counter;
	counter = 0;
	pch = strtok(seatsChosen, ", ");

	while(counter < counterSaved)
	{
		newSeats[counter] = (atoi)(pch);
		pch = strtok(NULL, ", ");
		counter++;
	}
	counterSaved = counter;

	counter = fseek(fd, len, SEEK_SET);
	fprintf(fd, "%s             \n", seatsTemp);
	
	counter = 0;
	while(counter < counterSaved)
	{
		p[oldSeats[counter]] = empty;
		p[newSeats[counter]] = temp[oldSeats[counter]];
		counter++;
	}
	writeDate(seatsFileName, p);
	fclose(fd);
	free(p);
	free(temp);
	send(new_socket, read_only_message, strlen(read_only_message), 0);
	read(new_socket, buffer, 1024);
	memset(buffer, 0, sizeof(buffer));
}

// ===========================================================================
// thread function for client to inquire about reservation
// ===========================================================================
void inquire_res(int new_socket) {
	char fileName[20];
	char *ticket_msg = "\nPlease enter ticket number: ";
	int n=client_req_and_recv_response(new_socket,ticket_msg,strlen(ticket_msg),
		fileName,sizeof(fileName));
	fileName[n] = 0;
	strcat(fileName, ".txt");
	
	FILE * fd=fopen(fileName,"rb");
	if(fd == NULL)
	{
        char *error_msg = "Unable to find receipt.\n";
        send(new_socket, error_msg, strlen(error_msg), 0);
		return;
    }
	char readline[100];
	fgets(readline, 100, fd);
	while(1)
	{
		sleep(0.1);
		printf("%s", readline);
		send(new_socket, read_only_message, strlen(read_only_message), 0);
		read(new_socket, buffer, 1024);
		memset(buffer, 0, sizeof(buffer));
		send(new_socket, readline, strlen(readline), 0);
		fgets(readline, 100, fd);
		if(feof(fd))
		{
			break;
		}
	}
	fclose(fd);
}

// ===========================================================================
// thread function for client to modify about reservation
// ===========================================================================
void *modify_res(int new_socket) {
	printf("\n\nModify reservation enter\n\n");
	changeSeats(new_socket);
    printf("\n\nModify reservation end\n\n");
    pthread_exit(NULL);
}

// ===========================================================================
// thread function for client to cancel reservation
// ===========================================================================
void cancel_res(int new_socket) {
	char fileName[20];
	char *ticket_msg = "\nPlease enter ticket number: ";
	int n=client_req_and_recv_response(new_socket,ticket_msg,strlen(ticket_msg),
		fileName,sizeof(fileName));
	fileName[n] = 0;
	strcat(fileName, ".txt");
	FILE * fd=fopen(fileName,"rb");
	if(fd == NULL)
	{
        char *error_msg = "Unable to find receipt.\n";
        send(new_socket, error_msg, strlen(error_msg), 0);
		return;
    }
	CSeat * p;
	CSeat empty = {'\0', '\0', '\0', '\0'};
	char * pch;
	char readline[100], seatsFileName[15], * temp1, * temp2;
	int counter = 0;
	fgets(readline, 100, fd);
	fgets(readline, 100, fd);
	fgets(readline, 100, fd);
	strcpy(seatsFileName, readline);// Find seats filename
	seatsFileName[strlen(seatsFileName)-1]=0; // Remove "\n"
	strcat(seatsFileName, ".DAT");
	p = readData(seatsFileName, 10);
	fgets(readline, 100, fd);
	fgets(readline, 100, fd);
	readline[strlen(readline)-1]=0; // Remove "\n"
	pch = strtok(readline, ", ");
	while(pch != NULL)
	{
		counter++;
		p[(atoi)(pch)] = empty;
		pch = strtok(NULL, ", ");
	}
	char tempInt[10];
	counter = (atoi)(&(p->m_Name[0])) - counter;
	sprintf(tempInt,"%d",counter);
	strcpy(&(p->m_Name[0]), tempInt);
	writeDate(seatsFileName, p);
	free(p);
	fclose(fd);
	remove(fileName);
	char *error_msg = "Reservation has been cancelled\n";
    send(new_socket, error_msg, strlen(error_msg), 0);
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

        // Loop For Single Client
        while (keepRunning == 1)
        {
            // Send Default Message
            send(new_socket, conMessage, strlen(conMessage), 0);

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
					modify_res(new_socket);
                    send(new_socket, conMessage, strlen(conMessage), 0);
                    pthread_mutex_unlock(&lock);
                }
                // Cancel Reservations
                else if (strcmp(buffer, "4") == 0)
                {
                    printf("SELECTED 4...\n");
					cancel_res(new_socket);
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
