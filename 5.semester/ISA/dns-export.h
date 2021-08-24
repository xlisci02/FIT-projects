/*
*  File: dns-export.h
*  ISA 2018 - Project - Export DNS information using Syslog protocol
*  Author: Matus Liscinsky
*  Login: xlisci02
*  Mail: xlisci02@stud.fit.vutbr.cz
*  Date: 16.11.2018
*/

#ifndef DNS_H
#define DNS_H

#include <signal.h>

#include "syslog.h"
#include "stats.h"
#include "pkt-headers.h"
#include "dns-msg.h"

extern char * syslog;
extern long int seconds;

/**
 * @brief      Function checks correctness of program arguments and their combination. 
 *
 * @param[in]  argc       Number of arguments
 * @param      argv       Pointer to array of arguments
 * @param      file       Pointer to pcap filename
 * @param      interface  Pointer to name of network interface for listening
 * @param      syslog     Hostname/ipv4/ipv6 address of syslog server
 * @param      seconds    Determines time between each sending data to syslog server
 * @param      online     The flag indenticating online sniffing
 *
 * @return     0 if everything is correct, 1 otherwise
 */

void print_help();
int check_arguments(int argc, char * argv [], char ** file, char ** interface, char ** syslog, long int * seconds, int * online);

#endif /* DNS_H */