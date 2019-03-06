# Modeling the TCP protocol using Raw Sockets

## IP Header
![IP Header](/images/IP-header.png)

## TCP Header
![TCP Header](/images/TCP-header.png)

## Problems
If you receiving a packet with **RST** flag right after sending a packet with **SYN** flag use this commands:
* Delete TCP packet with RST flag  
`iptables -I OUTPUT 1 -d <destination> -p tcp --tcp-flags RST RST -j DROP`  
* Rollback  
`iptables -D OUTPUT -d <destination> -p tcp --tcp-flags RST RST -j DROP`  

or this: 
* Block input packets with **SYN, ACK**  
`iptables -I INPUT 1 -s <source> -p tcp --tcp-flags SYN,ACK SYN,ACK -j DROP`  
* Rollback  
`iptables -D INPUT -s <source> -p tcp --tcp-flags SYN,ACK SYN,ACK -j DROP`
