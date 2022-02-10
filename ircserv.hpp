/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 16:35:56 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/10 06:20:01 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_H
# define IRCSERV_H

#include <exception>

#include <string>
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


class IRC_Server
{
	private:
		enum		State
		{
			ALIVE,
			RESTART,
			DIE
		}			_state;

		struct BadArgumentException : public std::exception
		{
			char const *	what() const throw()
			{
				return ("Unable to Create Server due to Bad Argument");
			}
		};
		std::string	_nethost;
		std::string	_netport;
		std::string	_netpass;
		std::string _servport;
		std::string _servpass;
		//Copy constructor
						IRC_Server(IRC_Server const & server);
		//Assignment overload
		IRC_Server &	operator=(IRC_Server const & server);

		//Parsing
		bool	get_network_info(std::string const & arg);
		//bool	get_network_info(std::pair<std::string::const_iterator, std::string::const_iterator> & range) const;
		void	init(std::string const & arg) throw(BadArgumentException);
	public:
		//Default constructor
						IRC_Server(void);
		//Argument constructor
						IRC_Server(std::string const & arg);
		//Destructor
						~IRC_Server(void);
		IRC_Server &	operator=(std::string const & arg);
};

int		get_listener_socket(void);
void	close_connection(int const fd);
void	close_server(int const exit_type, std::string const & close_event);
void	print_err(std::string const & msg);

#endif
