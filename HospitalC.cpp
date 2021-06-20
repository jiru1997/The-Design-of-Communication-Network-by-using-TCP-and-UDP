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
#define UDPHC 21318
#define filename "HosptialC.txt"

using namespace std;

int main()
{
    int cfd, sockfd;
    struct sockaddr_in serv_addr, clientAddr,udpserv_addr, udpclientAddr;
    socklen_t clie_addr_len;
    socklen_t  clientAddrLen = sizeof(clientAddr);
    char buf[BUFSIZ];
    char clie_ip[BUFSIZ];
    int n;
    char ipAddress[INET_ADDRSTRLEN];
    char str[INET_ADDRSTRLEN];

    //--------------------------------------read HosptialC.txt-----------------------------------
    vector<string> hospitala;
    int index = 3;
    ifstream file(filename);
    assert( file.is_open() );
    while(index > 0)
    {
        string line;
        string temp;
        assert( getline(file,line) );
        stringstream ss( line );
        ss >> temp;
        hospitala.push_back(temp);
        index--;
    }

    //------------------------------------build connection with server---------------------------
    cfd = socket(AF_INET, SOCK_STREAM,0);

    memset(&serv_addr, 0 , sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons( SERV_PORT );
    //serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);

    connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    getsockname(cfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    printf("<Hospital C> has TCP port %d and IP address %s for Phase 1 \n",ntohs(clientAddr.sin_port), inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress)));
    printf("<Hospital C> is now connected to the health center \n");
    sleep(1);

    //------------------------------------send data------------------------------------------------

    string data;
    printf("<Hospital C> has sent <department> to the health center \n");
    memset(buf,'\0',sizeof(buf));
    data = "Hospital C";
    write(cfd, data.c_str(), strlen(data.c_str()));
    sleep(1);

    write(cfd, data.c_str(), strlen(data.c_str()));
    sleep(1);

    for(int i = 0; i < 3; i ++)
    {
        
        data = "";
        data = hospitala[i];
        write(cfd, data.c_str(), strlen(data.c_str()));
        sleep(1);
    }
    printf("Updating the health center is done for <Hospital C> \n");
    printf("End of Phase 1 for <Hospital C> \n");

    close(cfd);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&udpserv_addr, sizeof(udpserv_addr));

    udpserv_addr.sin_family = AF_INET;
    udpserv_addr.sin_port = htons(UDPHC);
    inet_pton(AF_INET, "127.3.0.0", &udpserv_addr.sin_addr.s_addr);

    bind(sockfd, (struct sockaddr *)&udpserv_addr, sizeof(udpserv_addr));

    getsockname(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    printf("<Hosptial C> has UDP port %d and IP address %s for phase 2 \n",UDPHC, inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress)));

    for(int i = 0 ; i < 33; i ++)
    {
        clie_addr_len = sizeof(udpclientAddr);
        n = recvfrom(sockfd, buf, BUFSIZ, 0, (struct sockaddr *)&udpclientAddr, &clie_addr_len);
        string temp = string(buf);
        int point = temp.find('#');
        string studentname = temp.substr(0, point);
        memset(&serv_addr, 0 , sizeof(serv_addr));
        printf("<%s> has been admitted to <Hosptial C> \n",studentname.c_str());
        sleep(0.1);
    }
    
    printf("End of Phase2 for <Hosptial C> \n");
    close(sockfd);

    return 0;
}
