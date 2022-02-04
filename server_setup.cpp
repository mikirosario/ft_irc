/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_setup.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 18:55:07 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/04 20:36:59 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

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
int	get_listener_socket(void)
{
	char const *			service = "irc";
	struct addrinfo			hints;
	struct addrinfo *		res = NULL; //linked list
	//struct addrinfo *		list_index;
	int						gai_status;
	int						ret = -1;

	std::memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE; // fill in my IP for me
	gai_status = getaddrinfo(NULL, service, &hints, &res);
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
				print_err(std::string("Bind socket file descriptor to IRC port failed."));
			else if (fcntl(connection_sockfd, F_SETFL, O_NONBLOCK) == - 1) //non-blocking flag
				print_err(std::string("Fcntl non-blocking call failed failed."));
			else if (listen(connection_sockfd, 20) == -1)
				print_err(std::string("Listen on IRC port through socket file descriptor failed."));
			else
			{
				ret = connection_sockfd;
				std::cout << "Got connection socket!" << std::endl;
			}
			if (ret == -1)
				if ((close(connection_sockfd)) == -1)
					print_err(std::string("Close socket from get_listener_socket failed."));
		}
		freeaddrinfo(res); //debug res is assigned if getaddrinfo fails? freeaddrinfo handles NULL ref?
	}
	return (ret);
}
