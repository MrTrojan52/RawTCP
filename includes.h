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

#define TCPFlag_FIN 1
#define TCPFlag_SYN 2
#define TCPFlag_RST 4
#define TCPFlag_PSH 8
#define TCPFlag_ACK 16
#define TCPFlag_URG 32
#define TCPFlag_ECE 64
#define TCPFlag_CWR 128
#define TCPFlag_NS  256

//TODO: Исправить косяк с битовыми полями

#define PCKT_LEN 8192


struct ipheader {
#if __BYTE_ORDER == BIG_ENDIAN

    u_int8_t           version:4;

    u_int8_t           IHL:4;

    u_int8_t           typeOfService;

    u_int16_t          totalLength;

    u_int16_t          identification;

    u_int16_t          flags:3;

    u_int16_t          fragmentOffset:13;

    u_int8_t           timeToLive;

    u_int8_t           protocol;

    u_int16_t          headerChecksum;

    u_int32_t          sourceIP;

    u_int32_t          destinationIP;
#elif __BYTE_ORDER == LITTLE_ENDIAN

    u_int8_t           IHL:4;

    u_int8_t           version:4;

    u_int8_t           typeOfService;

    u_int16_t          totalLength;

    u_int16_t          identification;

    u_int16_t          fragmentOffset:13;

    u_int16_t          flags:3;

    u_int8_t           timeToLive;

    u_int8_t           protocol;

    u_int16_t          headerChecksum;

    u_int32_t          sourceIP;

    u_int32_t          destinationIP;

#else
#error MIDDLE-ENDIAN SYSTEM PIZDEC
#endif

};



/* Structure of a TCP header */

struct tcpheader {
    u_int16_t           sourcePort;
    u_int16_t           destinationPort;
    u_int32_t           sequenceNumber;
    u_int32_t           acknowledgeNumber;
#if __BYTE_ORDER == BIG_ENDIAN
    u_int16_t           dataOffset:4;
    u_int16_t           reserved:3;
    u_int16_t           controlBits:9;
#elif __BYTE_ORDER == LITTLE_ENDIAN
    u_int16_t           controlBits:9;
    u_int16_t           reserved:3;
    u_int16_t           dataOffset:4;
#else
#error MIDDLE-ENDIAN SYSTEM PIZDEC
#endif
    u_int16_t           window;
    u_int16_t           checkSum;
    u_int16_t           urgentPointer;
};

struct tcpheaderOptions
{

    u_int16_t
            tcph_mssOpt:8,
            tcph_mssLen:8;
    u_int16_t
            tcph_mss;
    u_int16_t
            tcph_sack:8,
            tcph_sackLen:8;
    u_int16_t
            tcph_winOpt:8,
            tcph_winLen:8;
    u_int32_t
            tcph_win:8,
            tcph_winNOP:8,
            tcph_timeOpt:8,
            tcph_timeLen:8;
    u_int32_t tcph_time;
    u_int32_t tcph_timeEcho;
};


#endif //RAWTCP_INCLUDES_H
