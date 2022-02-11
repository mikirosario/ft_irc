/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 16:35:56 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/11 21:47:01 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_H
# define IRCSERV_H

#include <exception>

#include <string>
#include <vector>
#include <cstdio> //perror
#include <algorithm>
#include <set>

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
				User(User const & src);
				~User(void);
				User(std::string const & user_info);
		User &	operator=(User const & src);
};

//One option could be to have a set of registered Users, and an array of clients that just mirrors the _pfds array
// and searches for an existing user_profile on instantiation.

class Client
{
	private:
		typedef std::set<User>::iterator t_user_ptr;
		int			_pfds_pos;
		t_user_ptr	_user_profile;
	public:
		Client(User const & src);
		~Client(void);
		Client(std::string const & user_info)
		{
			(void)user_info;
			//search User set for existing profile; if exists, save address to User profile, or iterator, or whatever
		}
		Client &	operator=(User const & src);
};

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
		
		/* UNUSABLE CONSTRUCTORS AND OVERLOADS */
						IRC_Server(void);						//Default constructor
						IRC_Server(IRC_Server const & server);	//Copy constructor
		IRC_Server &	operator=(IRC_Server const & server);	//Assignment overload

		/* PRIVATE MEMBER FUNCTIONS */
		//Parsing
		bool	get_network_info(std::string const & arg);

		//Server initialization
		bool	init(std::string const & netinfo);
		void *	get_in_addr(struct sockaddr * sa) const;
		int		get_listener_socket(void) const;

		//Closing
		void	close_server(int const exit_type, std::string const & close_event);
		void	close_connection(int fd);

		//Pollfd array
		void	remove_connection(int fd);
		void	add_connection(int fd);

		//Running
		void	server_loop(void);
		void	accept_connection(void);
		bool	poll_listener(void) const;
		bool	poll_client(int i) const;
		void	process_client_message(int i);
		
	public:
		/* CONSTRUCTORS AND DESTRUCTOR */
			IRC_Server(std::string const & port, std::string const & pass, std::string const & netinfo = std::string());	//Argument constructor
			~IRC_Server(void);																								//Destructor
		
		/* GETTERS */
		std::string const &	get_port(void) const;

};

#endif
