/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 16:35:56 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/27 20:14:06 by mrosario         ###   ########.fr       */
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
#include <bitset>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_ntop
#include <netdb.h>
#include <unistd.h> //close
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <fstream> // I'm iostream but better
#include <cstring> //for memset

#include "irc_numerics.hpp"
#include "constants.hpp"

enum Args
{
	PASSWORD = 1,
	PORT,
	NETINFO
};

class IRC_Server
{
	private:
		enum State
		{
			CONFIG_FAIL = -1,
			OFFLINE,
			ONLINE,
			RESTART
		}			_state;
		//Modules
		#include "client.hpp"
		#include "ircserv_modules.hpp"
		#include "channel.hpp"
		#include "database.hpp"

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
		int								_connections;

		/* CASE-INSENSITIVE STRING COMPARE OBJECT */
		struct case_insensitive_less : std::binary_function<std::string, std::string, bool>
		{
			bool	operator() (std::string const & str1, std::string const & str2) const;
		};
		std::map<std::string, Channel, case_insensitive_less>	_channels;
		
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
		//void	process_client_message(int i);
		void	process_client_message(Client & client);
		void	remove_client_from_server(size_t pos);
		void	remove_client_from_server(Client const & client);
		void	remove_flagged_clients(void);
		
		//Channel controls
		//void	add_channel()

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
