/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_other_replies.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 19:00:50 by mrosario          #+#    #+#             */
/*   Updated: 2022/03/02 18:19:49 by mrosario         ###   ########.fr       */
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
void	IRC_Server::send_rpl_NICK(Client const & recipient, std::string const & old_source) const
{
	std::string msg = old_source;	//:old_nickname!username@hostname

	msg += " NICK ";
	msg += recipient.get_nick();	//new nick
	non_numeric_reply_end(msg, std::string());
	recipient.send_msg(msg);
}

/*!
** @brief	Builds and sends @a message from @a source to @a recipient following
**			successful PRIVMSG command when @a recipient is a Client.
**
** @param	recipient	The message recipient.
** @param	source		The message sender.
** @param	message		The text message being sent.
*/
void		IRC_Server::send_rpl_PRIVMSG(Client const & recipient, Client const & source, std::string const & message) const
{
	std::string msg = source.get_source() + " ";

	msg += "PRIVMSG ";
	msg += recipient.get_nick();
	non_numeric_reply_end(msg, message);
	recipient.send_msg(msg);
}

/*!
** @brief	Builds and sends @a message from @a source to all members of
**			@a channel with @a privileges.
**
** @param	channel		The target channel of the message.
** @param	source		The message sender.
** @param	privileges	The minimum allowed privilege level of recipients ('~' Founder/Owner, '@' Chanop, '%' Halfop, '' All)
** @param	message		The text message being sent.
*/
void		IRC_Server::send_rpl_PRIVMSG(Channel const & channel, Client const & source, std::string const & privileges, std::string const & message) const
{
	char		privilege_level = 0;
	std::string msg = source.get_source() + " ";

	msg += "PRIVMSG ";
	msg += "#" + channel.getChannelName();
	non_numeric_reply_end(msg, message);
	if (privileges.size() > 0) //find lowest privilege level
		for (size_t i = sizeof(SUPPORTED_CHANNEL_PREFIXES) - 1; i > 0; )
			if (privileges.find(SUPPORTED_CHANNEL_PREFIXES[--i]) != std::string::npos)
			{
				privilege_level = SUPPORTED_CHANNEL_PREFIXES[i];
				break ;
			}
	channel.send_msg(privilege_level, msg, *this);
}
