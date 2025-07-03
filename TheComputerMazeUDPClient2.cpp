// TheComputerMazeUDPClient.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "wsock32.lib")

#define STUDENT_NUMBER "23051901"
#define STUDENT_FIRSTNAME "sahifa"
#define STUDENT_FAMILYNAME "syed"

#define IP_ADDRESS_SERVER "127.0.0.1"
//#define IP_ADDRESS_SERVER "164.11.80.69"


#define PORT_SERVER 0x1984 // We define a port that we are going to use.
#define PORT_CLIENT 0x1985 // We define a port that we are going to use.

#define MAX_FILENAME_SIZE 500

#define MAX_BUFFER_SIZE   5000
#define MAX_STRING_SIZE   200
#define MAX_NO_TOKENS     50

#define MAX_ITEMS_IN_ROOM 20
#define MAX_ITEMS_IN_BACKPACK 50


enum directions
{
	DIRECTION_NORTH = 0,
	DIRECTION_SOUTH = 1,
	DIRECTION_EAST = 2,
	DIRECTION_WEST = 3,
	DIRECTION_UP = 4,
	DIRECTION_DOWN = 5
};


enum direction_possible
{
	DIRECTION_NOT_PRESENT = -1,
	DIRECTION_LOCKED = 0,
	DIRECTION_OPEN = 1
};


enum item_types
{
	ITEM_NONE = 0,
	ITEM_BOOK = 1,
	ITEM_JUNK = 2,
	ITEM_EQUIPMENT = 3,
	ITEM_MANUSCRIPT = 4,
	ITEM_TEST = 5,
	ITEM_OTHER = 10
};



struct Item
{
	int  number;
	int  value;
	int  volume;
};


struct Student
{
	int level;
	int rooms_visited;
	int doors_openned;
	int number_of_moves;
	int score;
};


struct Room
{
	char name[5];
	int  type;
	int  direction[6];
	int  number_of_keys;
	int  keys[4];
	int  number_of_items;
	Item items[MAX_ITEMS_IN_ROOM];
};


struct Backpack
{
	int number_of_items;
	Item items[MAX_ITEMS_IN_BACKPACK];
};


enum typeofroom
{
	ROOM_NONE = 0,
	ROOM_LECTURE_SMALL = 1,
	ROOM_LECTURE_MEDIUM = 2,
	ROOM_LECTURE_LARGE = 3,
	ROOM_CORRIDOR = 4,
	ROOM_LAB_SMALL = 5,
	ROOM_LAB_MEDIUM = 6,
	ROOM_LAB_LARGE = 7,
	ROOM_MEETING_ROOM = 8,
	ROOM_SEMINAR = 9,
	ROOM_HIVE = 10, //one per floor
	ROOM_COFFEESHOP = 11, //one
	ROOM_LIBRARY = 12, //only one
	ROOM_SHOP_SELL = 13,
	ROOM_SHOP_BUY = 14,
	ROOM_SHOP_BUYSELL = 15,
	ROOM_OFFICE = 16, //maybe only one door
	ROOM_LOBBY = 17, //Only one
	ROOM_EXIT = 18, //only one
	ROOM_STAIRS = 19,
	ROOM_ENTRANCE = 20 //only one
};


#define MAX_OPTIONS 50

int number_of_options;
int options[MAX_OPTIONS];


Student student;
Room room;
Backpack backpack;



SOCKADDR_IN server_addr;
SOCKADDR_IN client_addr;

SOCKET sock;  // This is our socket, it is the handle to the IO address to read/write packets

WSADATA data;




char InputBuffer[MAX_BUFFER_SIZE];

char Tokens[MAX_NO_TOKENS][MAX_STRING_SIZE];

char text_student[1000];
char text_backpack[1000];
char text_room[1000];
char text_keys[1000];
char text_items[1000];
char text_options[1000];



void sentOption(int option, int key)
{
	char buffer[100];

	sprintf(buffer, "Option %d, %x", option, key);
	sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR*)&server_addr, sizeof(SOCKADDR));
}



/********* Your tactics code starts here *********************/


#define OPTION_MOVE_NORTH 1
#define OPTION_MOVE_SOUTH 2
#define OPTION_MOVE_EAST 3
#define OPTION_MOVE_WEST 4
#define OPTION_MOVE_UP 5
#define OPTION_MOVE_DOWN 6

#define OPTION_UNLOCK_NORTH 7
#define OPTION_UNLOCK_SOUTH 8
#define OPTION_UNLOCK_EAST 9
#define OPTION_UNLOCK_WEST 10

#define OPTION_BASE_FOR_READS 200
#define OPTION_BASE_FOR_PICKUPS 500
#define OPTION_BASE_FOR_DROPS 800
#define OPTION_BASE_FOR_DOS 1100
#define OPTION_BASE_FOR_EVENTS 1300
#define NUMBER_OF_ITEMS 140

#define EVENT_NONE 0
#define EVENT_FIND_ITEM 1
#define EVENT_LOCKED_DOOR 2
#define EVENT_HAZARD 3
#define EVENT_HIDDEN_PASSAGE 4


int rooms_visited[5][10][10];


void initialise_rooms_visited()
{
	int floor;
	int ns;
	int ew;

	for (floor = 0; floor < 5; floor++)
	{
		for (ns = 0; ns < 10; ns++)
		{
			for (ew = 0; ew < 10; ew++)
			{
				rooms_visited[floor][ns][ew] = 0;
			}
		}
	}
}

void print_room_name() {

	switch (room.type) {
	case 0:
		printf(" You are in ROOM_NONE %s\n", room.name);
		break;
	case 1:
		printf(" You are in ROOM_LECTURE_SMALL %s\n", room.name);
		break;
	case 2:
		printf(" You are in ROOM_LECTURE_MEDIUM %s\n", room.name);
		break;
	case 3:
		printf(" You are in ROOM_LECTURE_LARGE %s\n", room.name);
		break;
	case 4:
		printf(" You are in ROOM_CORRIDOR %s\n", room.name);
		break;
	case 5:
		printf(" You are in ROOM_LAB_SMALL %s\n", room.name);
		break;
	case 6:
		printf(" You are in ROOM_LAB_MEDIUM %s\n", room.name);
		break;
	case 7:
		printf(" You are in ROOM_LAB_LARGE %s\n", room.name);
		break;
	case 8:
		printf(" You are in ROOM_MEETING_ROOM %s\n", room.name);
		break;
	case 9:
		printf(" You are in ROOM_SEMINAR %s\n", room.name);
		break;
	case 10:
		printf(" You are in ROOM_HIVE %s\n", room.name);
		break;
	case 11:
		printf(" You are in ROOM_COFFEESHOP  %s\n", room.name);
		break;
	case 12:
		printf(" You are in ROOM_LIBRARY  %s\n", room.name);
		break;
	case 13:
		printf(" You are in ROOM_SHOP_SELL %s\n", room.name);
		break;
	case 14:
		printf(" You are in ROOM_SHOP_BUY %s\n", room.name);
		break;
	case 15:
		printf(" You are in ROOM_SHOP_BUYSELL %s\n", room.name);
		break;
	case 16:
		printf(" You are in ROOM_OFFICE %s\n", room.name);
		break;
	case 17:
		printf(" You are in ROOM_LOBBY %s\n", room.name);
		break;
	case 18:
		printf(" You are in ROOM_EXIT %s\n", room.name);
		break;
	case 19:
		printf(" You are in ROOM_STAIRS %s\n", room.name);
		break;
	case 20:
		printf(" You are in ROOM_ENTRANCE %s\n", room.name);
		break;
	default:
		printf(" You are in the middle of nowhere %s\n", room.name);
		break;
	}
}

int rand_01() {
	return rand() % 5;
}

int best_direction = -1;
int last_direction = 0;

void bestDirection(int floor, int ns, int ew) {
	if ((room.direction[DIRECTION_NORTH] == DIRECTION_OPEN)
		&& (!rand_01() || last_direction != DIRECTION_SOUTH)
		&& (room.type == ROOM_CORRIDOR || rooms_visited[floor][ns - 1][ew] < 1)
		)
	{
		best_direction = OPTION_MOVE_NORTH;
		last_direction = DIRECTION_NORTH;
		printf("Your Move is OPTION_MOVE_NORTH\n");
	}
	else if ((room.direction[DIRECTION_SOUTH] == DIRECTION_OPEN)
		&& (!rand_01() || last_direction != DIRECTION_NORTH)
		&& (room.type == ROOM_CORRIDOR || rooms_visited[floor][ns + 1][ew] < 1)
		)
	{
		best_direction = OPTION_MOVE_SOUTH;
		last_direction = DIRECTION_SOUTH;
		printf("Your Move is OPTION_MOVE_SOUTH\n");
	}
	else if ((room.direction[DIRECTION_EAST] == DIRECTION_OPEN)
		&& (!rand_01() || last_direction != DIRECTION_WEST)
		&& (room.type == ROOM_CORRIDOR || rooms_visited[floor][ns][ew + 1] < 1)
		)
	{
		best_direction = OPTION_MOVE_EAST;
		last_direction = DIRECTION_EAST;
		printf("Your Move is OPTION_MOVE_EAST\n");
	}
	else if ((room.direction[DIRECTION_WEST] == DIRECTION_OPEN)
		&& (!rand_01() || last_direction != DIRECTION_EAST)
		&& (room.type == ROOM_CORRIDOR || rooms_visited[floor][ns][ew - 1] < 1)
		)
	{
		best_direction = OPTION_MOVE_WEST;
		last_direction = DIRECTION_WEST;
		printf("Your Move is OPTION_MOVE_WEST\n");
	}
	else if ((room.direction[DIRECTION_UP] == DIRECTION_OPEN)
		&& (!rand_01() || last_direction != DIRECTION_DOWN)
		&& (room.type == ROOM_CORRIDOR || rooms_visited[floor + 1][ns][ew] < 1)
		)
	{
		best_direction = OPTION_MOVE_UP;
		last_direction = DIRECTION_UP;
		printf("Your Move is OPTION_MOVE_UP\n");
	}
	else if ((room.direction[DIRECTION_DOWN] == DIRECTION_OPEN)
		&& (!rand_01() || last_direction != DIRECTION_UP)
		&& (room.type == ROOM_CORRIDOR || rooms_visited[floor - 1][ns][ew] < 1)
		)
	{
		best_direction = OPTION_MOVE_DOWN;
		last_direction = DIRECTION_DOWN;
		printf("Your Move is OPTION_MOVE_DOWN\n");
	}
	else
	{
		int rand_direction = 1 + rand() % 6;
		best_direction = rand_direction;
		last_direction = rand_direction - 1;
		printf("Your Move is rand_direction = %d default\n", rand_direction);
	}
}




//

int number_of_move = 0;

#define KNOWN_KEYS 100

int saved_keys[KNOWN_KEYS] = { 0x200F, 0xBCD5, 0xE4A8, 0x71DA,
							  0x29FC, 0xA100, 0xBE27, 0x9F4A,
							  0x8E28, 0xD75B, 0x090D, 0x0172,
							  0xD2F7, 0xC567, 0x8FD4, 0xFD5A,
							  0x711A, 0x8E28, 0x8F32, 0x1AE1,
							  0x1AE1, 0x0EB9, 0x43AB, 0xD2E7,
							  0xC567, 0x8FD4, 0xFD5A, 0x395D,
							  0x3189, 0xA9BC, 0x6F0,  0x88DE,
							  0x421D, 0x4125, 0x19F4, 0x80FA,
							  0x1489, 0x56B5, 0x69D4, 0x27F0,

};
int number_of_saved_keys = 11;





int doors_tried[5][10][10][4];

void initDoorsTried()
{
	int floor;
	int ns;
	int ew;
	int door;

	for (floor = 0; floor < 5; floor++)
	{
		for (ns = 0; ns < 10; ns++)
		{
			for (ew = 0; ew < 10; ew++)
			{
				for (door = 0; door < 4; door++)
				{
					doors_tried[floor][ns][ew][door] = -1;
				}
			}
		}
	}
}


struct Open_door_option {
	int option;
	int key;
};

Open_door_option unlockDoor()
{
	Open_door_option best_option_open_door = { 0, 0 };
	int key_index;

	int floor;
	int ns;
	int ew;

	floor = room.name[0] - '0';
	ns = room.name[2] - '0';
	ew = room.name[3] - '0';

	for (int door = 0; door < 4; door++)
	{
		if (room.direction[door] == DIRECTION_LOCKED)
		{
			doors_tried[floor][ns][ew][door]++;
			key_index = doors_tried[floor][ns][ew][door];
			if (key_index < number_of_saved_keys)
			{
				best_option_open_door.option = OPTION_UNLOCK_NORTH + door;
				best_option_open_door.key = saved_keys[key_index];
				return best_option_open_door;
			}
		}
	}
	return best_option_open_door;
}

bool key_already_saved(int new_key) {
	for (int j = 0; j < number_of_saved_keys; j++) {
		if (saved_keys[j] == new_key) {
			return true;
		}
	}
	return false;
}

int generateRandomEvent() {
	int event = rand() % 5; // Generate a random number between 0 and 4

	switch (event) {
	case 0:
		return EVENT_NONE;
	case 1:
		return EVENT_FIND_ITEM;
	case 2:
		return EVENT_LOCKED_DOOR;
	case 3:
		return EVENT_HAZARD;
	case 4:
		return EVENT_HIDDEN_PASSAGE;
	default:
		return EVENT_NONE;
	}
}




void yourMove()
{
	int floor;
	int ns;
	int ew;

	int best_option = 0;
	int key = 0;
	Open_door_option best_option_open_door = { 0, 0 };

	number_of_move++;
	printf("number_of_move = %d\n", number_of_move);
	if (number_of_move > 2001)
		exit(0);

	// Record where we have been...

	floor = room.name[0] - '0';
	ns = room.name[2] - '0';
	ew = room.name[3] - '0';

	rooms_visited[floor][ns][ew]++;

	print_room_name();

	// Collect keys...
	if (room.number_of_keys > 0)
	{
		for (int i = 0; i < room.number_of_keys; i++)
		{
			if (!key_already_saved(room.keys[i]))
			{
				saved_keys[number_of_saved_keys] = room.keys[i];
				number_of_saved_keys++;
			}
		}
	}

	printf("number_of_saved_keys =  %d\n", number_of_saved_keys);

	// pick up item...
	if (room.number_of_items > 0)
	{
		int highest_value_item_index = -1;
		int highest_value = 0;
		for (int i = 0; i < room.number_of_items; i++)
		{
			if (room.items[i].value > highest_value)
			{
				highest_value = room.items[i].value;
				highest_value_item_index = i;
			}
		}
		if (highest_value_item_index != -1)
		{
			printf("Move = %d\n", OPTION_BASE_FOR_PICKUPS + room.items[highest_value_item_index].number);
			best_option = OPTION_BASE_FOR_PICKUPS + room.items[highest_value_item_index].number;
		}
	}
	// Read Books 
	for (int i = 0; i < number_of_options; i++) {
		if ((options[i] >= OPTION_BASE_FOR_READS)
			&& (options[i] < (OPTION_BASE_FOR_READS + NUMBER_OF_ITEMS)))
		{
			//	printf("Read a book");
			best_option = options[i];
			break;
		}
	}

	// Take Test
	for (int i = 0; i < number_of_options; i++) {
		if ((options[i] >= OPTION_BASE_FOR_DOS)
			&& (options[i] < OPTION_BASE_FOR_DOS + NUMBER_OF_ITEMS))
		{
			//	printf("Do test ");
			best_option = options[i];
			break;
		}
	}

	// Take part in Events
	for (int i = 0; i < number_of_options; i++) {
		if ((options[i] >= OPTION_BASE_FOR_EVENTS)
			&& (options[i] < OPTION_BASE_FOR_EVENTS + NUMBER_OF_ITEMS))
		{
			//	printf("Do Events ");
			best_option = options[i];
			break;
		}
	}


	if (best_option == 0)
	{

		best_option_open_door = unlockDoor();
		best_option = best_option_open_door.option;
		key = best_option_open_door.key;
		if (best_option == 0)
		{

			// Find best direction...
			bestDirection(floor, ns, ew);
			best_option = best_direction;
		}
		else
		{
			//	printf("Your Move is Unlock Door\n");
		}
	}
	// Take action...
	sentOption(best_option, key);
}


//




/********* Your tactics code ends here ***********************/




int getTokens(char* instring, char seperator)
{
	int  number_of_tokens;
	char chr;
	int  state;
	int  i;
	int  j;


	for (i = 0; i < MAX_NO_TOKENS; i++)
	{
		for (j = 0; j < MAX_STRING_SIZE; j++)
		{
			Tokens[i][j] = '\0';
		}
	}

	number_of_tokens = -1;
	chr = instring[0];
	state = 0;
	i = 0;

	while (chr != '\0')
	{
		switch (state)
		{
		case 0:  // Initial state
			if (chr == seperator)
			{
				number_of_tokens++;
				state = 1;
			}
			else if ((chr == ' ') || (chr == '\t') || (chr == '\n'))
			{
				state = 1;
			}
			else
			{
				number_of_tokens++;
				j = 0;
				Tokens[number_of_tokens][j] = chr;
				Tokens[number_of_tokens][j + 1] = '\0';
				state = 2;
			}
			break;

		case 1:  // Leading white space
			if (chr == seperator)
			{
				number_of_tokens++;
				state = 1;
			}
			else if ((chr == ' ') || (chr == '\t') || (chr == '\n'))
			{
				state = 1;
			}
			else
			{
				number_of_tokens++;
				j = 0;
				Tokens[number_of_tokens][j] = chr;
				Tokens[number_of_tokens][j + 1] = '\0';
				state = 2;
			}
			break;

		case 2:  // Collecting chars
			if (chr == seperator)
			{
				//number_of_tokens++;
				state = 1;
			}
			else
			{
				j++;
				Tokens[number_of_tokens][j] = chr;
				Tokens[number_of_tokens][j + 1] = '\0';
				state = 2;
			}
			break;

		default:
			break;
		}

		i++;
		chr = instring[i];
	}

	return (number_of_tokens + 1);
}



bool getline(FILE* fp, char* buffer)
{
	bool rc;
	bool collect;
	char c;
	int  i;

	rc = false;
	collect = true;

	i = 0;
	while (collect)
	{
		c = getc(fp);

		switch (c)
		{
		case EOF:
			if (i > 0)
			{
				rc = true;
			}
			collect = false;
			break;

		case '\n':
			if (i > 0)
			{
				rc = true;
				collect = false;
				buffer[i] = '\0';
			}
			break;

		default:
			buffer[i] = c;
			i++;
			break;
		}
	}

	return (rc);
}



void printRoom()
{
	int i;

	printf("Room\n");
	printf("Room = %s, Room type = %d\n", room.name, room.type);

	printf("Directions = ");
	for (i = 0; i < 6; i++)
	{
		printf("%d  ", room.direction[i]);
	}
	printf("\n");

	if (room.number_of_keys > 0)
	{
		printf("Keys = ");
		for (i = 0; i < room.number_of_keys; i++)
		{
			printf("0x%X  ", room.keys[i]);
		}
		printf("\n");
	}
	else
	{
		printf("No keys in this room\n");
	}

	if (room.number_of_items > 0)
	{
		for (i = 0; i < room.number_of_items; i++)
		{
			printf("Item=%d, Value=%d, Volume=%d\n", room.items[i].number, room.items[i].value, room.items[i].volume);
		}
	}
	else
	{
		printf("No items in this room\n");
	}

	printf("\n");
}


void printStudent()
{
	printf("Student\n");
	printf("Level=%d,  Number of rooms visited = %d,  Number of doors openned = %d,  Number of moves = %d,  Score = %d\n", student.level, student.rooms_visited, student.doors_openned, student.number_of_moves, student.score);
	printf("\n");
}


void printBackpack()
{
	int i;

	printf("Backpack\n");

	if (backpack.number_of_items > 0)
	{
		for (i = 0; i < backpack.number_of_items; i++)
		{
			printf("Item=%d, Value=%d, Volume=%d\n", backpack.items[i].number, backpack.items[i].value, backpack.items[i].volume);
		}
	}
	else
	{
		printf("Your backpack is empty\n");
	}
	printf("\n");
}


void printOptions()
{
	int i;

	printf("Options\n");
	printf("Options = ");
	for (i = 0; i < number_of_options; i++)
	{
		printf("%d  ", options[i]);
	}
	printf("\n");
	printf("\n");
}




void communicate_with_server()
{
	char buffer[4096];
	int  len = sizeof(SOCKADDR);
	int  i;
	char* p;
	int number_of_tokens;


	sprintf_s(buffer, "Register  %s %s %s", STUDENT_NUMBER, STUDENT_FIRSTNAME, STUDENT_FAMILYNAME);
	sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR*)&server_addr, sizeof(SOCKADDR));

	while (true)
	{
		memset(buffer, '\0', sizeof(buffer));

		if (recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (SOCKADDR*)&client_addr, &len) != SOCKET_ERROR)
		{
			p = ::inet_ntoa(client_addr.sin_addr);

			if ((strcmp(IP_ADDRESS_SERVER, "127.0.0.1") == 0) || (strcmp(IP_ADDRESS_SERVER, p) == 0))
			{
				printf("%s\n\n", buffer);

				number_of_tokens = getTokens(buffer, '|');

				if (number_of_tokens == 6)
				{
					strcpy(text_student, Tokens[0]);
					strcpy(text_backpack, Tokens[1]);
					strcpy(text_room, Tokens[2]);
					strcpy(text_keys, Tokens[3]);
					strcpy(text_items, Tokens[4]);
					strcpy(text_options, Tokens[5]);

					printf("Student  = '%s'\n", text_student);
					printf("Backpack = '%s'\n", text_backpack);
					printf("Room     = '%s'\n", text_room);
					printf("Keys     = '%s'\n", text_keys);
					printf("Items    = '%s'\n", text_items);
					printf("Options  = '%s'\n", text_options);

					student.level = -1;
					student.rooms_visited = -1;
					student.doors_openned = -1;
					student.number_of_moves = -1;

					if (sscanf(text_student, "%d,%d,%d,%d,%d", &student.level, &student.rooms_visited, &student.doors_openned, &student.number_of_moves, &student.score) == 5)
					{
					}

					if (strlen(text_backpack) > 0)
					{
						backpack.number_of_items = getTokens(text_backpack, '&');

						if (backpack.number_of_items > 0)
						{
							for (i = 0; i < backpack.number_of_items; i++)
							{
								if (i < MAX_ITEMS_IN_BACKPACK)
								{
									backpack.items[i].number = -1;

									if (sscanf(Tokens[i], "%d, %d, %d", &backpack.items[i].number, &backpack.items[i].value, &backpack.items[i].volume) == 3)
									{
									}
								}
							}
						}
					}
					else
					{
						backpack.number_of_items = 0;
					}

					sscanf(text_room, "%s ,%d, %d, %d, %d, %d, %d, %d", &room.name, &room.type, &room.direction[DIRECTION_NORTH], &room.direction[DIRECTION_SOUTH], &room.direction[DIRECTION_EAST], &room.direction[DIRECTION_WEST], &room.direction[DIRECTION_UP], &room.direction[DIRECTION_DOWN]);

					if (strlen(text_keys) > 0)
					{
						room.number_of_keys = getTokens(text_keys, '&');

						if (room.number_of_keys > 0)
						{
							for (i = 0; i < room.number_of_keys; i++)
							{
								if (i < 4)
								{
									room.keys[i] = -1;

									if (sscanf(Tokens[i], "%x", &room.keys[i]) == 1)
									{
									}
								}
							}
						}
					}
					else
					{
						room.number_of_keys = 0;
					}

					if (strlen(text_items) > 0)
					{
						room.number_of_items = getTokens(text_items, '&');

						if (room.number_of_items > 0)
						{
							for (i = 0; i < room.number_of_items; i++)
							{
								if (i < MAX_ITEMS_IN_ROOM)
								{
									room.items[i].number = -1;

									if (sscanf(Tokens[i], "%d, %d, %d", &room.items[i].number, &room.items[i].value, &room.items[i].volume) == 3)
									{
									}
								}
							}
						}
					}
					else
					{
						room.number_of_items = 0;
					}

					if (strlen(text_options) > 0)
					{
						number_of_options = getTokens(text_options, ',');

						if (number_of_options > 0)
						{
							for (i = 0; i < number_of_options; i++)
							{
								if (i < MAX_OPTIONS)
								{
									options[i] = -1;

									if (sscanf(Tokens[i], "%d", &options[i]) == 1)
									{
									}
								}
							}
						}
					}
					else
					{
						number_of_options = 0;
					}
				}

				printStudent();
				printBackpack();
				printRoom();
				printOptions();

				// system("timeout /t 60");

				yourMove();
			}
		}
		else
		{
			printf_s("recvfrom error = %d\n", WSAGetLastError());
		}
	}

	printf_s("Student %s\n", STUDENT_NUMBER);
}




int main()
{
	char chr = '\0';

	printf("\n");
	printf("The Computer Maze Student Program\n");
	printf("UWE Computer and Network Systems Assignment 2 \n");
	printf("\n");

	initialise_rooms_visited();

	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) return(0);

	//sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	//if (!sock)
	//{
	// printf("Socket creation failed!\n");
	//}

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	if (!sock)
	{
		// Creation failed!
	}

	memset(&server_addr, 0, sizeof(SOCKADDR_IN));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER);
	server_addr.sin_port = htons(PORT_SERVER);

	memset(&client_addr, 0, sizeof(SOCKADDR_IN));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	client_addr.sin_port = htons(PORT_CLIENT);

	//int ret = bind(sock_recv, (SOCKADDR *)&receive_addr, sizeof(SOCKADDR));
	//// int ret = bind(sock_send, (SOCKADDR *)&receive_addr, sizeof(SOCKADDR));
	//if (ret)
	//{
	// printf("Bind failed! %d\n", WSAGetLastError());
	//}

	communicate_with_server();

	closesocket(sock);
	WSACleanup();

	while (chr != '\n')
	{
		chr = getchar();
	}

	return 0;
}