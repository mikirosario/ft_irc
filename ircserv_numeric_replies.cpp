/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_numeric_replies.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 15:40:22 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/21 16:09:29 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

/*!
** @brief	Builds the initial, invariant part of any numeric reply, consisting
**			of COLON<prefix>SPACE<numeric>SPACE<client>SPACE.
**
** @param	recipient	A reference to the client to whom the reply will be sent.
** @param	numeric		The numeric. ( see @a irc_numerics.hpp ).
** @return	A string containing the initial, invariant part of a numeric reply.
*/
std::string	IRC_Server::numeric_reply_start(Client const & recipient, char const * numeric) const
{
	std::string	error;

	//part of the msg with controlled size:
	//: IPv4Address SPACE NUMERIC SPACE CLIENT SPACE:
	//uncontrolled part:
	//description
	//NOTE: ensure NICK size is limited during registration...

	error += get_source() + " " ;
	error += numeric;
	error += " ";
	if (recipient.is_registered() == true) //first parameter should be client name, but if client is unregistered this hasn't yet been recorded
		error += recipient.get_nick() + " "; //debug //will have a get_client() for formatted replies: nick!user@userIP
	return (error);
}

/*!
** @brief	The @a reply will be properly ended with the @a description, or with
**			an empty argument if an empty @a description is provided, and
**			and crlf-termination.
**
** @details	This function will automatically append a @a description to a
**			numeric reply as the final parameter and properly crlf-terminate it.
**			The @a description is optional and an empty string may be sent in
**			lieu of one. If including @a description would make @a reply longer
**			than MSG_BUF_SIZE, @a description will be truncated as needed to
**			fit.
**
**			This function will correctly terminate @a reply leaving it ready to
**			be sent to the client **IF** the calling function correctly filled
**			in the parameters for the given numeric. It does NOT guarantee
**			correctness of the string PRECEDING the final parameter.
**
**			If @a reply was already crlf-terminated or terminated with a space
**			or space and colon, or null-terminated, all such existing
**			terminations will be erased and replaced.
** @param	reply		A reference to the numeric reply to be ended.
** @param	description	A reference to the description to append to the end of
**						the numeric reply as the final parameter.
*/
void		IRC_Server::numeric_reply_end(std::string & reply, std::string const & description) const
{
	size_t	bytes_remaining;
	size_t	pos;

	if ((pos = reply.find_last_not_of(" :\r\n\0")) != std::string::npos)
		reply.erase(pos + 1, std::string::npos);
	reply += " :";
	bytes_remaining = MSG_BUF_SIZE - reply.size() - 2;
	reply.append(description, 0, bytes_remaining);
	reply += "\r\n";
}

void		IRC_Server::send_rpl_WELCOME(Client const & recipient)
{
	std::string msg = numeric_reply_start(recipient, RPL_WELCOME);

	std::string	welcome_msg;

	welcome_msg += "Welcome to the ";
	welcome_msg += _networkname;
	welcome_msg += " Network, ";
	welcome_msg += recipient.get_nick();
	welcome_msg += "!";
	welcome_msg += recipient.get_username();
	welcome_msg += "@";
	welcome_msg += recipient.get_hostname();

	numeric_reply_end(msg, welcome_msg);
	recipient.send_msg(msg);
}