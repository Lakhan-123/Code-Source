#define usi unsigned short int 
#define uc unsigned char
#define ui unsigned int

typedef struct ipHeader {
    uc ip_ihl:5, ip_ver:4;
    uc ip_tos;
    usi ip_length;
    usi ip_ident;
    uc ip_flag;
    usi ip_fragOff;
    uc ip_ttl;
    uc ip_protocol;
    usi ip_checksum;
    ui ip_sIP, ip_dIP;
}iph;

typedef struct udpHeader {
    usi udp_sPort, udp_dPort;
    usi udp_length, udp_checksum;
}udph;