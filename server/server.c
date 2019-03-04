#include "../functions.h"

TCP_Status SERVER_STATUS = CLOSED;

void start_server(const char* port)
{
    int sd;

    char buffer[PCKT_LEN];

    struct ipheader *ip = (struct ipheader *) buffer;

    struct tcpheader *tcp = (struct tcpheader *) (buffer + sizeof(struct ipheader));

    struct tcpheaderOptions *tcpOptions = (struct tcpheaderOptions *) (buffer + sizeof(struct ipheader) + sizeof(struct tcpheader));

    struct sockaddr_in sin;


    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_family = AF_PACKET;
    sin.sin_port = htons(atoi(port));

    sd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sd < 0)

    {

        perror("socket() error");

        exit(-1);

    }

    else

        printf("socket()-SOCK_RAW and tcp protocol is OK.\n");
    int one = 1;

    const int *val = &one;

    if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)

    {

        perror("setsockopt() error");

        exit(-1);

    }

    else

        printf("setsockopt() is OK\n");


    bind(sd, (struct sockaddr*)&sin, sizeof(sin));
    while(1)
    {
        SERVER_STATUS = LISTEN;
        int bytes = recv(sd, buffer, PCKT_LEN, 0);
        if(tcp->destinationPort == htons(atoi(port)))
        {
            if(tcp->controlBits == TCPFlag_SYN) {
                build_packet(buffer, ip->destinationIP, htons(atoi(port)), ip->sourceIP, tcp->sourcePort, TCPFlag_SYN | TCPFlag_ACK, NULL, 0);
            }
        }

    }

}

int main(int argc, char *argv[])

{
    if(argc == 2)
    {
        start_server(argv[1]);

    } else {
        printf("- Invalid parameters!!!\n");

        printf("- Usage: %s  <port>\n", argv[0]);

        exit(-1);
    }

    return 0;

}

