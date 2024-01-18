// Client.cpp : Defines the entry point for the console application.
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

#define DEFAULT_PORT	5019


int main(int argc, char **argv){
	
	char szBuff[100];
	int msg_len;
	//int addr_len;
	struct sockaddr_in server_addr;
	struct hostent *hp;
	SOCKET connect_sock;
	WSADATA wsaData;
	//argc=3;
	char			*server_name = "localhost";
	unsigned short	port = DEFAULT_PORT;
	unsigned int	addr;
	//printf("%d\n",argc);
	if (argc != 3){
		printf("echoscln [server name] [port number]\n");
		return -1;
	}
	else{
		server_name = argv[1];
		port = atoi(argv[2]);
	}

	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR){
		// stderr: standard error are printed to the screen.
		fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
		//WSACleanup function terminates use of the Windows Sockets DLL. 
		WSACleanup();
		return -1;
	}

	if (isalpha(server_name[0]))
		hp = gethostbyname(server_name);
	else{
		addr = inet_addr(server_name);
		hp = gethostbyaddr((char*)&addr, 4, AF_INET);
	}

	if (hp==NULL)
	{
		fprintf(stderr, "Cannot resolve address: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//copy the resolved information into the sockaddr_in structure
	memset(&server_addr, 0, sizeof(server_addr));
	memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
	server_addr.sin_family = hp->h_addrtype;
	server_addr.sin_port = htons(port);

	
	connect_sock = socket(AF_INET,SOCK_STREAM, 0);	//TCp socket


	if (connect_sock == INVALID_SOCKET){
		fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	printf("Client connecting to: %s\n", hp->h_name);

	if (connect(connect_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) 
		== SOCKET_ERROR){
		fprintf(stderr, "connect() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	char sBuff[100];
	int num;
	recv(connect_sock, szBuff, sizeof(szBuff), 0);
	num=atoi(szBuff);
	//printf("%d\n",num);
	recv(connect_sock, szBuff, sizeof(szBuff), 0);
	printf("%s\n",szBuff);
	gets(sBuff);
	msg_len = send(connect_sock, sBuff, sizeof(sBuff), 0);
	char name[20];
	recv(connect_sock, name, sizeof(name), 0);
	if(strcmp(name,"0")==0&&strcmp(sBuff,"admin")==0){
		//printf("ready to recive/n");
		//选操作
		while(strcmp(szBuff,"3")!=0){
			recv(connect_sock, szBuff, sizeof(szBuff), 0);
			printf("%s\n",szBuff);
			//gets(szBuff);
			scanf("%s",szBuff);
			send(connect_sock, szBuff, sizeof(szBuff), 0);
			if(strcmp(szBuff,"1")==0){
				//选删除还是增加
				recv(connect_sock, szBuff, sizeof(szBuff), 0);
				printf("%s\n",szBuff);
				scanf("%s",szBuff);
				//gets(szBuff);
				send(connect_sock, szBuff, sizeof(szBuff), 0);
				if(strcmp(szBuff,"1")==0){
					//写要删除的用户的名字
					recv(connect_sock, szBuff, sizeof(szBuff), 0);
					printf("%s\n",szBuff);
					scanf("%s",szBuff);
					//scanf("%s",szBuff);
					send(connect_sock, szBuff, sizeof(szBuff), 0);
					//回收删除信息
					recv(connect_sock, szBuff, sizeof(szBuff), 0);
					printf("%s\n",szBuff);
					strcpy(szBuff,"");
				}
				else if(strcmp(szBuff,"2")==0){
					//写要加入的用户的名字
					recv(connect_sock, szBuff, sizeof(szBuff), 0);
					printf("%s\n",szBuff);
					scanf("%s",szBuff);
					//scanf("%s",szBuff);
					send(connect_sock, szBuff, sizeof(szBuff), 0);
					//回收添加信息
					recv(connect_sock, szBuff, sizeof(szBuff), 0);
					printf("%s\n",szBuff);
					strcpy(szBuff,"");
				}
				//strcpy(szBuff,"");
			}
			if(strcmp(szBuff,"2")==0){
				while(1){//选change操作
					//recv(connect_sock, szBuff, sizeof(szBuff), 0);
					//printf("%s\n",szBuff);
					printf("Choose a opration:\n    1. Check all info\n    2. Update info\n    3. Delete info\n    4. Quit\n");
					scanf("%s",szBuff);
					send(connect_sock, szBuff, sizeof(szBuff), 0);
					if(strcmp(szBuff,"1")==0){
						for(int i=0;i<num; i++){
							char temp[100];
							recv(connect_sock, temp, sizeof(temp), 0);
							printf("%s\n",temp);
						}
						//strcpy(szBuff,"");
					}
					else if(strcmp(szBuff,"2")==0){
						while(1){
							recv(connect_sock, szBuff, sizeof(szBuff), 0);
							printf("%s\n",szBuff);
							scanf("%s",szBuff);
							send(connect_sock, szBuff, sizeof(szBuff), 0);
							if(strcmp(szBuff,"1")==0){
								int wrong=1;
								recv(connect_sock, szBuff, sizeof(szBuff), 0);
								printf("%s\n",szBuff);
								scanf("%s",szBuff);
								send(connect_sock, szBuff, sizeof(szBuff), 0);
								char sen[20];
								strcpy(sen,szBuff);
								for(int i=0;i<num;i++){
									recv(connect_sock, szBuff, sizeof(szBuff), 0);
									if(strcmp(sen,szBuff)==0){
										recv(connect_sock, szBuff, sizeof(szBuff), 0);
										printf("%s\n",szBuff);
										scanf("%s",szBuff);
										send(connect_sock, szBuff, sizeof(szBuff), 0);
										strcpy(sen,szBuff);
										recv(connect_sock, szBuff, sizeof(szBuff), 0);
										if(strcmp(sen,szBuff)==0){
											wrong=0;
											recv(connect_sock, szBuff, sizeof(szBuff), 0);
											printf("%s\n",szBuff);
											scanf("%s",szBuff);
											send(connect_sock, szBuff, sizeof(szBuff), 0);
											recv(connect_sock, szBuff, sizeof(szBuff), 0);
											printf("%s\n",szBuff);
											break;
										}
										/*else{
											recv(connect_sock, szBuff, sizeof(szBuff), 0);
											printf("%s\n",szBuff);
										}*/
									}
								}
								if(wrong){
									printf("No such section or class!\n");
									break;
								}
								else
									break;
							}
							else if(strcmp(szBuff,"2")==0){
								int wrong=1;
								printf("Enter class code:");
								scanf("%s",szBuff);
								send(connect_sock, szBuff, sizeof(szBuff), 0);
								for(int i=0 ;i<num; i++)
								{
								//printf("c[i] is :%s\n", c[i].title);
								recv(connect_sock, szBuff, sizeof(szBuff), 0);
								if(strcmp(szBuff,"1")==0)
								{
									//strcpy(szBuff,"");
									printf("Enter class section:");
									char tempsection[10], neww[20];
									scanf("%s", tempsection);
									send(connect_sock, tempsection, sizeof(tempsection), 0);
									recv(connect_sock, szBuff, sizeof(szBuff), 0);
									if(strcmp(szBuff,"1")==0)
									{
										//printf("The current classroom is: %s\n", c[i].classroom);
										recv(connect_sock, szBuff, sizeof(szBuff), 0);
										printf("%s",szBuff);
										printf("Enter a new classroom:");
										scanf("%s", neww);
										send(connect_sock, neww, sizeof(neww), 0);
										//strcpy(c[i].classroom, neww);
										printf("Update success\n");
										wrong=0;
										break;
									}
									if(i==num)
									{
										printf("No such section!\n");
									}
								}  
							}
                        if(wrong)
                        {
                            printf("No such class!\n");
                        }
                        else
                            break;
							}
							else if(strcmp(szBuff,"3")==0){
								int wrong=1;
								printf("Enter class code:");
								scanf("%s",szBuff);
								send(connect_sock, szBuff, sizeof(szBuff), 0);
								for(int i=0 ;i<num; i++)
								{
								//printf("c[i] is :%s\n", c[i].title);
								recv(connect_sock, szBuff, sizeof(szBuff), 0);
								if(strcmp(szBuff,"1")==0)
								{
									//strcpy(szBuff,"");
									printf("Enter class section:");
									char tempsection[10], neww[20];
									scanf("%s", tempsection);
									send(connect_sock, tempsection, sizeof(tempsection), 0);
									recv(connect_sock, szBuff, sizeof(szBuff), 0);
									if(strcmp(szBuff,"1")==0)
									{
										recv(connect_sock, szBuff, sizeof(szBuff), 0);
										printf("%s",szBuff);
										printf("Enter a new instructor:");
										scanf("%s", neww);
										send(connect_sock, neww, sizeof(neww), 0);
										//strcpy(c[i].classroom, neww);
										printf("Update success\n");
										wrong=0;
										break;
									}
									if(i==num)
									{
										printf("No such section!\n");
									}
								}  
							}
                        if(wrong)
                        {
                            printf("No such class!\n");
                        }
                        else
                            break;
							}
							else if(strcmp(szBuff,"4")==0){
								break;
							}
							else{
								printf("Invalid operation, try again\n");
								printf("Choose a opration:\n");
							}
						}
					}
					else if(strcmp(szBuff,"3")==0){
						for(int i =0; i < num; i++){
							recv(connect_sock, szBuff, sizeof(szBuff), 0);
							printf("%s",szBuff);
						}
						//recv(connect_sock, szBuff, sizeof(szBuff), 0);
						printf("Input the info to delete: ");
						scanf("%s",szBuff);
						send(connect_sock, szBuff, sizeof(szBuff), 0);
					}
					else if(strcmp(szBuff,"4")==0){
						break;
					}
				}
			}
		}
	}
	else if(strcmp(name,"1")==0){
			while(1){
				printf("Choose a opration:\n1. Check timetable\n2. Search timetable\n3. Quit\n");
				//recv(connect_sock, szBuff, sizeof(szBuff), 0);
				//printf("%s\n",szBuff);
				scanf("%s",szBuff);
				send(connect_sock, szBuff, sizeof(szBuff), 0);
				if(strcmp(szBuff,"1")==0){
					for(int i=0 ;i<num; i++){
						char temp[100];
						recv(connect_sock, temp, sizeof(temp), 0);
						printf("%s\n",temp);		
					}
				}
				else if(strcmp(szBuff,"2")==0){
					printf("1. Monday\n2. Tuesday\n3. Wednesday\n4. Thursday\n5. Friday\nInput the day you want to search: ");
					scanf("%s",szBuff);
					send(connect_sock, szBuff, sizeof(szBuff), 0);
					recv(connect_sock, szBuff, sizeof(szBuff), 0);
					printf("%s\n",szBuff);
				}
				else if(strcmp(szBuff,"3")==0){
					break;
				}
			}
	}
	else{
		printf("You are not our user!");
	}
	
	closesocket(connect_sock);
	WSACleanup();
}
