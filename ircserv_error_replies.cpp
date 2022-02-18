/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_error_replies.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 15:12:34 by miki              #+#    #+#             */
/*   Updated: 2022/02/18 15:51:20 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

void	IRC_Server::send_err_UNKNOWNCOMMAND(Client const & client, std::string const & description)
{
	std::string msg;
	size_t		bytes_remaining;

	//part of the msg with controlled size:
	//: IPv4Address SPACE NICK SPACE :
	//uncontrolled part:
	//description
	//NOTE: ensure NICK size is limited during registration...
	msg += get_source() + " ";
	if (client.is_registered() == true)
		msg += client.get_nick() + " ";
	msg += ERR_UNKNOWNCOMMAND;
	msg += " :";
	bytes_remaining = MSG_BUF_SIZE - msg.size() - 2;
	msg.append(description, 0, bytes_remaining);
	msg += "\r\n";
	
	//debug
	std::cout << msg.data() << std::endl;
	//debug
	//send()	
}
