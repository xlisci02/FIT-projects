/*
*  File: ipk-dhcpstarve.h
*  IPK 2018 - 2.Projekt - DHCP Starvation attack
*  Autor: Matus Liscinsky
*  Login: xlisci02
*  Mail: xlisci02@stud.fit.vutbr.cz
*  Datum: 9.4.2018
*/

#ifndef DHCP_STARVE_H
#define DHCP_STARVE_H

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <time.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <signal.h>

#define SUCCESS 0
#define ERR -1

#define CHADDR_LEN 16
#define SNAME_LEN 64
#define FILE_LEN 128
#define OPTIONS_LEN 8 

#define BOOTREQUEST 1
#define ETHERNET_HARDWARE_ADDRESS_TYPE 1
#define ETHERNET_HARDWARE_ADDRESS_LENGTH 6
#define BROADCAST_FLAG 32768 // (10000000 00000000), only first bit is set 
#define MESSAGE_TYPE 53
#define DHCP_DISCOVER 1
#define BROADCAST_MAC 0xFF
#define BUFSIZE 1024

/*
* http://www.tcpipguide.com/free/t_DHCPMessageFormat.htm
* https://tools.ietf.org/html/rfc2131
*/

/* struktura DHCP packetu */ 
/**
 * @brief struktura DHCP packetu
 * 
 */
typedef struct dhcp_packet_struct{
	uint8_t  op;                   
	uint8_t  htype;                
    uint8_t  hlen;                 
    uint8_t  hops;                 
    uint32_t xid;                  
    uint16_t secs;                 
    uint16_t flags;                
    struct in_addr ciaddr;          
    struct in_addr yiaddr;          
    struct in_addr siaddr;          
    struct in_addr giaddr;          
    unsigned char chaddr [CHADDR_LEN];      
    char sname [SNAME_LEN];    
    char file [FILE_LEN];      
	char options[OPTIONS_LEN];  
}dhcp_packet;

/**
 * @brief ziska index rozhrania
 * 
 * @param if_idx ukazatel na strukturu pre ulozenie indexu
 * @param socket socket
 * @param interface retazec identifikujuci rozhranie
 * @return vracia 0, pri chybe -1
 */
int get_interface_index( struct ifreq * if_idx, int socket, char * interface );

/**
 * @brief generuje MAC adresu
 * 
 * @param mac retazec kam sa ulozi vygenerovana MAC adresa
 */
void generate_mac_addr( unsigned char * mac );

/**
 * @brief ziska MAC adresu rozhrania
 * 
 * @param if_mac struktura pre ulozenie MAC adresy
 * @param socket socket
 * @param interface retazec identifikujuci rozhranie
 * @return vracia 0, pri chybe -1
 */
int get_interface_address( struct ifreq * if_mac, int socket, char * interface );

/**
 * @brief generuje, zapise ethernet hlavicku do packetu/bufferu
 * 
 * @param if_mac struktura, kde je ulozena MAC adresa rozhrania
 * @param pack_len aktualna dlzka zapisanych dat v bufferi (akt. dlzka packetu)
 * @param sendbuf buffer pre packet
 */
void generate_ethernet_header( struct ifreq * if_mac, int * pack_len, char * sendbuf );

/**
 * @brief generuje, zapise IP hlavicku do packetu/bufferu
 * 
 * @param pack_len aktualna dlzka zapisanych dat v bufferi (akt. dlzka packetu)
 * @param sendbuf buffer pre packet
 * 
 * @return ukazatel na IP hlavicku v packete
 */
struct iphdr * generate_ip_header( int * pack_len, char * sendbuf );

/**
 * @brief generuje, zapise IP hlavicku do packetu/bufferu
 * 
 * @param pack_len aktualna dlzka zapisanych dat v bufferi (akt. dlzka packetu)
 * @param sendbuf buffer pre packet
 * 
 * @return ukazatel na UDP hlavicku
 */
struct udphdr * generate_udp_header ( int * pack_len, char * sendbuf );

/**
 * @brief spocita checksum (kontrolny sucet)
 * @details 
 * Title: UDP Raw packets
 * Author: Raheem A. Beyah, Ph.D.
 * Availability: http://rbeyah.ece.gatech.edu/classes/spring2013/ece4110/handouts/rawudp.c
 * @return IPv4 header checksum
 */
unsigned short csum( unsigned short *buf, int nwords );

/**
 * @brief Naplni data(payload) v packete 
 * 
 * @param pack_len aktualna dlzka zapisanych dat v bufferi (akt. dlzka packetu)
 * @param sendbuf buffer pre packet
 * @return ukazatel na strukturu dat(payload) v packete
 */
dhcp_packet * fill_dhcp_discover( int * pack_len, char * sendbuf );

/**
 * @brief Doplni UDP a IP hlavicku o potrebne informacie
 * 
 * @param pack_len aktualna dlzka zapisanych dat v bufferi (akt. dlzka packetu)
 * @param udph ukazatel na strukturu UDP hlavicky v packete
 * @param iph ukazatel na strukturu IP hlavicky v packete
 * @param sendbuf buffer pre packet
 */
void complete_headers( int * pack_len, struct udphdr * udph, struct iphdr * iph, char * sendbuf );

/**
 * @brief Posiela DHCP packet 
 * 
 * @param socket socket, na ktory sa posielaju data
 * @param if_idx struktura s indexom rozhrania
 * @param pack_len dlzka packetu v bytoch
 * @param sendbuf buffer pre packet
 * @return vracia 0, pri chybe -1
 */
int send_dhcp_packet( int socket, struct ifreq * if_idx , int pack_len, char * sendbuf );

#endif