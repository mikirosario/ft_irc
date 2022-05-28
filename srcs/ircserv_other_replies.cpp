/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_other_replies.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikiencolor <mikiencolor@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 19:00:50 by mrosario          #+#    #+#             */
/*   Updated: 2022/05/28 17:50:22 by mikiencolor      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include  "../includes/ircserv.hpp"

/*!
** @brief	Properly terminates a non-numeric reply, adding @a last_param if one
**			exists.
**
** @details	This function will correctly terminate @a reply leaving it ready to
**			be sent to the client. It does NOT guarantee correctness of the
**			string PRECEDING the final parameter, though it WILL eliminate
**			trailing spaces, NUL characters or premature crlf-termination.
**
**			If a last parameter (such as a chat message) is provided, it will be
**			appended to the extent that it can be without going over
**			MSG_BUF_SIZE. The reply will always be crlf-terminated. All last
**			parameters appended with this function will be preceded by ':'.
**
**			If an empty string is passed as last_param, no last parameter will
**			be appended.
**
**			Last parameters such as chat messages should be appended with this
**			function, though any final parameter can be.
** @param	reply		The reply to be terminated.
** @param	last_param	Optionally, a last parameter for the reply, or an empty
**			string if there is none.
*/
void		IRC_Server::non_numeric_reply_end(std::string & reply, std::string const & last_param) const
{
	size_t	bytes_remaining;
	size_t	pos;

	if ((pos = reply.find_last_not_of(" \r\n\0")) != std::string::npos)
		reply.erase(pos + 1, std::string::npos);
	if (last_param.empty() == false)
	{
		reply += " :";
		bytes_remaining = MSG_BUF_SIZE - reply.size() - 2;
		reply.append(last_param, 0, bytes_remaining);
	}
	reply += "\r\n";
}

/*!
** @brief	Builds and sends a reply message to the registered client following
**			a successful NICK command.
**
** @details	The <source> of this reply message is the recipient's old <source>
**			before the nickname change, so the caller is expected to make a copy
**			and pass it to this reply function.
**
**			The source of this message is also the recipient.
** @param	recipient	The reply recipient.
** @param	old_source	The recipient's old source, before the nick change
**						(call get_source() on sender before implementing nick
**						change to get this).
*/
void	IRC_Server::send_rpl_NICK(Client & recipient, std::string const & old_source) const
{
	std::string msg = old_source;	//:old_nickname!username@hostname

	msg += " NICK ";
	msg += recipient.get_nick();	//new nick
	non_numeric_reply_end(msg, std::string());
	recipient.send_msg(msg);
}

/*!
** @brief	Builds and sends a reply message from @a source to @a recipient
**			following successful PRIVMSG command when @a recipient is a Client.
**
** @param	recipient	The message recipient.
** @param	source		The message sender.
** @param	message		The message text input by the user @a source.
*/
void		IRC_Server::send_rpl_PRIVMSG(Client & recipient, Client const & source, std::string const & message) const
{
	std::string msg = source.get_source() + " ";

	msg += "PRIVMSG ";
	msg += recipient.get_nick();
	non_numeric_reply_end(msg, message);
	recipient.send_msg(msg);
}

/*!
** @brief	Builds and sends a reply message from @a source to all members of
**			Channel @a recipient with @a privileges following successful PRIVMSG
**			command when @a recipient is a Channel.
**
** @details	Valid @a privileges are: '~' Founder/Owner, '@' Chanop, '%' Halfop,
**			or those included in SUPPORTED_CHANNEL_PREFIXES. An empty string is
**			interpreted as 'All', or the lowest privilege level.
** @param	recipient	The target channel of the message.
** @param	source		The message sender.
** @param	privileges	The minimum allowed privilege level of recipients.
** @param	message		The message text input by the user @a source.
*/
void		IRC_Server::send_rpl_PRIVMSG(Channel const & recipient, Client const & source, std::string const & privileges, std::string const & message) const
{
	char		privilege_level = 0;
	std::string msg = source.get_source() + " ";

	msg += "PRIVMSG ";
	msg += recipient.getChannelName();
	non_numeric_reply_end(msg, message);
	if (privileges.size() > 0) //find lowest privilege level
		for (size_t i = sizeof(SUPPORTED_CHANNEL_PREFIXES) - 1; i > 0; )
			if (privileges.find(SUPPORTED_CHANNEL_PREFIXES[--i]) != std::string::npos)
			{
				privilege_level = SUPPORTED_CHANNEL_PREFIXES[i];
				break ;
			}
	recipient.send_msg(&source, privilege_level, msg);
}

/*!
** @brief	Builds and sends a reply message from @a source to @a recipient
**			following successful NOTICE command.
**
** @param	recipient	The message recipient.
** @param	source		The message sender.
** @param	message		The message text input by the user @a source.
*/
void		IRC_Server::send_rpl_NOTICE(Client & recipient, Client const & source, std::string const & message) const
{
	std::string msg = source.get_source() + " ";

	msg += "NOTICE ";
	msg += recipient.get_nick();
	non_numeric_reply_end(msg, message);
	recipient.send_msg(msg);
}

/*!
** @brief	Builds and sends a reply message from @a source to all members of
**			Channel @a recipient following successful JOIN command.
**
** @param	recipient	The target channel of the message.
** @param	source		The message sender.
*/
void		IRC_Server::send_rpl_JOIN(Channel const & recipient, Client const & source) const
{
	std::string msg = ":" + source.get_nick();

	msg += " JOIN ";
	msg += recipient.getChannelName();
	non_numeric_reply_end(msg, std::string());
	//debug
		std::cout << msg << std::endl;
	//debug
	recipient.send_msg(NULL, 0, msg);
}

void		IRC_Server::send_rpl_PART(Client & recipient, Channel const & channel, std::string const & part_message) const
{
	//std::string msg_recipient = get_source() + " PART ";
	// std::string msg_channel = recipient.get_source() + " PART ";
	std::string msg_channel = ":" + recipient.get_nick() + " PART ";

	//msg_recipient += channel.getChannelName();
	msg_channel += channel.getChannelName();
	//non_numeric_reply_end(msg_recipient, part_message);
	non_numeric_reply_end(msg_channel, part_message);
	//recipient.send_msg(msg_channel);
	channel.send_msg(NULL, 0, msg_channel);
}

void		IRC_Server::send_rpl_KICK(Client const & kicker, Client & recipient, Channel const & channel, std::string const & kick_message) const
{
	std::string msg = kicker.get_source() + " KICK ";

	msg += channel.getChannelName() + " ";
	msg += recipient.get_nick();
	non_numeric_reply_end(msg, kick_message);
	recipient.send_msg(msg);
	channel.send_msg(NULL, 0, msg);
}

void		IRC_Server::send_rpl_PONG(Client & recipient, std::string const & token) const
{
	std::string msg = get_source() + " ";
	
	msg += "PONG ";
	msg += _servername + " ";
	msg += token;
	non_numeric_reply_end(msg, std::string());
	// //debug
	// std::cerr << msg << std::endl;
	// //debug
	recipient.send_msg(msg);
}

void		IRC_Server::send_rpl_MODE(Client & recipient, std::string const & applied_changes) const
{
	std::string msg = recipient.get_source() + " ";

	msg += "MODE ";
	msg += recipient.get_nick() + " ";
	non_numeric_reply_end(msg, applied_changes);
	recipient.send_msg(msg);
}

void		IRC_Server::send_rpl_MODE(Client & recipient, Channel const & channel, std::string const & applied_changes) const
{
	std::string msg = recipient.get_source() + " ";
	msg += "MODE ";
	msg += channel.getChannelName() + " ";
	non_numeric_reply_end(msg, applied_changes);
	
	channel.send_msg(NULL, 0, msg);
}

void		IRC_Server::send_rpl_QUIT(Client & quitter, std::string const & reason)
{
	typedef IRC_Server::Client::t_ChanMap::iterator t_ChanMapIt;
	std::string msg = quitter.get_source() + " ";
	
	msg += "QUIT ";
	non_numeric_reply_end(msg, reason);
	quitter.send_msg(msg);
	for (t_ChanMapIt begin = quitter.get_chanlist().begin(), end = quitter.get_chanlist().end(); begin != end; ++begin)
		begin->second->second.send_msg(&quitter, 0, msg);
	remove_client_from_server(quitter);
	if (quitter.is_disconnected() == false)
		quitter.set_state_disconnected();
}

void		IRC_Server::send_rpl_KILL(Client & killer, Client & killed, std::string const & reason)
{
	std::string	msg = killer.get_source() + " ";
	std::string	quitmsg = "Closing Link: " + _servername + " (Killed (" + killer.get_nick() + " (" + reason + ")))";

	msg += "KILL ";
	msg += killed.get_nick();
	non_numeric_reply_end(msg, reason);
	killer.send_msg(msg);
	send_rpl_QUIT(killed, quitmsg);
}

//special overload for server-directed kills
void		IRC_Server::send_rpl_KILL(Client & killed, std::string const & reason)
{
	typedef IRC_Server::Client::t_ChanMap::iterator t_ChanMapIt;
	std::string msg = killed.get_source() + " ";
	std::string	quitmsg = "Closing Link: " + _servername + " (Killed (" + _servername + " (" + reason + ")))";
	
	msg += "QUIT ";
	non_numeric_reply_end(msg, quitmsg);
	killed.set_out_buf(msg);
	killed.send_output_buf();
	for (t_ChanMapIt begin = killed.get_chanlist().begin(), end = killed.get_chanlist().end(); begin != end; ++begin)
		begin->second->second.send_msg(&killed, 0, msg);
	remove_client_from_server(killed);
	if (killed.is_disconnected() == false)
		killed.set_state_disconnected();
}
