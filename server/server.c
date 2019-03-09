#include "../functions.h"

TCP_Status SERVER_STATUS = CLOSED;

void start_server(const char* port)
{
    int sd;

    char recv_buffer[PCKT_LEN];

    char send_buffer[PCKT_LEN];

    struct ipheader *ip = (struct ipheader *) recv_buffer;

    struct tcpheader *tcp = (struct tcpheader *) (recv_buffer + sizeof(struct ipheader));

    struct tcpheaderOptions *tcpOptions = (struct tcpheaderOptions *) (recv_buffer + sizeof(struct ipheader) + sizeof(struct tcpheader));

    struct sockaddr_in sin, din;


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
        int bytes = recv(sd, recv_buffer, PCKT_LEN, 0);
        if(tcp->destinationPort == htons(atoi(port)))
        {
            if(tcp->controlBits == TCPFlag_SYN) {
                printf("\nSYN: Client request connection\n");
                din.sin_family = AF_INET;
                din.sin_port = tcp->sourcePort;
                din.sin_addr.s_addr = ip->sourceIP;
                memset(send_buffer, 0, PCKT_LEN);
                build_packet(send_buffer, ip->destinationIP, htons(atoi(port)), ip->sourceIP, tcp->sourcePort, TCPFlag_SYN | TCPFlag_ACK, NULL, 0);
                struct ipheader* send_ip = (struct ipheader*)send_buffer;
                if(sendto(sd, send_buffer, send_ip->totalLength, 0, (struct sockaddr *)&din, sizeof(din)) < 0)
                {
                    perror("sendto() error");

                    exit(-1);
                } else {
                    SERVER_STATUS = SYN_RECEIVED;
                    printf("SYN-ACK: Send to client\n");
                }
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

