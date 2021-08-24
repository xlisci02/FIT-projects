/*
*  File: stats.h
*  ISA 2018 - Project - Export DNS information using Syslog protocol
*  Author: Matus Liscinsky
*  Login: xlisci02
*  Mail: xlisci02@stud.fit.vutbr.cz
*  Date: 16.11.2018
*/

#ifndef STATS_H
#define STATS_H

#include <iostream>
#include <sys/types.h>
#include <unistd.h>

#include <iterator>
#include <map>

/**
 * Statistics map, key is DNS response answer, data is count of that answers
 */
extern std::map<std::string, unsigned int > stats;

/**
 * @brief      Inserts DNS response answer to statistics
 *
 * @param[in]  DNS response answer  
 */
void stats_insert(std::string dns_answer);

/**
 * @brief      Signal SIGUSR1 handler function for printing stats to STDOUT
 *
 * @param[in]  signo  Signal number
 */
void print_handler(int signo);

/**
 * @brief      Prints statistics on STDOUT
 */
void print_stats();

#endif