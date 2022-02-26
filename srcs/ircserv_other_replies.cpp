/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_other_replies.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 19:00:50 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/26 11:44:02 by acortes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ircserv.hpp"

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
**			Since the source of this message
** @param	recipient	The reply recipient.
*/
void	IRC_Server::send_rpl_NICK(Client const & recipient, std::string const & old_source)
{
	std::string msg = old_source;	//:old_nickname!username@hostname
	msg += " NICK ";
	msg += recipient.get_nick();	//new nick
	non_numeric_reply_end(msg, std::string());
	recipient.send_msg(msg);
}
