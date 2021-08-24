/*
*  File: dns-export.cc
*  ISA 2018 - Project - Export DNS information using Syslog protocol
*  Author: Matus Liscinsky
*  Login: xlisci02
*  Mail: xlisci02@stud.fit.vutbr.cz
*  Date: 16.11.2018
*/

#include "dns-export.h"

char * syslog = NULL;
long int seconds = 60;

/* Main */
int main(int argc, char * argv[]){

	/* variables determining program behavioral */
	char * file = NULL;
	char * interface = NULL;
	int online = 1; 

	/* packet sniffing variables */
	const u_char *packet;
	struct pcap_pkthdr header;
	pcap_t *handle;
	struct bpf_program fp;		/* The compiled Berkeley Packet Filter expression */
	char errbuf[PCAP_ERRBUF_SIZE];

	/* arguments */
	if(check_arguments( argc, argv , &file, &interface, &syslog, &seconds, &online)){
		print_help();
		return 1;
	}

	/* SIGUSR1 signal handler */
	if (signal(SIGUSR1, print_handler) == SIG_ERR) {
		std::cerr << "An error occurred while setting a signal handler." << std::endl;
		return 1;
	}

	/* Alarm handler for sending data to SYSLOG */
	if(syslog && online){
		if(signal(SIGALRM, alarm_handler) == SIG_ERR){
			std::cerr << "An error occurred while setting a signal handler." << std::endl;
			return 1;
		}
		alarm(seconds);
	}

	/* offline mode */
	if(!online){

		/* testing file */
    	if ( access( file, R_OK ) == -1 ){
			std::cerr << "Couldn't open file " << file << std::endl;
			return 1;
    	}

		handle = pcap_open_offline(file, errbuf);
		if(!handle) {
			std::cerr <<  "pcap_open_offline() error :" << errbuf << std::endl;
			return 1;
		}
	}

	/* online mode */
	else{

		/* Open the session */
		handle = pcap_open_live(interface, BUFSIZ, 1, 1000, errbuf);
		if (handle == NULL) {
			std::cerr <<  "Couldn't open device " << errbuf << std::endl;
			return 1;
		}
	}

	if (pcap_compile(handle, &fp, "src port 53", 0, PCAP_NETMASK_UNKNOWN) == -1) {
			std::cerr <<  "Couldn't parse filter" << std::endl;
			return 1;
	}

	if (pcap_setfilter(handle, &fp) == -1) {
		std::cerr <<  "Couldn't install filter" << std::endl;
		return 1;
	}

	while(1)
	{
		/* Read next packet */
		if((packet = pcap_next(handle, &header)) == NULL){

			/* No packets were read from a live capture */
			if(online) continue; 
			
			/* No more packets in pcap file */
			else {
				if(syslog)
					send_to_syslog();
				else
					print_stats();
				break;
			}
		}

		/* pointer to first byte behind the alloceted space fot captured packet */
		dead_zone = packet + header.caplen;

		/*  */
		if((packet = process_headers(handle, packet, header.caplen)) == NULL)
			continue;
		process_dns_message(packet);
	}
	pcap_close(handle);
	//send_to_syslog("fdc7:1576:b74d:1f4c:0:0:0:0");
	return 0;
}

void print_help(){
	std::cerr << "Usage: ./dns-export [-r file.pcap] [-i interface] [-s syslog-server] [-t seconds]" << std::endl;
	std::cerr << "    -r : process given pcap file" << std::endl;
	std::cerr << "    -i : listen on given interface for DNS communication" << std::endl;
	std::cerr << "    -s : hostname/ipv4/ipv6 address of syslog server"<< std::endl;
	std::cerr << "    -t : stats collecting time in seconds, default is 60" << std::endl;
}

int check_arguments(int argc, char * argv [], char ** file, char ** interface, char ** syslog, long int * seconds, int * online){
	int c;
	int r=0, i=0, s=0, t=0;

	char * ptr;
	while ((c = getopt (argc,(char * const*)argv, "hr:i:s:t:")) != -1){
	switch (c){
		case 'r':
			r = 1;
			*file = optarg;
			*online = 0;
			break;
		case 'i':
			i = 1;
			*interface = optarg;
			*online = 1;
			break;
		case 's':
			s = 1;
			*syslog = optarg;
			break;
		case 't':
			t = 1;
			*seconds = strtol (optarg, &ptr, 10);
			if(*ptr != '\0' || *seconds < 1){
				return 1;
			}
			break;
		case 'h':
			print_help();exit(0);
		default:
			return 1;
		}
	}
	
	/* Validity of syslog server hostname/IPv4/IPv6 address */
	if(*syslog){
		struct addrinfo hints;
		struct addrinfo *result;
	
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = 0;
		hints.ai_protocol = 0;

		if(getaddrinfo(*syslog, NULL, &hints, &result))
			return 1;
		
	}

	/* valid arguments combination */
	if( (argc == 1) ||
		(argc == 3 && ((r ^ i) || s) ) || 
		(argc == 5  && ((i && s) || (r && s) || (t && s)) ) || 
		(argc == 7 && i && t && s) )
			return 0;
	return 1;
}