/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 12:23:47 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/10 11:01:45 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifdef __linux__
# include <csignal> //for SIGNAL DEFINITIONS ON LINUX
# include <cstdlib> //for EXIT definitions on LINUX
#endif
#include "ircserv.hpp"

//Global - for use with signal
int	listener_fd; //global variable initializes to 0

//Notes
//service	==	port
//------------------
//http		==	80
//https		==	443
//irc		==	194, 6667 sin permisos

//ADDRINFO STRUCT
/*
    struct addrinfo {
        int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
        int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
        int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
        int              ai_protocol;  // use 0 for "any"
        size_t           ai_addrlen;   // size of ai_addr in bytes
        struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
        char            *ai_canonname; // full canonical hostname
    
        struct addrinfo *ai_next;      // linked list, next node
    };
*/

//SOCKADDR STRUCT
/*
    struct sockaddr {																		BYTES
        unsigned short    sa_family;    // address family, AF_xxx							2
        char              sa_data[14];  // 14 bytes of protocol address						14
    }; 																						16 OK
*/

//SOCKADDR_IN STRUCT
// (IPv4 only)
/*
    struct sockaddr_in {																	BYTES
        short int          sin_family;  // Address family, AF_INET							2
        unsigned short int sin_port;    // Port number										2
        struct in_addr     sin_addr;    // Internet address (uint32_t / 4-byte address)		4 -> IPv4 addr starts here (sockaddr + 4)
        unsigned char      sin_zero[8]; // Same size as struct sockaddr						8 -> padded with zeros here
    };																						16 OK
*/

//SOCKADDR_IN6 STRUCT
// (IPv6 only)
/*
    struct sockaddr_in6 {																	BYTES
        u_int16_t       sin6_family;   // address family, AF_INET6							2
        u_int16_t       sin6_port;     // port number, Network Byte Order					2
        u_int32_t       sin6_flowinfo; // IPv6 flow information								4
        struct in6_addr sin6_addr;     // IPv6 address (16-byte monster address)			16 -> IPv6 addr starts here (sockaddr + 8)
        u_int32_t       sin6_scope_id; // Scope ID											4 -> What??
    };																						28 -> WHAT??? How is it not overflow of sockaddr??? :?
*/

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
void *	get_in_addr(struct sockaddr * sa)
{
    if (sa->sa_family == AF_INET)
		return (&(reinterpret_cast<struct sockaddr_in *>(sa)->sin_addr));
    else
		return (&(reinterpret_cast<struct sockaddr_in6 *>(sa)->sin6_addr));
}


/*! @brief	Adds a new connection socket to the end of the unordered pfds array.
**
** @param pfds	A reference to an array of MAX_CONNECTIONS pollfd structs.
** @param newfd	The new connection socket to be added.
** @param count	A reference to the integer keeping track of the number of
**				elements in the pfds array.
**
*/
void	add_to_pfds(struct pollfd (&pfds)[MAX_CONNECTIONS], int const newfd, int & count)
{
	// std::cerr << "received: " << newfd << std::endl; //debug
	// std::cerr << "count: " << count << std::endl; //debug
	pfds[count].fd = newfd;
	//std::cerr << "recorded: " << pfds[count].fd << std::endl; //debug
	pfds[count].events = POLLIN;
	++count;
}

//debug THIS PFDS THING COULD BE TURNED INTO A CLASS TO MAKE IT MORE C++-like and user-amigable :P
/*! @brief	Removes the indexed connection from the unordered pfds array.
**
** @details	The connection to be removed is closed by calling the
**			close_connection function. The last element in the pfds array is
**			copied over the element to be removed and the count is reduced by
**			one.
**
** @param pfds	A reference to an array of MAX_CONNECTIONS pollfd structs.
** @param index	A valid index within the pfds array indicating the pollfd object
**				to remove. Behaviour is undefined if the index is invalid.
** @param count	A reference to the integer keeping track of the number of
**				elements in the pfds array.
*/
void	del_from_pfds(struct pollfd (&pfds)[MAX_CONNECTIONS], int const index, int & count)
{
	close_connection(pfds[index].fd);
	pfds[index] = pfds[count - 1];
	--count;
}

/*! @brief Closes program.
**
** @param exit_type EXIT_SUCCESS or EXIT_FAILURE
** @param close_event Description of reason for closure.
*/
void	close_server(int const exit_type, std::string const & close_event)
{
	//close all connections.
	if (listener_fd > 0)
		close(listener_fd);
	if (exit_type == EXIT_SUCCESS)
		std::cout << '\n' << close_event << std::endl;
	else
		std::cerr << '\n' << close_event << '\n' << "CONTACT YOUR SERVER ADMIN." << std::endl;
	exit(exit_type);
}

/*! @brief Handles any keyboard signals while the server is running.
**
** @param sig Signal type (SIGINT, SIGQUIT, etc.)
*/
void	signal_handler(int sig)
{
	if (sig == SIGINT)
		close_server(EXIT_SUCCESS, std::string("IRCSERV CLOSED ON CTRL+C/SIGINT."));
}

int	main(int argc, char ** argv)
{
	// int							connection_count = 0;
	// //int							listener;
	// int							new_connection;
	// struct	sockaddr_storage	remoteaddr;
	// socklen_t					addrlen = sizeof(remoteaddr);
	// struct pollfd				pfds[MAX_CONNECTIONS];
	// char						remoteIP[INET6_ADDRSTRLEN];
	// char						msgbuf[1024];
	//std::string					msgbuf(1024, '\0'); //pre-reserve 1024 bytes
	
	IRC_Server	test;

	if (argc == 2)
	{
		char *	arg = *(argv + 1);
		//remove leading white spaces
		while (*arg && std::isspace(*arg))
			++arg;
		test = std::string(arg);
	}

	// //server setup
	// if ((pfds[connection_count].fd = get_listener_socket()) == -1)
	// 	close_server(EXIT_FAILURE, std::string ("IRCSERV CLOSED ON GET_LISTENER_SOCKET CALL FAILED."));
	// listener_fd = pfds[connection_count].fd;
	// if (signal(SIGINT, signal_handler) == SIG_ERR) //set up signal handler; if they fail, exit
	// 	close_server(EXIT_FAILURE, std::string("IRCSERV CLOSED ON SIGNAL CALL FAILED."));
	// pfds[connection_count++].events = POLLIN; //report ready to read on incoming connection

	// //server loop
	// while (1)
	// {
	// 	int	poll_count = poll(pfds, connection_count, -1);
		
	// 	//Poll listener first
	// 	if (poll_count == -1)
	// 		close_server(EXIT_FAILURE, std::string("FATAL poll error"));
	// 	if (pfds[0].revents & POLLIN) //if listener is ready to read, we have new connection
	// 	{
	// 		new_connection = accept(pfds[0].fd, reinterpret_cast<struct sockaddr *>(&remoteaddr), &addrlen);
	// 		if (new_connection == -1)
	// 			std::cerr << "accept error" << std::endl;
	// 		else
	// 		{
	// 			add_to_pfds(pfds, new_connection, connection_count);
	// 			std::cout << "pollserver: new connection from " << inet_ntop(remoteaddr.ss_family, get_in_addr(reinterpret_cast<struct sockaddr *>(&remoteaddr)), remoteIP, INET6_ADDRSTRLEN) << " on socket " << new_connection << std::endl;
	// 		}
	// 		--poll_count;
	// 	}

	// 	for (int i = 1, polled = 0; polled < poll_count; ++i) //first POLLIN with listener-only array MUST be a new connection; this for only tests client fds
	// 	{
	// 		if (pfds[i].revents & POLLIN) //client fd pending data receipt
	// 		{
	// 			int nbytes = recv(pfds[i].fd, msgbuf, sizeof msgbuf, 0);
	// 			switch (nbytes) //error cases and default successful data reception case
	// 			{
	// 				case 0 :
	// 					std::cerr << "pollserver: socket " << pfds[i].fd << " hung up." << std::endl;
	// 					del_from_pfds(pfds, i, connection_count);
	// 					break ;
	// 				case -1 :
	// 					std::cerr << "recv error" << std::endl;
	// 					del_from_pfds(pfds, i, connection_count);
	// 					break ;
	// 				default :
	// 					for (int j = 1; j < connection_count; ++j) //send to all clients
	// 						if (j != i) //do not send to self
	// 							if (send(pfds[j].fd, msgbuf, nbytes, 0) == -1)
	// 								std::cerr << "send error" << std::endl;
	// 			}
	// 			++polled;
	// 		}
	// 	}
	// }
	// close_server(EXIT_SUCCESS, "Channel closed");
	return (0);
}
