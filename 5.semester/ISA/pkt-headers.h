/*
*  File: pkt-headers.h
*  ISA 2018 - Project - Export DNS information using Syslog protocol
*  Author: Matus Liscinsky
*  Login: xlisci02
*  Mail: xlisci02@stud.fit.vutbr.cz
*  Date: 16.11.2018
*/

#ifndef PKT_HEADERS_H
#define PKT_HEADERS_H

#include <pcap.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip6.h>

#include "dns-msg.h"

#define SLL_HEADER_LEN 16
#define ETH_HEADER_LEN 14
#define ETH_P_IPV4   0x0800
#define ETH_P_IPV6   0x86DD
#define IPV6_HEADER_LENGTH 40
#define UDP_HEADER_LENGTH 8 
#define FRAGMENT_HEADER_LENGTH 8 

/**
 * TCP segments map, key is next SEQ number, data vector already catched TCP segments
 */
extern std::map <unsigned int,std::vector <std::pair <const u_char *, unsigned int > > > tcp_map;

/**
 * @brief      Finds complete TCP streams, creates one message from them, and
 *             sends further for decoding
 */
void process_tcp_segments_map();

/**
 * @brief      Adds TCP segment to TCP segments map
 *
 * @param[in]  seq_num      SEQ num of TCP the segment
 * @param[in]  tcp_segment  Pointer to payload of segment
 */
void add_to_tcp_map( unsigned int seq_num ,std::pair<const u_char *, unsigned int > tcp_segment);

/**
 * @brief      Process TCP segment
 *
 * @param[in]  packet      Pointer to TCP header of segment
 * @param[in]  packet_len  Length of TCP header + TCP payload
 */
void process_tcp_segment(const u_char * packet, unsigned int packet_len);


/* Headers */

/**
 * @brief      Process UDP header
 *
 * @param[in]  packet  Pointer to UDP header in packet
 *
 * @return     Pointer to space behind UDP header on success, NULL otherwise.
 */
const u_char * process_udp_header(const u_char * packet);

/**
 * @brief      Skips IPv6 extension headers
 *
 * @param[in]  packet              Pointer to first IPv6 ext. header in packet
 * @param      transport_protocol  Pointer to number specifying transport
 *                                 protocol
 *
 * @return     On success pointer to L4 header, NULL otherwise.
 */
const u_char * process_ipv6_ext_headers(const u_char * packet, uint8_t * transport_protocol);

/**
 * @brief      Process IPv6 header(s)
 *
 * @param[in]  packet              Pointer to IPv6 header in packet
 * @param      transport_protocol  Pointer to number specifying transport
 *                                 protocol
 *
 * @return     On success pointer to L4 header, NULL otherwise.
 */
const u_char * process_ipv6_header(const u_char * packet, uint8_t * transport_protocol);

/**
 * @brief      Process IPv4 header
 *
 * @param[in]  packet              Pointer to IPv4 header in packet
 * @param      transport_protocol  Pointer to number specifying transport
 *                                 protocol
 *
 * @return     On success pointer to L4 header, NULL otherwise.
 */
const u_char * process_ipv4_header(const u_char * packet, uint8_t * transport_protocol);

/**
 * @brief      Process ETHERNET header
 *
 * @param[in]  packet      Pointer to ETHERNET header in packet
 * @param      ether_type  Pointer to number specifying L3 protocol
 *
 * @return     On success pointer to L3 header, NULL otherwise.
 */
const u_char * process_eth_header(const u_char * packet, uint16_t * ether_type);

/**
 * @brief      Process LINUX SLL header
 *
 * @param[in]  packet      Pointer to SLL header in packet
 * @param      ether_type  Pointer to number specifying L3 protocol
 *
 * @return     On success pointer to L3 header, NULL otherwise.
 */
const u_char * process_sll_header(const u_char * packet, uint16_t * ether_type);

/**
 * @brief      Process L2,L3, and L4 headers.
 *
 * @param      handle      Handle to session
 * @param[in]  packet      Pointer to packet
 * @param[in]  packet_len  Captured length of packet
 *
 * @return     On success pointer to DNS header, NULL otherwise.
 */
const u_char * process_headers(pcap_t * handle,	const u_char * packet, uint32_t packet_len);

#endif 