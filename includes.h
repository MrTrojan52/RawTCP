//
// Created by trojan52 on 25.02.19.
//

#ifndef RAWTCP_INCLUDES_H
#define RAWTCP_INCLUDES_H

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


#endif //RAWTCP_INCLUDES_H
