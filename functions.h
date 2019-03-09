//
// Created by trojan52 on 04.03.19.
//

#ifndef RAWTCP_FUNCTIONS_H
#define RAWTCP_FUNCTIONS_H

#include "includes.h"

    void build_packet(char* buffer,in_addr_t sip, u_int16_t sport, in_addr_t dip, u_int16_t dport, TCP_Flags flags, const char* data, unsigned short dataLen, u_int32_t seq, u_int32_t ack);
    unsigned short csum(unsigned short *buf, int len);
    void* wait_tcp_packet_with_flag(int __fd, void* buffer, size_t size, int flags, TCP_Flags tcpFlags, unsigned short port);
    void set_server_status(TCP_Status status);
    const char* toString(TCP_Status status);
#endif //RAWTCP_FUNCTIONS_H
