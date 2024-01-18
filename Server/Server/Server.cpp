// Server.cpp : create a console application, and include the sources in the project
//
// 1. open the *.c in the Visual C++, then "rebuild all".
// 2. click "yes" to create a project workspace.
// 3. You need to -add the library 'ws2_32.lib' to your project 
//    (Project -> Properties -> Linker -> Input -> Additional Dependencies) 
// 4. recompile the source.

#include "stdafx.h"
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include<time.h>
#define MAX 100

#define DEFAULT_PORT	5019
int num=0;


void rec(void *a);
void change_user(SOCKET tmsg_sock);
void change_table(SOCKET tmsg_sock);
void delete_user(SOCKET tmsg_sock);
void add_user(SOCKET tmsg_sock);
void record(char* ip);
void search(int n);
void classinf(SOCKET tmsg_sock);
void guest(SOCKET tmsg_sock);
void read();

typedef struct{
    char code[20];
    char title[50];
    char section[10];
    char time[20];
    char classroom[10];
    char instructor[20];
}classinfo;

classinfo c[MAX];

int msg_len;
int addr_len;
struct sockaddr_in local, client_addr;

SOCKET sock, msg_sock;
WSADATA wsaData;

int main(int argc, char **argv){
	read();
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR){
		// stderr: standard error are printed to the screen.
		fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
		//WSACleanup function terminates use of the Windows Sockets DLL. 
		WSACleanup();
		return -1;
	}
	// Fill in the address structure
	local.sin_family		= AF_INET;
	local.sin_addr.s_addr	= INADDR_ANY;
	local.sin_port		= htons(DEFAULT_PORT);

	sock = socket(AF_INET,SOCK_STREAM, 0);	//TCp socket


	if (sock == INVALID_SOCKET){
		fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	if (bind(sock, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR){
		fprintf(stderr, "bind() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	HANDLE hThread[3];
	for(int i=0;i<3;i++){
	
		//waiting for the connections
		if (listen(sock, 3) == SOCKET_ERROR){
			fprintf(stderr, "listen() failed with error %d\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}

	
		printf("Waiting for connections ........\n");

		addr_len = sizeof(client_addr);
		msg_sock = accept(sock, (struct sockaddr*)&client_addr, &addr_len);
		if (msg_sock == INVALID_SOCKET){
			fprintf(stderr, "accept() failed with error %d\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}

		printf("accepted connection from %s, port %d\n",
			inet_ntoa(client_addr.sin_addr),
			htons(client_addr.sin_port));
		record(inet_ntoa(client_addr.sin_addr));
		hThread[i] = (HANDLE)_beginthread(rec,0,NULL);
		
	}
	WaitForSingleObject( hThread[0], INFINITE );
	WaitForSingleObject( hThread[1], INFINITE );
	WaitForSingleObject( hThread[2], INFINITE );
	//closesocket(msg_sock);
	WSACleanup();
}
		
void rec(void *a){
	SOCKET tmsg_sock=msg_sock;
	char sendBuf[100];
	char szBuff[100];
	char numb[10];
	//printf("%d",num);
	//sprintf(numb,"%d",num);
	itoa(num,numb,10);
	//strcpy(numb,itoa(num));
	printf("%s",numb);
	send(tmsg_sock,numb,strlen(numb)+1,0);
	strcpy(sendBuf,"enter you ID");
	send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
	msg_len = recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
	char s[] = "admin";
	char name[20],key[20];
	int flag=0;
	strcpy(name,szBuff);
	int length = strlen(name);
    name[length] = '\n';
	name[length+1]='\0';
	FILE *fp;
	fp = fopen("user.txt", "rt+");
	while(!feof(fp)){
        fgets(key, sizeof(key), fp);
        if(strcmp(key, name)==0){
            flag=1;
        }
    }
	if(flag==1){
		send(tmsg_sock,"1",strlen("1")+1,0);
		//printf("you are uesr");
	}
	else{
		send(tmsg_sock,"0",strlen("0")+1,0);
		//printf("aaa");
	}
	fclose(fp);
    if(flag==0&&strcmp(szBuff,"admin")==0){
        while(strcmp(szBuff,"3")!=0){
			//strcpy(szBuff,"");
			strcpy(sendBuf,"Choose your action\n1.Change User\n2.Change class table\n3.Quit\n");
			send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
			recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
            if(strcmp(szBuff,"1")==0){
				//printf("你成功了");
                change_user(tmsg_sock);
				//strcpy(szBuff,"");
            }
			if(strcmp(szBuff,"2")==0){
				classinf(tmsg_sock);
				//strcpy(szBuff,"");
			}
			//strcpy(szBuff,"");
        }
		//printf("成功退出");
    }
	else if(flag==1){
		guest(tmsg_sock);
	}
	closesocket(tmsg_sock);
	_endthread();
}

void change_user(SOCKET tmsg_sock){
    //int n = 0;
	char sendBuf[100];
	char szBuff[100];
	strcpy(sendBuf,"Choose your action\n1.Delete user\n2.Add user\n");
	send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
	recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
	if(strcmp(szBuff,"1")==0){
		delete_user(tmsg_sock);
	}
	else if(strcmp(szBuff,"2")==0){
		add_user(tmsg_sock);
	}
}

void delete_user(SOCKET tmsg_sock){
    FILE *fp, *ft;
	char sendBuf[100];
	char szBuff[100];
    char name[20];
	strcpy(sendBuf,"Input the name: ");
	send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
	recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
	strcpy(name,szBuff);
    int length = strlen(name);
    name[length] = '\n';
	name[length+1]='\0';
    char key[20];
    fp = fopen("user.txt", "rt+");
    ft = fopen("temp.txt", "wt");
    while(!feof(fp)){
        fgets(key, sizeof(key), fp);
        if(strcmp(key, name)){
            fputs(key, ft);
        }
    }
	strcpy(sendBuf,"Delete Success\n");
	send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
    fclose(fp);
    fclose(ft);
    remove("user.txt");
    rename("temp.txt", "user.txt");
}

void add_user(SOCKET tmsg_sock){
	char sendBuf[100];
	char szBuff[100];
    int flag = 0;
    FILE *fp;
    char name[20];
	strcpy(sendBuf,"Input the name: ");
	send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
	recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
	strcpy(name,szBuff);
    //int length = strlen(name);
    //name[length] = '\n';
    char key[20];
    fp = fopen("user.txt", "r+");
    while(!feof(fp)){
        fgets(key, sizeof(key), fp);
        if(!strcmp(key, name)){
            //cout<<key;
            flag++;
			strcpy(sendBuf,"Add failed\n");
			send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
            break;
        }    
    }
    if(flag == 0){
        fputs(name, fp);
		fputs("\n", fp);
		strcpy(sendBuf,"Add Success\n");
		send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
    }
    fclose(fp);
}

void record(char* ip){
	FILE *fp = fopen("record.txt", "r+");
    time_t t;
    struct tm *tmp;
	char key[64];
    char buf2[64];
    time(&t);
    tmp = localtime(&t);
    strftime(buf2, 64, "%Y-%m-%d %H:%M:%S ", tmp);
	int a = strlen(buf2); 
    //buf2[a+1] = '\n';
    while(!feof(fp)){
        fgets(key, sizeof(key), fp); 
    }
	strcat(buf2,ip);
	a=strlen(buf2);
	buf2[a] = '\n';
	buf2[a+1] = '\0';
    fputs(buf2, fp);
	fclose(fp);
    //printf("%s\n", buf2);
}

void search(int n){
    FILE* fp;
    fp = fopen("info.txt", "r");
    char classes[100];
    while(!feof(fp)){
        fgets(classes, sizeof(classes), fp);
        printf("%s\n", classes);
    }
	fclose(fp);
}

void classinf(SOCKET tmsg_sock){
	char sendBuf[150];
	char szBuff[150];
	FILE* fpread;
    char str[MAX];
    classinfo c[5];
    int n=0;
    fpread=fopen("info.txt", "r");
    if(fpread==NULL)
    {
        printf("Open failed.");
        //exit(0);
    }
    
    while(!feof(fpread))
    {
        fgets(str, 100, fpread);
        //printf("%s\n", str);
        char *p=strtok(str, " ");
        if(p)
            strcpy(c[n].code, p);
        p=strtok(NULL, " ");
            strcpy(c[n].title, p);
        p=strtok(NULL, " ");
            strcpy(c[n].section, p);
        p=strtok(NULL, " ");
            strcpy(c[n].time, p);
        p=strtok(NULL, " ");
            strcpy(c[n].classroom, p);
        p=strtok(NULL, " ");
            strcpy(c[n].instructor, p);
        n++;
    }
    
    fclose(fpread);

    char msg[50];
    int flag;
	char op[100];
    //printf("Enter user id:");
    //scanf("%d", &flag);
    if(1)
    {
        while(1){
			//strcpy(sendBuf,"");
			//strcpy(sendBuf,"Choose a opration:\n    1. Check all info\n    2. Update info\n    3. Delete info\n    4. Quit\n");
			//send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
			recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
			strcpy(op,szBuff);
            if(strcmp(op,"1")==0)
            {
                for(int i=0 ;i<num; i++){
					char temp[100];
                    sprintf(temp,"%s %s %s %s %s %s", c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
					//printf("%d\n",strlen(temp));
					//strcpy(sendBuf,temp);
					send(tmsg_sock,temp,strlen(temp)+1,0);
					//printf("%s %s %s %s %s %s\n", c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
					//strcpy(sendBuf,"\0");
				}
            }
            else if(strcmp(op,"2")==0)
            {
				char upop[20];
                while(1)
                {
					strcpy(sendBuf,"Select the item need to update:\n1. Time\n2. Classroom\n3. Instructor\n4. Quit\n");
					send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
					recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
					strcpy(upop,szBuff);
                    if(strcmp(upop,"1")==0)
                    {

                        char tempcode[20];
                        int wrong=1;
                        //printf("Enter class code:");
                        //scanf("%s", tempcode);
						strcpy(sendBuf,"Enter class code:");
						send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
						recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
						strcpy(tempcode,szBuff);
                        //printf("enter is: %s\n", temp);
                        for(int i=0 ;i<num; i++)
                        {
                            //printf("c[i] is :%s\n", c[i].title);
							strcpy(sendBuf,c[i].code);
							send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
                            if((strcmp(c[i].code, tempcode))==0)
                            {
								char tempsection[10], neww[20];
                                //printf("Enter class section:");
								strcpy(sendBuf,"Enter class section:");
								send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
								recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
								strcpy(tempsection,szBuff);
								strcpy(sendBuf,c[i].section);
								send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
								//scanf("%s", tempsection);
                                if((strcmp(c[i].section, tempsection))==0)
                                {
                                    sprintf(sendBuf,"The current time is: %s\nEnter a new time:\0", c[i].time);
                                    //printf("Enter a new time:");
									send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
									recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
									strcpy(neww,szBuff);
                                    //scanf("%s", neww);
                                    strcpy(c[i].time, neww);
                                    //printf("Update success\n");
									strcpy(sendBuf,"Update success\n");
									send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
									fpread=fopen("info.txt", "w");
									if(fpread==NULL)
									{
										printf("Open failed.");
										//exit(0);
									}
									for(int i=0; i<num; i++)
										fprintf(fpread, "%s %s %s %s %s %s", c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
									fclose(fpread);
                                    break;
                                }
                            }  
                        }
                        if(wrong)
                        {
                            //printf("No such class!\n");
							break;
                        }
                        else
                            break;
                    }
					else if(strcmp(upop,"2")==0){
						char tempcode[20];
                        int wrong=1;
						recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
						strcpy(tempcode,szBuff);
                        //printf("Enter class code:");
                        //scanf("%s", tempcode);
                        //printf("enter is: %s\n", temp);
                        for(int i=0 ;i<num; i++)
                        {
                            //printf("c[i] is :%s\n", c[i].title);
							if(strcmp(c[i].code, tempcode)==0){
								send(tmsg_sock,"1",strlen("1")+1,0);
							}
							else{
								send(tmsg_sock,"0",strlen("0")+1,0);
							}
                            if((strcmp(c[i].code, tempcode))==0)
                            {
								//send(tmsg_sock,"1",strlen("1")+1,0);
                                //printf("Enter class section:");
                                char tempsection[10], neww[20];
								recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
                                //scanf("%s", tempsection);
								//printf("aa");
								strcpy(tempsection,szBuff);
								//printf("%d",strcmp(c[i].section, tempsection));
								if(strcmp(c[i].section, tempsection)==0){
									send(tmsg_sock,"1",strlen("1")+1,0);
								}
								else{
									send(tmsg_sock,"0",strlen("0")+1,0);
								}
                                if((strcmp(c[i].section, tempsection))==0)
                                {
                                    sprintf(sendBuf,"The current classroom is: %s\n", c[i].classroom);
									send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
                                    //scanf("%s", neww);
									recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
                                    strcpy(c[i].classroom, szBuff);
                                    //printf("Update success\n");
                                    wrong=0;
									fpread=fopen("info.txt", "w");
									if(fpread==NULL)
									{
										printf("Open failed.");
										//exit(0);
									}
									for(int i=0; i<num; i++)
										fprintf(fpread, "%s %s %s %s %s %s", c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
									fclose(fpread);
                                    break;
                                }
                                if(i==num)
                                {
                                    //printf("No such section!\n");
                                }
                            }  
                        }
                        if(wrong)
                        {
                            //printf("No such class!\n");
                        }
                        else
                            break;
					}
					else if(strcmp(upop,"3")==0){
						char tempcode[20];
                        int wrong=1;
						recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
						strcpy(tempcode,szBuff);
                        //printf("Enter class code:");
                        //scanf("%s", tempcode);
                        //printf("enter is: %s\n", temp);
                        for(int i=0 ;i<num; i++)
                        {
                            //printf("c[i] is :%s\n", c[i].title);
							if(strcmp(c[i].code, tempcode)==0){
								send(tmsg_sock,"1",strlen("1")+1,0);
							}
							else{
								send(tmsg_sock,"0",strlen("0")+1,0);
							}
                            if((strcmp(c[i].code, tempcode))==0)
                            {
								//send(tmsg_sock,"1",strlen("1")+1,0);
                                //printf("Enter class section:");
                                char tempsection[10], neww[20];
								recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
                                //scanf("%s", tempsection);
								//printf("aa");
								strcpy(tempsection,szBuff);
								//printf("%d",strcmp(c[i].section, tempsection));
								if(strcmp(c[i].section, tempsection)==0){
									send(tmsg_sock,"1",strlen("1")+1,0);
								}
								else{
									send(tmsg_sock,"0",strlen("0")+1,0);
								}
                                if((strcmp(c[i].section, tempsection))==0)
                                {
                                    sprintf(sendBuf,"The current instructor is: %s\n", c[i].instructor);
									send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
                                    //scanf("%s", neww);
									recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
									int len=strlen(szBuff);
									szBuff[len]='\n';
									szBuff[len+1]='\0';
                                    strcpy(c[i].instructor, szBuff);
                                    //printf("Update success\n");
                                    wrong=0;
									fpread=fopen("info.txt", "w");
									if(fpread==NULL)
									{
										printf("Open failed.");
										//exit(0);
									}
									for(int i=0; i<num; i++)
										fprintf(fpread, "%s %s %s %s %s %s", c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
									fclose(fpread);
                                    break;
                                }
                                if(i==num)
                                {
                                    //printf("No such section!\n");
                                }
                            }  
                        }
                        if(wrong)
                        {
                            //printf("No such class!\n");
                        }
                        else
                            break;
					}
                    else if(strcmp(upop,"4")==0){
						break;
					}
                    else
                    {
                        //printf("Invalid operation, try again\n");
                        //printf("Choose a opration:\n");
                    }
					fpread=fopen("info.txt", "w");
					if(fpread==NULL)
					{
						printf("Open failed.");
						//exit(0);
					}
					for(int i=0; i<num; i++)
						fprintf(fpread, "%s %s %s %s %s %s", c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
					fclose(fpread);
                }
            }
            else if(strcmp(op,"3")==0){
                for(int i =0; i < 2; i++){
                    sprintf(sendBuf,"%d. %s %s %s %s %s %s\n", i, c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
					send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
					//recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
					//strcpy(tempcode,szBuff);
				}
                //printf("\n");
				//strcpy(sendBuf,"Input the info to delete: ");
				//send(tmsg_sock,"Input the info to delete: ",strlen("Input the info to delete: ")+1,0);
				recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
				//printf("%s",szBuff);
                //printf("Input the info to delete: ");
                //int index = 0;
                //scanf("%d", &index);
                char *space = " ";
                char *enter = "\n";
                FILE *ft;
				int index;
                ft = fopen("temp.txt", "wt");
				//printf("%d",strcmp(szBuff,"1"));
				index=atoi(szBuff);
				for(int i = 0; i < num; i++){
                    if(i == index)
                        continue;
                    fputs(c[i].code, ft);
                    fputs(space, ft);
                    fputs(c[i].title, ft);
                    fputs(space, ft);
                    fputs(c[i].section, ft);
                    fputs(space, ft);
                    fputs(c[i].time, ft);
                    fputs(space, ft);
                    fputs(c[i].classroom, ft);
                    fputs(space, ft);
                    fputs(c[i].instructor, ft);
                }
				fclose(ft);
                remove("info.txt");
                rename("temp.txt", "info.txt");
            }
            else if(strcmp(op,"4")==0){
				break;
			}
            /*else
            strcpy(sendBuf,"Input the name: ");
			send(tmsg_sock,sendBuf,strlen(sendBuf)+1,0);
			recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
			strcpy(op,szBuff);*/
        }
    }
    
    //system("pause");
}

void guest(SOCKET tmsg_sock){
	char sendBuf[150];
	char szBuff[150];
	FILE* fpread;
    char str[MAX];
    classinfo c[5];
    int n=0;
    fpread=fopen("info.txt", "r");
    if(fpread==NULL)
    {
        printf("Open failed.");
        //exit(0);
    }
    
    while(!feof(fpread))
    {
        fgets(str, 100, fpread);
        //printf("%s\n", str);
        char *p=strtok(str, " ");
        if(p)
            strcpy(c[n].code, p);
        p=strtok(NULL, " ");
            strcpy(c[n].title, p);
        p=strtok(NULL, " ");
            strcpy(c[n].section, p);
        p=strtok(NULL, " ");
            strcpy(c[n].time, p);
        p=strtok(NULL, " ");
            strcpy(c[n].classroom, p);
        p=strtok(NULL, " ");
            strcpy(c[n].instructor, p);
        n++;
    }
    
    fclose(fpread);
	char op[20],temp[100];
    while(1)
    {
		//strcpy(temp,"Choose a opration:\n1. Check timetable\n2. Search timetable\n3. Quit\n");
		//send(tmsg_sock,temp,strlen(temp)+1,0);
		recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
		strcpy(op,szBuff);
        if(strcmp(op,"1")==0)
        {
            //printf("The timetable is:\n");
            for(int i=0 ;i<num; i++){
				char temp[100];
                sprintf(temp,"%s %s %s %s %s %s", c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
				send(tmsg_sock,temp,strlen(temp)+1,0);
			}
			strcpy(sendBuf,"");
		}
        else if(strcmp(op,"2")==0){
			recv(tmsg_sock, szBuff, sizeof(szBuff), 0);
			if(strcmp(szBuff,"1")==0){
				for(int i = 0; i < num; i++){
					if(!strcmp(c[i].time, "Monday")){
						sprintf(szBuff,"%s %s %s %s %s %s", c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
						send(tmsg_sock, szBuff, sizeof(szBuff), 0);
					}
                }
			}
			else if(strcmp(szBuff,"2")==0){
				for(int i = 0; i < num; i++){
					if(!strcmp(c[i].time, "Tuesday")){
						sprintf(szBuff,"%s %s %s %s %s %s", c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
						send(tmsg_sock, szBuff, sizeof(szBuff), 0);
					}
                }
			}
			else if(strcmp(szBuff,"3")==0){
				for(int i = 0; i < num; i++){
					if(!strcmp(c[i].time, "Wednesday")){
						sprintf(szBuff,"%s %s %s %s %s %s", c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
						send(tmsg_sock, szBuff, sizeof(szBuff), 0);
					}
                }
			}
			else if(strcmp(szBuff,"4")==0){
				for(int i = 0; i < num; i++){
					if(!strcmp(c[i].time, "Thursday")){
						sprintf(szBuff,"%s %s %s %s %s %s", c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
						send(tmsg_sock, szBuff, sizeof(szBuff), 0);
					}
                }
			}
			else if(strcmp(szBuff,"5")==0){
				for(int i = 0; i < num; i++){
					if(!strcmp(c[i].time, "Friday")){
						sprintf(szBuff,"%s %s %s %s %s %s", c[i].code, c[i].title, c[i].section, c[i].time, c[i].classroom, c[i].instructor);
						send(tmsg_sock, szBuff, sizeof(szBuff), 0);
					}
                }
			}   
        }
        else if(strcmp(op,"3")==0)
            break;
        else
            printf("Invalid operation, try again\n");
    }
	return;
}

void read()
{
    FILE* fpread;
    char str[MAX];
    int n=0;
    fpread=fopen("info.txt", "r");
    if(fpread==NULL)
    {
        printf("Open failed.");
        exit(0);
    }
    while(!feof(fpread))
    {
        fgets(str, 100, fpread);
        num++;
    }
    //printf("num = %d\n", num);
    rewind(fpread);
    while(!feof(fpread))
    {
        fgets(str, 100, fpread);
        //printf("%d\n", strlen(str));
        char *p=strtok(str, " ");
        if(p)
            strcpy(c[n].code, p);
        p=strtok(NULL, " ");
            strcpy(c[n].title, p);
        p=strtok(NULL, " ");
            strcpy(c[n].section, p);
        p=strtok(NULL, " ");
            strcpy(c[n].time, p);
        p=strtok(NULL, " ");
            strcpy(c[n].classroom, p);
        p=strtok(NULL, " ");
            strcpy(c[n].instructor, p);
        n++;
    }
    
    fclose(fpread);
}