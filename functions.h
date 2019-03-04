//
// Created by trojan52 on 04.03.19.
//

#ifndef RAWTCP_FUNCTIONS_H
#define RAWTCP_FUNCTIONS_H

#include "includes.h"

    void build_packet(char* buffer,in_addr_t sip, u_int16_t sport, in_addr_t dip, u_int16_t dport, TCP_Flags flags, const char* data, unsigned short dataLen);
    unsigned short csum(unsigned short *buf, int len);

#endif //RAWTCP_FUNCTIONS_H
