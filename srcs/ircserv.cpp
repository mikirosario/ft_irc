/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/10 03:18:04 by mrosario          #+#    #+#             */
/*   Updated: 2022/03/10 18:30:54 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ircserv.hpp"
#include <ctime>

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
IRC_Server::IRC_Server(std::string const & port, std::string const & pass, std::string const & netinfo) :	_servport(port),
																											_servpass(pass),
																											_networkname("mikinet"),
																											_servername("mikiserv"),
																											_server_creation_time(get_datetime()),
																											_connections(0)
{
	for (size_t i = 0; i < MAX_CONNECTIONS; ++i)
		const_cast<size_t &>(_clients[i].pos) = i;
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

/*!
** @brief	Performs a case-insensitive comparison of str1 with str2 in which
**			ASCII characters 97-122 (a-z) are equal to ASCII characters 65-90
**			(A-Z). This is meant ONLY for username, servername and channelname
**			comparisons!
**
** @details	Use in std::locale("C") only and only with strings containing
**			exclusively ASCII characters. This is the default C++ locale, but to
**			be extra sure it is explicitly set at program initiation. For
**			casemapping=ascii mode.
** @param	str1	First string to compare.
** @param	str2	Second string to compare.
** @return			true if both strings compare equal, otherwise false
*/
bool		IRC_Server::case_insensitive_ascii_compare(std::string const & str1, std::string const & str2) const
{
	if (str1.size() == str2.size())
	{
		for (std::string::size_type i = 0, strsize = str1.size(); i < strsize; ++i)
			if (std::toupper(str1[i]) != std::toupper(str2[i])) //locale is "C"; names are limited to ASCII alphanumeric
				return (false);
		return (true);
	}
	return (false);
}

/*!
** @brief	Returns a pointer to the client with nickname @a nick or a NULL
**			pointer if none exists. Search is case-insensitive.
**
** @details	Performs a case-insensitive search for the client with nickname
**			@a nick ("C" locale, US-ASCII case system) in the _clients array.
** @param	nick	The nick to search for.
** @return	A pointer to the client with the nickname @a nick if one exists,
**			otherwise a NULL pointer.
*/
IRC_Server::Client *		IRC_Server::find_client_by_nick(std::string const & nick)
{
	for (int i = 1; i < _connections; ++i)
		if (case_insensitive_ascii_compare(_clients[i].get_nick(), nick) == true)
			return (&_clients[i]);
	return (NULL);
}

IRC_Server::Client const *	IRC_Server::find_client_by_nick(std::string const & nick) const
{
	for (int i = 1; i < _connections; ++i)
		if (case_insensitive_ascii_compare(_clients[i].get_nick(), nick) == true)
			return (&_clients[i]);
	return (NULL);
}

void		IRC_Server::remove_source(std::string & message)
{
	message.erase(0, message.find_first_of(" \r\n"));
	message.erase(0, message.find_first_not_of(' '));	
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
	hints.ai_family = AF_INET; // IPv4
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
void *	IRC_Server::get_in_addr(struct sockaddr * sa)
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
	if (listener_fd == -1)				//setup failed; abort
		close_server((EXIT_FAILURE), std::string ("IRC_SERV OFFLINE ON GET_LISTENER_SOCKET CALL FAILED."));
	else if (set_serveraddr() == false)	//setup failed;
		close_server((EXIT_FAILURE), std::string ("IRC_SERV OFFLINE ON SET_SERVERADDR CALL FAILED."));
	else 								//setup succeeded; server initialization
	{
		_state = State(ONLINE);
		add_connection(listener_fd, _serveraddr.data());
		//debug
		std::cout << "IRC Server: " << _serveraddr << std::endl;
		//debug
		ret = true;
		server_loop();
	}
	return (ret);
}

// bool	IRC_Server::init(std::string const & netinfo)
// {
// 	bool	ret = false;
// 	int		listener_fd;
// 	if (get_network_info(netinfo))	//we don't use it
// 		std::cout << "Has network info"	<< std::endl;

// 	//server setup
// 	listener_fd = get_listener_socket();
// 	if (listener_fd == -1)				//setup failed; abort
// 		close_server((EXIT_FAILURE), std::string ("IRC_SERV OFFLINE ON GET_LISTENER_SOCKET CALL FAILED."));
// 	else if (add_connection(listener_fd, ) == false)	//setup failed; we need servername to use as prefix in communications with clients
// 		close_server((EXIT_FAILURE), std::string ("IRC_SERV OFFLINE ON ADD_CONNECTION CALL FAILED."));
// 	else 								//setup succeeded; server initialization
// 	{
// 		_serveraddr = _clients[0].get_hostname();
// 		_state = State(ONLINE);
// 		//debug
// 		std::cout << "IRC Server: " << _serveraddr << std::endl;
// 		//debug
// 		ret = true;
// 		server_loop();
// 	}
// 	return (ret);
// }

bool	IRC_Server::set_serveraddr(void)
{
	struct hostent	*self;
	bool			ret = false;
	
	if ((self = gethostbyname("localhost")) == NULL)
		hstrerror(h_errno); //debug; this isn't explicitly allowed by subject either :p but will keep it here until evaluation for debugging purposes
	else
	{
		_serveraddr = inet_ntoa(*(reinterpret_cast<struct in_addr *>(self->h_addr)));
		ret = true;
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

	// -- CONNECTION HANDLING -- //

// /*! @brief	Retrieves the current address to which the Client passed as @a client
// **			is connected.
// */
// std::string	

/*!
** @brief	Adds an open connection to the IRC_Server's @a _pollfds and
**			@a _clents arrays.
**
** @details Each pollfd struct in the array contains the socket file descriptor
**			for the open connection in @a fd and the type of event to poll this
**			socket for in @a events . We poll for incoming unread data. The
**			socket file descriptor should be a valid fd returned by the
**			@a accept() function. @see accept()
**
**			The @a _clients array mirrors the @a _pollfds array. Some Client
**			data can't be filled in until sent by the user, which is done from
**			an RFC module (register client).
**
**			If remoteIP is NULL, no remoteIP will be recorded for the Client.
** @param	fd			The new socket file descriptor to add.
** @param	remoteIP	The IP address or hostname of @a fd.
** @return	true if successful, otherwise false
*/
void	IRC_Server::add_connection(int fd, char const * remoteIP)
{
	_pfds[_connections].fd = fd;
	_pfds[_connections].events = POLLIN; //report ready to read on incoming connection
	_clients[_connections].set_clientaddr(remoteIP);
	_clients[_connections].set_sockfd(fd);
	++_connections;
}

/*!
** @brief	Removes an open connection from the @a _pollfds array. DO NOT USE IN
**			INTERPRETING MODULES TO REMOVE CLIENTS FROM THE SERVER!!!!
**
** @details	The @a _pollfds array is unordered, so deletion merely consists of
**			overwriting the deleted connection with the data of the last
**			connection in the array and decrementing the _connections counter.
**			The @a _clients array must always have the same sequence as the
**			@a _pollfds array, as every indexed client must remain associated
**			with the pollfd at the same index, so we mirror this action.
**
**			The @a _remove_list keeps track of clients flagged for removal and
**			likewise mirrors the @a _pollfds array.
**
**			This method now uses the client move method to make this O(1)
**			constant time.
**
**			NOTE: DO NOT use this DANGEROUS function to kick clients!! This can
**			ONLY be called at a VERY specific part of the server_loop(),
**			otherwise TOTAL AND UTTER DESTRUCTION will be the result. To safely
**			remove clients at any time or place in the server_loop(), use
**			remove_client_from_server() instead!
** @param index The position in the @a _pollfds array of the open connection to
**				be removed.
*/
void	IRC_Server::remove_connection(int index)
{
	close_connection(_pfds[index].fd);
	_pfds[index] = _pfds[_connections - 1];
	_clients[index].move(_clients[_connections - 1]);		//move references of last client to this position
	_remove_list[index] = _remove_list[_connections - 1];	//move corresponding remove_list bit to this position
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

/*!
** @brief	Returns the true server address.
**
** @return	The true server address.
*/
std::string const &	IRC_Server::get_serveraddr(void) const
{
	return (_serveraddr);
}

/*!
** @brief	Returns a string that can be used as the <prefix> or <source> in
**			messages sent to clients.
**
** @return	A string containing the server <prefix> or <source>.
*/
std::string	IRC_Server::get_source(void) const
{
	std::string	source = ":" + _serveraddr;

	return (source);
}

	// -- RUNNING -- //
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
**
**			The remote IP will be recorded in the Client's clientaddr variable.
**			If a remote IP could not be resolved, the connection will not be
**			accepted.
*/
void		IRC_Server::accept_connection(void)
{
	struct sockaddr_in			remoteaddr;
	socklen_t					addrlen = sizeof(remoteaddr);
	char *						remoteIP;
	int							new_connection;
	
	new_connection = accept(_pfds[0].fd, reinterpret_cast<struct sockaddr *>(&remoteaddr), &addrlen);
	if (new_connection == -1)
		perror("poll_listener could not accept connection");
	else if (remoteaddr.sin_family != AF_INET)
		perror("accept_connection unable to resolve remote IP");
	else
	{
		remoteIP = inet_ntoa(remoteaddr.sin_addr);
		add_connection(new_connection, remoteIP);
		std::cout << "pollserver: new connection from "
		<< _clients[_connections - 1].get_clientaddr()
		<< " on socket " << new_connection
		<< " to server " << _serveraddr
		<< std::endl;
	}
}

/*!
** @brief	Queries client to determine whether or not new data is pending receipt.
**
** @param	i The position in the @a _pfds array of the client to be polled.
** @return	true if a client has new data pending receipt, otherwise false.
*/
bool	IRC_Server::poll_client(int i) const
{
	return (_pfds[i].revents & POLLIN); //socket fd pending data receipt
}

/*!
** @brief	Queries listener to determine whether or not a new connection request
**			is pending.
**
** @details	This is just a wrapper around @a ::poll_client() to improve
**			code clarity through explicit naming. "Client 0" is the listener.
**
** @return	true if a new connection request is pending, otherwise false.
*/
bool	IRC_Server::poll_listener(void) const
{
	return (poll_client(0)); //if listener is ready to read, we have new connection
}

/*!
** @brief	Sends client message to interpreters for processing.
**
** @details The default case hands over the client message to the interpreter
**			module, where everyone on the team can develop encapsulated
**			submodules for different RFC functionalities.
** @param	i The position in the @a _pfds array of the client with a message.
*/
void	IRC_Server::process_client_message(Client & client)
{
	char	server_msgbuf[MSG_BUF_SIZE];
	int		nbytes = recv(client.get_sockfd(), server_msgbuf, MSG_BUF_SIZE, 0);

	switch (nbytes) //error cases and default successful data reception case
	{
		case 0 :
			std::cerr << "pollserver: socket " << client.get_sockfd() << " hung up." << std::endl;
			remove_client_from_server(client);
			break ;
		case -1 :
			std::cerr << "recv error" << std::endl;
			remove_client_from_server(client);
			break ;
		//handover to interpreter module in default case
		default : //loverly RFC stuff here; parse message, interpret commands, execute them, send messages to and fro, frolic, etc.
			//insert message in client's message buffer
			//debug
			std::cout << "Received " << nbytes << " bytes from " << (client.is_registered() ? client.get_nick() : "new client") << std::endl;
			//debug
			if (client.append_to_msg_buf(server_msgbuf, nbytes) == false)
				send_err_INPUTTOOLONG(client, "Input line was too long");
			//do stuff

			//debug
			for (int j = 1; j < _connections; ++j)	//send to all clients (this is just test code, no RFC stuff yet)
				if (static_cast<size_t>(j) != client.get_pos()) //do not send to self
					if (send(_pfds[j].fd, server_msgbuf, nbytes, 0) == -1)
						std::cerr << "send error" << std::endl;
			//debug

			while (client.msg_is_ready())		//while loop here, keep interpreting all received client msgs until none are left
				interpret_msg(client);

				
				//debug
				//debug

				// //debug get_param_count and get_message tests
				// //MAKE get_param_count() PUBLIC TO TEST THIS
				// // std::cout	<< "TEST PARAM_COUNT: \n"
				// // 			<< _clients[i].get_param_count() << std::endl;
				// std::cout << "TEST GET_MESSAGE:" << std::endl;
				// //debug

				// std::vector<std::string>	argv = _clients[i].get_message();

				// //debug
				// std::cout << "vector size: " << argv.size() << std::endl;
				// for (size_t i = 0; i < argv.size(); ++i)
				// 	std::cout << argv[i] << "\n";
				// //debug
	}
}

/*!
** @brief	Safely flags the Client at position @a pos in the @a _clients array
**			for removal from the server.
**
** @details	The Client at position @a pos will be marked for removal at the next
**			available opportunity. Use with find_client_pos_by_nick() or
**			client.get_pos() ANYWHERE and AT ANY TIME to remove a Client from
**			the server.
**
**			If @a pos is out of bounds of the _remove_list, or corresponds to
**			the server's position, nothing will be done.
** @param	pos	The position of the Client to be removed in the @a _clients
**			array.
*/
void	IRC_Server::remove_client_from_server(size_t pos)
{
	if (_remove_list.size() > pos && pos > 0)
		_remove_list.set(pos, true);
}

/*!
** @brief	Safely flags the Client @a client for removal from the server.
**
** @details	The Client @a client will be marked for removal at the next
**			available opportunity. Use ANYWHERE and AT ANY TIME to remove a
**			Client from the server.
**
**			This overload queries @a client for its position in the	@a _clients
**			array and uses remove_client_from_server(size_t pos).
** @param	pos	The Client to be removed from the server.
*/
void	IRC_Server::remove_client_from_server(Client const & client)
{
	remove_client_from_server(client.get_pos());
}

// /*!
// ** @brief	Safely flags the Channel @a channel for removal from the server.
// **
// ** @details	The Channel @a channel will be marked for removal at the next
// **			available opportunity. Use ANYWHERE and AT ANY TIME to remove a
// **			Channel from the server.
// **
// **			Queries the @a _channels vector for an iterator pointing to
// **			@a channel and stores it in @a  _chan_remove_list. If @a channel is
// **			is not present in the @a _channels vector, nothing is done, though
// **			this should not be possible. :p
// ** @param	channel	The Channel to be removed from the server.
// */
// void	IRC_Server::remove_channel_from_server(Channel & channel)
// {
// 	t_Channel_Map::iterator	it(_channels.find(channel.getChannelName()));

// 	if (it != _channels.end())
// 		_chan_remove_list.push_back(it);
// }

// /*!
// ** @brief	Safely flags the Channel @a channel for removal from the server.
// **
// ** @details	The Channel @a channel will be marked for removal at the next
// **			available opportunity. Use ANYWHERE and AT ANY TIME to remove a
// **			Channel from the server.
// **
// **			This overload queries the @a _channels vector for an iterator
// **			pointing to @a channel using the provided channel_name. The iterator
// **			is stored in @a _chan_remove_list. If @a channel_name is not present
// **			in the @a _channels vector, nothing is done.
// ** @param	channel	The Channel to be removed from the server.
// */
// void	IRC_Server::remove_channel_from_server(std::string & channel_name)
// {
// 	t_Channel_Map::iterator	it(_channels.find(channel_name));

// 	if (it != _channels.end())
// 		_chan_remove_list.push_back(it);
// }

/*!
** @brief	Removes all Clients flagged for removal from the server.
**
** @details	Clients may be flagged for removal at any time, for any number of
**			reasons. Maybe accept() failed, maybe they didn't ping us, maybe
**			blasted our server with a thousand PASS requests and still haven't
**			tried to register, maybe some operator kicked them.
**
**			Whatever the reasons, removal is handled here, and carefully. We
**			have to be careful, since upon removal our arrays copy one over from
**			the end, every removal invalidates any external references and may
**			screw up a for loop.
**
**			To avoid such a catastrophe, we use a bitset called @a _remove_list.
**			Every time anyone wants to remove a Client, they set the bit
**			corresponding to the Client's position in the @a _clients and
**			@a _pfds arrays in the @a _remove_list. After all messages are
**			processed, we execute this loop, which will not increment the index
**			if a client is removed because it knows the client at that index has
**			been removed, and another client (formerly the last one) has
**			replaced it and will also need to be checked for removal. Finally,
**			the remove_connection() function makes damn sure the @a _remove_list
**			bitset mirrors any changes to the @a _clients array. Whew.
*/
void	IRC_Server::remove_flagged_clients(void)
{
	for (size_t i = 1, remove_count = _remove_list.count(); remove_count > 0; )
	{
		if (_remove_list[i] == true)
		{
			_clients[i].leave_all_channels();
			remove_connection(i);
			--remove_count;
		}
		else
			++i;
	}
	_remove_list.reset();
}

// void	IRC_Server::remove_flagged_channels(void)
// {
// 	for (std::vector<t_Channel_Map::iterator>::iterator it = _chan_remove_list.begin(), end = _chan_remove_list.end(); it != end; ++it)
// 	{
// 		(*it)->second.removeAllMembers();
// 		_channels.erase(*it);
// 	}
// }

// ---- CHANNEL CONTROL ---- //
// bool	IRC_Server::add_channel(Channel const & new_channel)
// {
// 	return ((_channels.insert(std::make_pair(new_channel.getChannelName(), new_channel))).second);
// }

/*!
** @brief	Attempts to add a channel named @a channel_name with key @a key and
**			owner @a creator to the server.
**
** @details	We try to insert the new channel. A grammar check of @a channel_name
**			must be performed before calling this function.
**
**			If an exception is thrown, it is printed server-side and false is
**			returned.
**
**			If the new channel could not be inserted but an exception was not
**			returned, false is returned.
**
**			If the new channel was inserted but channel_membership could not
**			successfully be registered in the client object, the new channel is
**			deleted and false is returned.
** @param	creator			The client creating the new channel.
** @param	channel_name	The channel_name. This should be parsed beforehand
**							for correctness.
** @param	key				The channel password, if any. An empty string if
**							there is none.
** @return	if successful, an iterator to the added channel, otherwise
**			_channels.end().
*/
IRC_Server::t_Channel_Map::iterator	IRC_Server::add_channel(Client & creator, std::string const & channel_name, std::string const & key)
{
	Channel										new_channel(creator, *this, channel_name, key);
	std::pair<t_Channel_Map::iterator, bool>	chan_insert_result;
	
	try
	{
		chan_insert_result = _channels.insert(std::make_pair(new_channel.getChannelName(), new_channel));
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return _channels.end();
	}
	bool	did_insert = chan_insert_result.second;
	if (did_insert != false && (did_insert &= creator.set_channel_membership(chan_insert_result.first)) == false)
		_channels.erase(chan_insert_result.first);
	return (did_insert == false ? _channels.end() : chan_insert_result.first);
}

void	IRC_Server::remove_channel(std::string const & channel_name)
{
	t_Channel_Map::iterator	it = _channels.find(channel_name);
	if (it != _channels.end())
	{
		it->second.removeAllMembers();
		_channels.erase(channel_name);
	}
}

bool	IRC_Server::find_channel(std::string const & channel_name)
{
	if ( _channels.end() == _channels.find(channel_name))
		return(0);
	return(1);
}

void	IRC_Server::remove_user_from_channel(Client const &client, std::string const & channel_name)
{
	t_Channel_Map::iterator it = _channels.find(channel_name);
	if  (it != _channels.end())
		it->second.removeMember(client.get_nick());
	//_channels[channel_name].removeClient(client);
}

// void	IRC_Server::remove_user_from_channel(Client const &client, std::string const & channel_name, std::string const &msg)
// {
// 	t_Channel_Map::iterator it = _channels.find(channel_name);
// 	if  (it != _channels.end())
// 		it->second.removeMember(client.get_nick(), msg);
// 	//_channels[channel_name].removeClient(client, msg);
// }


/*!
** @brief	Compares @a client_pass to the server's password.
**
** @details	Per the IRC standard. We can use this to decide whether we accept
**			the client's registration attempt.
** @param	client_pass	The password provided by the client during registration.
** @return	true if equal, otherwise false.
*/
bool	IRC_Server::confirm_pass(std::string const & client_pass) const
{
	return (_servpass == client_pass);
}

std::string	IRC_Server::get_datetime(void)
{
	time_t		raw_timestamp = std::time(NULL);

	if (raw_timestamp == -1)
		return ("time unknown");
	struct tm *	processed_timestamp = std::localtime(&raw_timestamp);
	char		timestamp[25];

	std::strftime(timestamp, 25, "%X %a %d %b %Y", processed_timestamp);
	return (std::string(timestamp));
}

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
void	IRC_Server::server_loop(void)
{
	// //debug
	// Client	test_client;
	// test_client.set_nick("miki");
	// Channel	test_channel(test_client, std::string("cutre"));
	// add_channel(test_channel);
	// //debug
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
			for (int i = 1; poll_count > 0; ++i) //first POLLIN with listener-only array MUST be a new connection; this for only tests client fds
			{
				if (poll_client(i) == true)
				{
					process_client_message(_clients[i]);
					--poll_count;
				}
			}

			//Remove clients flagged for removal
			remove_flagged_clients();
		}
	}
}
