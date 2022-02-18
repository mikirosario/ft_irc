/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_error_replies.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 15:12:34 by miki              #+#    #+#             */
/*   Updated: 2022/02/18 17:17:24 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

/*!
** @brief	Builds the initial, invariant part of an error reply, consisting of
**			COLON<prefix>SPACE<numeric>SPACE<client>SPACE.
**
** @param	client A reference to the client to whom the reply will be sent.
** @param	numeric	The error numeric. ( see @a irc_numerics.hpp ).
** @return	A string containing the initial, invariant part of an error reply.
*/
std::string	IRC_Server::err_reply_start(Client const & client, char const * numeric) const
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
	if (client.is_registered() == true) //first parameter should be client name, but if client is unregistered this hasn't yet been recorded
		error += client.get_nick() + " "; //will have a get_client() for formatted replies: nick!user@userIP
	return (error);
}

/*!
** @brief	The @a error_reply will be properly ended with the @a description,
**			or with an empty argument if no @a description is provided, and
**			crlf-termination.
**
** @details	This function will automatically append a @a description to an error
**			reply as the final parameter and properly crlf-terminate it. The
**			@a description is optional and an empty parameter will appended in
**			lieu of one. If including @a description would make @a error_reply
**			longer than MSG_BUF_SIZE, @a description will be truncated as needed
**			to fit.
**
**			This function will correctly terminate @a error_reply leaving it
**			ready to be sent to the client **IF** the calling function correctly
**			filled out the parameters for the given error numeric. It does NOT
**			guarantee correctness of the string PRECEDING the final parameter.
**
**			If @a error_reply was already crlf-terminated or terminated with
**			a space or space and colon, or null-terminated, all such existing
**			terminations will be erased and replaced.
** @param	error_reply A reference to the error_reply to be ended.
** @param	description A reference to the description to append to the end of
**						the error_reply as the final parameter.
*/
void		IRC_Server::err_reply_end(std::string & error_reply, std::string const & description) const
{
	size_t	bytes_remaining;
	size_t	pos;

	if ((pos = error_reply.find_last_not_of(" :\r\n\0")) != std::string::npos)
		error_reply.erase(pos + 1, std::string::npos);
	error_reply += " :";
	bytes_remaining = MSG_BUF_SIZE - error_reply.size() - 2;
	error_reply.append(description, 0, bytes_remaining);
	error_reply += "\r\n";
}

/*!
** @brief	Sends an UNKNOWNCOMMAND error reply to the @a client.
**
** @details	An error reply will be sent to @a client indicating that @a command
**			received from @a client is unknown and cannot be interpreter. The
**			@a description argument is optional. If @a description is not
**			provided an empty description will be sent. If including
**			@a description would cause the message to be over 512 bytes long, it
**			will be truncated to fit.
**
**			The UNKNOWNCOMMAND error follows this format:
**
**			 ":prefix numeric client command :description"
** @param	client A reference to the client to whom the reply will be sent.
** @param	command The command from the client that was not recognized.
** @param	description Optional additional description of the problem.
*/
void	IRC_Server::send_err_UNKNOWNCOMMAND(Client const & client, std::string const & command, std::string const & description) const
{
	std::string msg = err_reply_start(client, ERR_UNKNOWNCOMMAND);

	msg += command;
	err_reply_end(msg, description);
	
	//debug
	std::cout << msg.data() << std::endl;
	//debug
	//send()
}
