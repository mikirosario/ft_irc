/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 16:35:56 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/25 19:48:06 by mrosario         ###   ########.fr       */
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

#define INT_TO_STR( x ) static_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << x ) ).str() //itoa-like macro for std::string that saves me from loading up my functions with ugly stringstreams :p
#define MAX_CONNECTIONS 1024	//maximum number of simultaneous connections
#define MSG_BUF_SIZE 512		//maximum message length in IRC RFC including \r\n termination.
#define MAX_NICK_SIZE 9 		//maximum nickname length
#define MAX_HOSTNAME_SIZE 20	//maximum hostname size; //debug use clientaddr instead if this is too long
#define MAX_USERNAME_SIZE 25	//maximum username size
#define MAX_REALNAME_SIZE 35	//maximum real name size
#define MAX_PASS_ATTEMPTS 10	//maximum number of PASS commands allowed during registration before we reject connection
// class Channel
// {
// 	private:
// 		std::string	_name; //up to 50 characters, first character must be '&', '#', '+' or '!', and no spaces, ASCII 7 (bel) or commas allowed. case-insensitive
// }

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

		#include "channel.hpp"

		class Client
		{
			private:
				enum State
				{
					UNREGISTERED,
					REGISTERED
				}			_state;
				enum Buffer_State
				{
					UNREADY,
					READY
				}			_buf_state;
				std::string	_serveraddr;	//Server to which the Client is connected; can be used in replies requiring <servername>
				int			_sockfd;		//Client's sockfd
				int			_pass_attempts;	//Number of PASS commands sent by client on registration; limiting this to 10
				bool		_pass_validated;
				std::string	_nick;			//Client's nick
				std::string _clientaddr;	//Client's IP or canonical hostname, done by getaddrinfo() lookup
				std::string _hostname;		//Client's self-reported hostname?
				std::string _username;		//Client's username
				std::string _realname;		//Client's "real" name (sure it is, Gandalf Baggins)
				//std::string	_longname;		//Client's name in format nickname!username@hostname. maybe replace build_source with this
				std::string	_msg_buf;
				std::string	_message;

				/* PRIVATE UTILS */
				size_t		get_param_count(void) const;
				std::string	get_cmd(void) const;
				bool		msg_buf_is_crlf_terminated(void) const;
			public:
				Client(void);
				~Client(void);
				Client &	operator=(Client const & src);

				size_t const	pos; //Client's position in server's _clients array and _pfds array. Invariant.

				/* MOVE */
				void		move(Client & src);

				/* UTILS */
				static bool	is_endline(char const c);
				bool		msg_is_ready(void) const;
				bool		is_registered(void) const;
				bool		reg_pass_attempt(void);
				void		send_msg(std::string const & msg) const;

				/* SETTERS */
				void	flush_msg_buf(size_t stop);
				bool	append_to_msg_buf(char const (& server_msgbuf)[MSG_BUF_SIZE], int nbytes);
				void	set_sockfd(int sockfd);
				void	set_nick(std::string const & nick);
				void	set_username(std::string const & username);
				void	set_realname(std::string const & realname);
				bool	set_clientaddr(char const * remoteIP);
				void	set_hostname(std::string const & hostname);
				void	set_pass_validated(bool state);
				void	set_state_registered(void);

				void	clear(void);

				/* GETTERS */
				std::vector<std::string>	get_message(void);
				std::string					get_source(void) const;
				std::string const &			get_serveraddr(void) const;
				std::string const &			get_nick(void) const;
				std::string const &			get_username(void) const;
				std::string const &			get_realname(void) const;
				std::string const &			get_hostname(void) const;
				std::string const &			get_clientaddr(void) const;
				int							get_sockfd(void) const;
				int							get_pass_attempts(void) const;
				size_t						get_pos(void) const;
				bool						get_pass_validated(void) const;
				std::string const &			see_next_message(void) const;
				std::string const &			see_msg_buf(void) const;

		};
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
		//void	process_client_message(int i);
		void	process_client_message(Client & client);
		void	remove_client_from_server(size_t pos);
		void	remove_client_from_server(Client const & client);
		void	remove_flagged_clients(void);
		
		//Utils
		bool				is_endline(char const c);
		bool				confirm_pass(std::string const & client_pass) const;
		static std::string	get_datetime(void);

		//Command interpreting modules
		#include "ircserv_modules.hpp"

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
