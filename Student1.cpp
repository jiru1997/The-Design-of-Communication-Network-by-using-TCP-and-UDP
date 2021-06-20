#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <assert.h>

#define SERV_IP "127.0.0.8"
#define SERV_PORT 6018
#define UDPS1 21418
#define filename "student1.txt"

using namespace std;

int main()
{
    //sleep(5);

    char buffer[BUFSIZ];
    char ipAddress[INET_ADDRSTRLEN], str[INET_ADDRSTRLEN];
    int clientfd, n, soc;
    struct sockaddr_in serveraddr, clientAddr, udpserv_addr, udpclientAddr;
    socklen_t  clientAddrLen = sizeof(clientAddr);
    socklen_t clieaddr_len;

    //--------------------------------------read student1.txt-----------------------------------
    vector<string> student1;
    int index = 3;
    ifstream file(filename);
    assert( file.is_open() );
    while(!file.eof())
    {
        string line;
        string temp;
        getline(file,line);
        stringstream ss( line );
        ss >> temp;
        student1.push_back(temp);
        index--;
    }

    //------------------------------------build connection with server---------------------------
    clientfd = socket(AF_INET, SOCK_STREAM,0);
    memset(&serveraddr, 0 , sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons( SERV_PORT );
    inet_pton(AF_INET, SERV_IP, &serveraddr.sin_addr.s_addr);

    connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    getsockname(clientfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    printf("<Student 1> has TCP port %d and IP address %s \n",ntohs(clientAddr.sin_port), inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress)));
    sleep(1);

    //-----------------------------------send data to health center -------------------------------  
    string data;
    memset(buffer,'\0',sizeof(buffer));
    data = "Student1";
    write(clientfd, data.c_str(), strlen(data.c_str()));
    sleep(1);

    write(clientfd, data.c_str(), strlen(data.c_str()));
    sleep(1);

    for(int i = 0 ; i < student1.size(); i++)
    {
        data = "";
        data = student1[i];
        write(clientfd, data.c_str(), strlen(data.c_str()));
        sleep(1);
    }
    data = "";
    data = "end";
    write(clientfd, data.c_str(), strlen(data.c_str()));
    printf("Completed sending applicaiont for <Student 1> \n");

    //-----------------------------------receive data to health center -------------------------------
    n = read(clientfd, buffer, sizeof(buffer));
    printf("<Student 1> has received the reply from the health center \n");
    if(buffer[0] == '0')
    {
        printf("<Student1> has reveived the application result, the result is : rejection \n");
    }
    else
    {
        soc = socket(AF_INET, SOCK_DGRAM, 0);
        bzero(&udpserv_addr, sizeof(udpserv_addr));
        udpserv_addr.sin_family = AF_INET;
        udpserv_addr.sin_port = htons(UDPS1);
        inet_pton(AF_INET, "127.0.0.2", &udpserv_addr.sin_addr.s_addr);
        bind(soc, (struct sockaddr *)&udpserv_addr, sizeof(udpserv_addr));

        getsockname(soc, (struct sockaddr*)&clientAddr, &clientAddrLen);
        printf("<Student 1> has UDP port %d and IP address %s for phase 2 \n",UDPS1, inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress)));
        
        printf("<Student 1> has received the application result \n");

        clieaddr_len = sizeof(udpclientAddr);
        n = recvfrom(soc, buffer, BUFSIZ, 0, (struct sockaddr *)&udpclientAddr, &clieaddr_len);
        printf("<Student 1> has reveived the application result, the result is : %s \n", buffer);

        close(soc);
    }


        
    return 0;
}
