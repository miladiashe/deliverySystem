#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *content : package contents (message string)
  ㄴ오타인거같아서 바꿔드렸습니다*/ 
typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	//char *content;
	//안돼서 구조체 변수 새로 만들었어요 
	char content[MAX_MSG_SIZE+1];
} storage_t;
//storage_t 라는 타입으로 위의 구조체를 불러올 수 있게 한다. 


static storage_t** deliverySystem; 			//deliverySystem
//malloc함수를 쓰기 위해 이차원 배열 구조를 갖는 구조체로 정의한 것 같다 
static int storedCnt = 0;					//number of cells occupied
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN+1];	//master password




// ------- inner functions ---------------

//print the inside content of a specific cell
//int x, int y : cell to print the content
static void printStorageInside(int x, int y) {
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].content);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y, FILE *p) {
	int temp;
	int i;
	int checkEOF = 0;
	int scanX;
	int scanY;
	//deliverySystem[scanX][scanY] 번 메모리에 접근하기 위함
	int scanNUM;
	char scanPWD[PASSWD_LEN+1];
	char scanMSG[MAX_MSG_SIZE+1]; 
	//파일의 끝인지 체크하는 변수 하나 만들었다. 
	
	deliverySystem = (storage_t**)malloc(x*sizeof(storage_t*));
	//systemSize[0]'층' 만큼을 만든다. 한 층에 systemSize[1]개의 보관함 존재 
	for (temp=0; temp<x; temp++)
	{
		deliverySystem[temp] = (storage_t*)malloc(y*sizeof(storage_t));
	}
	//각 '층'에 보관함 만들기
	
	//일단 모든 보관함 초기화하기
	for (temp=0; temp<x; temp++)
	{
		for(i=0; i<y; i++)
		{
			deliverySystem[temp][i].building = 0;
			deliverySystem[temp][i].room = 0;
			deliverySystem[temp][i].cnt = 0;
			strcpy (deliverySystem[temp][i].passwd , masterPassword);
			strcpy (deliverySystem[temp][i].content , "\0");
		}
	}
	
	/*파일의 끝까지 반복문 만들어서
	while(scan != EOF) 하면 되나?
	while(1) 하고 break 쓸까?
	첫번째= 층 두번째= 칸 
	동 ->building 호 ->room 비밀번호 문자열 순으로 읽어서 동적 메모리에 넣어주기 
	*/
	while (checkEOF != 1)
	//checkEOF가 1이 아닌 동안만 계속 돌기. EOF인 경우 1로 만들고 루프를 끝내기. 
	{
		//일단 다른 변수도 저장용 변수를 만들자  
		//문자열은 strcpy 쓰기? 난 내 프로그램이 잘 작동될걸 믿으니까 한줄 단위로 처리할것
		if (ftell(p) == SEEK_END)
		{
			checkEOF = 1;
		}
		fscanf(p, "%d", &scanX);
		
		fscanf(p, "%d", &scanY);
		
		fscanf(p, "%d", &scanNUM);
		deliverySystem[scanX][scanY].building = scanNUM;
		//동 읽기
		fscanf(p, "%d", &scanNUM);
		deliverySystem[scanX][scanY].room = scanNUM;
		//호 읽기
		fscanf(p, "%s", scanPWD);
		strcpy (deliverySystem[scanX][scanY].passwd, scanPWD);
		//비밀번호 읽기
		fscanf(p, "%s", scanMSG);
		strcpy (deliverySystem[scanX][scanY].content, scanMSG);
		//내용물 읽기
		deliverySystem[scanX][scanY].cnt = 1;
		//cnt에 값 저 장 
	}
	
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) {
	
}





// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) {
	//바꾼 상태를 저장. 
	FILE *fp;
	fp = fopen(filepath, "w");
	//우선 쓰기 모드로 연다
	//raw colomn \n 마스터패스워드 순으로 출력 
	fprintf(fp, "%i %i\n%s", systemSize[0], systemSize[1], masterPassword);
	//현재 보관함 상태 출력(해야할것) 
	fclose(fp);
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {
	int temp;
	/*값을 잠시 여기다 저장해두자...*/ 
	
	/*일단 파일부터 열자. 닫는 건 메인에 하나?
	파일포인터가 어디까지 유지돌지 모르니 일단 매번 여닫는 게 안전할 것 같다.*/ 
	FILE *fp;
	fp = fopen(filepath, "r");
	if (fp == NULL)
	{
		return -1;
	}
	//파일이 안 열렸을 경우 못열었다고 리턴함 
	fscanf(fp, "%d", &temp);
	systemSize[0] = temp; 
	//첫번째 숫자는 raw를 나타내는 systemsize[0]에 입력 
	
	fscanf(fp, "%d", &temp);
	systemSize[1] = temp; 
	//두번째 숫자를 column 나타내는 systemsize[1]에 입력
	fscanf(fp, "%s", masterPassword);
	//마스터 패스워드를 입력? 
	
	initStorage(systemSize[0], systemSize[1], fp);
	//보관함 칸 수 만큼 동적 메모리 할당 
	
	fclose(fp);
	return 0;
}

//free the memory of the deliverySystem 
//deliverySystem 에 줬던 메모리 되돌려놓기
void str_freeSystem(void) {
	int temp;
	for (temp=0; temp<systemSize[0]; temp++)
	{
		free(deliverySystem[temp]);
	}
	free(deliverySystem);
}



//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
	int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0;j<systemSize[1];j++)
	{
		printf(" %i\t\t",j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0;i<systemSize[0];i++)
	{
		printf("%i|\t",i);
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}



//check if the input cell (x,y) is valid and whether it is occupied or not
//systemsize에 크기를 정의? 
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;
	//deliverySystem이라는 구조체의 cnt 값을 반환.  
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1]) {
	
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {
	int cnt;
	//컴파일하려고 임시로 집어넣음;; 
	return cnt;
}
