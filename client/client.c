#include "../functions.h"

u_int32_t SEQ_NUM = 1;
u_int32_t ACK_NUM = 0;

TCP_Status CLIENT_STATUS = CLOSED;

void send_package(in_addr_t sip, u_int16_t sport, in_addr_t dip, u_int16_t dport)
{
    int sd;

    char send_buffer[PCKT_LEN];

    char receive_buffer[PCKT_LEN];

    memset(send_buffer, 0, PCKT_LEN);

    build_packet(send_buffer, sip, sport, dip, dport, TCPFlag_SYN, NULL, 0, SEQ_NUM, ACK_NUM);

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
    sleep(100);
    //close(sd);

}




int main(int argc, char *argv[])

{
    if (argc == 5) {
        send_package(inet_addr(argv[1]), htons(atoi(argv[2])), inet_addr(argv[3]), htons(atoi(argv[4])));
    } else {
        printf("- Invalid parameters!!!\n");

        printf("- Usage: %s <source hostname/IP> <source port> <target hostname/IP> <target port>\n", argv[0]);

        exit(-1);
    }

    return 0;

}
