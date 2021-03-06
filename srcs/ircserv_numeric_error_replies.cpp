/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_numeric_error_replies.cpp                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikiencolor <mikiencolor@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 15:12:34 by miki              #+#    #+#             */
/*   Updated: 2022/05/28 16:36:04 by mikiencolor      ###   ########.fr       */
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
void		IRC_Server::send_err_UNKNOWNERROR(Client & recipient, std::string const & command, std::string const & description) const
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
void	IRC_Server::send_err_UNKNOWNCOMMAND(Client & recipient, std::string const & command, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_UNKNOWNCOMMAND);

	msg += command;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void		IRC_Server::send_err_ERR_UNKNOWNMODE(Client & recipient, char const &command, std::string const & description) const
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
void	IRC_Server::send_err_ALREADYREGISTERED(Client & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_ALREADYREGISTERED);

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
void	IRC_Server::send_err_NEEDMOREPARAMS(Client & recipient, std::string const & command, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NEEDMOREPARAMS); 

	msg += command;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NONICKNAMEGIVEN(Client & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NONICKNAMEGIVEN); 

	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NICKNAMEINUSE(Client & recipient, std::string const & nick, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NICKNAMEINUSE); 
	
	msg += nick;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_ERRONEOUSNICKNAME(Client & recipient, std::string const & nick, std::string const & description) const
{
	std::string	msg = numeric_reply_start(recipient, ERR_ERRONEOUSNICKNAME);

	msg += nick;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_PASSWDMISMATCH(Client & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_PASSWDMISMATCH); 
	
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_INPUTTOOLONG(Client & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_INPUTTOOLONG); 

	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NOTREGISTERED(Client & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOTREGISTERED); 

	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NOTEXTTOSEND(Client & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOTEXTTOSEND); 

	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NORECIPIENT(Client & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NORECIPIENT); 

	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NOSUCHNICK(Client & recipient, std::string const & nick, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOSUCHNICK); 

	msg += nick;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

// Join errors

void	IRC_Server::send_err_NOSUCHCHANNEL(Client & recipient, std::string const & channel_name, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOSUCHCHANNEL); 

	msg += channel_name;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_TOOMANYCHANNELS(Client & recipient, std::string const & channel_name, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_TOOMANYCHANNELS); 

	msg += channel_name;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_BADCHANMASK(Client & recipient, std::string const & channel_name, std::string const & description) const
{
	std::string	msg = numeric_reply_start(recipient, ERR_BADCHANMASK);

	msg += channel_name;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_BADCHANNELKEY(Client & recipient, Channel const & channel, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_BADCHANNELKEY); 

	msg += channel.getChannelName();
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_USERNOTINCHANNEL(Client & recipient, Client const & target, Channel const & channel, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_USERNOTINCHANNEL);

	msg += target.get_nick() + " ";
	msg += channel.getChannelName();
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NOTONCHANNEL(Client & recipient, Channel const & channel, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOTONCHANNEL);

	msg += channel.getChannelName();
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void		IRC_Server::send_err_INVITEONLYCHAN(Client & recipient, std::string const & channel_name) const
{
	std::string msg = numeric_reply_start(recipient, ERR_INVITEONLYCHAN);

	msg += channel_name;
	numeric_reply_end(msg, " :Cannot join channel (+i)");
	recipient.send_msg(msg);

}

void		IRC_Server::send_err_USERONCHANNEL(Client & recipient, std::string const &client_name, std::string const &client_nick, Channel const & channel) const
{
	std::string msg = numeric_reply_start(recipient, ERR_USERONCHANNEL);
	msg += client_name + " ";
	msg += client_nick + " ";
	msg += channel.getChannelName();
	std::string description = "is already on channel"; 
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void		IRC_Server::send_err_NOSUCHSERVER(Client & recipient, std::string const & server_name, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOSUCHSERVER);
	msg += server_name;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void		IRC_Server::send_err_NOMOTD(Client & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOMOTD);
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void		IRC_Server::send_err_ERR_CHANOPRIVSNEEDED(Client & recipient, Channel const & channel, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_CHANOPRIVSNEEDED);

	msg += channel.getChannelName();
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void		IRC_Server::send_err_USERSDONTMATCH(Client & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_USERSDONTMATCH);
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void		IRC_Server::send_err_UMODEUNKNOWNFLAG(Client & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_UMODEUNKNOWNFLAG);
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_BANNEDFROMCHAN(Client & recipient, Channel const & channel, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_BANNEDFROMCHAN); 

	msg += channel.getChannelName();
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_CHANNELISFULL(Client & recipient, std::string const & command, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_CHANNELISFULL); 

	msg += command;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_INVITEONLYCHAN(Client & recipient, std::string const & command, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_INVITEONLYCHAN); 

	msg += command;
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void	IRC_Server::send_err_NOPRIVILEGES(Client & recipient, std::string const & description) const
{
	std::string msg = numeric_reply_start(recipient, ERR_NOPRIVILEGES);
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}
