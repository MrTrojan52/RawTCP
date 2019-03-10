#include "../functions.h"

int accept_client(in_addr_t dip, u_int16_t dport, in_addr_t sip, u_int16_t sport);

TCP_Status SERVER_STATUS = CLOSED;

u_int32_t ACK_NUM = 0;
u_int32_t SEQ_NUM = 1;

void start_server(const char* port)
{
    int sd;

    char recv_buffer[PCKT_LEN];

    struct ipheader *ip = (struct ipheader *) recv_buffer;

    struct tcpheader *tcp = (struct tcpheader *) (recv_buffer + sizeof(struct ipheader));

    struct tcpheaderOptions *tcpOptions = (struct tcpheaderOptions *) (recv_buffer + sizeof(struct ipheader) + sizeof(struct tcpheader));

    struct sockaddr_in sin, din;


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


    if(bind(sd, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    {
        perror("bind error");
        exit(-1);
    };


    set_status(&SERVER_STATUS, LISTEN);
    wait_tcp_packet_with_flag(sd, recv_buffer, PCKT_LEN, 0, TCPFlag_SYN, htons(atoi(port)));

    set_status(&SERVER_STATUS, SYN_RECEIVED);
    printf("\nSYN: Client request connection\n");
    ACK_NUM = tcp->acknowledgeNumber + 1;
    accept_client(ip->destinationIP, htons(atoi(port)), ip->sourceIP, tcp->sourcePort);

}

void disconnect(int sock, in_addr_t dip, u_int16_t dport, in_addr_t sip, u_int16_t sport)
{
    char send_buffer[PCKT_LEN];

    char receive_buffer[PCKT_LEN];
    struct sockaddr_in din;

    din.sin_family = AF_INET;

    din.sin_port = dport;

    din.sin_addr.s_addr = dip;

    memset(send_buffer, 0, PCKT_LEN);

    build_packet(send_buffer, sip, sport, dip, dport, TCPFlag_ACK, NULL, 0, ++SEQ_NUM, ACK_NUM);
    sendto(sock, send_buffer, ((struct ipheader*)send_buffer)->totalLength, 0, (struct sockaddr *)&din, sizeof(din));
    set_status(&SERVER_STATUS, CLOSE_WAIT);

    memset(send_buffer, 0, PCKT_LEN);

    build_packet(send_buffer, sip, sport, dip, dport, TCPFlag_FIN, NULL, 0, ++SEQ_NUM, ACK_NUM);
    sendto(sock, send_buffer, ((struct ipheader*)send_buffer)->totalLength, 0, (struct sockaddr *)&din, sizeof(din));
    set_status(&SERVER_STATUS, LAST_ACK);

    memset(receive_buffer, 0, PCKT_LEN);
    wait_tcp_packet_with_flag(sock, receive_buffer, PCKT_LEN, 0, TCPFlag_ACK, sport);
    ACK_NUM = ((struct tcpheader*)( receive_buffer + sizeof(struct ipheader)))->sequenceNumber + 1;
    set_status(&SERVER_STATUS, CLOSED);
}

int accept_client(in_addr_t dip, u_int16_t dport, in_addr_t sip, u_int16_t sport)
{
    char buf[PCKT_LEN];

    memset(buf, 0, PCKT_LEN);

    struct ipheader *ip = (struct ipheader *) buf;
    int csock = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    if (csock < 0)
    {
        perror("socket() error");

        exit(-1);
    }
    struct sockaddr_in din;

    din.sin_family = AF_INET;
    din.sin_port = sport;
    din.sin_addr.s_addr = sip;

    int one = 1;

    const int *val = &one;

    if(setsockopt(csock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)

    {

        perror("setsockopt() error");

        exit(-1);

    }

    if(bind(csock, (struct sockaddr*)&din, sizeof(din)) < 0)
    {
        perror("bind() error");

        exit(-1);
    }

    build_packet(buf, dip, dport, sip, sport, TCPFlag_SYN | TCPFlag_ACK, NULL, 0, SEQ_NUM, ACK_NUM);
    if(sendto(csock, buf, ip->totalLength, 0, (struct sockaddr *)&din, sizeof(din)) < 0)
    {
        perror("sendto() error");

        exit(-1);
    } else {

        printf("SYN-ACK: Send to client\n");
    }
    memset(buf, 0, PCKT_LEN);
    int flag = wait_tcp_packet_with_flag(csock, buf, PCKT_LEN, 0, TCPFlag_ACK, sport);

    ACK_NUM = ((struct tcpheader*)( buf + sizeof(struct ipheader)))->sequenceNumber + 1;
    if( flag == TCPFlag_FIN ) {
        disconnect(csock, dip, dport, sip, sport);
        return -1;
    }
    set_status(&SERVER_STATUS, ESTABLISHED);
    return csock;
}


int main(int argc, char *argv[])

{
    if(argc == 2)
    {
        start_server(argv[1]);

    } else {
        printf("- Invalid parameters!!!\n");

        printf("- Usage: %s  <port>\n", argv[0]);

        exit(-1);
    }

    return 0;

}

