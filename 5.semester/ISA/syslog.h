/*
*  File: syslog.h
*  ISA 2018 - Project - Export DNS information using Syslog protocol
*  Author: Matus Liscinsky
*  Login: xlisci02
*  Mail: xlisci02@stud.fit.vutbr.cz
*  Date: 16.11.2018
*/

#ifndef SYSLOG_H
#define SYSLOG_H

#include <string>
#include <netdb.h>

#include "dns-export.h"

#define BUF_SIZE 1024

/**
 * @brief      Gets the ip address of from connected socket.
 *
 * @param[in]  socket_fd   Socket file descriptor.
 * @param[in]  add_family  Address family, AF_INET or AF_INET6.
 *
 * @return     The ip address.
 */
std::string get_ip_addr(int socket_fd, int add_family);

/**
 * @brief      Gets actual time in ISO 8601 format.
 *
 * @return     Actual time in ISO 8601 format.
 */
std::string get_iso_time();

/**
 * @brief      Creates a syslog header.
 *
 * @param[in]  socket_fd   Socket file descriptor.
 * @param[in]  add_family  Address family, AF_INET or AF_INET6
 *
 * @return     String containing syslog header.
 */
std::string create_syslog_hdr(int socket_fd, int add_family);

/**
 * @brief      Handler function for SIGALRM, chlid process sends syslog msg, parent leaves.
 *
 * @param[in]  signo  Signal number.
 */
void alarm_handler(int signo);

/**
 * @brief      Creates new syslog message and sends it to syslog server.
 */
void send_to_syslog();

#endif