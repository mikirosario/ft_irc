/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikiencolor <mikiencolor@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 16:35:56 by mrosario          #+#    #+#             */
/*   Updated: 2022/06/03 19:34:26 by mikiencolor      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_H
# define IRCSERV_H

#include <exception>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>		//perror
#include <algorithm>
#include <map>
#include <set>
#include <bitset>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_ntop
#include <netdb.h>
#include <unistd.h>		//close
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <fstream>
#include <cstring>		//for memset

#include "irc_numerics.hpp"
#include "constants.hpp"

class	IRC_Server;

extern IRC_Server *	g_serverinstance;

enum Args
{
	PASSWORD = 1,
	PORT,
	NETINFO
};

/* CASE-INSENSITIVE STRING COMPARE OBJECT */
struct case_insensitive_less : std::binary_function<std::string, std::string, bool>
{
	bool	operator() (std::string const & str1, std::string const & str2) const;
};

/* WILDCARD PATTERN MATCHING STRING COMPARE */
bool	wildcard_matching_equality(std::string const & normal_str, std::string const & wildcard_pattern);
bool	dual_wildcard_matching_equality(std::string const & str1, std::string const & str2);
bool	case_insensitive_ascii_compare(std::string const & str1, std::string const & str2);
// NOTE: the STD::BINARY_FUNCTION inheritance above is to obtain the folowing:
// result_type, first_argument_type and second_argument_type
// in this case: string, string and bool, respectively
// was deprecated as of c++17, but... we *ARE* technically using c++98 so... ;p

class IRC_Server
{
	class Channel;
	public:
		typedef std::map<std::string, Channel, case_insensitive_less> t_Channel_Map;
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
		#include "channel.hpp"
		#include "ircserv_modules.hpp"
		typedef void (IRC_Server::*CmdFunPtr)(IRC_Server::Client &, std::vector<std::string> const &);
		typedef std::map<std::string, CmdFunPtr> t_Command_Map;

		std::string								_oper_info[2];
		std::string								_nethost; //no longer needed?? what??
		std::string								_netport;
		std::string								_netpass;
		std::string								_servport; 
		std::string								_servpass;
		std::string								_networkname;
		std::string								_servername;
		std::string								_serveraddr;
		std::string const						_server_creation_time;
		struct pollfd							_pfds[MAX_CONNECTIONS];
		Client									_clients[MAX_CONNECTIONS];
		std::bitset<MAX_CONNECTIONS>			_remove_list;
		std::vector<t_Channel_Map::iterator>	_chan_remove_list;
		int										_connections;
		t_Channel_Map							_channels;
		t_Command_Map							_commands;
	
		/* UNUSABLE CONSTRUCTORS AND OVERLOADS */
						IRC_Server(void);						//Default constructor
						IRC_Server(IRC_Server const & server);	//Copy constructor
		IRC_Server &	operator=(IRC_Server const & server);	//Assignment overload

		/* PRIVATE MEMBER FUNCTIONS */
		//Parsing
		bool			get_network_info(std::string const & arg);
		Client *		find_client_by_nick(std::string const & nick);
		Client const *	find_client_by_nick(std::string const & nick) const;
		static void		remove_source(std::string & message);

		//Server initialization
		bool			build_command_map(void);
		bool			init(std::string const & netinfo);
		bool			set_serveraddr(void);
		static void *	get_in_addr(struct sockaddr * sa);
		int				get_listener_socket(void) const;
		

		//Closing
		void	close_connection(int fd);

		//Connection handling
		void	remove_connection(int fd);
		void	add_connection(int fd, char const * remoteIP);

		//Running
		void	server_loop(void);
		void	accept_connection(void);
		bool	poll_listener(void) const;
		bool	poll_client(int i) const;
		void	process_client_message(Client & client);
		void	remove_client_from_server(size_t pos);
		void	remove_client_from_server(Client const & client);
		void	remove_flagged_clients(void);
		
		//Channel controls
		t_Channel_Map::iterator	add_channel(Client & creator, std::string const & channel_name, std::string const & key);
		void	remove_channel(std::string const & channel_name);
		bool	find_channel(std::string const & channel_name);
		t_Channel_Map::iterator	get_channel_by_name(std::string const & channel_name);
		void	remove_user_from_channel(Client const &client, std::string const & channel_name);

		//Utils
		bool				is_endline(char const c);
		bool				confirm_pass(std::string const & client_pass) const;
		static std::string	get_datetime(void);

	public:
		/* CONSTRUCTORS AND DESTRUCTOR */
			IRC_Server(std::string const & port, std::string const & pass, std::string const & netinfo = std::string());
			~IRC_Server(void);
		
		/* CLOSE */
		void	close_server(int const exit_type, std::string const & close_event);

		/* GETTERS */
		std::string const &	get_port(void) const;
		std::string const & get_serveraddr(void) const;
		std::string	get_source(void) const;
};

#endif
