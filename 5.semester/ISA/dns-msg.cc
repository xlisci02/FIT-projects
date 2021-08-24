/*
*  File: dns-msg.cc
*  ISA 2018 - Project - Export DNS information using Syslog protocol
*  Author: Matus Liscinsky
*  Login: xlisci02
*  Mail: xlisci02@stud.fit.vutbr.cz
*  Date: 16.11.2018
*/

#include "dns-msg.h"

const u_char * dead_zone;

std::string process_A_record(const u_char * packet){
	if(!check_pointer(packet + 3)) // 4B long addr
		return {};
	char str[INET_ADDRSTRLEN];
	if( inet_ntop(AF_INET, packet, str, INET_ADDRSTRLEN) == NULL)
		return {}; // empty string
	return std::string(str);
}

std::string process_AAAA_record(const u_char * packet){
	if(!check_pointer(packet + 15)) // 16B long addr
		return {};
	char str[INET6_ADDRSTRLEN];
	if ( inet_ntop(AF_INET6, packet, str, INET6_ADDRSTRLEN) == NULL)
		return {}; // empty string
	return std::string(str);
}

std::string process_CNAME_record(const u_char * packet, const u_char * dns_message){
	std::string domain_name;
	if(read_name(packet, dns_message, domain_name) < 0 ){
		return {}; // empty string
	}
	return domain_name;
}

std::string process_MX_record(const u_char * packet, const u_char * dns_message){
	std::string rdata;
	std::string mx;
	
	if(check_pointer(packet + 1)){
		/* Preferences */
		rdata.append(std::to_string(ntohs( *((uint16_t *)packet) )));
		rdata.append(" ");
		packet += 2;
	}
	else 
		return {}; // empty string
	
	/* Mail Exchange*/
	if(read_name(packet, dns_message, mx) < 0)
		return {}; // empty string
	
	rdata.append(mx);

	return rdata;
}

std::string process_SOA_record(const u_char * packet, const u_char * dns_message){
	std::string rdata;
	std::string admin_NB;
	int bytes_read;

	/* Reading primary NS and change the pointer */
	if((bytes_read = read_name(packet, dns_message, rdata)) != -1)
		packet += bytes_read;
	else return {};
	
	/* Reading admin_NB */ 
	if((bytes_read = read_name(packet, dns_message, admin_NB)) != -1)
		packet += bytes_read;
	else return {};

	rdata.append(" ");
	rdata.append(admin_NB.append(" "));

	/* processing serial number, refresh interval and so on */
	for(unsigned int i = 5; i > 0; i--){
		if(check_pointer(packet + (sizeof(uint32_t)-1))){
			std::string str = std::to_string( ntohl( *((uint32_t *)packet) ) );
			rdata.append( str.append(" "));
			packet += sizeof(uint32_t);
		}
		else return {};
	}
	if(rdata.size())
		rdata.pop_back();

	return rdata;
}

std::string process_TXT_record(const u_char * packet, uint16_t rd_length){
	std::string rdata;
	uint8_t txt_len = 0;
	while(rd_length){
		/* TXT length */
		if(check_pointer(packet)){
			txt_len = ( *((uint8_t *)packet) );
			packet += 1;
		}
		else return {};
		
		/* TXT */
		if(txt_len > 0){
			std::string text;
			if((text = get_text(packet, txt_len)).empty())
				return {};
			rdata.append(text).append(" ");
		}
		rd_length -= (1 + txt_len);
		packet += txt_len;
	}
	if(rdata.size())
		rdata.pop_back();
	return rdata;
}

std::string process_SRV_record(const u_char * packet, const u_char * dns_message){
	std::string rdata;

	if(check_pointer(packet + 5)){
		uint16_t priority = ntohs(*((uint16_t *)packet));
		rdata.append(std::to_string(priority)).append(" ");
		packet += 2;
		
		uint16_t weight = ntohs(*((uint16_t *)packet));
		rdata.append(std::to_string(weight)).append(" ");
		packet += 2;
		
		uint16_t port = ntohs(*((uint16_t *)packet));
		rdata.append(std::to_string(port)).append(" ");
		packet += 2;
	}
	else return {};

	std::string target;
	if(read_name(packet, dns_message, target) < 0)
			return {}; // empty string
		
	rdata.append(target);
	return rdata;
}


std::string process_DNSKEY_record(const u_char * packet, uint16_t rd_length){
	std::string rdata;

	if(check_pointer(packet + 3)){

		/* Flags */
		rdata.append(std::to_string(ntohs( *((uint16_t *)packet) )) );
		rdata.append(" ");
		packet += 2;

		/* Protocol */
		rdata.append(std::to_string( *((uint8_t *)packet) ) );
		rdata.append(" ");
		packet += 1;

		/* Algorithm */
		rdata.append(std::to_string( *((uint8_t *)packet) ) );
		rdata.append(" ");
		packet += 1;
	}
	else return {};

	uint16_t public_key_len = rd_length-4;
	if(!check_pointer(packet + public_key_len -1))
		return {};
	
	std::string public_key = base64_encode(packet, public_key_len);
	rdata.append(public_key);


	return rdata;
}


std::string process_DS_record(const u_char * packet, uint16_t rd_length){
	std::string rdata;

	if(check_pointer(packet + 3)){

		/* Key id */
		rdata.append(std::to_string(ntohs( *((uint16_t *)packet) )));
		rdata.append(" ");
		packet += 2;

		/* Algorithm */
		rdata.append(std::to_string( *((uint8_t *)packet) ) );
		rdata.append(" ");
		packet += 1;

		/* Digest type */
		rdata.append(std::to_string( *((uint8_t *)packet) ) );
		rdata.append(" ");
		packet += 1;
	}
	
	/* Digest */
	uint16_t digest_len = rd_length-4;
	if(digest_len > 0){
		std::string digest = get_hex_key(packet, digest_len);
		if(digest.empty())
			return {};
		rdata.append(digest);
	}

	return rdata;
}

std::string process_NSEC_record(const u_char * packet, const u_char * dns_message){
	std::string rdata;
	std::string domain_name;
	int bytes_read = read_name(packet, dns_message, domain_name);
	if(bytes_read < 0)
		return {};
	rdata.append(domain_name);
	rdata.append(" ");
	packet += bytes_read;

	/* Length of bit maps */
	if(!(check_pointer(packet+1)))
		return {};
	uint16_t bit_map_len = ntohs(*(uint16_t*)packet);
	packet += 2;

	/* Type Bit Map(s) */
	std::vector <int> bit_set = set_bit_positions (packet, bit_map_len);
	for (unsigned int i = 0; i < bit_set.size(); ++i)
		rdata.append(get_record_type (bit_set[i]));
	
	if(rdata.size())
		rdata.pop_back(); // erases last character (space)

	return rdata;
}

std::string process_NSEC3_record(const u_char * packet){
	std::string rdata;

	/* This function reads equivalent fields with NSEC3 record (Hash Alg.,Flags,Iteration,Salt length,Salt)*/
	rdata.append(process_NSEC3PARAM_record(packet));
	rdata.append(" ");

	/* Skipping fields to get the Salt length field value */
	packet += 4;

	if(!check_pointer(packet))
		return {};
	uint8_t salt_len = ( *((uint8_t *)packet) );
	/* Skipping Salt length and Salt */
	packet += 1 + salt_len;


	/* Hash length */
	if(!check_pointer(packet))
		return {};
	uint8_t hash_len = ( *((uint8_t *)packet) );
	packet += 1;

	/* Next Hashed Owner Name */
	std::string hash = base32_encode(packet, hash_len);
	if(hash.empty())
		return {};
	rdata.append(hash);
	rdata.append(" ");
	packet += hash_len;

	/* Length of bit maps */
	if(!check_pointer(packet + 1))
		return {};
	uint16_t bit_map_len = ntohs(*(uint16_t*)packet);
	packet += 2;

	/* Type Bit Map(s) */
	std::vector <int> bit_set = set_bit_positions (packet, bit_map_len);
	for (unsigned int i = 0; i < bit_set.size(); ++i){
		rdata.append(get_record_type (bit_set[i]));
	}
	if(rdata.size())
		rdata.pop_back(); // erases last character (space)

	return rdata;
}

std::string process_NSEC3PARAM_record(const u_char * packet){
	std::string rdata;
	uint8_t salt_len = 0;
	
	if((check_pointer(packet + 4))){
		/* Hash Alg. */
		rdata.append(std::to_string( *((uint8_t *)packet) ) );
		rdata.append(" ");
		packet += 1;

		/* Flags */
		rdata.append(std::to_string( *((uint8_t *)packet) ) );
		rdata.append(" ");
		packet += 1;

		/* Iterations */
		rdata.append(std::to_string(ntohs( *((uint16_t *)packet) )));
		rdata.append(" ");
		packet += 2;

		/* Salt length */
		salt_len = ( *((uint8_t *)packet) );
		packet += 1;
	}
	else return {};
	
	/* Salt */
	if(salt_len == 0)
		rdata.append("-");
	else{
		std::string salt = get_hex_key(packet, salt_len);
		if(salt.empty())
			return {};
		rdata.append(salt);
	}

	return rdata;
}

std::string process_RRSIG_record(const u_char * packet, const u_char * dns_message, uint16_t rd_length){
	std::string rdata;

	if(check_pointer(packet + 17)){
		/* Type covered */
		rdata.append(get_record_type(ntohs( *((uint16_t *)packet) )));
		packet += 2;

		/* Algorithm */
		rdata.append(std::to_string( *((uint8_t *)packet) ) );
		rdata.append(" ");
		packet += 1;

		/* Labels */
		rdata.append(std::to_string( *((uint8_t *)packet) ) );
		rdata.append(" ");
		packet += 1;

		/* Original TTL */
		rdata.append(std::to_string( ntohl( *((uint32_t *)packet) ) ) );
		rdata.append(" ");
		packet += 4;

		/* Signature Expiration */ 
		time_t exp = (( ntohl( *((uint32_t *)packet) ) )); /* get time info */
		struct tm * tm_time = gmtime(&exp);
		char str_time [15] = {0};
	 	strftime (str_time, 15, "%Y%m%d%H%M%S", tm_time);
		rdata.append(std::string(str_time));
		rdata.append(" ");
		packet += 4;

		/* Signature Inception */ /*Dtto*/
		exp = (( ntohl( *((uint32_t *)packet) ) )); /* get time info */
		tm_time = gmtime(&exp);
	 	strftime (str_time, 15, "%Y%m%d%H%M%S", tm_time);
		rdata.append(std::string(str_time));
		rdata.append(" ");
		packet += 4;
	
		/* Key tag */
		rdata.append(std::to_string(ntohs( *((uint16_t *)packet) )));
		rdata.append(" ");
		packet += 2;
	}
	else return {};
	
	/* Signer's Name */
	std::string name;
	int bytes_read = read_name(packet, dns_message, name);
	
	if(bytes_read < 0)
		return {};

	rdata.append(name);
	rdata.append(" ");
	packet += bytes_read;

	/* Signature */
	uint16_t signature_len = rd_length-bytes_read-18;
	if(!check_pointer(packet + signature_len -1))
		return {};
	std::string signature = base64_encode(packet, signature_len);
	rdata.append(signature);

	return rdata;
}

const u_char * decode_message(const u_char * packet, uint16_t type, uint16_t rd_length, std::string & dns_answer, const u_char * dns_message ){
	//std::cout << "Message type : " << get_record_type(type) << std::endl;
	dns_answer.append(" ").append(get_record_type(type));
	std::string answer;

	switch(type){
		case DNS_RECORD_A : answer = process_A_record(packet); break;

		case DNS_RECORD_AAAA : answer = process_AAAA_record(packet); break;
		
		/* PTR, NS and CNAME have the same format of RDATA */
		case DNS_RECORD_PTR:
		case DNS_RECORD_NS:
		case DNS_RECORD_CNAME : answer = process_CNAME_record(packet, dns_message); break;

		case DNS_RECORD_MX : answer = process_MX_record(packet, dns_message); break;

		case DNS_RECORD_SOA : answer = process_SOA_record(packet, dns_message); break;

		case DNS_RECORD_SPF:
		case DNS_RECORD_TXT : answer = process_TXT_record(packet, rd_length); break;

		case DNS_RECORD_SRV : answer = process_SRV_record(packet, dns_message); break;

		// DNSSEC
		case DNS_RECORD_DNSKEY: answer = process_DNSKEY_record(packet, rd_length); break;

		case DNS_RECORD_DS : answer = process_DS_record(packet, rd_length); break;

		case DNS_RECORD_NSEC : answer = process_NSEC_record(packet, dns_message); break;


		case DNS_RECORD_NSEC3 : answer = process_NSEC3_record(packet); break;

		case DNS_RECORD_NSEC3PARAM : answer = process_NSEC3PARAM_record(packet); break;

		case DNS_RECORD_RRSIG : answer = process_RRSIG_record(packet, dns_message, rd_length); break;

		default: answer = std::string("Unsupported DNS record type");break;
	}

	if(!answer.size() && type != DNS_RECORD_TXT && type != DNS_RECORD_SPF)
		return NULL;
	
	string_to_quot_marks(answer);
	dns_answer.append(answer);
	packet += rd_length;
	//std::cout << "\t"<< dns_answer << std::endl;
	return packet;


}

void process_dns_message(const u_char * packet){
	if (packet == NULL)
		return;
	const u_char * dns_message = packet;
	if(!(check_pointer(packet + sizeof(struct dnshdr)-1)))
		return;
	struct dnshdr * dns_header = (struct dnshdr *)(packet);
	/* checking QR , reply code and num of answers */
	if(dns_header->qr != 1 || dns_header->rcode != 0 || ntohs(dns_header->ancount) == 0)
		return;

	uint16_t q_num = ntohs(dns_header->qcount); // reading count of questions
	uint16_t a_num = ntohs(dns_header->ancount); // reading count of answers

	packet += DNS_HEADER_LEN;

	if((packet = skip_dns_questions(packet, q_num)) == NULL)
		return;

	/* READING ANSWERS */
	

	for(uint16_t i = 0; i < a_num; i++){

		std::string dns_answer;
		int qname_len = read_name(packet, dns_message, dns_answer);
		if(qname_len < 0)
			return;
		packet += qname_len; // skipping qname


		if(!check_pointer(packet + 1))
			return;
		uint16_t type = ntohs(*((uint16_t *)packet));
		packet += 8; // skipping Type, CLASS and TTL


		if(!check_pointer(packet + 1))
			return;
		uint16_t rd_length = ntohs(*((uint16_t *)packet));
		packet += 2; // skipping RDLENGTH

		if((packet = decode_message(packet, type, rd_length, dns_answer, dns_message)) != NULL)
			stats_insert(dns_answer);
		else
			break;
	}
	//std::cout << "packet spracovany" << std::endl;

	return;
}

std::string get_hex_key(const u_char * packet, uint16_t key_len){
	if(!check_pointer(packet + key_len -1))
		return {};
	std::string key;
	char key_str[3] = {0};
	for (uint16_t i = 0; i < key_len; ++i){
		sprintf(key_str, "%02X", packet[i]);
		key.append(key_str);
	}
	return key;
}

std::string get_text(const u_char * packet, uint16_t txt_len){
	std::string text;
	for (uint16_t i = 0; i < txt_len; ++i){
		if(!(check_pointer(&packet[i])))
			break;
		text.push_back(packet[i]);
	}
	return text;
}

void string_to_quot_marks(std::string & str){
	str.insert(str.begin(), '"');
	str.insert(str.end(), '"');
}

int read_name(const u_char * packet, const u_char * dns_message, std::string & domain_name){

	if(packet == NULL)
		return -1;

	unsigned int label_size;
	int char_counter = 0;
	for(unsigned int i = 0; packet [i] != '\0'; ++i){
		if(!(check_pointer(&(packet[i]))))
			return -1;
		label_size = (unsigned char) packet[i];
		if(label_size >= 192){
			if(!check_pointer(packet + i + 1))
				return -1;
			//printf("ukazatel : %d\n", (ntohs(*((uint16_t *)(packet+i))) & 0x3FFF));
			if(read_name( dns_message + (ntohs(*((uint16_t *)(packet+i))) & 0x3FFF), dns_message, domain_name) < 0 )
				return -1;
			char_counter +=2; // 2 bytes long pointer address
			if(domain_name.size() == 0)
				domain_name.append("<Root>");
			return char_counter; // pointer can be only at the end of name
		}

		char_counter++;
		for(;label_size != 0; label_size--){
			if(!(check_pointer(&(packet[i+1]))))
				return -1;
			domain_name.push_back(packet[++i]);
			char_counter++;
		}
		domain_name.append(".");
	}
	if(domain_name.size())
		domain_name.pop_back(); // erasing last dot
	else
		domain_name.append("<Root>");
	return ++char_counter; // we count zero byte too
}

const u_char * skip_dns_questions(const u_char * packet, uint16_t q_num){
	if(packet == NULL)
		return NULL;
	for(uint16_t q = 0; q < q_num; q++){
		packet += strlen((char *)packet)+1; // skipping QNAME including last character
		packet += QTYPE_QCLASS_LEN; // skipping QTYPE & QCLASS
	}
	return packet;
}

std::string get_record_type(unsigned int type_id){

	switch(type_id){
		case	1	: return std::string("A ");
		case	2	: return std::string("NS ");
		case	3	: return std::string("MD ");
		case	4	: return std::string("MF ");
		case	5	: return std::string("CNAME ");
		case	6	: return std::string("SOA ");
		case	7	: return std::string("MB ");
		case	8	: return std::string("MG ");
		case	9	: return std::string("MR ");
		case	10	: return std::string("NULL ");
		case	11	: return std::string("WKS ");
		case	12	: return std::string("PTR ");
		case	13	: return std::string("HINFO ");
		case	14	: return std::string("MINFO ");
		case	15	: return std::string("MX ");
		case	16	: return std::string("TXT ");
		case	17	: return std::string("RP ");
		case	18	: return std::string("AFSDB ");
		case	19	: return std::string("X25 ");
		case	20	: return std::string("ISDN ");
		case	21	: return std::string("RT ");
		case	22	: return std::string("NSAP ");
		case	23	: return std::string("NSAP-PTR ");
		case	24	: return std::string("SIG ");
		case	25	: return std::string("KEY ");
		case	26	: return std::string("PX ");
		case	27	: return std::string("GPOS ");
		case	28	: return std::string("AAAA ");
		case	29	: return std::string("LOC ");
		case	30	: return std::string("NXT ");
		case	31	: return std::string("EID ");
		case	32	: return std::string("NIMLOC ");
		case	33	: return std::string("SRV ");
		case	34	: return std::string("ATMA ");
		case	35	: return std::string("NAPTR ");
		case	36	: return std::string("KX ");
		case	37	: return std::string("CERT ");
		case	38	: return std::string("A6 ");
		case	39	: return std::string("DNAME ");
		case	40	: return std::string("SINK ");
		case	41	: return std::string("OPT ");
		case	42	: return std::string("APL ");
		case	43	: return std::string("DS ");
		case	44	: return std::string("SSHFP ");
		case	45	: return std::string("IPSECKEY ");
		case	46	: return std::string("RRSIG ");
		case	47	: return std::string("NSEC ");
		case	48	: return std::string("DNSKEY ");
		case	49	: return std::string("DHCID ");
		case	50	: return std::string("NSEC3 ");
		case	51	: return std::string("NSEC3PARAM ");
		case	52	: return std::string("TLSA ");
		case	53	: return std::string("SMIMEA ");
		case	54	: return std::string("Unassigned ");
		case	55	: return std::string("HIP ");
		case	56	: return std::string("NINFO ");
		case	57	: return std::string("RKEY ");
		case	58	: return std::string("TALINK ");
		case	59	: return std::string("CDS ");
		case	60	: return std::string("CDNSKEY ");
		case	61	: return std::string("OPENPGPKEY ");
		case	62	: return std::string("CSYNC ");

		case	99	: return std::string("SPF ");
		case	100	: return std::string("UINFO ");
		case	101	: return std::string("UID ");
		case	102	: return std::string("GID ");
		case	103	: return std::string("UNSPEC ");
		case	104	: return std::string("NID ");
		case	105	: return std::string("L32 ");
		case	106	: return std::string("L64 ");
		case	107	: return std::string("LP ");
		case	108	: return std::string("EUI48 ");
		case	109	: return std::string("EUI64 ");

		case	249	: return std::string("TKEY ");
		case	250	: return std::string("TSIG ");
		case	251	: return std::string("IXFR ");
		case	252	: return std::string("AXFR ");
		case	253	: return std::string("MAILB ");
		case	254	: return std::string("MAILA ");
		case	255	: return std::string("* ");
		case	256	: return std::string("URI ");
		case	257	: return std::string("CAA ");
		case	258	: return std::string("AVC ");
		case	259	: return std::string("DOA ");

		case	32768	: return std::string("TA ");
		case	32769	: return std::string("DLV ");
		default : return std::string("TYPE" + std::to_string(type_id) + " ");

	}
}

/* Function to get set bits in array, returns vector of set bit positions*/
/* e.g pointer is pointing to byte with value 64 = 0100 0000 in bin, so bit no.1 is set */ 
std::vector <int> set_bit_positions (const u_char * packet, unsigned int len){
	std::vector <int> arr;
	for(unsigned int i = 0; i < len; i++){
		if(!check_pointer(&packet[i]))
			break;
		uint8_t byte = packet[i];
		for(unsigned int j = 0; j < 8; j++){
			if(byte >= 128)
				arr.push_back(i*8+j);
			byte <<= 1;
		}
	}
	return arr;
}

/**
 * Title: Encoding and decoding base 64 with c++
 * Author: René Nyffenegger
 * Date: 2017
 * Availability: https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp?fbclid=IwAR0ed70lCwJydv1lAsyR-j4s3irhzmpEl9DZCB7l9-AYxMOhId6To7VtWgU
 */
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while((i++ < 3))
			ret += '=';

	}

	return ret;

}

/**
 * Title:  base32 (de)coder implementation as specified by RFC4648.
 * Author: Adrien Kunysz
 * Date: 2010
 * Availability: https://github.com/mjg59/tpmtotp/blob/master/base32.c
 */
static size_t min(size_t x, size_t y)
{
	return x < y ? x : y;
}

static const unsigned char PADDING_CHAR = '=';

static void pad(unsigned char *buf, int len)
{
	for (int i = 0; i < len; i++)
		buf[i] = PADDING_CHAR;
}

static unsigned char encode_char(unsigned char c)
{
	static unsigned char base32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
	return base32[c & 0x1F];  // 0001 1111
}


static int get_octet(int block)
{
	assert(block >= 0 && block < 8);
	return (block*5) / 8;
}


static int get_offset(int block)
{
	assert(block >= 0 && block < 8);
	return (8 - 5 - (5*block) % 8);
}

static unsigned char shift_right(unsigned char byte, char offset)
{
	if (offset > 0)
		return byte >>  offset;
	else
		return byte << -offset;
}

static void encode_sequence(const unsigned char *plain, int len, unsigned char *coded)
{
	assert(CHAR_BIT == 8);  // not sure this would work otherwise
	assert(len >= 0 && len <= 5);

	for (int block = 0; block < 8; block++) {
		int octet = get_octet(block);  // figure out which octet this block starts in
		int junk = get_offset(block);  // how many bits do we drop from this octet?

		if (octet >= len) { // we hit the end of the buffer
			pad(&coded[block], 8 - block);
			return;
		}

		unsigned char c = shift_right(plain[octet], junk);  // first part

		if (junk < 0  // is there a second part?
		&&  octet < len - 1)  // is there still something to read?
		{
			c |= shift_right(plain[octet+1], 8 + junk);
		}
		coded[block] = encode_char(c);
	}
}

std::string base32_encode(const unsigned char *plain, size_t len)
{
	// Base32 allows to encode each 5 bits ( as 32 = 2^5 ) using single character.
	unsigned dst_size = ((len * 8 + 4) / 5) + 1;
	unsigned char * coded = (unsigned char *)malloc(dst_size);
	// All the hard work is done in encode_sequence(),
	// here we just need to feed it the data sequence by sequence.
	for (size_t i = 0, j = 0; i < len; i += 5, j += 8) {
		encode_sequence(&plain[i], min(len - i, 5), &coded[j]);
	}
	std::string coded_str = std::string((char *)coded);
	free(coded);
	return coded_str;
}