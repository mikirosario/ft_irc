/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 22:02:27 by miki              #+#    #+#             */
/*   Updated: 2022/02/19 20:02:21 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

IRC_Server::Client::Client(void) : _state(IRC_Server::Client::State(UNREGISTERED)), _buf_state(IRC_Server::Client::Buffer_State(UNREADY))
{
	_msg_buf.reserve(MSG_BUF_SIZE);	//pre-reserve MSG_BUF_SIZE bytes
}


IRC_Server::Client &	IRC_Server::Client::operator=(Client const & src)
{
	_state = src._state;
	_buf_state = src._buf_state;
	_servername = src._servername;
	_sockfd = src._sockfd;
	_pass = src._pass;
	_nick = src._nick;
	_msg_buf = src._msg_buf;
	_user_profile = src._user_profile;
	
	return (*this);
}

IRC_Server::Client::~Client(void)
{}

/*!
** @brief	Moves all string references and copies all other variables from src
**			to this instance using std::swap and then clears src.
**
** @details	This method is just a basic imitation of std::move functionality.
**			I'm not allowed to use std::move because 42 syllabus requires us to
**			use only C++98, so I use std::swap() and clear() as a workaround.
**
**			It is just meant to make the unordered clients array more efficient
**			during the removal operation than it would be with assignments by
**			passing references instead of copying all the strings character by
**			character. The enums and int are still just copied.
**
** @param	src The Client to be moved.
*/
void		IRC_Server::Client::move(Client & src)
{
	_state = src._state;
	_buf_state = src._buf_state;
	std::swap(this->_servername, src._servername);
	_sockfd = src._sockfd;
	std::swap(this->_pass, src._pass);
	std::swap(this->_nick, src._nick);
	std::swap(this->_msg_buf, src._msg_buf);
	_user_profile = src._user_profile;
	src.clear();
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
** @brief	Flushes the message buffer for this Client and sets its buffer state
**			to UNREADY.
**
** @details	This should be called whenever a full message is reaped for
**			processing and I THINK whenever a new connection request is
**			received... though there is something in the subject about
**			recovering from lost connections so might need a timeout for
**			removal...
*/
void	IRC_Server::Client::flush_msg_buf(void)
{
	_buf_state = IRC_Server::Client::Buffer_State(UNREADY);	
	_msg_buf.clear();
}

/*!
** @brief	Appends up to MSG_BUF_SIZE characters to message buffer, truncating
**			any extra bytes, if the message buffer is UNREADY to be reaped.
**
** @details	This function is used to add to the buffer, while get_message is
**			used to empty the buffer. A buffer can be emptied when
**			crlf-terminated. If a buffer is filled to MSG_BUF_SIZE, it will
**			automatically be crlf-terminated and any extra bytes will be
**			truncated. If calling this function leads to crlf-termination of the
**			message buffer, the @a _buf_state changes to READY.
**
** @param	msg_register	Incoming message data.
** @param	nbytes			Number of bytes in incoming message data.
** @return	true if the entire message was appended, false if it was truncated
**			or not copied due to existing buffer that is READY to be reaped.
*/
bool	IRC_Server::Client::append_to_msg_buf(char const (& msg_register)[MSG_BUF_SIZE], int nbytes)
{
	int	bytes_remaining = MSG_BUF_SIZE - _msg_buf.size();
	int ret;

	if (_buf_state == IRC_Server::Client::Buffer_State(READY))
		ret = false;
	else if (nbytes > bytes_remaining)
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
	if (msg_buf_is_crlf_terminated())
		_buf_state = IRC_Server::Client::Buffer_State(READY);
	return (ret);
}

/*!
** @brief	Sets Client's @a _sockfd to the value passed as an argument and
**			sets the Client's @a _servername to the current address to which the
**			@a _sockfd is bound.
**
** @param	sockfd The Client's sockfd.
*/
void	IRC_Server::Client::set_sockfd(int sockfd)
{
	struct sockaddr	serverIP;
	socklen_t		addrlen(sizeof(sockaddr));
	std::string		servername;

	_sockfd = sockfd;
	if (getsockname(sockfd, &serverIP, &addrlen) == -1)
		perror("getsockname() failed in set_sockfd");
	else
		//debug inet_ntoa in_addr must be IPv4, but rereading the subject requirements inet_ntop is not officially allowed... :/
		_servername = inet_ntoa(*(reinterpret_cast<struct in_addr *>(IRC_Server::get_in_addr(&serverIP))));	
}

/*!
** @brief	Sets Client's @a _pass to the value passed as an argument.
**
** @param	pass	The string containing the Client's password.
*/
void	IRC_Server::Client::set_pass(std::string const & pass)
{
	_pass = pass;
	// //debug
	// std::cout << _pass << std::endl;
	// //debug
}

/*!
** @brief	Sets Client's @a _nick to the value passed as an argument.
**
** @param	nick	The string containing the Client's nickname.
*/
void	IRC_Server::Client::set_nick(std::string const & nick)
{
	_nick = nick;
	//debug
	std::cout << _nick << std::endl;
	//debug
}

/*!
** @brief	Clears all Client data.
*/
void	IRC_Server::Client::clear(void)
{
	_state = Client::State(UNREADY);
	_buf_state = Client::Buffer_State(UNREGISTERED);
	_servername.clear();
	_sockfd = 0;
	_pass.clear();
	_nick.clear();
	_msg_buf.clear();
	_user_profile = t_user_ptr();
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

/*!
** @brief	Determines whether the message buffer is crlf-terminated. We are
**			fault-tolerant so we accept either '\r' or '\n' as a valid
**			termination. Messages cannot be reaped until this function returns
**			true.
**
** @return	true if message buffer is crlf-terminated, otherwise false.
*/
bool	IRC_Server::Client::msg_buf_is_crlf_terminated(void) const
{
	return(_msg_buf.find_first_of("\r\n") != std::string::npos);
}

/*!
** @brief	Determines whether or not a message is ready to be reaped.
**
** @details	In practice, this is an alias for msg_buf_is_crlf_terminated, as
**			that is what determines readiness, but I make this one available in
**			the public methods to improve clarity.
** @return	true if Client's message is ready to be reaped, otherwise false.
**/
bool	IRC_Server::Client::msg_is_ready(void) const
{
	return (_buf_state == IRC_Server::Client::Buffer_State(READY));
}

/*!
** @brief	Determines whether or not a client has been registered. A registered
**			client is guaranteed to have an associated nick and username. An
**			unregistered client is not.
**
** @return	true if the client is registered, otherwise false
*/
bool		IRC_Server::Client::is_registered(void) const
{
	return (_state == IRC_Server::Client::State(REGISTERED));
}

void		IRC_Server::Client::send_msg(std::string const & msg) const
{
	send(_sockfd, msg.data(), msg.size(), 0);
}

/* GETTERS */

/*!
** @brief	Returns a read-only reference to the Client's message buffer. The
**			buffer and client state remain unchanged.
**
** @return	A read-only reference to the Client's message buffer.
*/
std::string const &	IRC_Server::Client::get_msg_buf(void) const
{
	return(_msg_buf);
}

/*!
** @brief	Retrieves the command from the Client's message.
**
** @details NOTE: Assumes crlf termination. Behaviour undefined if used on
**			non-crlf-terminated string.
**
** @return	A string containing the command, or an empty string if no command
**			exists in the message.
*/
std::string	IRC_Server::Client::get_cmd(void) const
{
	int	start_pos = _msg_buf.find_first_not_of(' ');			//Tolerate leading spaces
	int	end_pos = _msg_buf.find_first_of(" \r\n", start_pos);
	return (_msg_buf.substr(start_pos, end_pos - start_pos));
}

/*!
** @brief	Returns the number of parameters in the Client's message buffer.
**
** @details	All parameters are preceded by SPACE. A parameter preceded by SPACE
**			and COLON is the last parameter, and all subsequent spaces are
**			interpreted as part of the parameter proper. Parameters are only
**			counted after the command. A buffer with only spaces does not count
**			as a parameter.
**
**			NOTE: Assumes crlf termination. Behaviour undefined if used on
**			non-crlf-terminated string.
**
** @return	The number of parameters in the message.
*/
size_t	IRC_Server::Client::get_param_count(void) const
{
	size_t	end_pos;
	size_t	i = 0;
	size_t	p_count = 0;
	std::string cmd = get_cmd();

	if (cmd.empty() == false)
		i = _msg_buf.find(cmd);
	end_pos = _msg_buf.find(" :", i);
	if (end_pos != std::string::npos)						//if there is " :", that is last param	
		++end_pos;
	else							 						//if no " :", first char from end that is neither '\r', '\n' nor ' ' is endpos
		end_pos = _msg_buf.find_last_not_of(" \r\n\0") + 1;	//last param cannot be empty unless preceded by ':', so " \r\n" doesn't count;
															//the null terminator is just in case one gets in there
	while (i < end_pos && (i = _msg_buf.find_first_of(" \r\n", i)) < end_pos)
	{
		++p_count;
		i = _msg_buf.find_first_not_of(' ', i);				//tolerate trailing spaces
	}
	return (p_count);
}

//debug DEPRECATED
// /*! @brief	Returns a vector containing all parameters in the Client's message
// **			buffer STRIPPING crlf termination. Make sure to add crlf back in to
// **			any messages you want to echo!
// **
// ** @return	A vector of strings containing the message parameters in the same
// **			order as in the message. If there are no parameters, an empty vector
// **			is returned.
// */
// std::vector<std::string>	IRC_Server::Client::get_params(void) const
// {
// 	std::vector<std::string>	ret;
// 	size_t	start_pos = 0;
// 	size_t	end_pos;

// 	start_pos = _msg_buf.find_first_of(" \r\n", start_pos);				//get first space or endline
// 	start_pos = _msg_buf.find_first_not_of(' ', start_pos);				//tolerate leading spaces
// 	while (_msg_buf[start_pos] != '\r' && _msg_buf[start_pos] != '\n')	//NOTHING not crlf terminated should get this far, if so fix at source!
// 	{	
// 		if (_msg_buf[start_pos] == ':')									//last param colon case
// 		{
// 			++start_pos;
// 			end_pos = _msg_buf.find_last_not_of("\r\n\0") + 1;			//strip crlf from last parameter
// 		}
// 		else															//general param case (if starting pos >= _msg_buf.size(), npos is returned, but this should NOT happen here as everything MUST be cr or lf terminated)
// 			end_pos = _msg_buf.find_first_of(" \r\n", start_pos);		//strip crlf from last parameter
// 		// //debug
// 		// std::cerr << "START_POS: " << start_pos << " END_POS: " << end_pos << std::endl;
// 		// //debug
// 		ret.push_back(_msg_buf.substr(start_pos, end_pos - start_pos));	//add parameter to vector; 0 bytes == empty string
// 		start_pos = _msg_buf.find_first_not_of(' ', end_pos);			//tolerate trailing spaces
// 	}
// 	return (ret);
// }
//debug DEPRECATED

/*!
** @brief	If the Client's message buffer is READY, attempts to retrieve the
**			Client's message as a string vector in which the first string is the
**			command and the rest are parameters, and FLUSHES THE BUFFER.
**
** @details This method MUST be called when the @a _buf_state is READY in order
**			to reap the message from the buffer, though there is a redundant
**			check to prevent unready message buffers from being reaped.
**
**			If there is no command in the buffer, a vector with an empty string
**			in position 0 is returned. If the buffer is not READY (hasn't yet
**			been crlf-terminated), an empty vector is returned.
**
**			If the message is READY the buffer will ALWAYS be flushed and the
**			buffer state changed to UNREADY to make way for new incoming data.
**
**			If the message is UNREADY (empty vector returned), the buffer will
**			NOT be flushed.
**
**			NOTE:	Once get_message() is called to reap the Client's message,
**					the Client's message buffer is cleared and, unless a prior
**					copy was made, the return value of get_message() is the
**					ONLY remaining copy of the message!
**
**					To check the Client's message buffer WITHOUT reaping the
**					message, use the get_msg_buf() method instead.
** @return	A string vector in which the first string is the command and the
**			rest are parameters. If the message lacks a command, an empty string
**			will be provided in place of the command. If the buffer state is not
**			READY, an empty vector is returned.
*/
std::vector<std::string>	IRC_Server::Client::get_message(void)
{
	std::vector<std::string>	ret;

	if (_buf_state == IRC_Server::Client::Buffer_State(READY))
	{
		std::string	cmd = get_cmd();
		if (cmd.empty() == false)												//if there is a command, we retrieve command and parameters
		{
			size_t	start_pos = 0;
			size_t	end_pos;

			ret.push_back(cmd);													//add command
			start_pos = _msg_buf.find(cmd);										//set start_pos to beginning of cmd
			start_pos = _msg_buf.find_first_of(" \r\n", start_pos);				//get first space or endline after cmd
			start_pos = _msg_buf.find_first_not_of(' ', start_pos);				//tolerate leading spaces
			while (_msg_buf[start_pos] != '\r' && _msg_buf[start_pos] != '\n')	//NOTHING not crlf terminated should get this far, if so fix at source!
			{	
				if (_msg_buf[start_pos] == ':')									//last param colon case
				{
					++start_pos;
					end_pos = _msg_buf.find_last_not_of("\r\n\0") + 1;			//strip crlf from last parameter
				}
				else															//general param case (if starting pos >= _msg_buf.size(), npos is returned, but this should NOT happen here as everything MUST be cr or lf terminated)
					end_pos = _msg_buf.find_first_of(" \r\n", start_pos);		//strip crlf from last parameter
				// //debug
				// std::cerr << "START_POS: " << start_pos << " END_POS: " << end_pos << std::endl;
				// //debug
				ret.push_back(_msg_buf.substr(start_pos, end_pos - start_pos));	//add parameter to vector; 0 bytes == empty string
				start_pos = _msg_buf.find_first_not_of(' ', end_pos);			//tolerate trailing spaces
			}
		}
		else
			ret[0] = cmd;														//we guarantee interpreters will receive argv with a command string when Client_Buffer state reports READY; if none exists, we provide an empty one
		flush_msg_buf(); //buffer is always flushed with get_message when READY, even if there is no command
	}
	return  (ret);
}

std::string const &			IRC_Server::Client::get_servername(void) const
{
	return(_servername);
}

std::string const &			IRC_Server::Client::get_nick(void) const
{
	return(_nick);
}

int							IRC_Server::Client::get_sockfd(void) const
{
	return(_sockfd);
}
