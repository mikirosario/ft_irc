/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_numeric_error_replies.cpp                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 15:12:34 by miki              #+#    #+#             */
/*   Updated: 2022/03/12 12:39:06 by acortes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ircserv.hpp"

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
	std::string	msg = numeric_reply_start(recipient, ERR_UNKNOWNERROR);

	msg += command;
	numeric_reply_end(msg, description);
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
	std::string msg = numeric_reply_start(recipient, ERR_UNKNOWNCOMMAND);
	msg += recipient.get_source() + " ";

	msg += command;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void		IRC_Server::send_err_ERR_UNKNOWNMODE(Client const & recipient, char const &command, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_UNKNOWNMODE);

	msg += command;
	numeric_reply_end(msg, description);
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
	std::string msg = numeric_reply_start(recipient, ERR_ALREADYREGISTERED); 
	msg += recipient.get_source() + " ";

	numeric_reply_end(msg, description);
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
	std::string msg = numeric_reply_start(recipient, ERR_NEEDMOREPARAMS); 
	msg += recipient.get_source() + " ";

	msg += command;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NONICKNAMEGIVEN(Client const & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NONICKNAMEGIVEN); 
	msg += recipient.get_source() + " ";

	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NICKNAMEINUSE(Client const & recipient, std::string const & nick, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NICKNAMEINUSE); 
	msg += recipient.get_source() + " ";
	
	msg += nick;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_ERRONEOUSNICKNAME(Client const & recipient, std::string const & nick, std::string const & description) const
{
	std::string	msg = numeric_reply_start(recipient, ERR_ERRONEOUSNICKNAME);

	msg += nick;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_PASSWDMISMATCH(Client const & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_PASSWDMISMATCH); 
	msg += recipient.get_source() + " ";
	
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_INPUTTOOLONG(Client const & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_INPUTTOOLONG); 
	msg += recipient.get_source() + " ";

	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NOTREGISTERED(Client const & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOTREGISTERED); 
	msg += recipient.get_source() + " ";

	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NOTEXTTOSEND(Client const & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOTEXTTOSEND); 
	msg += recipient.get_source() + " ";

	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NORECIPIENT(Client const & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NORECIPIENT); 
	msg += recipient.get_source() + " ";

	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NOSUCHNICK(Client const & recipient, std::string const & nick, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOSUCHNICK); 
	msg += recipient.get_source() + " ";

	msg += nick;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

// Join errors

void	IRC_Server::send_err_NOSUCHCHANNEL(Client const & recipient, std::string const & command, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOSUCHCHANNEL); 
	msg += recipient.get_source() + " ";

	msg += command;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_TOOMANYCHANNELS(Client const & recipient, std::string const & command, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_TOOMANYCHANNELS); 
	msg += recipient.get_source() + " ";

	msg += command;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_BADCHANMASK(Client const & recipient, std::string const & channel_name, std::string const & description) const
{
	std::string	msg = numeric_reply_start(recipient, ERR_BADCHANMASK);
	msg += recipient.get_source() + " ";

	msg += channel_name;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_BADCHANNELKEY(Client const & recipient, Channel const & channel, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_BADCHANNELKEY); 
	msg += recipient.get_source() + " ";

	msg += channel.getChannelName();
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NOTONCHANNEL(Client const & recipient, Channel const & channel, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOTONCHANNEL);
	msg += recipient.get_source() + " ";

	msg += channel.getChannelName();
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_BANNEDFROMCHAN(Client const & recipient, std::string const & command, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_BANNEDFROMCHAN); 
	msg += recipient.get_source() + " ";

	msg += command;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_CHANNELISFULL(Client const & recipient, std::string const & command, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_CHANNELISFULL); 
	msg += recipient.get_source() + " ";

	msg += command;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_INVITEONLYCHAN(Client const & recipient, std::string const & command, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_INVITEONLYCHAN); 
	msg += recipient.get_source() + " ";

	msg += command;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}
