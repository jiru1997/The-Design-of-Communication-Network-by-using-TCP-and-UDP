# The-Design-of-Communication-Network-by-using-TCP-and-UDP
The Design of Communication Network by using TCP and UDP

In this project you will simulate a covid-19 tesing application process. This process involves the USC students who apply for covid-19 virus testing, the USC Health Center that manages the testing, and the different hospitals that will provide students the chances to get the test. The communication among the USC students, health center and hospitals are performed over TCP and UDP sockets in a network with a client-server architecture.

The project has 2 major phases: 1) Hospitals provide the USC health center, the list of available departments that could do the testing process, 2) Students apply to the USC center to have the chance of getting tested. The USC health center makes the decision and sends the results to both students and hospitals. In phase 1 all communications are through TCP sockets.

In phase 2 the communication is through both TCP and UDP sockets. You will start with Phase 2, as soon as your Phase 1 work is complete. 

compliation steps: 
```
g++ -o Healthcenter Healthcenter.cpp
g++ -o HospitalA HospitalA.cpp
g++ -o HospitalB HospitalB.cpp
g++ -o HospitalC HospitalC.cpp
g++ -o Student1 Student1.cpp
g++ -o Student2 Student2.cpp
g++ -o Student3 Student3.cpp
g++ -o Student4 Student4.cpp
g++ -o Student5 Student5.cpp
./Healthcenter
./HospitalA
./HospitalB
./HospitalC
./Student1
./Student2
./Student3
./Student4
./Student5
```
