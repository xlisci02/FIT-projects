/*
*  File: syslog.cc
*  ISA 2018 - Project - Export DNS information using Syslog protocol
*  Author: Matus Liscinsky
*  Login: xlisci02
*  Mail: xlisci02@stud.fit.vutbr.cz
*  Date: 16.11.2018
*/

#include "syslog.h"

std::string get_ip_addr(int socket_fd, int add_family){

	/* getting my IP from connected socket*/
	char ip[INET6_ADDRSTRLEN]; // in case of IPv6 address
	struct sockaddr_in my_addr;
	unsigned int len = sizeof(my_addr);

	getsockname(socket_fd, (struct sockaddr *) &my_addr, &len);
	inet_ntop(add_family, &my_addr.sin_addr, ip, sizeof(ip) ); 
	return std::string(ip);
}

std::string get_iso_time(){

	/* getting current datetime data */
	timeval actual_time;
	gettimeofday(&actual_time, NULL);
	long ms = actual_time.tv_usec / 1000; /* ms calculating from us */
	
	/* buffer for timestamp*/
	const unsigned max_size = strlen("2018-09-20T22:14:15.003Z")+1;
	char buf [max_size] = {0};

	/* printing to string date & time (UTC) */
	strftime(buf, max_size-5, "%FT%T", gmtime(&actual_time.tv_sec));

	/* appending milliseconds */
	char milli_str [6] = {0};
	sprintf(milli_str,".%03ldZ",ms);
	strcat(buf, milli_str);
	return std::string(buf);
}

std::string create_syslog_hdr(int socket_fd, int add_family){
	std::string sys_msg;
	/* PRI and version */
	sys_msg.append("<134>1 ");
	
	/* TIMESTAMP in ISO 8601 format */
	sys_msg.append(get_iso_time().append(" "));
	
	/* getting my ip address*/
	sys_msg.append(get_ip_addr(socket_fd, add_family).append(" dns-export "));
	
	/* getting pid of parent (I am child used only for sending data end then I exit)*/
	pid_t parent_pid = getppid();
	sys_msg.append(std::to_string(parent_pid).append(" - - "));
	return sys_msg;
}

void alarm_handler(int signo) {
	(void)signo;
	
	/* fork */
	pid_t f_pid;

	/*  On success parent process leaves the func., and child will continue.
		On failure parent will continue */
	if((f_pid = fork()) > 0){
		alarm(seconds);
		return;
	}
	send_to_syslog();
	
	/* exit child process */ 
	if(f_pid == 0)
		exit(0);

}

void send_to_syslog(){
	
	struct addrinfo * rp;
	struct addrinfo hints;
	struct addrinfo *res;
	int sfd = -1;
	
	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM; 
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if(getaddrinfo(syslog, "514", &hints, &res))
		return;
	
	for (rp = res; rp != NULL; rp = rp->ai_next) {
		/* creating socket with correct family, socket type and protocol */
		sfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol); 
		if(sfd == -1)
			continue;

		/* checking connect */
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) 
			break;
	}

	if(rp == NULL)
		return;
	
	/* creating syslog msg header */
	std::string msg = create_syslog_hdr(sfd, rp->ai_family);
	unsigned int msg_len = msg.length();
	std::string ans;

	for (auto const& x : stats){
		ans = std::string(x.first).append(" ").append(std::to_string(x.second)).append("\n");

		/* Appending stats to one syslog msg */
		if( msg_len + ans.length() <  BUF_SIZE){
			msg.append(ans);
			msg_len = msg.length();
		}

		/* Current msg can't be appended, because of maximum syslog msg length */
		else{

			/* Sending message */ 
			if(send(sfd, msg.c_str(), msg_len,0) < 0)
				return;

			/* Creating new one */
			msg = create_syslog_hdr(sfd, rp->ai_family);
			msg.append(ans);
			msg_len = msg.length();
		}
	}
	
	/* Sends residue */
	if(send(sfd, msg.c_str(), msg_len,0) < 0)
		return;
	
	if(res != NULL)
		freeaddrinfo(res);
	close(sfd);

	return;
}