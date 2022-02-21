/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 16:35:56 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/21 19:33:34 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_H
# define IRCSERV_H

#include <exception>

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
#include <cstring> //for memset

#include "irc_numerics.hpp"

#define MAX_CONNECTIONS 1024	//maximum number of simultaneous connections
#define MSG_BUF_SIZE 512		//maximum message length in IRC RFC including \r\n termination.
#define MAX_NICK_SIZE 9 		//maximum nickname length
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
	//One option could be to have a map of registered Users, and an array of clients that just mirrors the _pfds array
	// and searches for an existing user_profile on instantiation. We can use add_connection and remove_connection to
	//ensure they mirror each other precisely. If no registered User exists, we add one to the map.
		class User
		{
			private:
				enum State
				{
					DISCONNECTED,
					CONNECTED
				}			_state;
				std::string	_nick; //maximum nick length 9 chars
				std::string _pass; //super secure!! xD I guess this should be scrambled before storage?
				std::string	_IPaddr; //I AM KNOW WHERE U LIVE BISH I AM HAS YOURE IP I IS H@X000R
						User(void);
			public:
				//		User(User const & src) {}
						~User(void) {}
						User(std::string const & user_info) { _nick = user_info; }
				//User &	operator=(User const & src);
		};
		class Client
		{
			private:
				typedef std::map<std::string, User>::iterator t_user_ptr;
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

				/* PRIVATE UTILS */
				size_t		get_param_count(void) const;
				std::string	get_cmd(void) const;
				bool		msg_buf_is_crlf_terminated(void) const;
			public:
				Client(void);
				Client(User const & src);
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
				void	flush_msg_buf(void);
				bool	append_to_msg_buf(char const (& msg_register)[MSG_BUF_SIZE], int nbytes);
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
				std::string const &			get_msg_buf(void) const;
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
		bool	is_endline(char const c);
		bool	confirm_pass(std::string const & client_pass) const;

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
