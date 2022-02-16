/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 22:02:27 by miki              #+#    #+#             */
/*   Updated: 2022/02/16 17:19:08 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

IRC_Server::Client::Client(void) : _state(IRC_Server::Client::State(UNREGISTERED))
{
	flush_msg_buf();
}

IRC_Server::Client &	IRC_Server::Client::operator=(Client const & src)
{
	_nick = src._nick;
	_user_profile = src._user_profile;
	return (*this);
}

IRC_Server::Client::~Client(void)
{}

/*!
** @brief	Searches the registered user map of the @a server for the @a nick and
**			associates the Client instance with it if it exists.
*/
void	IRC_Server::Client::find_nick(std::string const & nick, IRC_Server & server)
{
	IRC_Server::Client::t_user_ptr user = server._reg_users.upper_bound(nick);

	if ((*user).first == nick)
		this->_user_profile = user;
	//always create new user profile?
	// if ((*user).first != nick)
	// 	user = server._reg_users.insert(user, std::make_pair(nick, User(nick)));
	// this->_user_profile = user;
}

/*!
** @brief	Compares the client's password to the server's password.
**
** @details	Per the IRC standard, this should be called when registration ends,
**			which is when we receive NICK and USER commands. We can use this to
**			decide whether we accept the client or boot it. First step.
** @param server_pass The server's password.
** @return true if equal, otherwise false.
*/
bool	IRC_Server::Client::confirm_pass(std::string const & server_pass)
{
	return (this->_pass == server_pass);
}

/*!
** @brief	Flushes the message buffer for this client.
**
** @details	This should be called whenever a full message is received and sent
**			for processing and I THINK whenever a new connection request is
**			received. Also on instantiation.
*/
void	IRC_Server::Client::flush_msg_buf(void)
{
	std::memset(_msg_buf, 0, MSG_BUF_SIZE);
	_msg_buf_char_count = 0;
}

/*!
** @brief	Appends up to MSG_BUF_SIZE characters to message buffer, truncating
**			any extra bytes.
**
** @param	msg_register	Incoming message data.
** @param	nbytes			Number of bytes in incoming message data.
** @return	true if the entire message was appended, false if it was truncated.
*/
bool	IRC_Server::Client::append_to_msg_buf(char const (& msg_register)[MSG_BUF_SIZE], int nbytes)
{
	int	bytes_remaining = MSG_BUF_SIZE - _msg_buf_char_count;
	int ret;

	// //debug
	// std::cout << msg_register << std::endl;
	// //debug

	if (nbytes > bytes_remaining - 2) //if msg_register would fill or overflow the buffer
	{
		_msg_buf[MSG_BUF_SIZE - 1] = '\n';
		_msg_buf[MSG_BUF_SIZE - 2] = '\r';
		nbytes = bytes_remaining - 2; // subtract 2 bytes for mandatory CRLF termination
	}
	if (nbytes > bytes_remaining) //if msg_register would overflow the buffer
		ret = false;
	else
		ret = true;
	std::memcpy(_msg_buf, msg_register, nbytes);
	return (ret);
}

/*!
** @brief	Determines whether or not a character marks the end of a message.
**
** @details	The standard requires all lines to terminate in '\r\n', but we will be
**			fault-tolerant and accept either '\r' or '\n' as the end of a line.
** @param	c A character.
** @return	true if end of line, otherwise false.
**/
bool	IRC_Server::Client::is_endline(char const c)
{
	return (c == '\r' || c == '\n');
}

bool	IRC_Server::Client::msg_buf_is_crlf_terminated(void)
{
	// //debug
	// _msg_buf[_msg_buf_char_count - 1] = '\n';
	// //debug
	return (is_endline(_msg_buf[_msg_buf_char_count - 1]));
}
