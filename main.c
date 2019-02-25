#include <unistd.h>

#include <stdio.h>

#include <sys/socket.h>

#include <netinet/ip.h>

#include <netinet/tcp.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define TCPFlag_URG 0
#define TCPFlag_ACK 2
#define TCPFlag_PSH 4
#define TCPFlag_RST 8
#define TCPFlag_SYN 16
#define TCPFlag_FYN 32


#define PCKT_LEN 8192


struct ipheader {
#if __BYTE_ORDER == BIG_ENDIAN

    unsigned char      version:4;

    unsigned char      IHL:4;

    unsigned char      typeOfService;

    unsigned short     totalLength;

    unsigned short     identification;

    unsigned short     flags:3;

    unsigned short     fragmentOffset:13;

    unsigned char      timeToLive;

    unsigned char      protocol;

    unsigned short     headerChecksum;

    unsigned int       sourceIP;

    unsigned int       destinationIP;
#elif __BYTE_ORDER == LITTLE_ENDIAN

    unsigned char      IHL:4;

    unsigned char      version:4;

    unsigned char      typeOfService;

    unsigned short     totalLength;

    unsigned short     identification;

    unsigned short     fragmentOffset:13;

    unsigned short     flags:3;

    unsigned char      timeToLive;

    unsigned char      protocol;

    unsigned short     headerChecksum;

    unsigned int       sourceIP;

    unsigned int       destinationIP;

#else
    #error MIDDLE-ENDIAN SYSTEM PIZDEC
#endif

};



/* Structure of a TCP header */

struct tcpheader {

    unsigned short     sourcePort;

    unsigned short     destinationPort;

    unsigned int       sequenceNumber;

    unsigned int       acknowledgeNumber;

#if __BYTE_ORDER == BIG_ENDIAN

    unsigned int       dataOffset:4;

    unsigned int       reserved:6;

    unsigned int       controlBits:6;

    unsigned int       window:16;
#elif __BYTE_ORDER == LITTLE_ENDIAN

    unsigned int       window:16;

    unsigned int       controlBits:6;

    unsigned int       reserved:6;

    unsigned int       dataOffset:4;
#else
    #error MIDDLE-ENDIAN SYSTEM PIZDEC
#endif
    
    unsigned short     checkSum;

    unsigned short     urgentPointer;

};



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

    ip->totalLength = sizeof(struct ipheader) + sizeof(struct tcpheader);

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

    tcp->dataOffset = 5;

    tcp->controlBits = TCPFlag_SYN;

    tcp->window = htons(16*1024);

    tcp->checkSum = 0; // Done by kernel

    tcp->urgentPointer = 0;

// IP checksum calculation

    ip->headerChecksum = csum((unsigned short *) buffer, (sizeof(struct ipheader) + sizeof(struct tcpheader)));



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
