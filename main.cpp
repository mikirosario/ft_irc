/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 12:23:47 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/26 21:26:16 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifdef __linux__
# include <csignal> //for SIGNAL DEFINITIONS ON LINUX
# include <cstdlib> //for EXIT definitions on LINUX
#elif __APPLE__
#include <clocale> //for std::setlocale on Mac
#endif
#include "./includes/ircserv.hpp"

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

// ---- CLOSE_SERVER ---- //
/*!
** @brief Closes program.
**
** @param exit_type EXIT_SUCCESS or EXIT_FAILURE
** @param close_event Description of reason for closure.
*/
void	close_server(int const exit_type, std::string const & close_event)
{
	if (exit_type == EXIT_SUCCESS)
		std::cout << '\n' << close_event << std::endl;
	else
		std::cerr << '\n' << close_event << '\n' << "CONTACT YOUR SERVER ADMIN." << std::endl;
	exit(exit_type);
}

/*!
** @brief Handles any keyboard signals while the program is running.
**
** @param sig Signal type (SIGINT, SIGQUIT, etc.)
*/
void	signal_handler(int sig)
{
	if (sig == SIGINT)
		close_server(EXIT_SUCCESS, std::string("IRCSERV CLOSED ON CTRL+C/SIGINT."));
}

// ---- GET_ARG --- //
/*!
** @brief	Retrieves the argument arg_name from the argv array.
**
** @details	The Args enum defines NETINFO, PORT and PASSWORD arguments,
**			equivalent to 3, 2 and 1, respectively, in terms of their distance
**			from argc. (NETINFO is argc - 3, PASSWORD is argc - 1, etc.) This
**			relation is invariant to argument count and is used to quickly
**			calculate the desired argument address using pointer arithmetic.
**
** @param	argv The argument array passed to the program from the command line.
** @param	argc The argument count.
** @param	arg_name Args(NETINFO), Args(PORT) or Args(PASSWORD).
** @return	A pointer to the argument indicated by arg_name in the argv array.
*/
static char const *	get_arg(char const * const * argv, int const argc, enum Args const & arg_name)
{
	return (*(argv + argc - arg_name));
}

// ---- MAIN ---- //
/*!
** @brief	If the argument count is correct, attempts to instantiate the
**			IRC_Server server object.
**
** @details Due to obsolete subject specifications, we are required to parse for
**			network information for server-to-server connections, even though we
**			no longer use them in this iteration of the project. The server
**			object will accept and save network information if given, but it
**			will not use it. Otherwise, an empty string can be passed in place
**			of 'netinfo'.
**
**			In the 4 arguments case, we will accept the netinfo argument unless
**			it is filled only with white spaces, removing any leading white
**			spaces.
**
**			In both the 4 and 3 arguments cases we accept the port and
**			password arguments and attempt to instantiate a server instance
**			using all arguments. If no netinfo argument was given, or netinfo
**			was full of white spaces, it will be an empty string.
**
**			In any other case we scold the user for not entering the requisite
**			number of arguments.
**
** @param	argc The command line argument count.
** @param	argv The command line argument array.
** @return	Returns -1 if the wrong number of arguments are given, otherwise 0.
**			We can use the server state in future to return more error codes if
**			the server closes unexpectedly. (See IRC_Server::State)
**/
int	main(int argc, char ** argv)
{
	std::string	netinfo;
	std::string	port;
	std::string	pass;

	std::setlocale(LC_ALL, "C"); //explicitly set locale

	// //debug
	// std::cout << "Version: " << VERSION << std::endl;
	// //debug
	signal(SIGINT, signal_handler);
	switch (argc)
	{
		case (4):
			for (char const * arg = get_arg(argv, argc, Args(NETINFO)); *arg; ++arg) //remove leading white spaces
				if (!std::isspace(*arg))
					netinfo = arg;
		case (3):
		{
			port = get_arg(argv, argc, Args(PORT));
			pass = get_arg(argv, argc, Args(PASSWORD));
			IRC_Server	server_instance(port, pass, netinfo);
			break ;
		}
		default:
			std::cerr	<< "> ./ircserv [host:port_network:password_network] port password\n"
						<< "> ./ircserv port password"
						<< std::endl;
	}
	return (0);
}
