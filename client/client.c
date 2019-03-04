#include "../functions.h"

TCP_Status CLIENT_STATUS = CLOSED;

void send_package(in_addr_t sip, u_int16_t sport, in_addr_t dip, u_int16_t dport)
{
    int sd;

    char send_buffer[PCKT_LEN];

    char receive_buffer[PCKT_LEN];

    memset(send_buffer, 0, PCKT_LEN);

    build_packet(send_buffer, sip, sport, dip, dport, TCPFlag_SYN, NULL, 0);

    struct sockaddr_in din;

    int one = 1;

    const int *val = &one;

    sd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);

    if(sd < 0)

    {

        perror("socket() error");

        exit(-1);

    }

    else

        printf("socket()-SOCK_RAW and tcp protocol is OK.\n");


    din.sin_family = AF_INET;

    din.sin_port = dport;

    din.sin_addr.s_addr = dip;

// Inform the kernel do not fill up the headers' structure, we fabricated our own

    if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)

    {

        perror("setsockopt() error");

        exit(-1);

    }

    else

        printf("setsockopt() is OK\n");



    printf("Using:::::Source:  port: %u, Target: port: %u.\n", ntohs(sport), ntohs(dport));


    if(sendto(sd, send_buffer, ((struct ipheader*)send_buffer)->totalLength, 0, (struct sockaddr *)&din, sizeof(din)) < 0)
    {

        perror("sendto() error");

        exit(-1);

    } else {
        CLIENT_STATUS = SYN_SENT;
        printf(" sendto() is OK\n");
    }


    memset(receive_buffer, 0, PCKT_LEN);
    int bytes_received = recv(sd, receive_buffer, PCKT_LEN, 0);
    struct ipheader* ip = (struct ipheader*) receive_buffer;
    struct tcpheader* tcp = (struct tcpheader*) (receive_buffer + sizeof(struct ipheader));
    if(tcp->controlBits == TCPFlag_SYN | TCPFlag_ACK)
    {
        struct tcpheader* sndtcp = (struct tcpheader*)(send_buffer + sizeof(struct ipheader));
        sndtcp->controlBits = TCPFlag_ACK;
        sndtcp->acknowledgeNumber = tcp->sequenceNumber;
        sndtcp->sequenceNumber = htonl(2);
        sendto(sd, send_buffer, ((struct ipheader*)send_buffer)->totalLength, 0, (struct sockaddr *)&din, sizeof(din));
    }
    sleep(100);
    //close(sd);

}




int main(int argc, char *argv[])

{
    if (argc == 5) {
        send_package(inet_addr(argv[1]), htons(atoi(argv[2])), inet_addr(argv[3]), htons(atoi(argv[4])));
    } else {
        printf("- Invalid parameters!!!\n");

        printf("- Usage: %s <source hostname/IP> <source port> <target hostname/IP> <target port>\n", argv[0]);

        exit(-1);
    }

    return 0;

}
