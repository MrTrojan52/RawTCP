#include "../functions.h"

u_int32_t SEQ_NUM = 0;
u_int32_t ACK_NUM = 0;

TCP_Status CLIENT_STATUS = CLOSED;

int connect_to_server(in_addr_t sip, u_int16_t sport, in_addr_t dip, u_int16_t dport)
{
    int sd;

    char send_buffer[PCKT_LEN];

    char receive_buffer[PCKT_LEN];

    memset(send_buffer, 0, PCKT_LEN);

    build_packet(send_buffer, sip, sport, dip, dport, TCPFlag_SYN, NULL, 0, ++SEQ_NUM, ACK_NUM);

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

    din.sin_port = dport;

    din.sin_addr.s_addr = dip;

// Inform the kernel do not fill up the headers' structure, we fabricated our own

    if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)

    {

        perror("setsockopt() error");

        exit(-1);

    }

    else

        printf("setsockopt() is OK\n");



    printf("Using:::::Source:  port: %u, Target: port: %u.\n", ntohs(sport), ntohs(dport));


    if(sendto(sd, send_buffer, ((struct ipheader*)send_buffer)->totalLength, 0, (struct sockaddr *)&din, sizeof(din)) < 0)
    {

        perror("sendto() error");

        exit(-1);

    } else {
        set_status(&CLIENT_STATUS, SYN_SENT);
    }


    memset(receive_buffer, 0, PCKT_LEN);
    wait_tcp_packet_with_flag(sd, receive_buffer, PCKT_LEN, 0, TCPFlag_SYN | TCPFlag_ACK, sport);
    struct ipheader* ip = (struct ipheader*) receive_buffer;
    struct tcpheader* tcp = (struct tcpheader*) (receive_buffer + sizeof(struct ipheader));
    ACK_NUM = tcp->sequenceNumber + 1;
    memset(send_buffer, 0, PCKT_LEN);
    build_packet(send_buffer, sip, sport, dip, dport, TCPFlag_ACK, NULL, 0, ++SEQ_NUM, ACK_NUM);
    sendto(sd, send_buffer, ((struct ipheader*)send_buffer)->totalLength, 0, (struct sockaddr *)&din, sizeof(din));
    set_status(&CLIENT_STATUS, ESTABLISHED);
    return sd;

}

void terminate_connection(int sock, in_addr_t sip, u_int16_t sport, in_addr_t dip, u_int16_t dport)
{
    char send_buffer[PCKT_LEN];

    char receive_buffer[PCKT_LEN];
    struct sockaddr_in din;

    din.sin_family = AF_INET;

    din.sin_port = dport;

    din.sin_addr.s_addr = dip;

    memset(send_buffer, 0, PCKT_LEN);

    build_packet(send_buffer, sip, sport, dip, dport, TCPFlag_FIN, NULL, 0, ++SEQ_NUM, ACK_NUM);
    sendto(sock, send_buffer, ((struct ipheader*)send_buffer)->totalLength, 0, (struct sockaddr *)&din, sizeof(din));
    set_status(&CLIENT_STATUS, FIN_WAIT1);
    memset(receive_buffer, 0, PCKT_LEN);
    wait_tcp_packet_with_flag(sock, receive_buffer, PCKT_LEN, 0, TCPFlag_ACK, sport);
    ACK_NUM = ((struct tcpheader*)( receive_buffer + sizeof(struct ipheader)))->sequenceNumber + 1;
    set_status(&CLIENT_STATUS, FIN_WAIT2);
    memset(receive_buffer, 0, PCKT_LEN);
    wait_tcp_packet_with_flag(sock, receive_buffer, PCKT_LEN, 0, TCPFlag_FIN, sport);
    ACK_NUM = ((struct tcpheader*)( receive_buffer + sizeof(struct ipheader)))->sequenceNumber + 1;
    memset(send_buffer, 0, PCKT_LEN);

    build_packet(send_buffer, sip, sport, dip, dport, TCPFlag_ACK, NULL, 0, ++SEQ_NUM, ACK_NUM);
    sendto(sock, send_buffer, ((struct ipheader*)send_buffer)->totalLength, 0, (struct sockaddr *)&din, sizeof(din));
    set_status(&CLIENT_STATUS, CLOSED);
}




int main(int argc, char *argv[])

{
    if (argc == 5) {
        set_status(&CLIENT_STATUS, CLOSED);
        int sock = connect_to_server(inet_addr(argv[1]), htons(atoi(argv[2])), inet_addr(argv[3]), htons(atoi(argv[4])));
        terminate_connection(sock,inet_addr(argv[1]), htons(atoi(argv[2])), inet_addr(argv[3]), htons(atoi(argv[4])));
    } else {
        printf("- Invalid parameters!!!\n");

        printf("- Usage: %s <source hostname/IP> <source port> <target hostname/IP> <target port>\n", argv[0]);

        exit(-1);
    }

    return 0;

}
