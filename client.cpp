/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 22:02:27 by miki              #+#    #+#             */
/*   Updated: 2022/02/17 08:42:43 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

IRC_Server::Client::Client(void) : _state(IRC_Server::Client::State(UNREGISTERED))
{
	// 	flush_msg_buf();
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
	// std::memset(_msg_buf, 0, MSG_BUF_SIZE);
	// // for (size_t i = 0; i < MSG_BUF_SIZE; ++i)
	// // 	_msg_buf[i] = '\0';
	// _msg_buf_char_count = 0;
	_msg_buf.clear();
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
	//int	bytes_remaining = MSG_BUF_SIZE - _msg_buf_char_count;
	int	bytes_remaining = MSG_BUF_SIZE - _msg_buf.size();
	//int	grow_by;
	int ret;

	// //debug
	// std::cout << msg_register << std::endl;
	// //debug

	// if (nbytes > bytes_remaining) //if msg_register would fill or overflow the buffer
	// {
	// 	_msg_buf[MSG_BUF_SIZE - 1] = '\n';
	// 	_msg_buf[MSG_BUF_SIZE - 2] = '\r';
	// 	nbytes = bytes_remaining - 2; // subtract 2 bytes for mandatory CRLF termination
	// 	grow_by = bytes_remaining;
	// 	ret = false;
	// }
	// else
	// {
	// 	ret = true;
	// 	grow_by = nbytes;
	// }

	// std::memcpy(&_msg_buf[_msg_buf_char_count], msg_register, nbytes);
	// _msg_buf_char_count += grow_by;

	if (nbytes > bytes_remaining)
	{
		_msg_buf.append(msg_register, bytes_remaining - 2);
		_msg_buf += "\r\n";
		ret = false;
	}
	else
	{
		_msg_buf.append(msg_register, nbytes);
		ret = true;
	}
	
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
	// _msg_buf[_msg_buf_char_count++] = '\n';
	// //debug
	//return (is_endline(_msg_buf[_msg_buf_char_count - 1]));
	return(_msg_buf.find_first_of("\r\n") != std::string::npos);
}

/* GETTERS */

std::string const &	IRC_Server::Client::get_msg_buf(void) const
{
	return(_msg_buf);
}

/*!
** @brief	Returns number of parameters in a messsage.
**
** @details	All parameters are preceded by SPACE. A parameter preceded by SPACE
**			and COLON is the last parameter, and all subsequent spaces are
**			interpreted as part of the parameter proper.
** @param	msg Client message.
** @return	The number of parameters in the message.
*/
size_t	IRC_Server::Client::get_param_count(void) const
{
	size_t	end_pos;
	size_t	i = 0;
	size_t	p_count = 0;

	end_pos = _msg_buf.find(" :");
	if (end_pos != std::string::npos)					//if there is " :", that is last param	
		++end_pos;
	else							 					//if no " :", first char from end that is neither '\r', '\n' nor ' ' is endpos
		end_pos = _msg_buf.find_last_not_of(" \r\n\0") + 1;	//last param cannot be empty unless preceded by ':', so " \r\n" doesn't count;
														//the null terminator is just in case one gets in there
	while (i < end_pos && (i = _msg_buf.find_first_of(" \r\n", i)) < end_pos)
	{
		++p_count;
		i = _msg_buf.find_first_not_of(' ', i);				//tolerate trailing spaces
	}
	return (p_count);
}

/*! @brief	Returns a vector containing all parameters in @a msg WITHOUT crlf.
**			Make sure to add crlf back in to any messages being echoed!
**
** @param	msg A reference to the message to parse.
** @return	A vector of strings containing the message parameters in the same
**			order as in the message. If there are no parameters, an empty vector
**			is returned.
*/
std::vector<std::string>	IRC_Server::Client::get_params(void) const
{
	std::vector<std::string>	ret;
	size_t	start_pos = 0;
	size_t	end_pos;

	start_pos = _msg_buf.find_first_of(" \r\n", start_pos);				//get first space or endline
	start_pos = _msg_buf.find_first_not_of(' ', start_pos);				//tolerate leading spaces
	while (_msg_buf[start_pos] != '\r' && _msg_buf[start_pos] != '\n')		//NOTHING not crlf terminated should get this far, if so fix at source!
	{	
		if (_msg_buf[start_pos] == ':')									//last param colon case
		{
			++start_pos;
			//end_pos = _msg_buf.find_last_not_of("\r\n") + 1;				//strip crlf from last parameter
			end_pos = _msg_buf.find_first_of("\r\n");
		}
		else														//general param case (if starting pos >= _msg_buf.size(), npos is returned, but this should NOT happen here as everything MUST be cr or lf terminated)
			end_pos = _msg_buf.find_first_of(" \r\n", start_pos);		//strip crlf from last parameter
		//debug
		std::cerr << "START_POS: " << start_pos << " END_POS: " << end_pos << std::endl;
		//debug
		ret.push_back(_msg_buf.substr(start_pos, end_pos - start_pos));	//add parameter to vector; 0 bytes == empty string
		start_pos = _msg_buf.find_first_not_of(' ', end_pos);			//tolerate trailing spaces
	}
	return (ret);
}
