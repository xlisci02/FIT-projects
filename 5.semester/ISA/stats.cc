/*
*  File: stats.cc
*  ISA 2018 - Project - Export DNS information using Syslog protocol
*  Author: Matus Liscinsky
*  Login: xlisci02
*  Mail: xlisci02@stud.fit.vutbr.cz
*  Date: 16.11.2018
*/

#include "stats.h"

std::map<std::string, unsigned int > stats;

void stats_insert(std::string dns_answer){
	std::map<std::string, unsigned int>::iterator it;
	it = stats.find(dns_answer);

	/* Item was found */
	if (it != stats.end()){
		it->second ++;
	}

	/* Adding new item (dns_answer) to stats */
	else{
		stats.insert(std::make_pair(dns_answer, 1));
	}
}

void print_handler(int signo){
	(void)signo;
	
	/* fork */
	pid_t f_pid;

	/*  On success parent process leaves the func., and child will continue.
		On failure parent will continue */
	if((f_pid = fork()) > 0){
		return;
	}
	print_stats();
	
	/* exit child process */ 
	if(f_pid == 0)
		exit(0);
}

void print_stats(){
	std::string str;
	for (auto const& x : stats){
		str = std::string(x.first).append(" ").append(std::to_string(x.second));
		std::cout << str << std::endl;
	}
}