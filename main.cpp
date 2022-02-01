/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 12:23:47 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/01 20:44:54 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_ntop
#include <netdb.h>
#include <unistd.h> //close
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <cstring> //for memset


#define MAX_CONNECTIONS 1024 //debug move me to header

int	listener_fd;
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

// //Listen test
// int	main(void)
// {
// 	char const *			service = "irc";
// 	struct addrinfo			hints;
// 	struct addrinfo *		res; //linked list
// 	//struct addrinfo *		list_index;
// 	int						gai_status;

// 	std::memset(&hints, 0, sizeof(addrinfo));
// 	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
// 	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
// 	hints.ai_flags = AI_PASSIVE; // fill in my IP for me
// 	gai_status = getaddrinfo(NULL, service, &hints, &res);
// 	if (gai_status) //non-zero gai_status means error, no allocation in that case
// 		std::cerr << "getaddrinfo error: " << gai_strerror(gai_status) << std::endl;
// 	else //attempt to open socket and listen
// 	{
// 		//iterate results list for valid entry

// 		int	connection_sockfd;

// 		if ((connection_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
// 			std::cerr << "Socket file descriptor open failed." << "\n";
// 		else
// 		{
// 			if (bind(connection_sockfd, res->ai_addr, res->ai_addrlen) == -1)
// 				std::cerr << "Bind socket file descriptor to " << service << " port failed." << "\n";
// 			else if (listen(connection_sockfd, 20) == -1)
// 				std::cerr << "Listen on " << service << " port through socket file descriptor failed." << "\n";
// 			else
// 			{
// 				struct sockaddr_storage	their_addr;
// 				socklen_t				addr_size = sizeof(struct sockaddr_storage);
// 				char					ipstr[INET6_ADDRSTRLEN];
// 				int						data_sockfd;

// 				while (1)
// 				{
// 					std::cout << "Chiripi" << std::endl;
// 					data_sockfd = accept(connection_sockfd, reinterpret_cast<struct sockaddr *>(&their_addr), &addr_size); //connect() request over connection_sockfd returns
// 					if (data_sockfd < 0)
// 						std::cerr << "Accept " << service << " connection through socket file descriptor failed." << "\n";
// 					//do stuff

// 					inet_ntop(their_addr.ss_family, get_in_addr(reinterpret_cast<struct sockaddr *>(&their_addr)), ipstr, INET6_ADDRSTRLEN);
// 					std::cout << "server: got connection from " << ipstr << std::endl;

// 					if (fork() == 0)
// 					{
// 						close(connection_sockfd);
// 						if (send(data_sockfd, "Hello world!", 13, 0) == -1) //this would be in own loop if only chunk was sent?
// 							perror("send");
// 						close(data_sockfd);
// 						exit(EXIT_SUCCESS);
// 					}
// 				}
// 			}
// 			if (close(connection_sockfd) == -1)
// 				std::cerr << "Close socket file descriptor failed." << "\n";
// 			else
// 				std::cout << "Everything OK!" << std::endl;
// 		}
		

// 		freeaddrinfo(res);
// 	}
// 	return (0);
// }

//Return listener socket
int	get_listener_socket(void)
{
	char const *			service = "irc";
	struct addrinfo			hints;
	struct addrinfo *		res; //linked list
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

		if ((connection_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
			std::cerr << "Socket file descriptor open failed." << "\n";
		else
		{
			if (bind(connection_sockfd, res->ai_addr, res->ai_addrlen) == -1)
				std::cerr << "Bind socket file descriptor to " << service << " port failed." << "\n";
			else if (fcntl(connection_sockfd, F_SETFL, O_NONBLOCK) == - 1)
			{
				close(connection_sockfd);
				std::cerr << "Fcntl non-blocking call failed failed.\n";
			}
			else if (listen(connection_sockfd, 20) == -1)
			{
				close(connection_sockfd);
				std::cerr << "Listen on " << service << " port through socket file descriptor failed." << "\n";
			}
			else
			{
				ret = connection_sockfd;
				std::cout << "Everything OK!" << std::endl;
			}
		}
		freeaddrinfo(res);
	}
	return (ret);
}

void	add_to_pfds(struct pollfd (&pfds)[MAX_CONNECTIONS], const int newfd, int & count)
//void	add_to_pfds(struct pollfd * pfds, const int newfd, int & count)
{
	// std::cerr << "received: " << newfd << std::endl; //debug
	// std::cerr << "count: " << count << std::endl; //debug
	pfds[count].fd = newfd;
	//std::cerr << "recorded: " << pfds[count].fd << std::endl; //debug
	pfds[count].events = POLLIN;
	++count;
}

void	del_from_pfds(struct pollfd (&pfds)[MAX_CONNECTIONS], const int index, int & count)
//void	del_from_pfds(struct pollfd * pfds, const int index, int & count)
{
	pfds[index] = pfds[count - 1];
	--count;
}

void	signal_handler(int sig)
{
	if (sig == SIGINT && listener_fd > 0)
	{
		close(listener_fd);
		exit(0);
	}
}

int	main(void)
{
	int							connection_count = 0;
	//int							listener;
	int							new_connection;
	struct	sockaddr_storage	remoteaddr;
	socklen_t					addrlen = sizeof(remoteaddr);
	struct pollfd				pfds[MAX_CONNECTIONS];
	char						remoteIP[INET6_ADDRSTRLEN];
	char						msgbuf[1024];
	//std::string					msgbuf(1024, '\0'); //pre-reserve 1024 bytes
	
	if ((pfds[connection_count].fd = get_listener_socket()) == -1)
		return (-1);
	listener_fd = pfds[connection_count].fd;
	signal(SIGINT, signal_handler);
	pfds[connection_count++].events = POLLIN; //report ready to read on incoming
	while (1)
	{
		int	poll_count = poll(pfds, connection_count, -1);
		//std::cerr << poll_count << std::endl; //debug
		if (poll_count == -1)
		{
			std::cerr << "FATAL poll error" << std::endl;
			return (-1);
		}
		if (pfds[0].revents & POLLIN) //if listener is ready to read, we have new connection
		{
			new_connection = accept(pfds[0].fd, reinterpret_cast<struct sockaddr *>(&remoteaddr), &addrlen);
			if (new_connection == -1)
				std::cerr << "accept error" << std::endl;
			else
			{
				add_to_pfds(pfds, new_connection, connection_count);
				//std::cerr << "new connection fd: " << new_connection << " recorded fd: " << pfds[1].fd << std::endl; //debug
				std::cout << "pollserver: new connection from " << inet_ntop(remoteaddr.ss_family, get_in_addr(reinterpret_cast<struct sockaddr *>(&remoteaddr)), remoteIP, INET6_ADDRSTRLEN) << " on socket " << new_connection << std::endl;
			}
			--poll_count;
		}
		for (int i = 1, polled = 0; polled < poll_count; ++i) //first POLLIN with listener-only array MUST be a new connection; this for only tests client fds
		{
			if (pfds[i].revents & POLLIN) //client fd pending data receipt
			{
				int nbytes = recv(pfds[i].fd, msgbuf, sizeof msgbuf, 0);
				switch (nbytes) //error cases and default successful data reception case
				{
					case 0 :
						std::cerr << "pollserver: socket " << pfds[i].fd << "hung up." << std::endl;
					case -1 :
						std::cerr << "recv error" << std::endl;
						close(pfds[i].fd);
						del_from_pfds(pfds, i, connection_count);
						break ;
					default :
						for (int j = 1; j < connection_count; ++j) //send to all clients
							if (j != i) //do not send to self
								if (send(pfds[j].fd, msgbuf, nbytes, 0) == -1)
									std::cerr << "send error" << std::endl;
				}
				++polled;
			}
		}
	}
	close(pfds[0].fd);
	return (0);
}