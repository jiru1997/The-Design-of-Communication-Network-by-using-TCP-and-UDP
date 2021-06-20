#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <strings.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <map>
#include <vector>
#include <fstream>
#include <assert.h>

#define SERV_PORT 6018
#define UDPS1 21418
#define UDPS2 21518
#define UDPS3 21618
#define UDPS4 21718
#define UDPS5 21818
#define UDPHA 21118
#define UDPHB 21218
#define UDPHC 21318


using namespace std;

int printIndex = 0;

void wait_child(int digno)
{
    while(waitpid(0, NULL, WNOHANG) > 0);
    return;
}

int main()
{
    pid_t pid;
    int serverfd, clientfd,n,line, socketfd ;
    char ipAddress[INET_ADDRSTRLEN],ipAddr[INET_ADDRSTRLEN];
    socklen_t  clie_addr_len;
    struct sockaddr_in serveraddr, clie_addr, listendAddr, listendAddr0, servaddr;
    char buffer[BUFSIZ];
    char clie_ip[BUFSIZ];
    string department, subrange;
    map<string,vector<int> > data_hospitals;
    vector<string> data_students;
    socklen_t listendAddrLen = sizeof(listendAddr);
    

    serverfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "127.0.0.8", &serveraddr.sin_addr.s_addr);
    bind(serverfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    listen(serverfd, 128);
    
    getsockname(serverfd, (struct sockaddr *)&listendAddr, &listendAddrLen);
    printf("The health center has TCP port %d and IP address %s \n",ntohs(listendAddr.sin_port), inet_ntoa(listendAddr.sin_addr));

    //------------------------------------------connecting-------------------------------------------------------------------------
    while(1)
    {

        clie_addr_len = sizeof(clie_addr);
        clientfd = accept(serverfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
        printIndex ++;

        pid = fork();
        if(pid < 0)
        {
            perror("fork error");
            exit(1);
        }
        else if(pid == 0)
        {
            close(serverfd);
            break;
        }
        else
        {
            close(clientfd);
            signal(SIGCHLD, wait_child);
        }
    }


    //------------------------------------------processing--------------------------------------------------------------------------

    if(pid == 0)
    {
        while(1)
        {
            n = read(clientfd, buffer, sizeof(buffer));
            if(n == 0)
            {
                close(clientfd);
                return 0;
            }
            else if(n == -1)
            {
                perror("readError");
                exit(1);
            }
            else
            {
                ofstream ofs;
                line = read(clientfd, buffer, sizeof(buffer));
                string name = string(buffer);

                //--------------------------------------------------data from hospitals ------------------------------------------
                if(name[0] == 'H')
                {
                    printf("Received the department list from <%s> \n",name.c_str());
                    memset(buffer,'\0',sizeof(buffer));

                    for(int i = 0 ; i < 3; i++)
                    {
                        ofstream ofs;
                        line = read(clientfd, buffer, sizeof(buffer));
                        string temp = string(buffer);
                        ofs.open("A.txt",ios::app);
                        ofs << temp << endl;
                        memset(buffer,'\0',sizeof(buffer));
                    }
                    ofs.close();

                    if(printIndex == 3)
                    {
                        sleep(3);
                        printf("End of phase 1 for health center \n");
                    }
                }


                //-------------------------------------------------data from students --------------------------------------------
                if(name[0] == 'S')
                {
                    memset(buffer,'\0',sizeof(buffer));
                    string tempinfo = "";
                    ofstream ofs;
                    for(int i = 0 ; i < 4; i++)
                    {
                        line = read(clientfd, buffer, sizeof(buffer));
                        tempinfo = string(buffer);
                        if(tempinfo == "end")
                            break;
                        int point = tempinfo.find(':');
                        subrange = tempinfo.substr(point + 1);
                        data_students.push_back(subrange);
                        memset(buffer,'\0',sizeof(buffer));
                        tempinfo = "";
                    }
                    ofs.close();

                    ifstream file("A.txt");
                    assert( file.is_open());

                    while(true)
                    {
                        string line;
                        string temp;
                        getline(file,line);
                        stringstream ss( line );
                        ss >> temp;
                        if(temp.empty())
                            break;
                        int point = temp.find('#');
                        department = temp.substr(0, point);
                        subrange = temp.substr(point + 1);
                        int point2 = subrange.find(',');
                        string sbeginPoint = subrange.substr(0, point2);
                        string sendPoint = subrange.substr(point2 + 1);
                        int beginPoint = atoi(sbeginPoint.c_str());
                        int endPoint = atoi(sendPoint.c_str());
                        vector<int> arr;
                        arr.push_back(beginPoint);
                        arr.push_back(endPoint);
                        data_hospitals[department] = arr;
                        memset(buffer,'\0',sizeof(buffer));
                    }

                    if(printIndex == 4)
                    {
                       sleep(3);
                       printf("The health center has TCP port %d and IP address %s \n",ntohs(listendAddr.sin_port), inet_ntoa(listendAddr.sin_addr));
                    }
                    
    
                    int result = 0;
                    for(int i = 1; i < data_students.size(); i++)
                    {
                       
                        if(data_hospitals.count(data_students[i]) != 1)
                        {
                            result++;    
                        }
                            
                    }

                    //send data to inform student whether use udp
                    string answer;
                    if(result == 3)
                        answer = "0";
                    else
                        answer = "1";

                    write(clientfd, answer.c_str(), strlen(answer.c_str()));
                    sleep(1);

                    // ------------------------------------------------------compare data from students and hospital ----------------------------

                    bool flag = false;
                    string targetDp = "";
                    string targetHospital = "Hospital";

                    for(int i = 1 ; i < data_students.size(); i++)
                    {
                        if(data_hospitals.count(data_students[i]) == 1)
                        {
                            int number = atoi(data_students[0].c_str());
                            if(data_hospitals[data_students[i]][0] <= number && number <= data_hospitals[data_students[i]][1])
                            {
                                targetDp = data_students[i];
                                targetHospital = targetHospital + targetDp[0];
                                flag = true;
                                break;
                            }

                        }
                    }

                    //---------------------------------------------------------build UDP connection to students-------------------------------
                    printf("health center receive the application from <%s> \n",name.c_str());
                    if(printIndex != 8)
                        sleep(50- printIndex);
                        
                    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
                    bzero(&servaddr, sizeof(servaddr));
                    servaddr.sin_family = AF_INET;
                    if(name == "Student1") 
                    {
                        servaddr.sin_port = htons(UDPS1);
                        inet_pton(AF_INET, "127.0.0.2", &servaddr.sin_addr.s_addr);
                    }
                    else if(name == "Student2") 
                    {
                        servaddr.sin_port = htons(UDPS2);
                        inet_pton(AF_INET, "127.0.0.3", &servaddr.sin_addr.s_addr);
                    }
                    else if(name == "Student3") 
                    {
                        servaddr.sin_port = htons(UDPS3);
                        inet_pton(AF_INET, "127.0.0.4", &servaddr.sin_addr.s_addr);
                    }
                    else if(name == "Student4") 
                    {
                        servaddr.sin_port = htons(UDPS4);
                        inet_pton(AF_INET, "127.0.0.5", &servaddr.sin_addr.s_addr);
                    }
                    else if(name == "Student5") 
                    {
                        servaddr.sin_port = htons(UDPS5);
                        inet_pton(AF_INET, "127.0.0.6", &servaddr.sin_addr.s_addr);
                    }
                        
                    inet_pton(AF_INET, "127.0.0.1", &listendAddr0.sin_addr.s_addr);
                    bind(socketfd, (struct sockaddr *)&listendAddr0, sizeof(listendAddr0));
                    if(flag)
                    {
                        string message = "Accept#" + targetDp + "#" + targetHospital;
                        sleep(3);
                        n = sendto(socketfd, message.c_str(), strlen(message.c_str()), 0 , (struct sockaddr *)&servaddr, sizeof(servaddr));
                        getsockname(socketfd, (struct sockaddr *)&listendAddr, &listendAddrLen);
                        if(printIndex == 8)
                        {
                            printf("The health center has UDP port %d and IP address %s for Phase 2 \n",ntohs(listendAddr.sin_port), inet_ntoa(listendAddr0.sin_addr));
                            sleep(50- printIndex);
                        }
                        
                     }
                    else
                    {
                        sleep(3);
                        string reject = "Reject";
                        n = sendto(socketfd, reject.c_str(), strlen(reject.c_str()), 0 , (struct sockaddr *)&servaddr, sizeof(servaddr));
                        getsockname(socketfd, (struct sockaddr *)&listendAddr, &listendAddrLen);
                        if(printIndex == 8)
                        {
                            printf("The health center has UDP port %d and IP address %s for Phase 2 \n",ntohs(listendAddr.sin_port), inet_ntoa(listendAddr0.sin_addr));
                            sleep(50- printIndex);
                        }
                        
                    }
                    close(socketfd);

                    //---------------------------------------------------------build UDP connection to hospital-------------------------------

                    printf("The health center has send the application result to <%s>\n" ,name.c_str());
                    if(flag)
                    {
                        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
                        bzero(&servaddr, sizeof(servaddr));
                        servaddr.sin_family = AF_INET;
                        if(targetHospital == "HospitalA")
                        {
                            servaddr.sin_port = htons(UDPHA);
                            inet_pton(AF_INET, "127.1.0.0", &servaddr.sin_addr);
                        }
                        else if(targetHospital == "HospitalB") 
                        {
                            servaddr.sin_port = htons(UDPHB);
                            inet_pton(AF_INET, "127.2.0.0", &servaddr.sin_addr);
                        }   
                        else if(targetHospital == "HospitalC") 
                        {
                            servaddr.sin_port = htons(UDPHC);
                            inet_pton(AF_INET, "127.3.0.0", &servaddr.sin_addr);
                        }

                        string information  = name + "#" + data_students[0] + "#" + targetDp;

                        n = sendto(socketfd, information.c_str(), strlen(information.c_str()), 0 , (struct sockaddr *)&servaddr, sizeof(servaddr));

                        printf("The health center has send one admitted student to <%s> \n" ,targetHospital.c_str());

                        close(socketfd);
                    }

                }


            }


        }
    }

    return 0;
}
