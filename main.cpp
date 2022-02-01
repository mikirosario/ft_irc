/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 12:23:47 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/01 17:02:51 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_ntop
#include <netdb.h>
#include <unistd.h> //close
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

//DNS resolve test
// int	main(void)
// {
// 	char const *			node = "www.github.com";
// 	char const *			service = "https";
// 	char					ipstr[INET6_ADDRSTRLEN];
// 	struct addrinfo			hints;
// 	struct addrinfo *		res; //linked list
// 	struct addrinfo *		list_index;
// 	int						gai_status;

// 	std::memset(&hints, 0, sizeof(addrinfo));
// 	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
// 	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
// 	hints.ai_flags = AI_PASSIVE; // fill in my IP for me
// 	gai_status = getaddrinfo(node, service, &hints, &res);
// 	if (gai_status) //non-zero gai_status means error, no allocation in that case
// 		std::cerr << "getaddrinfo error: " << gai_strerror(gai_status) << std::endl;
// 	else
// 	{
// 		std::cout << "IP Address for: " << node << "\n";
		
// 		for (list_index = res; list_index != NULL; list_index = list_index->ai_next)
// 		{
// 			void *		addr;
// 			std::string	ipver;

// 			if (list_index->ai_family == AF_INET) //IPv4
// 			{
// 				struct sockaddr_in *	ipv4 = reinterpret_cast<struct sockaddr_in *>(list_index->ai_addr);
// 				addr = &(ipv4->sin_addr);
// 				ipver = "IPv4";
// 			}
// 			else //IPv6
// 			{
// 				struct sockaddr_in6 *	ipv6 = reinterpret_cast<struct sockaddr_in6 *>(list_index->ai_addr);
// 				addr = &(ipv6->sin6_addr);
// 				ipver = "IPv6";
// 			}
// 			inet_ntop(list_index->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
// 			std::cout << ipver << ": " << ipstr << std::endl;
// 		}
// 		freeaddrinfo(res);
// 	}
// 	return (0);
// }

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
		return (&(reinterpret_cast<struct sockaddr_in *>(sa)->sin_addr));
    else
		return (&(reinterpret_cast<struct sockaddr_in6 *>(sa)->sin6_addr));
}

//Listen test
int	main(void)
{
	char const *			service = "irc";
	struct addrinfo			hints;
	struct addrinfo *		res; //linked list
	//struct addrinfo *		list_index;
	int						gai_status;

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

		if ((connection_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
			std::cerr << "Socket file descriptor open failed." << "\n";
		else
		{
			if (bind(connection_sockfd, res->ai_addr, res->ai_addrlen) == -1)
				std::cerr << "Bind socket file descriptor to " << service << " port failed." << "\n";
			else if (listen(connection_sockfd, 20) == -1)
				std::cerr << "Listen on " << service << " port through socket file descriptor failed." << "\n";
			else
			{
				struct sockaddr_storage	their_addr;
				socklen_t				addr_size = sizeof(struct sockaddr_storage);
				char					ipstr[INET6_ADDRSTRLEN];
				int						data_sockfd;
				while (1)
				{
					std::cout << "Chiripi" << std::endl;
					data_sockfd = accept(connection_sockfd, reinterpret_cast<struct sockaddr *>(&their_addr), &addr_size); //connect() request over connection_sockfd returns
					if (data_sockfd < 0)
						std::cerr << "Accept " << service << " connection through socket file descriptor failed." << "\n";
					//do stuff

					inet_ntop(their_addr.ss_family, get_in_addr(reinterpret_cast<struct sockaddr *>(&their_addr)), ipstr, INET6_ADDRSTRLEN);
					std::cout << "server: got connection from " << ipstr << std::endl;

					if (fork() == 0)
					{
						close(connection_sockfd);
						if (send(data_sockfd, "Hello world!", 13, 0) == -1) //this would be in own loop if only chunk was sent?
							perror("send");
						close(data_sockfd);
						exit(EXIT_SUCCESS);
					}
				}
			}
			if (close(connection_sockfd) == -1)
				std::cerr << "Close socket file descriptor failed." << "\n";
			else
				std::cout << "Everything OK!" << std::endl;
		}
		

		freeaddrinfo(res);
	}
	return (0);
}