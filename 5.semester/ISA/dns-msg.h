/*
*  File: dns-msg.h
*  ISA 2018 - Project - Export DNS information using Syslog protocol
*  Author: Matus Liscinsky
*  Login: xlisci02
*  Mail: xlisci02@stud.fit.vutbr.cz
*  Date: 16.11.2018
*/

#ifndef DNS_REC_H
#define DNS_REC_H

#include <stdint.h> // uint16_t ...
#include <sys/types.h> // u_char
#include <stdio.h>
#include <arpa/inet.h> // htons ...
#include <string.h>
#include <assert.h>  // assert()
#include <limits.h> // CHAR_BIT

#include <string>
#include <vector>

#include "stats.h"

#define QTYPE_QCLASS_LEN 4
#define DNS_HEADER_LEN 12

/* DNS records */
#define DNS_RECORD_A 1
#define DNS_RECORD_AAAA 28
#define DNS_RECORD_CNAME 5
#define DNS_RECORD_MX 15
#define DNS_RECORD_NS 2
#define DNS_RECORD_SOA 6
#define DNS_RECORD_TXT 16
#define DNS_RECORD_SPF 99
#define DNS_RECORD_PTR 12
#define DNS_RECORD_SRV 33

/* DNSSEC */
#define DNS_RECORD_DNSKEY 48
#define DNS_RECORD_DS 43
#define DNS_RECORD_NSEC 47
#define DNS_RECORD_NSEC3 50
#define DNS_RECORD_NSEC3PARAM 51
#define DNS_RECORD_RRSIG 46

/**
 * Pointer to identify where ends allocated memory
 */
extern const u_char * dead_zone;

/**
 * Title: DNS Header for packet forging
 * Author: unh0lys0da
 * Date: 2016
 * Availability: https://0x00sec.org/t/dns-header-for-c/618
 */
struct dnshdr{
	uint16_t id;
# if __BYTE_ORDER == __BIG_ENDIAN
	uint16_t qr:1;
	uint16_t opcode:4;
	uint16_t aa:1;
	uint16_t tc:1;
	uint16_t rd:1;
	uint16_t ra:1;
	uint16_t zero:3;
	uint16_t rcode:4;
# elif __BYTE_ORDER == __LITTLE_ENDIAN
	uint16_t rd:1;
	uint16_t tc:1;
	uint16_t aa:1;
	uint16_t opcode:4;
	uint16_t qr:1;
	uint16_t rcode:4;
	uint16_t zero:3;
	uint16_t ra:1;
# else
#  error "Adjust your <bits/endian.h> defines"
# endif
	uint16_t qcount;	/* question count */
	uint16_t ancount;	/* Answer record count */
	uint16_t nscount;	/* Name Server (Autority Record) Count */ 
	uint16_t adcount;	/* Additional Record Count */
};


/**
 * @brief      Decode DNS message
 *
 * @param[in]  packet       Pointer to RDATA of DNS answer in packet
 * @param[in]  type         Type of DNS msg answer
 * @param[in]  rd_length    Length of RDATA
 * @param      dns_answer   Reference to string where function writes formatted
 *                          content of answer
 * @param[in]  dns_message  Pointer to DNS msg header
 *
 * @return     Pointer to space behind dns answer (so usually to another dns
 *             answer, if current answer wasn't last one)
 */
const u_char * decode_message(const u_char * packet, uint16_t type, uint16_t rd_length, std::string & dns_answer, const u_char * dns_message );

/**
 * @brief      Reads DNS message header and decode DNS message
 *
 * @param[in]  packet  Pointer to DNS message in packet
 */
void process_dns_message(const u_char * packet);

/* Processing DNS records */

/**
 * @brief      Process A record
 *
 * @param[in]  packet  Pointer to RDATA of DNS A record
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_A_record(const u_char * packet);

/**
 * @brief      Process AAAA record
 *
 * @param[in]  packet  Pointer to RDATA of DNS AAAA record
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_AAAA_record(const u_char * packet);

/**
 * @brief      Process CNAME, PTR or NS record
 *
 * @param[in]  packet       Pointer to RDATA of DNS CNAME, PTR or NS record
 * @param[in]  dns_message  Pointer to DNS message
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_CNAME_record(const u_char * packet, const u_char * dns_message);

/**
 * @brief      Process MX record
 *
 * @param[in]  packet       Pointer to RDATA of DNS MX record
 * @param[in]  dns_message  The dns message header
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_MX_record(const u_char * packet, const u_char * dns_message);

/**
 * @brief      Process SOA record
 *
 * @param[in]  packet       Pointer to RDATA of DNS SOA record
 * @param[in]  dns_message  The dns message header
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_SOA_record(const u_char * packet, const u_char * dns_message);

/**
 * @brief      Process TXT record
 *
 * @param[in]  packet     Pointer to RDATA of DNS TXT record
 * @param[in]  rd_length  Length of RDATA
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_TXT_record(const u_char * packet, uint16_t rd_length);

/**
 * @brief      Process SRV record
 *
 * @param[in]  packet       Pointer to RDATA of DNS SRV record
 * @param[in]  dns_message  The dns message header
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_SRV_record(const u_char * packet, const u_char * dns_message);

/**
 * @brief      Process DNSKEY record
 *
 * @param[in]  packet     Pointer to RDATA of DNS DNSKEY record
 * @param[in]  rd_length  Length of RDATA
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_DNSKEY_record(const u_char * packet, uint16_t rd_length);

/**
 * @brief      Process DS record
 *
 * @param[in]  packet     Pointer to RDATA of DNS DS record
 * @param[in]  rd_length  Length of RDATA
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_DS_record(const u_char * packet, uint16_t rd_length);

/**
 * @brief      Process NSEC record
 *
 * @param[in]  packet       Pointer to RDATA of DNS NSEC record
 * @param[in]  dns_message  The dns message header
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_NSEC_record(const u_char * packet, const u_char * dns_message);

/**
 * @brief      Process NSEC3 record
 *
 * @param[in]  packet  Pointer to RDATA of DNS NSEC3 record
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_NSEC3_record(const u_char * packet);

/**
 * @brief      Process NSEC3PARAM record
 *
 * @param[in]  packet  Pointer to RDATA of DNS NSEC3PARAM record
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_NSEC3PARAM_record(const u_char * packet);

/**
 * @brief      Process RRSIG record
 *
 * @param[in]  packet       Pointer to RDATA of DNS RRSIG record
 * @param[in]  dns_message  The dns message header
 * @param[in]  rd_length    Length of RDATA
 *
 * @return     Formatted string with content of recieved record
 */
std::string process_RRSIG_record(const u_char * packet, const u_char * dns_message, uint16_t rd_length);


/* Helpful functions */

/**
 * @brief      Returns hexadecimal key.
 *
 * @param[in]  packet   Pointer to beginning of the key
 * @param[in]  key_len  Length of the key
 *
 * @return     On success the hexadecimal key, otherwise empty string
 */
std::string get_hex_key(const u_char * packet, uint16_t key_len);

/**
 * @brief      Reads byte by byte and returns string
 *
 * @param[in]  packet   Pointer to beggining of text
 * @param[in]  txt_len  Length of text in bytes
 *
 * @return     The string text.
 */
std::string get_text(const u_char * packet, uint16_t txt_len);

/**
 * @brief      Puts string into quotetion marks
 *
 * @param      str   Input string
 */
void string_to_quot_marks(std::string & str);

/**
 * @brief      Reads a domain-name.
 *
 * @param[in]  packet       Pointer to beggining of domain-name.
 * @param[in]  dns_message  Pointer to DNS header
 * @param      domain_name  Reference to output string.
 *
 * @return     Number of bytes to skip on success, -1 otherwise.
 */
int read_name(const u_char * packet, const u_char * dns_message, std::string & domain_name);

/**
 * @brief      Skips all questions in DNS message
 *
 * @param[in]  packet  Pointer to first DNS question in packet.
 * @param[in]  q_num   Questions number
 *
 * @return     Pointer to space behind the last question.
 */
const u_char * skip_dns_questions(const u_char * packet, uint16_t q_num);

/**
 * @brief      Gets DNS record type.
 *
 * @param[in]  type_id  Type of DNS record
 *
 * @return     Name of DNS record.
 */
std::string get_record_type(unsigned int type_id);

/**
 * @brief      Gets positions of bits which are set
 *
 * @param[in]  packet  Pointer to bit map
 * @param[in]  len     Length of bit map
 *
 * @return     Vector of set bit positions
 */
std::vector <int> set_bit_positions (const u_char * packet, unsigned int len);


/**
 * @brief      Checks if pointer is valid
 *
 * @param[in]  ptr   Pointer to space in memory
 *
 * @return     If pointer is valid returns 1, otherwise returns 0
 */
inline int check_pointer (const u_char * ptr){
	/* inline, cause the function is small and called very often */
	
	if(!(ptr && dead_zone)){
		return 0;
	}
	if(ptr < dead_zone) 
		return 1;

	else return 0;
}


/**
 * Title: Encoding and decoding base 64 with c++
 * Author: René Nyffenegger
 * Date: 2017
 * Availability: https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp?fbclid=IwAR0ed70lCwJydv1lAsyR-j4s3irhzmpEl9DZCB7l9-AYxMOhId6To7VtWgU
 */
static const std::string base64_chars = 
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);

/**
 * Title:  base32 (de)coder implementation as specified by RFC4648.
 * Author: Adrien Kunysz
 * Date: 2010
 * Availability: https://github.com/mjg59/tpmtotp/blob/master/base32.c
 */
std::string base32_encode(const unsigned char *plain, size_t len);


#endif