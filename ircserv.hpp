/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 16:35:56 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/10 21:59:20 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_H
# define IRCSERV_H

#include <exception>

#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_ntop
#include <netdb.h>
#include <unistd.h> //close
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <cstring> //for memset

#define MAX_CONNECTIONS 1024
#define MSG_BUF_SIZE 512 //maximum message length in IRC RFC including \r\n termination.

// class User
// {
// 	private:
// 		std::string	_nick; //maximum nick length 9 chars
// 		std::string _pass; //super secure!! xD I guess this should be scrambled before storage?
// 		User(void);
// 	public:
// 		User(std::string const & user_info);
		
// }

// class Channel
// {
// 	private:
// 		std::string	_name; //up to 50 characters, first character must be '&', '#', '+' or '!', and no spaces, ASCII 7 (bel) or commas allowed. case-insensitive
// }

/**
 * Parse any message using the IRC protocol.
 * \par Format:
 * [ ':' prefix hspace ] command [ params ] "\r\n"
 */

enum Args
{
	PASSWORD = 1,
	PORT,
	NETINFO
};

class IRC_Server
{
	private:
		enum		State
		{
			CONFIG_FAIL = -1,
			OFFLINE,
			ONLINE
		}			_state;

		struct BadArgumentException : public std::exception
		{
			char const *	what() const throw()
			{
				return ("Unable to Create Server due to Bad Argument");
			}
		};
		std::string			_nethost; //no longer needed?? what??
		std::string			_netport;
		std::string			_netpass;
		std::string			_servport; 
		std::string			_servpass;
		struct pollfd		_pfds[MAX_CONNECTIONS];
		int					_connections;
		//struct pollfd	_pfds[MAX_CONNECTIONS];
		
		//Copy constructor
						IRC_Server(IRC_Server const & server);
		//Assignment overload
		IRC_Server &	operator=(IRC_Server const & server);

		//Parsing
		bool	get_network_info(std::string const & arg);
		//void	get_port(std::string const & arg) throw (std::invalid_argument, std::out_of_range);
		//bool	get_network_info(std::pair<std::string::const_iterator, std::string::const_iterator> & range) const;
		bool	init(std::string const & netinfo);

		//Setup
		void *	get_in_addr(struct sockaddr * sa) const;
		int		get_listener_socket(void) const;

		//Closing
		void	close_server(int const exit_type, std::string const & close_event);
		void	close_connection(int fd);
		void	remove_connection(int fd);

		//Adding
		void	add_connection(int fd);

		//Running
		void	server_loop(void);
		
	public:
		//Default constructor
						IRC_Server(void);
		//Argument constructor
						IRC_Server(std::string const & port, std::string const & pass, std::string const & netinfo = std::string());
		//Destructor
						~IRC_Server(void);
		std::string const &	get_port(void) const;

};

int		get_listener_socket(void);
void	close_connection(int const fd);
void	close_server(int const exit_type, std::string const & close_event);
void	print_err(std::string const & msg);

#endif
