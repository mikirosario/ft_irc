/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/10 03:18:04 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/11 20:25:04 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

	// ---- CONSTRUCTORS AND DESTRUCTOR ---- //
/*!
** @brief	Construct a new IRC_Server object.
**
** @details	Attempts to initialize an IRC_Server object instance.
**
**			Accepts @p port (port number) @p pass (server password) and,
**			optionally, @p netinfo (calling server information, not used)
**			arguments. @p port and @p pass are saved to the IRC_Server variables
**			@p _servport and @p _servpass . @p netinfo is sent to ::init() for
**			parsing and was used for connecting a calling server to the server
**			object in previous iterations of the project, but is currently
**			unused.
**
** @param port		A string containing the port number represented in ASCII,
					(i.e. "6667")
** @param pass		A string containing the server password.
** @param netinfo	(Optional) A string containing the host name, port number
**					and password of the calling server. Currently unused.
*/
IRC_Server::IRC_Server(std::string const & port, std::string const & pass, std::string const & netinfo) : _servport(port), _servpass(pass), _connections(0)
{
	init(netinfo);
}

/*!
** @brief	Destruct an IRC_Server object.
**
** @details	Calls ::close_server() with EXIT_SUCCESS to attempt to gracefully
**			close all open connections as part of object destruction, printing
**			a confirmation message to standard output, or error messages if any
**			connection could not be closed.
*/
IRC_Server::~IRC_Server(void)
{
	close_server(EXIT_SUCCESS, "SERVER INSTANCE EXITED");
}

	// ---- METHODS ---- //
	// -- PARSING -- //
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
	if	(!arg.empty()													//argument is not empty
		&& *network_info_begin == '[' && network_info_end != end		//first character is '[' and there is a ']'
		&& std::count(network_info_begin, network_info_end, ':') == 2)	//there are 2 ':' betweeen '[' and ']'
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

	// -- SERVER INITIALIZATION -- //
/*!
** @brief	Attempts to open a TCP stream socket to the port number returned by
**			::get_port() to listen for incoming connections.
**
** @details The socket must be opened, bound to the IRC port, non-blocking and
**			marked for listening. If any of these operations fails, a message
**			will be printed to standard error indicating the point of failure.
**			If all succeed, a message will be printed to standard output
**			indicating success.
**
**			If the socket is successfully opened and a subsequent operation
**			fails, the socket will be closed. If the close fails, a message
**			will also be printed indicating this.
**
**			The getaddrinfo function returns a linked list with allocated
**			memory that is freed by freeaddrinfo.
**
** @return 	Socket file descriptor (a positive integer), or -1 if unsuccessful
**			for any reason.
*/
int	IRC_Server::get_listener_socket(void) const
{
	struct addrinfo			hints;
	struct addrinfo *		res = NULL; //linked list
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
		//debug iterate results list for valid entry

		int	connection_sockfd;

		if ((connection_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) //socket opened on success
			perror("Socket file descriptor open failed.");
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

/*!
** @brief	Gets a pointer to the IPv4 or IPv6 socket address as the address
**			family is AF_INET or AF_INET6, respectively.
**
** @details	By casting sockaddr to sockaddr_in or sockadddr_in6 we can get the
**			data appropriate to the family, whether IPv4 or IPv6. This function
**			does that.
**
** @param sa	A pointer to a filled-in sockaddr.
** @return		A void pointer to the socket's IPv4 or IPv6 address.
*/
void *	IRC_Server::get_in_addr(struct sockaddr * sa) const
{
    if (sa->sa_family == AF_INET)
		return (&(reinterpret_cast<struct sockaddr_in *>(sa)->sin_addr));
    else
		return (&(reinterpret_cast<struct sockaddr_in6 *>(sa)->sin6_addr));
}

/*!
** @brief	Attempts to bring an IRC_Server instance ONLINE.
**
** @details	This function will call ::get_network_info to parse netinfo and copy
**			its variables to the associated IRC_Server variables. @see ::init()
**			These variables are, however, not used in the current iteration of
**			the project.
**
**			The function then attempts to set up the server by requesting a
**			listener socket from the system. @see ::get_listener_socket()
**
**			If unsuccessful, the server's @a _state is set as @a OFFLINE and
**			an error message is output to standard error using ::close_server().
**			Any subsequent attempt to revive the server MUST call the ::init()
**			method again.
**
**			Otherwise the listener_fd is added to the pollfd array @a _pfds
**			using the ::add_connection() method and the server's @a _state is
**			set as @a ONLINE. The ::server_loop() method is called to begin
**			service. @see ::add_connection() @see ::server_loop()
**
** @param	netinfo A reference to a string containing connecting server
**			information in the following format: [host:port:password]. The
**			string may be empty or left as "[::]" if there is no such
**			information.
** @return	@a false if initialization failed; otherwise, @a true.
*/
bool	IRC_Server::init(std::string const & netinfo)
{
	bool	ret = false;
	int		listener_fd;
	if (get_network_info(netinfo))	//we don't use it
		std::cout << "Has network info"	<< std::endl;

	//server setup
	listener_fd = get_listener_socket();
	if (listener_fd == -1)			//setup failed; abort
	{
		close_server((EXIT_FAILURE), std::string ("IRC_SERV OFFLINE ON GET_LISTENER_SOCKET CALL FAILED."));
		return (false);
	}
	else 							//setup succeeded; server initialization
	{
		_state = State(ONLINE);
		add_connection(listener_fd);
		ret = true;
		server_loop();
	}
	return (ret);
}

	// -- CLOSING -- //
/*!
** @brief	Attempts to close all open connections, including the listener_fd,
**			before taking the server offline.
**
** @param exit_type EXIT_SUCCESS or EXIT_FAILURE
** @param close_event Description of reason for closure.
*/
void	IRC_Server::close_server(int const exit_type, std::string const & close_event)
{
	//close all connections.
	for (int i = 0; i < _connections; ++i)
		if (_pfds[i].fd > 0)
			close_connection(_pfds[i].fd);
	if (exit_type == EXIT_SUCCESS)
		std::cout << '\n' << close_event << std::endl;
	else
		std::cerr << '\n' << close_event << '\n' << "CONTACT YOUR SERVER ADMIN." << std::endl;
	_state = State(OFFLINE);
}

/*! @brief	Attempts to close a single open connection. Prints message to
**			standard error in case of failure.
**
** @param fd Socket file descriptor to close.
*/
void	IRC_Server::close_connection(int const fd)
{
	if (close(fd) == -1)
	{
		std::string	custom_err_msg = "Close connection ";
		custom_err_msg += fd;
		custom_err_msg += " failed.";
		perror(custom_err_msg.data());
	}

}

	// -- POLLFD ARRAY -- //
/*!
** @brief	Adds an open connection to the @a _pollfds array.
**
** @details Each pollfd struct in the array contains the socket file descriptor
**			for the open connection in @a fd and the type of event to poll this
**			socket for in @a events . We poll for incoming unread data. The
**			socket file descriptor should be a valid fd returned by the
**			@a accept() function. @see accept()
**
** @param	fd The new socket file descriptor to add.
*/
void	IRC_Server::add_connection(int fd)
{
	_pfds[_connections].fd = fd;
	_pfds[_connections].events = POLLIN; //report ready to read on incoming connection
	++_connections;
}

/*!
** @brief	Removes an open connection from the @a _pollfds array.
**
** @details	The @a _pollfds array is unordered, so deletion merely consists of
**			overwriting the deleted connection with the data of the last
**			connection in the array and decrementing the _connections counter.
** @param index The position in the @a _pollfds array of the open connection to
**				be removed.
*/
void	IRC_Server::remove_connection(int index)
{
	close_connection(_pfds[index].fd);
	_pfds[index] = _pfds[_connections - 1];
	--_connections;
}

	// -- GETTERS -- //
/*!
** @brief	Returns the listening port number.
**
** @details Um... I thought this might come in handy? :p
** @return	The port number over which the server listens for connections.
*/
std::string const &	IRC_Server::get_port(void) const
{
	return (_servport);
}

	// -- RUNNING -- //
/*!
** @brief	This is the server routine. Anything you want the server to do goes
**			here somewhere. ;)
**
** @details The server routine will continue while the server @a _state remains
**			@a ONLINE .
**
**			The server uses poll() to poll each open connection for incoming
**			data. Open connections are stored in the IRC_Server object's
**			@a _pfds array of pollfd structs. On initialization, the array holds
**			only a single open connection at @a _pfds[0], which is the
**			listener_fd socket that listens for new incoming connections. Each
**			pollfd struct is as follows:
**
**			struct pollfd {
**				int 	fd
**				short 	events
**				short 	revents
**			}
**
**			Where @a fd holds the socket file descriptor for the open
**			connection, @a events holds the event type we are listening for on
**			this connection (POLLIN for data to read in this case), and
**			@a revents is the variable to check against POLLIN to determine
**			whether a given connection has an unhandled event.
**
**			When any of the open connections has any unhandled event, the poll()
**			function will return the number of such events as @a poll_count. If
**			@a poll_count is -1 it signifies an error in poll, and the server is
**			immediately closed, with a message indicating the error output to
**			standard error.
**
**			Otherwise, when any unhandled events occur, first the listener_fd at
**			@a _pfds[0] is checked for new connection requests. If there is such
**			a request we attempt to @a accept() it. If this fails, we output a
**			message indicating the failure to standard error. Otherwise, on
**			success @a accept() will return a new open socket file descriptor
**			for communication with the requester (whose address is stored
**			temporarily in the @a remoteaddr variable). We then add the
**			connection to the @a _pfds array and output a message with
**			information about the new connection to standard output.
**
**			For the remaining events, if any, we check for all incoming data.
**			These will be data from the connecting IRC clients.
**
**			Data is received using the @a recv() function, which is similar to
**			read(). They are temporarily stored locally in the @a msgbuf
**			variable for processing.
**
*+			If @a recv() returns 0 it signals a disconnection, and if it returns
**			-1 it signals a receipt error. In either of those cases we boot the
**			offending client from the server.
**
**			Otherwise, it returns the number of bytes in the message. Per RFC,
**			that should	never be greater than 512 including the obligatory \r\n
**			at the end. We process this message. This is where all the lovely
**			RFC parsing and interpreting stuff will need to happen. :P
*/

/*!
** @brief	Attempts to connect the IRC_Server object to a new client. Should
**			only be called when (_pfds[0].revents & POLLIN) returns true.
**
** @details	This function will attempt to accept a client connection request
**			over the listener_fd. If unable to do so, an error message will be
**			output to standard error. Otherwise, the connection will be added to
**			the @a _pfds array and a confirmation message indicating the socket
**			file descriptor and the remote IP of the client will be output to
**			standard output.
*/
void		IRC_Server::accept_connection(void)
{
	struct	sockaddr_storage	remoteaddr;
	socklen_t					addrlen = sizeof(remoteaddr);
	char						remoteIP[INET6_ADDRSTRLEN];
	int							new_connection;
	
	new_connection = accept(_pfds[0].fd, reinterpret_cast<struct sockaddr *>(&remoteaddr), &addrlen);
	if (new_connection == -1)
		perror("poll_listener could not accept connection");
	else
	{
		add_connection(new_connection);
		std::cout << "pollserver: new connection from "
		<< inet_ntop(remoteaddr.ss_family, get_in_addr(reinterpret_cast<struct sockaddr *>(&remoteaddr)), remoteIP, INET6_ADDRSTRLEN)
		<< " on socket " << new_connection << std::endl;
	}
}

/*!
** @brief	Queries listener to determine whether or not a new connection request
**			is pending.
**
** @return	true if a new connection request is pending, otherwise false.
*/
bool	IRC_Server::poll_listener(void) const
{
	return (_pfds[0].revents & POLLIN); //if listener is ready to read, we have new connection
}

void	IRC_Server::server_loop(void)
{
	char						msgbuf[MSG_BUF_SIZE];

	while (_state == State(ONLINE))
	{
		int	poll_count = poll(_pfds, _connections, -1);
		
		
		if (poll_count == -1)
			close_server(EXIT_FAILURE, std::string("FATAL poll error"));
		else
		{
			//Poll listener first
			if (poll_listener() == true)
			{
				accept_connection();
				--poll_count; //If listener had to be polled, we decrement poll_count as we did that job here.
			}

			//Poll clients
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
						default : //loverly RFC stuff here; parse message, interpret commands, execute them, send messages to and fro, frolic, etc.
							for (int j = 1; j < _connections; ++j) //send to all clients (this is just test code, no RFC stuff yet)
								if (j != i) //do not send to self
									if (send(_pfds[j].fd, msgbuf, nbytes, 0) == -1)
										std::cerr << "send error" << std::endl;
					}
					++polled;
				}
			}
		}
		
	}
}