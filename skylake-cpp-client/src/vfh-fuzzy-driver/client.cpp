#define __DRIVER_INCLUDE__ "FuzzyDriver.h" // put here the filename of your driver header
#define __DRIVER_CLASS__   FuzzyDriver     // put here the name of your driver class

#include __DRIVER_INCLUDE__
#include "core/SimpleParser.h"

#ifdef _WIN32
#define WINDOWS
#endif

#ifdef WINDOWS
#include <WinSock.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>


/*** defines for UDP *****/
#define UDP_MSGLEN 1000
#define UDP_CLIENT_TIMEUOT 1000000
//#define __UDP_CLIENT_VERBOSE__

#ifdef WINDOWS
typedef sockaddr_in tSockAddrIn;
#define CLOSE(x) closesocket(x)
#define INVALID(x) x == INVALID_SOCKET
#else
typedef int SOCKET;
typedef struct sockaddr_in tSockAddrIn;
#define CLOSE(x) close(x)
#define INVALID(x) x < 0
#endif

#include <string.h>

class __DRIVER_CLASS__;
typedef __DRIVER_CLASS__ tDriver;

void parse_args(int argc, char *argv[], std::string &serverName, unsigned int &serverPort, 
    std::string &driverId, std::string &trackName, BaseDriver::tstage &stage, 
    unsigned int &maxEpisodes, unsigned int &maxSteps, std::string &fclFile)
{
    // Set default values
    serverName  = "localhost";
    serverPort  = 3001;
    driverId    = "SCR_Fuzzy";
    trackName   = "unknown";
    maxEpisodes = 0;
    maxSteps    = 0;
    stage       = BaseDriver::UNKNOWN;
    fclFile     = "driver.fcl";

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


int main(int argc, char *argv[])
{
    // ---------------------------- config init ----------------------------

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
    std::cout << " * driver id     " << driverId   << std::endl;
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

    // ---------------------------- driver init ----------------------------

    tDriver d(stage, fclFile);

    if (!d.loadFCLfile())
    {
        std::cerr << "Cannot load FCL file" << std::endl;
        exit(1);
    }

    bool shutdownClient = false;
    unsigned long curEpisode = 0;

    // -------------------------- UDP client loop --------------------------

    do
    {
        // Initialising driver on the server

        bool already_reconnecting = false;

        do
        {
            // Set angles of rangefinders
            float angles[19];
            d.init(angles);
            std::string initString = SimpleParser::stringify(std::string("init"), angles, 19);
            initString.insert(0, driverId);

#ifdef _DEBUG
            std::cout << "Sending init: " << initString << std::endl;
#endif

            if (sendto(socketDescriptor, initString.c_str(), (int)initString.length(), 0,
                (struct sockaddr *) &serverAddress,
                sizeof(serverAddress)) < 0)
            {
                std::cerr << "Cannot send data" << std::endl;
                CLOSE(socketDescriptor);
                exit(1);
            }

            // Wait until answer comes back, for up to UDP_CLIENT_TIMEUOT micro sec
            FD_ZERO(&readSet);
            FD_SET(socketDescriptor, &readSet);
            timeVal.tv_sec = 0;
            timeVal.tv_usec = UDP_CLIENT_TIMEUOT;

            if (select((int)(socketDescriptor + 1), &readSet, NULL, NULL, &timeVal))
            {
                // Read data sent by the solorace server
                memset(buf, 0x0, UDP_MSGLEN);  // Zero out the buffer.
                numRead = recv(socketDescriptor, buf, UDP_MSGLEN, 0);
                if (numRead < 0)
                {
                    if (!already_reconnecting)
                    {
                        already_reconnecting = true;
                        std::cerr << "Didn't get response from server. Reconnecting...";
                    }
                    else
                        std::cerr << ".";

                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
                else
                {
                    if (already_reconnecting)
                        std::cout << std::endl;

                    if (std::string(buf) == "***identified***")
                    {
#ifdef _DEBUG
                        std::cout << "Driver identified" << std::endl;
#endif
                        break;
                    }
#ifdef __UDP_CLIENT_VERBOSE__
                    else
                        std::cout << "Received: " << buf << std::endl;
#endif
                }
            }

        } while (true);

        unsigned long currentStep = 0;

        // Controlling the driver

        while (true)
        {
            // Wait until answer comes back, for up to UDP_CLIENT_TIMEUOT micro sec
            FD_ZERO(&readSet);
            FD_SET(socketDescriptor, &readSet);
            timeVal.tv_sec = 0;
            timeVal.tv_usec = UDP_CLIENT_TIMEUOT;

            if (select((int)(socketDescriptor + 1), &readSet, NULL, NULL, &timeVal))
            {
                // Read data sent by the solorace server
                memset(buf, 0x0, UDP_MSGLEN);
                numRead = recv(socketDescriptor, buf, UDP_MSGLEN, 0);
                if (numRead < 0)
                {
                    std::cerr << "Didn't get response from server?" << std::endl;
                    CLOSE(socketDescriptor);
                    exit(1);
                }

#ifdef __UDP_CLIENT_VERBOSE__
                std::cout << "Received: " << buf << std::endl;
#endif

                if (std::string(buf) == "***shutdown***")
                {
#ifdef _DEBUG
                    std::cout << std::endl << "Client shutdown" << std::endl;
#endif

                    d.onShutdown();
                    shutdownClient = true;
                    break;
                }

                if (std::string(buf) == "***restart***")
                {
#ifdef _DEBUG
                    std::cout << std::endl << "Client restart" << std::endl;
#endif

                    d.onRestart();
                    break;
                }

                // Compute The Action to send to the solorace sever

                if ((++currentStep) != maxSteps)
                {
                    std::string action = d.drive(std::string(buf));
                    memset(buf, 0x0, UDP_MSGLEN);
                    snprintf(buf, UDP_MSGLEN, "%s", action.c_str());
                }
                else
                {
                    memset(buf, 0x0, UDP_MSGLEN);
                    snprintf(buf, UDP_MSGLEN, "(meta 1)");
                }

                if (sendto(socketDescriptor, buf, (int)(strlen(buf) + 1), 0,
                    (struct sockaddr *) &serverAddress,
                    sizeof(serverAddress)) < 0)
                {
                    std::cerr << "Cannot send data" << std::endl;
                    CLOSE(socketDescriptor);
                    exit(1);
                }
#ifdef __UDP_CLIENT_VERBOSE__
                else
                    std::cout << "Sending " << buf << std::endl;
#endif
            }
            else
            {
                //std::cout << "* Server did not respond in 1 second" << std::endl;
            }
        }
    } while (shutdownClient == false && ((++curEpisode) != maxEpisodes));

    if (shutdownClient == false)
        d.onShutdown();

    CLOSE(socketDescriptor);
#ifdef WINDOWS
    WSACleanup();
#endif

    return 0;
}
