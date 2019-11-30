#include <stdio.h>
#include <stdlib.h>
#include "storage.h"

#define N_BUILDING				10
#define N_FLOOR					10
#define N_ROOM					10

#define STORAGE_FILEPATH 		"storage.txt"
//아예 설정 파일의 루트를 상수로 고정해둔다. 

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
	
//코드를 이해하기 위해 주석을 사용해서 설명을 적겠습니다. 

//get an integer value from keyboard
int getIntegerInput(void)
{
	int input = -1;
	scanf("%d", &input);
	fflush(stdin);
	
	return input;
}



//check whether building/room number is valid or not
int buildingValidityCheck(int nBuilding, int nRoom)
{
	if (nBuilding <= 0 || nBuilding > N_BUILDING)
	//빌딩번호가 0 이하거나, 정해둔 빌딩 수보다 클 때 불가능한 번호라고 판단. 
	{
		return -1;
	}
	
	if (nRoom/100 <= 0 || nRoom/100 > N_FLOOR)
	//방 호수가 0 이하거나, 정해진 층수보다 높은 층(백의 자리)을 입력한 경우 
	{
		return -1;
	}
	
	if (nRoom%100 <= 0 || nRoom%100 > N_ROOM)
	//방 호수가 음수이거나 한 층의 호수 정해진 것보다 클때 
	{
		return -1;
	}
	
	return 0;
}
//즉 방의 번호는 101호부터 1010호까지 한 층에 10호씩 존재한다. 


int main(int argc, char *argv[]) {


	int cmd;
	//사용자의 입력을 받는 변수 
	int x, y, nBuilding, nRoom;
	char msg[MAX_MSG_SIZE+1];
	char passwd[PASSWD_LEN+1];
	
	//1. initialize the delivery system
	if (str_createSystem(STORAGE_FILEPATH) != 0)
	{
		printf("failed to open storage config file! (%s)\n", STORAGE_FILEPATH);
		return -1;
	}
	
	printf("------- Unmanned Delivery Storage System START -------\n");
	
	do
	{
        //2. menu printing
		printf("\n\n");
		printf("----------------------------------------------\n");
		printf("1. put a package\n");
		printf("2. get my package\n");
		printf("3. check the storage status\n");
		printf("4. find my package\n");
		printf("0. exit\n");
		printf("----------------------------------------------\n");
		printf("command : ");
		
        //3. getting user command
		cmd = getIntegerInput();	
		
		switch(cmd)
		{
            //4-1. command analysis : exit the program
			case 0:
				printf(" -----------> exiting the program....\n");
				break;
			
            //4-2. command analysis : put package
			case 1:
                //provide the current status
				str_printStorageStatus();
				
                //storage cell selection
				printf(" - storage row : ");
				x = getIntegerInput();
				printf(" - storage column : ");
				y = getIntegerInput();
				if (str_checkStorage(x,y) != 0)
				{
					printf(" -----------> Storage (%i,%i) is already occupied or invalid!\n", x, y);
					continue;
				}
				
                //destination input
				printf(" - building # : ");
				nBuilding = getIntegerInput();
				printf(" - room # : ");
				nRoom = getIntegerInput();
				if (buildingValidityCheck(nBuilding, nRoom) != 0)
				{
					printf(" -----------> Invalid building/room number (%i,%i)\n", nBuilding, nRoom);
					continue;
				}
				
                //package context input (message string)
				printf(" - message (max : 100 characters) : ");
				scanf("%100s", msg);
				fflush(stdin);
				
                //password input
				passwd[0] = 0;
				printf(" - password : ");
				scanf("%4s", passwd);
				fflush(stdin);
				
                //put the package in the cell
				printf(" -----------> processing to put the package...\n\n");
				if (str_pushToStorage(x, y, nBuilding, nRoom, msg, passwd) == 0)
				{
					printf(" -----------> Succeeded to put package for (%i, %i) user in (%i,%i), msg : %s passwd : %s\n", nBuilding, nRoom, x, y, msg, passwd);
				}
				else
				{
					printf(" -----------> Failed to put the package in the storage!\n");
				}
				break;
                
			//4-3. command analysis : extract my package
			case 2:
                //input the storage cell
				printf(" - storage row : ");
				x = getIntegerInput();
				printf(" - storage column : ");
				y = getIntegerInput();
				
                //check the input storage cell
				if (str_checkStorage(x,y) <= 0)
				{
					printf(" -----------> Storage (%i,%i) is empty or invalid!\n", x, y);
					continue;
				}
                //try to extract the cell
				if (str_extractStorage(x,y) != 0)
				{
					printf(" -----------> Failed to extract my package!\n");
				}
				break;
				
            //4-4. command analysis : print the storage status
			case 3:
				str_printStorageStatus();
				break;
                
			//4-4. command analysis : find my package from the storage
			case 4:
                //input the destination (my address)
				printf(" - building # : ");
				nBuilding = getIntegerInput();
				printf(" - room # : ");
				nRoom = getIntegerInput();
                
				//check whether the address is valid or not
				if (buildingValidityCheck(nBuilding, nRoom) != 0)
				{
					printf(" -----------> Invalid building/room number (%i,%i)\n", nBuilding, nRoom);
					continue;
				}
				
                //try to find packages destinated to the address
				if (str_findStorage(nBuilding, nRoom) == 0)
				{
					printf(" -----------> Failed to find my package!\n");
				}
				
				break;
				
			default:
				printf(" -----------> Wrong cmd!! input again!..\n");
				break;
		}
		
        //backup the status of the storage
		if (str_backupSystem(STORAGE_FILEPATH) != 0)
		{
			printf("[WARNING] failed to backup the delivery system!\n");
		}
	} while (cmd != 0);
	
	str_freeSystem();
	
	return 0;
}
