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
  ����Ÿ�ΰŰ��Ƽ� �ٲ��Ƚ��ϴ�*/ 
typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	//char *content;
	//�ȵż� ����ü ���� ���� �������� 
	char content[MAX_MSG_SIZE+1];
} storage_t;
//storage_t ��� Ÿ������ ���� ����ü�� �ҷ��� �� �ְ� �Ѵ�. 


static storage_t** deliverySystem; 			//deliverySystem
//malloc�Լ��� ���� ���� ������ �迭 ������ ���� ����ü�� ������ �� ���� 
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
	//deliverySystem[scanX][scanY] �� �޸𸮿� �����ϱ� ����
	int scanNUM;
	char scanPWD[PASSWD_LEN+1];
	char scanMSG[MAX_MSG_SIZE+1]; 
	//������ ������ üũ�ϴ� ���� �ϳ� �������. 
	
	deliverySystem = (storage_t**)malloc(x*sizeof(storage_t*));
	//systemSize[0]'��' ��ŭ�� �����. �� ���� systemSize[1]���� ������ ���� 
	for (temp=0; temp<x; temp++)
	{
		deliverySystem[temp] = (storage_t*)malloc(y*sizeof(storage_t));
	}
	//�� '��'�� ������ �����
	
	//�ϴ� ��� ������ �ʱ�ȭ�ϱ�
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
	
	/*������ ������ �ݺ��� ����
	while(scan != EOF) �ϸ� �ǳ�?
	while(1) �ϰ� break ����?
	ù��°= �� �ι�°= ĭ 
	�� ->building ȣ ->room ��й�ȣ ���ڿ� ������ �о ���� �޸𸮿� �־��ֱ� 
	*/
	while (checkEOF != 1)
	//checkEOF�� 1�� �ƴ� ���ȸ� ��� ����. EOF�� ��� 1�� ����� ������ ������. 
	{
		//�ϴ� �ٸ� ������ ����� ������ ������  
		//���ڿ��� strcpy ����? �� �� ���α׷��� �� �۵��ɰ� �����ϱ� ���� ������ ó���Ұ�
		if (ftell(p) == SEEK_END)
		{
			checkEOF = 1;
		}
		fscanf(p, "%d", &scanX);
		
		fscanf(p, "%d", &scanY);
		
		fscanf(p, "%d", &scanNUM);
		deliverySystem[scanX][scanY].building = scanNUM;
		//�� �б�
		fscanf(p, "%d", &scanNUM);
		deliverySystem[scanX][scanY].room = scanNUM;
		//ȣ �б�
		fscanf(p, "%s", scanPWD);
		strcpy (deliverySystem[scanX][scanY].passwd, scanPWD);
		//��й�ȣ �б�
		fscanf(p, "%s", scanMSG);
		strcpy (deliverySystem[scanX][scanY].content, scanMSG);
		//���빰 �б�
		deliverySystem[scanX][scanY].cnt = 1;
		//cnt�� �� �� �� 
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
	//�ٲ� ���¸� ����. 
	FILE *fp;
	fp = fopen(filepath, "w");
	//�켱 ���� ���� ����
	//raw colomn \n �������н����� ������ ��� 
	fprintf(fp, "%i %i\n%s", systemSize[0], systemSize[1], masterPassword);
	//���� ������ ���� ���(�ؾ��Ұ�) 
	fclose(fp);
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {
	int temp;
	/*���� ��� ����� �����ص���...*/ 
	
	/*�ϴ� ���Ϻ��� ����. �ݴ� �� ���ο� �ϳ�?
	���������Ͱ� ������ �������� �𸣴� �ϴ� �Ź� ���ݴ� �� ������ �� ����.*/ 
	FILE *fp;
	fp = fopen(filepath, "r");
	if (fp == NULL)
	{
		return -1;
	}
	//������ �� ������ ��� �������ٰ� ������ 
	fscanf(fp, "%d", &temp);
	systemSize[0] = temp; 
	//ù��° ���ڴ� raw�� ��Ÿ���� systemsize[0]�� �Է� 
	
	fscanf(fp, "%d", &temp);
	systemSize[1] = temp; 
	//�ι�° ���ڸ� column ��Ÿ���� systemsize[1]�� �Է�
	fscanf(fp, "%s", masterPassword);
	//������ �н����带 �Է�? 
	
	initStorage(systemSize[0], systemSize[1], fp);
	//������ ĭ �� ��ŭ ���� �޸� �Ҵ� 
	
	fclose(fp);
	return 0;
}

//free the memory of the deliverySystem 
//deliverySystem �� ��� �޸� �ǵ�������
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
//systemsize�� ũ�⸦ ����? 
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
	//deliverySystem�̶�� ����ü�� cnt ���� ��ȯ.  
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
	//�������Ϸ��� �ӽ÷� �������;; 
	return cnt;
}
