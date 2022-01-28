/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 12:23:47 by mrosario          #+#    #+#             */
/*   Updated: 2022/01/28 13:16:45 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_ntop
#include <netdb.h>
#include <iostream>
#include <cstring> //for memset

//Notes
//service	==	port
//------------------
//http		==	80
//https		==	443
//irc		==	194

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

int	main(void)
{
	//DNS resolve test
	char const *			node = "www.google.com";
	char const *			service = "https";
	char					ipstr[INET6_ADDRSTRLEN];
	struct addrinfo			hints;
	struct addrinfo *		res; //linked list
	struct addrinfo *		list_index;
	int						status;

	std::memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE; // fill in my IP for me
	status = getaddrinfo(node, service, &hints, &res);
	if (status) //non-zero status means error, no allocation in that case
		std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
	else
	{
		std::cout << "IP Address for www.google.com: " << "\n";
		
		for (list_index = res; list_index != NULL; list_index = list_index->ai_next)
		{
			void *		addr;
			std::string	ipver;

			if (list_index->ai_family == AF_INET) //IPv4
			{
				struct sockaddr_in *	ipv4 = reinterpret_cast<struct sockaddr_in *>(list_index->ai_addr);
				addr = &(ipv4->sin_addr);
				ipver = "IPv4";
			}
			else
			{
				struct sockaddr_in6 *	ipv6 = reinterpret_cast<struct sockaddr_in6 *>(list_index->ai_addr);
				addr = &(ipv6->sin6_addr);
				ipver = "IPv6";
			}
			inet_ntop(list_index->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
			std::cout << ipver << ": " << ipstr << std::endl;
		}
		freeaddrinfo(res);
	}
	return (0);
}