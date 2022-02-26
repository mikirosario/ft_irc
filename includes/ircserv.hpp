/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 16:35:56 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/26 16:00:35 by acortes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_H
# define IRCSERV_H

#include <exception>

#include <sstream>
#include <string>
#include <vector>
#include <cstdio> //perror
#include <algorithm>
#include <map>
#include <utility>
#include <bitset>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_ntop
#include <netdb.h>
#include <unistd.h> //close
#include <fcntl.h>
#include <poll.h>
#include <cstring> //for memset
#include <iostream>
#include <fstream> // I'm iostream but better


#include "irc_numerics.hpp"

//Command interpreting modules
#include "ircserv_modules.hpp"

#define INT_TO_STR( x ) static_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << x ) ).str() //itoa-like macro for std::string that saves me from loading up my functions with ugly stringstreams :p
#define MAX_CONNECTIONS 1024	//maximum number of simultaneous connections
#define MSG_BUF_SIZE 512		//maximum message length in IRC RFC including \r\n termination.
#define MAX_NICK_SIZE 9 		//maximum nickname length
#define MAX_HOSTNAME_SIZE 20	//maximum hostname size; //debug use clientaddr instead if this is too long
#define MAX_USERNAME_SIZE 25	//maximum username size
#define MAX_REALNAME_SIZE 35	//maximum real name size
#define MAX_PASS_ATTEMPTS 10	//maximum number of PASS commands allowed during registration before we reject connection

// enum Args
// {
// 	PASSWORD = 1,
// 	PORT,
// 	NETINFO
// };

class IRC_Server
{
		enum State
		{
			CONFIG_FAIL = -1,
			OFFLINE,
			ONLINE,
			RESTART
		}			_state;

		//friend Client;
		std::string						_nethost; //no longer needed?? what??
		std::string						_netport;
		std::string						_netpass;
		std::string						_servport; 
		std::string						_servpass;
		std::string						_networkname;
		std::string						_servername;
		std::string						_serveraddr;
		std::string const				_server_creation_time;
		struct pollfd					_pfds[MAX_CONNECTIONS];
		Client							_clients[MAX_CONNECTIONS];
		std::bitset<MAX_CONNECTIONS>	_remove_list;
		std::map<std::string, User>		_reg_users; //for saving data from registered users//all users (not sure yet, would have to delete unregged users from here on disconnect in latter case, do two nick searches per connection in former)
		int								_connections;
		
		/* UNUSABLE CONSTRUCTORS AND OVERLOADS */
						IRC_Server(void);						//Default constructor
						IRC_Server(IRC_Server const & server);	//Copy constructor
		IRC_Server &	operator=(IRC_Server const & server);	//Assignment overload

		/* PRIVATE MEMBER FUNCTIONS */
		//Parsing
		bool		get_network_info(std::string const & arg);
		bool		case_insensitive_ascii_compare(std::string const & str1, std::string const & str2) const;
		Client *	find_client_by_nick(std::string const & nick);
		int			find_client_pos_by_nick(std::string const & nick);

		//Server initialization
		bool			init(std::string const & netinfo);
		bool			set_serveraddr(void);
		static void *	get_in_addr(struct sockaddr * sa);
		int				get_listener_socket(void) const;
		

		//Closing
		void	close_server(int const exit_type, std::string const & close_event);
		void	close_connection(int fd);

		//Connection handling
		void	remove_connection(int fd);
		void	add_connection(int fd, char const * remoteIP);

		//Running
		void	server_loop(void);
		void	accept_connection(void);
		bool	poll_listener(void) const;
		bool	poll_client(int i) const;
		void	process_client_message(int i);
		void	remove_client_from_server(size_t pos);
		void	remove_client_from_server(Client const & client);
		void	remove_flagged_clients(void);
		
		//Utils
		bool				is_endline(char const c);
		bool				confirm_pass(std::string const & client_pass) const;
		static std::string	get_datetime(void);

	public:
		/* CONSTRUCTORS AND DESTRUCTOR */
			IRC_Server(std::string const & port, std::string const & pass, std::string const & netinfo = std::string());	//Argument constructor
			~IRC_Server(void);																								//Destructor
		
		/* GETTERS */
		std::string const &	get_port(void) const;
		std::string const & get_serveraddr(void) const;
		std::string	get_source(void) const;

};

#endif
