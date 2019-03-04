//
// Created by trojan52 on 04.03.19.
//

#ifndef RAWTCP_FUNCTIONS_H
#define RAWTCP_FUNCTIONS_H

#include "includes.h"

    void build_packet(char* buffer,const char* sip, const char* sport, const char* dip, const char* dport, enum tcp_flags flags, const char* data, unsigned short dataLen);
    unsigned short csum(unsigned short *buf, int len);

#endif //RAWTCP_FUNCTIONS_H
