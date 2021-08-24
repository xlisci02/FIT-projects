/*
*  File: pkt-headers.cc
*  ISA 2018 - Project - Export DNS information using Syslog protocol
*  Author: Matus Liscinsky
*  Login: xlisci02
*  Mail: xlisci02@stud.fit.vutbr.cz
*  Date: 16.11.2018
*/

#include "pkt-headers.h"

std::map <unsigned int,std::vector <std::pair <const u_char *, unsigned int > > > tcp_map;

void process_tcp_segments_map(){

	for (auto const& x : tcp_map)
	{
		/* (0)=first messagge, first means pointer to unsigned char */
		unsigned int bytes = ntohs(*((uint16_t * )(x.second[0].first))) +2 ; // 2 octets length field
		unsigned int pointer = 0;
		/* memory to store all segments data together */
		const u_char * full_dns_mes = (const u_char *)malloc(bytes * sizeof(u_char));
		dead_zone = full_dns_mes + (bytes * sizeof(u_char));
		/* iterate over pairs of char * & data_len */
		for(auto const& y: x.second){
			bytes -= y.second;
			memcpy((u_char *)full_dns_mes + pointer, y.first, y.second);
			pointer += y.second;
		}

		/* not full message */
		if(bytes)
			continue;

		full_dns_mes += 2; // skipping the length field

		process_dns_message(full_dns_mes);
		tcp_map.erase(x.first);
		return;
	}
	return;
}



void add_to_tcp_map( unsigned int seq_num ,std::pair<const u_char *, unsigned int > tcp_segment){
	std::map<unsigned int,std::vector<std::pair<const u_char *, unsigned int >>>::iterator it;
	it = tcp_map.find(seq_num);
	if (it != tcp_map.end()){

		/* gets the vector */
		std::vector<std::pair<const u_char *, unsigned int >> vec;
		vec = it->second;
		/* adds new segment to vector */
		vec.push_back(tcp_segment);
		/* deletes item from map */
		tcp_map.erase(it);
		/* adds new item to the map with new sequence number*/
		unsigned int new_seq_num = (seq_num+std::get<1>(tcp_segment)) % 4294967296; // 4294967296 is 2^32, in case of overflow
		tcp_map.insert(std::make_pair(new_seq_num, vec));
	}
	/* Adding new item(segment) to tcp_map */
	else{
		/* creates new vector */
		std::vector<std::pair<const u_char *, unsigned int >> vec;
		/* adds new segment to vector */
		vec.push_back(tcp_segment);
		/* adds new item to the map with new sequence number*/
		unsigned int new_seq_num = (seq_num+std::get<1>(tcp_segment)) % 4294967296;

		tcp_map.insert(std::make_pair(new_seq_num, vec));
	}
	/* processes relevant tcp segments if all of them has arrived */
	process_tcp_segments_map();

}

void process_tcp_segment(const u_char * packet, unsigned int packet_len){
	if(packet == NULL)
		return;
	if(!(check_pointer(packet + sizeof(struct tcphdr)-1))) {
		return;
	}
	
	if(ntohs(((struct tcphdr *)packet)->source) == 53){

		#if (defined (__FAVOR_BSD))
			unsigned int tcp_hdr_len = (((struct tcphdr *)packet)->th_off ) *4;
			unsigned int seq_num = ntohl(((struct tcphdr *)packet)->th_seq);
		#else
			unsigned int tcp_hdr_len = (((struct tcphdr *)packet)->doff ) *4;
			unsigned int seq_num = ntohl(((struct tcphdr *)packet)->seq);
		#endif

		packet_len -= tcp_hdr_len; /* this changes the meaning of variable to data_len */
		packet += tcp_hdr_len;
		if(packet_len == 0 || packet == NULL){
			return;
		}
		const u_char * dns_data = (const u_char *)malloc(packet_len * sizeof(u_char));
		memcpy((u_char *)dns_data, packet, packet_len);
		add_to_tcp_map(seq_num, std::make_pair(dns_data,packet_len));
	}
	return;
}


const u_char * process_udp_header(const u_char * packet){

	if(packet == NULL)
		return NULL;
	if(!(check_pointer(packet + sizeof(struct udphdr)-1))) 
		return NULL;
	
	if(ntohs(((struct udphdr *)packet)->source) == 53)
		// returns a pointer to next header
		return packet + UDP_HEADER_LENGTH;
	
	else
		return NULL;
}

const u_char * process_ipv6_ext_headers(const u_char * packet, uint8_t * transport_protocol){
	if(packet == NULL || transport_protocol == NULL)
		return NULL;
	while(*transport_protocol != 17 && *transport_protocol != 6 ){

		switch(*transport_protocol){
			// hop by hop
			case 0 :	if(!(check_pointer(packet + sizeof(struct ip6_hbh)-1))) 
							return NULL;
						*transport_protocol = ((struct ip6_hbh *)packet)->ip6h_nxt;
						packet += 8 + ((struct ip6_hbh *)packet)->ip6h_len*8;
						break;
			// destination options
			case 60:	if(!(check_pointer(packet + sizeof(struct ip6_dest)-1))) 
							return NULL;
						*transport_protocol = ((struct ip6_dest *)packet)->ip6d_nxt;
						packet +=  8 + ((struct ip6_dest *)packet)->ip6d_len*8;
						break;
			// routing
			case 43:	if(!(check_pointer(packet + sizeof(struct ip6_rthdr)-1))) 
							return NULL;
						*transport_protocol = ((struct ip6_rthdr *)packet)->ip6r_nxt;
						packet +=  8 + ((struct ip6_rthdr *)packet)->ip6r_len*8;
						break;

			// fragment
			case 44:	if(!(check_pointer(packet + sizeof(struct ip6_frag)-1))) 
							return NULL;
						*transport_protocol = ((struct ip6_frag *)packet)->ip6f_nxt;
						packet += FRAGMENT_HEADER_LENGTH; // length of fragment extension header
						break;

			// authentication header
			case 51: 	if(!(check_pointer(packet + sizeof(struct ip6_ext)-1))) 
							return NULL;
						*transport_protocol = ((struct	ip6_ext *)packet)->ip6e_nxt;
						packet +=  (((struct ip6_ext *)packet)->ip6e_len+2)*4;
						break;

			// mobility header, 
			case 135:	if(!(check_pointer(packet + sizeof(struct ip6_ext)-1))) 
							return NULL;
						*transport_protocol = ((struct ip6_ext *)packet)->ip6e_nxt;
						packet +=  8 + ((struct ip6_ext *)packet)->ip6e_len*8;
						break;	
			default: return NULL;
		}
	}
	return packet;
}

const u_char * process_ipv6_header(const u_char * packet, uint8_t * transport_protocol){
	if(packet == NULL || transport_protocol == NULL)
		return NULL;
	
	if(!(check_pointer(packet + sizeof(struct ip6_hdr)-1)))
			return NULL;
	
	struct ip6_hdr * ipv6_header = (struct ip6_hdr *)packet;
	*transport_protocol = ipv6_header->ip6_ctlun.ip6_un1.ip6_un1_nxt;

	packet += IPV6_HEADER_LENGTH;

	if(*transport_protocol  != 17 && *transport_protocol != 6){
		packet = process_ipv6_ext_headers(packet, &(*transport_protocol));
	}
	return packet;
}

const u_char * process_ipv4_header(const u_char * packet, uint8_t * transport_protocol){

	if(packet == NULL || transport_protocol == NULL)
		return NULL;

	if(!(check_pointer(packet + sizeof(struct iphdr)-1)))
		return NULL;

	*transport_protocol = ((struct iphdr *)packet)->protocol; // reads transport protocol from IPv4 header
	packet += (((struct iphdr *)packet)->ihl) * 4; // preskoc ipv4 hlavicku
	
	return packet;
}

const u_char * process_eth_header(const u_char * packet, uint16_t * ether_type){
	if(packet == NULL || ether_type == NULL)
		return NULL;

	*ether_type = ((struct ether_header *) packet)->ether_type; // precita EthType (16b)
	/* skip the ethernet header */
	return packet + ETH_HEADER_LEN;
}

const u_char * process_sll_header(const u_char * packet, uint16_t * ether_type){
	if(packet == NULL || ether_type == NULL)
		return NULL;

	*ether_type =  *((uint16_t *)(packet + 14)); // precita Protocol type (16b)
	/* skip the sll header */
	return packet + SLL_HEADER_LEN;
}


const u_char * process_headers(pcap_t *handle, const u_char * packet, uint32_t packet_len){
	uint16_t ether_type;
	uint8_t transport_protocol;
	const u_char * original_packet = packet;

	/* LINKTYPE_ETHERNET */
	if (pcap_datalink(handle) == DLT_EN10MB) {
		/* Ethernet header */
		if((packet = process_eth_header(packet, &ether_type)) == NULL)
			return NULL;
	}

	/* LINKTYPE_LINUX_SLL = Linux "cooked" capture encapsulation */
	else if (pcap_datalink(handle) == DLT_LINUX_SLL) {
		if((packet = process_sll_header(packet, &ether_type)) == NULL)
			return NULL;
	}
	else
		return NULL;
	
	/* L3 header */
	if(ntohs(ether_type) == ETH_P_IPV4 ){
		if((packet = process_ipv4_header(packet, &transport_protocol)) == NULL)
			return NULL;
	}
	else if(ntohs(ether_type) == ETH_P_IPV6){
		if((packet = process_ipv6_header(packet, &transport_protocol)) == NULL)
			return NULL;
	}
	else
		return NULL;

	packet_len -= packet - original_packet;

	/* L4 header */
	if((transport_protocol) == 17){ // UDP
		if((packet = process_udp_header(packet)) == NULL)
			return NULL;
	}
	else if ((transport_protocol) == 6){ // TCP
		process_tcp_segment(packet, packet_len); /* second parameter is length of ethernet & L3 header(s)*/
		return NULL;
	}
	else
		return NULL;

	return packet;
}