/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/10 03:18:04 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/10 13:42:55 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

IRC_Server::IRC_Server(void)
{
}

IRC_Server::IRC_Server(std::string const & port, std::string const & pass, std::string const & netinfo) : _connections(0)
{
	init(port, pass, netinfo);
}

IRC_Server::~IRC_Server(void)
{
	close_server(EXIT_SUCCESS, "SERVER INSTANCE EXITED");
}

/**
 * @brief	Parses the program argument for network info and copies it to the
 * 			IRC_Server network info variables if it exists.
 * 
 * @details	A string is considered to have network info if its first character
 * 			is an opening bracket '[' and it contains a closing bracket ']' and
 * 			there are two colons ':' between the brackets. The network host,
 *			port and password will be the substrings before, between and after
 *			the colons, respectively. Empty strings will be copied as empty.
 * @param arg	A reference to the string to parse.
 * @return		If network info has been collected, true is returned, otherwise
 * 				false is returned.
*/
bool	IRC_Server::get_network_info(std::string const & arg)
{
	std::string	*				netinfo[3] = {&_nethost, &_netport, &_netpass};
	std::string::const_iterator	end = arg.end(),
								network_info_begin = arg.begin(),
								network_info_end = std::find(network_info_begin, end, ']'),
								token_begin,
								token_end;
	bool						ret = false;

	//does network info exist?
	if	(arg.size() > 0																//argument is not empty
		&& *network_info_begin == '[' && network_info_end != end					//first character is '[' and there is a ']'
		&& std::count(network_info_begin, network_info_end, ':') == 2)				//there are 2 ':' betweeen '[' and ']'
	{
		int i = 0;
		token_begin = network_info_begin + 1;
		do
		{
			token_end = std::find(token_begin, network_info_end, ':');
			(*netinfo[i++]).assign(token_begin, token_end);
			token_begin = token_end + 1;
		}
		while (token_end != network_info_end);
		ret = ((*netinfo[0]).size() & (*netinfo[1]).size() & (*netinfo[2]).size());	//if all netinfo fields have been filled, return true, else false
		// //debug
		// for (int x = 0; x < 3; ++x)
		// 	std::cout << *netinfo[x] << std::endl;
		// //debug
	}
	return (ret);
}

/*! @brief	Gets a pointer to the IPv4 or IPv6 socket address as the address
**			family is AF_INET or AF_INET6, respectively.
**
** @details	By casting sockaddr to sockaddr_in or sockadddr_in6 we can get the
**			data appropriate to the family, whether IPv4 or IPv6. This function
**			does that.
**
** @param sa	A pointer to a filled-in sockaddr.
** @return	A void pointer to the socket's IPv4 or IPv6 address.
*/
void *	IRC_Server::get_in_addr(struct sockaddr * sa) const
{
    if (sa->sa_family == AF_INET)
		return (&(reinterpret_cast<struct sockaddr_in *>(sa)->sin_addr));
    else
		return (&(reinterpret_cast<struct sockaddr_in6 *>(sa)->sin6_addr));
}

void	IRC_Server::server_loop(void)
{
	struct	sockaddr_storage	remoteaddr;
	socklen_t					addrlen = sizeof(remoteaddr);
	char						remoteIP[INET6_ADDRSTRLEN];
	char						msgbuf[MSG_BUF_SIZE];

	while (1)
	{
		int	poll_count = poll(_pfds, _connections, -1);
		
		//Poll listener first
		if (poll_count == -1)
		{
			close_server(EXIT_FAILURE, std::string("FATAL poll error"));
			break ;
		}
		if (_pfds[0].revents & POLLIN) //if listener is ready to read, we have new connection
		{
			int new_connection = accept(_pfds[0].fd, reinterpret_cast<struct sockaddr *>(&remoteaddr), &addrlen);
			if (new_connection == -1)
				std::cerr << "accept error" << std::endl;
			else
			{
				//add_to_pfds(_pfds, new_connection, _connections);
				add_connection(new_connection);
				std::cout << "pollserver: new connection from " << inet_ntop(remoteaddr.ss_family, get_in_addr(reinterpret_cast<struct sockaddr *>(&remoteaddr)), remoteIP, INET6_ADDRSTRLEN) << " on socket " << new_connection << std::endl;
			}
			--poll_count;
		}

		for (int i = 1, polled = 0; polled < poll_count; ++i) //first POLLIN with listener-only array MUST be a new connection; this for only tests client fds
		{
			if (_pfds[i].revents & POLLIN) //client fd pending data receipt
			{
				int nbytes = recv(_pfds[i].fd, msgbuf, sizeof msgbuf, 0);
				switch (nbytes) //error cases and default successful data reception case
				{
					case 0 :
						std::cerr << "pollserver: socket " << _pfds[i].fd << " hung up." << std::endl;
						remove_connection(i);
						break ;
					case -1 :
						std::cerr << "recv error" << std::endl;
						remove_connection(i);
						break ;
					default :
						for (int j = 1; j < _connections; ++j) //send to all clients
							if (j != i) //do not send to self
								if (send(_pfds[j].fd, msgbuf, nbytes, 0) == -1)
									std::cerr << "send error" << std::endl;
				}
				++polled;
			}
		}
	}
}

/*! @brief	Attempts to close an open connection. Prints message to standard
**			error in case of failure.
**
** @param fd Socket file descriptor to close.
*/
void	IRC_Server::close_connection(int const fd)
{
	if (close(fd) == -1)
		std::cerr << "Close connection " << fd << " failed." << std::endl;
}

void	IRC_Server::add_connection(int fd)
{
	_pfds[_connections].fd = fd;
	_pfds[_connections].events = POLLIN; //report ready to read on incoming connection
	++_connections;
}

void	IRC_Server::remove_connection(int index)
{
	close_connection(_pfds[index].fd);
	_pfds[index] = _pfds[_connections - 1];
	--_connections;
}

void	IRC_Server::get_port(std::string const & arg) throw (std::invalid_argument, std::out_of_range)
{
		//comprobaciones?
	// try
	// {
	// 	int port = std::stoi(arg);
	// 	_servport = port;
	// 	return (true);
	// }
	// catch (std::invalid_argument & e)
	// {
	// 	std::cerr << "get_port failed: ";
	// 	std::cerr << e.what() << std::endl;
	// }
	// catch (std::out_of_range & e)
	// {
	// 	std::cerr << "get_port failed: ";
	// 	std::cerr << e.what() << std::endl;
	// }
	_servport = arg;
}

bool	IRC_Server::init(std::string const & port, std::string const & pass, std::string const & netinfo = std::string())
{
	(void)pass;
	//bool	ret = false;
	int		listener_fd;
	//is there an argument to parse? default params???
	// if (netinfo.size() == 0)
	// 	return (false);	
	if (get_network_info(netinfo)) //we don't use it
	{
		std::cout << "Has network info"	<< std::endl;
	}
	get_port(port);
	//else <--default params here
	//server setup
	listener_fd = get_listener_socket();
	if (listener_fd == -1)
	{
		_state = OFFLINE;
		close_server(EXIT_FAILURE, std::string ("IRCSERV CLOSED ON GET_LISTENER_SOCKET CALL FAILED."));
		return (false);
	}
	else
	{
		add_connection(listener_fd);
		_state = ONLINE;
	}
	//signal handling
	server_loop();
	return (true);
}

/*! @brief Closes server.
**
** @param exit_type EXIT_SUCCESS or EXIT_FAILURE
** @param close_event Description of reason for closure.
*/
void	IRC_Server::close_server(int const exit_type, std::string const & close_event)
{
	//close all connections.
	//std::for_each(_pfds.begin(), _pfds.end(), close);
	for (int i = 0; i < _connections; ++i)
		if (_pfds[i].fd > 0)
			if (close(_pfds[i].fd) == -1)
				perror("IRC_Server close_server failed");
	if (exit_type == EXIT_SUCCESS)
		std::cout << '\n' << close_event << std::endl;
	else
		std::cerr << '\n' << close_event << '\n' << "CONTACT YOUR SERVER ADMIN." << std::endl;
	_state = OFFLINE;
}

std::string const &	IRC_Server::get_port(void) const
{
	return (_servport);
}

/*! @brief	Attempts to open a TCP stream socket to standard IRC port 194 to
** 			listen for incoming connections.
**
** @details The socket must be opened, bound to the IRC port, non-blocking and
** marked for listening. If any of these operations fails, a message will be
** printed to standard error indicating the point of failure. If all succeed, a
** message will be printed to standard output indicating success.
**
** If the socket is successfully opened and a subsequent operation fails, the
** socket will be closed. If the close fails, a message will also be printed
** indicating this.
**
** The getaddrinfo function returns a linked list with allocated memory that
** is freed by freeaddrinfo.
**
** @return Socket number (a positive integer), or -1 if unsuccessful for any
** reason.
*/
int	IRC_Server::get_listener_socket(void) const
{
	//char const *			service = this->get_port().data(); //"irc" lookup seems to be failing on Linux?
	struct addrinfo			hints;
	struct addrinfo *		res = NULL; //linked list
	//struct addrinfo *		list_index;
	int						gai_status;
	int						ret = -1;

	std::memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE; // fill in my IP for me
	gai_status = getaddrinfo(NULL, this->get_port().data(), &hints, &res);
	if (gai_status) //non-zero gai_status means error, no allocation in that case
		std::cerr << "getaddrinfo error: " << gai_strerror(gai_status) << std::endl;
	else //attempt to open socket and listen
	{
		//iterate results list for valid entry

		int	connection_sockfd;

		if ((connection_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) //socket opened on success
			print_err(std::string("Socket file descriptor open failed."));
		else
		{
			if (bind(connection_sockfd, res->ai_addr, res->ai_addrlen) == -1)
				perror("Bind socket file descriptor to IRC port failed");//debug, confirm whether strerror or perror is legal
			else if (fcntl(connection_sockfd, F_SETFL, O_NONBLOCK) == - 1) //non-blocking flag
				perror("Fcntl non-blocking call failed"); //debug, confirm whether strerror or perror is legal
			else if (listen(connection_sockfd, 20) == -1)
				perror("Listen on IRC port through socket file descriptor failed");//debug, confirm whether strerror or perror is legal
			else
			{
				ret = connection_sockfd;
				std::cout << "Got connection socket!" << std::endl;
			}
			if (ret == -1)
				if ((close(connection_sockfd)) == -1)
					perror("Close socket from get_listener_socket failed");//debug, confirm whether strerror or perror is legal
		}
		freeaddrinfo(res); //debug res is assigned if getaddrinfo fails? freeaddrinfo handles NULL ref?
	}
	return (ret);
}
