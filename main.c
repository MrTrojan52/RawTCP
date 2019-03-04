#include "includes.h"

// Simple checksum function, may use others such as Cyclic Redundancy Check, CRC

unsigned short csum(unsigned short *buf, int len)

{

    unsigned long sum;

    for(sum=0; len>0; len--)

        sum += *buf++;

    sum = (sum >> 16) + (sum &0xffff);

    sum += (sum >> 16);

    return (unsigned short)(~sum);

}

void build_packet(char* buffer,const char* sip, const char* sport, const char* dip, const char* dport, enum tcp_flags flags, const char* data, u_int16_t dataLen)
{
    char buff2[512];

    struct ipheader *ip = (struct ipheader *) buffer;

    struct tcpheader *tcp = (struct tcpheader *) (buffer + sizeof(struct ipheader));

    struct tcpheaderOptions *tcpOptions = (struct tcpheaderOptions *) (buffer + sizeof(struct ipheader) + sizeof(struct tcpheader));

    struct pseudo_tcp *tcp_pseudo = (struct pseudo_tcp *) buff2;

    // IP structure

    ip->version = 4;

    ip->IHL = 5;

    ip->typeOfService = 16;

    ip->totalLength = sizeof(struct ipheader) + sizeof(struct tcpheader) + sizeof(struct tcpheaderOptions);

    ip->identification = htons(54321);

    ip->fragmentOffset = 0;

    ip->timeToLive = 64;

    ip->protocol = 6; // TCP

    ip->headerChecksum = 0; // Done by kernel



// Source IP, modify as needed, spoofed, we accept through command line argument

    ip->sourceIP = inet_addr(sip);

// Destination IP, modify as needed, but here we accept through command line argument

    ip->destinationIP = inet_addr(dip);



// The TCP structure. The source port, spoofed, we accept through the command line

    tcp->sourcePort = htons(atoi(sport));

// The destination port, we accept through command line

    tcp->destinationPort = htons(atoi(dport));

    tcp->sequenceNumber = htonl(1);

    tcp->acknowledgeNumber = 0;

    tcp->dataOffset = 10;

    tcp->reserved = 0;

    tcp->controlBits = flags;

    tcp->window = htons(32768);

    tcp->checkSum = 0; // Done by kernel

    tcp->urgentPointer = 0;

// TCP OPTIONS

    tcpOptions->tcph_mssOpt       = 2;

    tcpOptions->tcph_mssLen       = 4;

    tcpOptions->tcph_winOpt       = 3;

    tcpOptions->tcph_winLen       = 3;

    tcpOptions->tcph_sack         = 4;

    tcpOptions->tcph_sackLen      = 2;

    tcpOptions->tcph_win          = 7;

    tcpOptions->tcph_winNOP       = 1;

    tcpOptions->tcph_mss          = htons(1460);

    tcpOptions->tcph_timeOpt      = 8;

    tcpOptions->tcph_timeLen      = 10;

    tcpOptions->tcph_time         = 0xdb2b0d00;

    // IP checksum calculation
    if(data) {
        strcpy(buffer + ip->totalLength, data);
        ip->totalLength += dataLen;
    }

    tcp_pseudo->protocol = 6;
    tcp_pseudo->sourceIP = inet_addr(sip);
    tcp_pseudo->destIP = inet_addr(dip);
    tcp_pseudo->res = 0;
    tcp_pseudo->tcpLength = htons(ip->totalLength - sizeof(struct ipheader));
    memcpy(buff2 + sizeof(struct pseudo_tcp), tcp, ip->totalLength - sizeof(struct ipheader));
    tcp->checkSum = csum((unsigned short *)buff2, sizeof(struct pseudo_tcp) + ip->totalLength - sizeof(struct ipheader));
}

void send_package(const char* sip, const char* sport, const char* dip, const char* dport)
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

    din.sin_port = htons(atoi(dport));

    din.sin_addr.s_addr = inet_addr(dip);

// Inform the kernel do not fill up the headers' structure, we fabricated our own

    if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)

    {

        perror("setsockopt() error");

        exit(-1);

    }

    else

        printf("setsockopt() is OK\n");



    printf("Using:::::Source IP: %s port: %u, Target IP: %s port: %u.\n", sip, atoi(sport), dip, atoi(dport));


    if(sendto(sd, send_buffer, ((struct ipheader*)send_buffer)->totalLength, 0, (struct sockaddr *)&din, sizeof(din)) < 0)
    {

        perror("sendto() error");

        exit(-1);

    } else {
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

    int bytes = recv(sd, buffer, PCKT_LEN, 0);
    printf("Received %d bytes", bytes);

}


int main(int argc, char *argv[])

{
    if(argc == 2)
    {
        start_server(argv[1]);

    } else if (argc == 5) {
        send_package(argv[1], argv[2], argv[3], argv[4]);
    } else {
        printf("- Invalid parameters!!!\n");

        printf("- Usage: %s <source hostname/IP> <source port> <target hostname/IP> <target port>\n", argv[0]);

        exit(-1);
    }

    return 0;

}
