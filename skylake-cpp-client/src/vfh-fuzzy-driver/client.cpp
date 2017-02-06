#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include <iostream>
#include <core-common/BaseDriver.h>
#include "VFHFuzzyDriver.h"
#include <string.h>

#define UDP_MSGLEN 1000
#define UDP_CLIENT_TIMEUOT 1000000
//#define __UDP_CLIENT_VERBOSE__

typedef int SOCKET;
typedef struct sockaddr_in tSockAddrIn;
#define CLOSE(x) close(x)
#define INVALID(x) x < 0

class VFHFuzzyDriver;
typedef VFHFuzzyDriver tDriver;

using namespace std;

//void parse_args(int argc, char *argv[], char *hostName, unsigned int &serverPort, char *id, unsigned int &maxEpisodes,
//      unsigned int &maxSteps, bool &noise, double &noiseAVG, double &noiseSTD, long &seed, char *trackName,
//      BaseDriver::tstage &stage);
void parse_args(int argc, char *argv[], std::string &serverName, unsigned int &serverPort,
                std::string &driverId, std::string &trackName, BaseDriver::tstage &stage,
                unsigned int &maxEpisodes, unsigned int &maxSteps, std::string &fclFile)
{
    // Set default values
    serverName  = "localhost";
    serverPort  = 3001;
    driverId    = "SCR";

    trackName   = "unknown";
    maxEpisodes = 0;
    maxSteps    = 0;
    stage       = BaseDriver::UNKNOWN;
    fclFile     = "mapping-driver.fcl";

    std::vector<std::string> args(argv, argv + argc);

    int i = 1;
    while (i < argc)
    {
        if (args[i].find("host:") == 0)
            serverName = args[i].substr(5);

        else if (args[i].find("port:") == 0)
            serverPort = std::stoi(args[i].substr(5));

        else if (args[i].find("id:") == 0)
            driverId = args[i].substr(3);

        else if (args[i].find("maxEpisodes:") == 0)
            maxEpisodes = std::stoi(args[i].substr(12));

        else if (args[i].find("maxSteps:") == 0)
            maxSteps = std::stoi(args[i].substr(9));

        else if (args[i].find("track:") == 0)
            trackName = args[i].substr(6);

        else if (args[i].find("stage:") == 0)
        {
            stage = static_cast<BaseDriver::tstage>(std::stoi(args[i].substr(6)));

            if (stage<BaseDriver::WARMUP || stage > BaseDriver::RACE)
                stage = BaseDriver::UNKNOWN;
        }

        else if (args[i].find("fcl:") == 0)
            fclFile = args[i].substr(4);

        i++;
    }
}
int main(int argc, char *argv[]) {
    std::string  serverName;
    unsigned int serverPort;
    std::string  driverId;
    std::string  trackName;
    BaseDriver::tstage stage;
    unsigned int maxEpisodes;
    unsigned int maxSteps;
    std::string  fclFile;

    parse_args(argc, argv, serverName, serverPort, driverId, trackName, stage, maxEpisodes, maxSteps, fclFile);

    std::cout << "Config:" << std::endl;

    std::cout << " * server name:  " << serverName << std::endl;
    std::cout << " * server port:  " << serverPort << std::endl;
    std::cout << " * mapping-driver id     " << driverId   << std::endl;
    std::cout << " * track name:   " << trackName  << std::endl;

    std::string switchStage;

    switch (stage)
    {
        case BaseDriver::WARMUP:
            switchStage = "warm-up" ; break;
        case BaseDriver::QUALIFYING:
            switchStage = "qualifying"; break;
        case BaseDriver::RACE:
            switchStage = "race"; break;
        default:
            switchStage = "unknown";
    }

    std::cout << " * stage:        " << switchStage << std::endl;;
    std::cout << " * max episodes: " << maxEpisodes << std::endl;
    std::cout << " * max steps:    " << maxSteps    << std::endl;
    std::cout << " * fcl file:     " << fclFile     << std::endl << std::endl;

    // ---------------------------- socket init ----------------------------

    SOCKET socketDescriptor;
    int    numRead;

    tSockAddrIn    serverAddress;
    struct hostent *hostInfo;
    struct timeval timeVal;
    fd_set readSet;
    char   buf[UDP_MSGLEN];


#ifdef WINDOWS
    /* WinSock startup */

    WSADATA wsaData = { 0 };
    WORD wVer = MAKEWORD(2, 2);
    int nRet = WSAStartup(wVer, &wsaData);

    if (nRet == SOCKET_ERROR)
    {
        std::cerr << "Failed to init WinSock library" << std::endl;
        exit(1);
    }
#endif

    hostInfo = gethostbyname(serverName.c_str());
    if (hostInfo == NULL)
    {
        std::cerr << "Problem interpreting server name: " << serverName << std::endl;
        exit(1);
    }

    // Create a socket (UDP on IPv4 protocol)
    socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (INVALID(socketDescriptor))
    {
        std::cerr << "Cannot create socket" << std::endl;
        exit(1);
    }

    // Set some fields in the serverAddress structure.
    serverAddress.sin_family = hostInfo->h_addrtype;
    memcpy((char *)&serverAddress.sin_addr.s_addr,
           hostInfo->h_addr_list[0], hostInfo->h_length);
    serverAddress.sin_port = htons(serverPort);

    tDriver d;
    strcpy(d.trackName, trackName.c_str());
    d.stage = stage;

    if (!d.loadFCLfile(std::string(fclFile))) {
        std::cerr << "Cannot load FCL file" << std::endl;
        exit(1);
    }

    bool shutdownClient = false;
    unsigned long curEpisode = 0;
    do {
        do {
            // Initialize the angles of rangefinders
            float angles[19];
            d.init(angles);
            string initString = SimpleParser::stringify(string("init"), angles, 19);
            cout << "Sending id to server: " << driverId << endl;
            initString.insert(0, driverId);
            cout << "Sending init string to the server: " << initString << endl;
            if (sendto(socketDescriptor, initString.c_str(), initString.length(), 0,
                       (struct sockaddr *) &serverAddress,
                       sizeof(serverAddress)) < 0) {
                cerr << "cannot send data ";
                CLOSE(socketDescriptor);
                exit(1);
            }

            // wait until answer comes back, for up to UDP_CLIENT_TIMEOUT micro sec
            FD_ZERO(&readSet);
            FD_SET(socketDescriptor, &readSet);
            timeVal.tv_sec = 0;
            timeVal.tv_usec = UDP_CLIENT_TIMEUOT;

            if (select(socketDescriptor + 1, &readSet, NULL, NULL, &timeVal)) {
                // Read data sent by the server
                memset(buf, 0x0, UDP_MSGLEN);  // Zero out the buffer
                numRead = recv(socketDescriptor, buf, UDP_MSGLEN, 0);
                if (numRead < 0) {
                    cerr << "Didn't get response from server.";
                } else {
                    cout << "Received: " << buf << endl;

                    if (strcmp(buf, "***identified***") == 0)
                        break;
                }
            }
        } while (1);

        unsigned long currentStep = 0;

        while (1) {
            // wait until answer comes back, for up to UDP_CLIENT_TIMEOUT micro sec
            FD_ZERO(&readSet);
            FD_SET(socketDescriptor, &readSet);
            timeVal.tv_sec = 0;
            timeVal.tv_usec = UDP_CLIENT_TIMEUOT;

            if (select(socketDescriptor + 1, &readSet, NULL, NULL, &timeVal)) {
                memset(buf, 0x0, UDP_MSGLEN);  // Zero out the buffer
                numRead = recv(socketDescriptor, buf, UDP_MSGLEN, 0);
                if (numRead < 0) {
                    cerr << "didn't get response from server?";
                    CLOSE(socketDescriptor);
                    exit(1);
                }

#ifdef __UDP_CLIENT_VERBOSE__
                cout << "Received: " << buf << endl;
#endif

                if (strcmp(buf, "***shutdown***") == 0) {
                    d.onShutdown();
                    shutdownClient = true;
                    cout << "Client Shutdown" << endl;
                    break;
                }

                if (strcmp(buf, "***restart***") == 0) {
                    d.onRestart();
                    cout << "Client Restart" << endl;
                    break;
                }

                if ((++currentStep) != maxSteps) {
                    string action = d.drive(string(buf));
                    memset(buf, 0x0, UDP_MSGLEN);
                    sprintf(buf, "%s", action.c_str());
                } else
                    sprintf(buf, "(meta 1)");

                if (sendto(socketDescriptor, buf, strlen(buf) + 1, 0,
                           (struct sockaddr *) &serverAddress,
                           sizeof(serverAddress)) < 0) {
                    cerr << "cannot send data ";
                    CLOSE(socketDescriptor);
                    exit(1);
                }
#ifdef __UDP_CLIENT_VERBOSE__
                else
                    cout << "Sending " << buf << endl;
#endif
            } else {
                cout << "** Server did not respond in 1 second.\n";
            }
        }
    } while (shutdownClient == false && ((++curEpisode) != maxEpisodes));

    if (shutdownClient == false)
        d.onShutdown();
    CLOSE(socketDescriptor);
    return 0;

}

//void parse_args(int argc, char *argv[], char *hostName, unsigned int &serverPort, char *id, unsigned int &maxEpisodes,
//		  unsigned int &maxSteps,bool &noise, double &noiseAVG, double &noiseSTD, long &seed, char *trackName,
//        BaseDriver::tstage &stage)