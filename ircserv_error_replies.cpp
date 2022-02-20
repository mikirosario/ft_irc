/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_error_replies.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 15:12:34 by miki              #+#    #+#             */
/*   Updated: 2022/02/20 16:50:23 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

/*!
** @brief	Builds the initial, invariant part of an error reply, consisting of
**			COLON<prefix>SPACE<numeric>SPACE<client>SPACE.
**
** @param	recipient A reference to the client to whom the reply will be sent.
** @param	numeric	The error numeric. ( see @a irc_numerics.hpp ).
** @return	A string containing the initial, invariant part of an error reply.
*/
std::string	IRC_Server::err_reply_start(Client const & recipient, char const * numeric) const
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
** @brief	The @a error_reply will be properly ended with the @a description,
**			or with an empty argument if an empty @a description is provided,
**			and crlf-termination.
**
** @details	This function will automatically append a @a description to an error
**			reply as the final parameter and properly crlf-terminate it. The
**			@a description is optional and an empty string may be sent in lieu
**			of one. If including @a description would make @a error_reply longer
**			than MSG_BUF_SIZE, @a description will be truncated as needed to
**			fit.
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
** @brief	Sends an UNKNOWNERROR error reply to the @a recipient.
**
** @details This is a generic error reply that can be sent to a client when a
**			command is rejected for any reason not explicitly covered by another
**			numeric. The error reply will indicate that @a command could not be
**			processed for the reason outlined in @a description.
**
**			A description of the problem should be provided in this case. If
**			including @a description would cause the message to be over 512
**			bytes long, it will be truncated to fit.
**
**			The UNKNOWNERROR error follows this format:
**
**			:prefix numeric recipient command :description
** @param	recipient	The client to whom the reply will be sent.
** @param	command		The command from the recipient that was not recognized.
** @param	description	Description of the problem.
*/
void		IRC_Server::send_err_UNKNOWNERROR(Client const & recipient, std::string const & command, std::string const & description) const
{
	std::string	msg = err_reply_start(recipient, ERR_UNKNOWNERROR);

	msg += command;
	err_reply_end(msg, description);
	recipient.send_msg(msg);
}

/*!
** @brief	Sends an UNKNOWNCOMMAND error reply to the @a recipient.
**
** @details	An error reply will be sent to @a recipient indicating that
**			@a command received from @a recipient is unknown and cannot be
**			interpreted. The @a description argument can be an empty string if
**			no description is desired. If including @a description would cause
**			the message to be over 512 bytes long, it will be truncated to fit.
**
**			The UNKNOWNCOMMAND error follows this format:
**
**			 ":prefix numeric recipient command :description"
** @param	recipient	The client to whom the reply will be sent.
** @param	command		The command from the recipient that was not recognized.
** @param	description	Optional additional description of the problem.
*/
void	IRC_Server::send_err_UNKNOWNCOMMAND(Client const & recipient, std::string const & command, std::string const & description) const
{
	std::string msg = err_reply_start(recipient, ERR_UNKNOWNCOMMAND);

	msg += command;
	err_reply_end(msg, description);
	recipient.send_msg(msg);
}

/*!
** @brief	Sends an ALREADYREGISTERED error reply to the @a recipient.
**
** @details	An error reply will be sent to @a recipient indicating that the
**			command they sent cannot be executed as it would affect data that
**			can only be set during registration. The @a description argument is
**			can be an empty string if no description is desired. If including
**			@a description would cause the message to be over 512 bytes long, it
**			will be truncated to fit.
**
**			The ALREADYREGISTERED error follows this format:
**
**			 ":prefix numeric recipient :description"
** @param	recipient	The client to whom the reply will be sent.
** @param	description	Optional additional description of the problem.
*/
void	IRC_Server::send_err_ALREADYREGISTERED(Client const & recipient, std::string const & description) const
{
	std::string msg = err_reply_start(recipient, ERR_ALREADYREGISTERED);

	err_reply_end(msg, description);
	recipient.send_msg(msg);
}

/*!
** @brief	Sends a NEEDMOREPARAMS error reply to the @a recipient.
**
** @details	An error reply will be sent to @a recipient indicating that the
**			@a command received from @a recipient was not executed because it
**			was missing some required parameter(s). The @a description argument
**			can be an empty string if no description is desired. If including
**			@a description would cause the message to be over 512 bytes long, it
**			will be truncated to fit.
**
**			The NEEDMOREPARAMS error follows this format:
**
**			 ":prefix numeric recipient command :description"
** @param	recipient	The client to whom the reply will be sent.
** @param	command		The command from @a recipient that was not executed.
** @param	description	Optional additional description of the problem.
*/
void	IRC_Server::send_err_NEEDMOREPARAMS(Client const & recipient, std::string const & command, std::string const & description) const
{
	std::string msg = err_reply_start(recipient, ERR_NEEDMOREPARAMS);

	msg += command;
	err_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NONICKNAMEGIVEN(Client const & recipient, std::string const & description) const
{
	std::string msg = err_reply_start(recipient, ERR_NONICKNAMEGIVEN);

	err_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NICKNAMEINUSE(Client const & recipient, std::string const & nick, std::string const & description) const
{
	std::string msg = err_reply_start(recipient, ERR_NICKNAMEINUSE);
	
	msg += nick;
	err_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_ERRONEOUSNICKNAME(Client const & recipient, std::string const & nick, std::string const & description) const
{
	std::string	msg = err_reply_start(recipient, ERR_ERRONEOUSNICKNAME);

	msg += nick;
	err_reply_end(msg, description);
	recipient.send_msg(msg);
}
