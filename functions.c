// Simple checksum function, may use others such as Cyclic Redundancy Check, CRC
#include "functions.h"
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