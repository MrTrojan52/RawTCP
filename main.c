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



int main(int argc, char *argv[])

{

    int sd;

    char buffer[PCKT_LEN];

    struct ipheader *ip = (struct ipheader *) buffer;

    struct tcpheader *tcp = (struct tcpheader *) (buffer + sizeof(struct ipheader));

    struct tcpheaderOptions *tcpOptions = (struct tcpheaderOptions *) (buffer + sizeof(struct ipheader) + sizeof(struct tcpheader));

    struct sockaddr_in sin, din;

    int one = 1;

    const int *val = &one;



    memset(buffer, 0, PCKT_LEN);



    if(argc != 5)

    {

        printf("- Invalid parameters!!!\n");

        printf("- Usage: %s <source hostname/IP> <source port> <target hostname/IP> <target port>\n", argv[0]);

        exit(-1);

    }



    sd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);

    if(sd < 0)

    {

        perror("socket() error");

        exit(-1);

    }

    else

        printf("socket()-SOCK_RAW and tcp protocol is OK.\n");



// The source is redundant, may be used later if needed

// Address family

    sin.sin_family = AF_INET;

    din.sin_family = AF_INET;

// Source port, can be any, modify as needed

    sin.sin_port = htons(atoi(argv[2]));

    din.sin_port = htons(atoi(argv[4]));

// Source IP, can be any, modify as needed

    sin.sin_addr.s_addr = inet_addr(argv[1]);

    din.sin_addr.s_addr = inet_addr(argv[3]);

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

    ip->sourceIP = inet_addr(argv[1]);

// Destination IP, modify as needed, but here we accept through command line argument

    ip->destinationIP = inet_addr(argv[3]);



// The TCP structure. The source port, spoofed, we accept through the command line

    tcp->sourcePort = htons(atoi(argv[2]));

// The destination port, we accept through command line

    tcp->destinationPort = htons(atoi(argv[4]));

    tcp->sequenceNumber = htonl(1);

    tcp->acknowledgeNumber = 0;

    tcp->dataOffset = 10;

    tcp->reserved = 0;

    tcp->controlBits = TCPFlag_FIN;

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
    buffer[ip->totalLength++] = 'T';
    buffer[ip->totalLength++] = 'E';
    buffer[ip->totalLength++] = 'S';
    buffer[ip->totalLength++] = 'T';
//    ip->headerChecksum = csum((unsigned short *) buffer, ip->totalLength);

// Inform the kernel do not fill up the headers' structure, we fabricated our own

    if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)

    {

        perror("setsockopt() error");

        exit(-1);

    }

    else

        printf("setsockopt() is OK\n");



    printf("Using:::::Source IP: %s port: %u, Target IP: %s port: %u.\n", argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));



// sendto() loop, send every 2 second for 50 counts

    unsigned int count;

    for(count = 0; count < 20; count++)

    {

        if(sendto(sd, buffer, ip->totalLength, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        {

            perror("sendto() error");

            exit(-1);

        } else {
            printf("Count #%u - sendto() is OK\n", count);
        }

        sleep(2);

    }

    close(sd);

    return 0;

}
