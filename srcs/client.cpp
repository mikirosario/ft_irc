/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikiencolor <mikiencolor@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 22:02:27 by miki              #+#    #+#             */
/*   Updated: 2022/05/05 16:34:23 by mikiencolor      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ircserv.hpp"

IRC_Server::Client::Client(void) :	_state(IRC_Server::Client::State(UNREGISTERED)),
									_buf_state(IRC_Server::Client::Buffer_State(UNREADY)),
									_pass_attempts(0),
									_pass_validated(false),
									pos(0)
{
	//pre-reserve some appropriate memory
	_serveraddr.reserve(INET6_ADDRSTRLEN);
	_nick.reserve(MAX_NICK_SIZE);
	_username.reserve(MAX_USERNAME_SIZE);
	_realname.reserve(MAX_REALNAME_SIZE);
	_msg_buf.reserve(MSG_BUF_SIZE);
	_message.reserve(MSG_BUF_SIZE);
}

IRC_Server::Client &	IRC_Server::Client::operator=(Client const & src)
{
	_state = src._state;
	_buf_state = src._buf_state;
	_serveraddr = src._serveraddr;
	_sockfd = src._sockfd;
	_pass_attempts = src._pass_attempts;
	_pass_validated = src._pass_validated;
	_nick = src._nick;
	_msg_buf = src._msg_buf;
	_message = src._message;
	_modes = src._modes;
	_username = src._username;
	_realname = src._realname;
	_hostname = src._hostname;
	//_channels = src._channels;
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
	std::swap(this->_serveraddr, src._serveraddr);
	_sockfd = src._sockfd;
	_pass_attempts = src._pass_attempts;
	_pass_validated = src._pass_validated;
	std::swap(this->_nick, src._nick);
	std::swap(this->_username, src._username);
	std::swap(this->_realname, src._realname);
	std::swap(this->_msg_buf, src._msg_buf);
	std::swap(this->_message, src._message);
	std::swap(this->_modes, src._modes);
	std::swap(this->_hostname, src._hostname);
	//std::swap(this->_channels, src._channels);
	src.clear();
}

/* ---- SETTERS ---- */

/*!
** @brief	Flushes the message buffer for this Client up to the @a stop
**			position and sets its buffer state to UNREADY if the buffer contains
**			no further crlf-terminated messages.
**
** @details	This should be called whenever a full message is reaped for
**			processing and I THINK whenever a new connection request is
**			received... though there is something in the subject about
**			recovering from lost connections so might need a timeout for
**			removal...
**
**			When a message is reaped, the get_message() method will call this
**			method to flush the message from the buffer, that is, to erase
**			elements from the buffer until the start of the next message (the
**			@a stop position). If the buffer was empty after the message was
**			reaped, get_message() will send string::npos to clear the whole
**			buffer.
**
**			If the buffer is crlf-terminated after being flushed, its state will
**			not change from READY and it will have to be read again. Otherwise
**			its state will change to UNREADY to signify it will not be read
**			again until it receives new data.
** @param	stop	The position preceding which all data in the buffer will be
**					cleared.
*/
void	IRC_Server::Client::flush_msg_buf(size_t stop)
{
	_msg_buf.erase(0, stop);
}
// void	IRC_Server::Client::flush_msg_buf(size_t stop)
// {
// 	_msg_buf.erase(0, stop);
// 	if (msg_buf_is_crlf_terminated() == false)
// 		_buf_state = IRC_Server::Client::Buffer_State(UNREADY);	
// }

/*!
** @brief	Appends incoming data to the Client's message buffer, setting the
**			Client's buffer state to READY if a crlf-terminated message is
**			present. Messages longer than MSG_BUF_SIZE will be truncated at
**			MSG_BUF_SIZE - 2 bytes.
**
** @details	Yeah, it's actually MUCH more complicated than that. ;)
**
+*			We use three buffers: @a incoming_data, @a _msg_buf and @a _message.
**
**			The @a incoming_data buffer is a stringified raw copy of the
**			512-byte server buffer used to receive data from each client and
**			simply contains all data received from recv() for the client
**			instance, which must be passed as a c-string of MSG_BUF_SIZE.
**
**			The Client's @a _msg_buf buffer, upon entry to this function, MUST
**			either be empty or contain the first part of an UNTERMINATED message
**			received in a previous call to append_to_msg_buf(). So ANY
**			crlf-termination, if there is one, MUST be located in
**			@a incoming_data. This is because we ALWAYS ensure @a _msg_buf is
**			fully cleared of terminated messages before calling
**			append_to_msg_buf() again.
**
**			Consequently, you can think of @a _msg_buf and @a incoming_data as
**			a single bisected buffer of MSG_BUF_SIZE * 2 size:
**			@a [_msg_buf+incoming_data] wherein any data in @a _msg_buf is
**			'waiting' to be terminated with data from @a incoming_data.
**
**			NOTE: ALL MESSAGES IN _MSG_BUF MUST BE REAPED BY GET_MESSAGE()
**			BEFORE THIS METHOD IS CALLED. @see ::get_message().
**
**			The Client's @a _message string is loaded with a single
**			crlf-terminated message ready for parsing. It will be loaded with
**			the first crlf-terminated message in @a [_msg_buf+incoming_data],
**			which may contain the first part of the message in @a _msg_buf and
**			the termination in @a incoming_data, OR may be entirely contained in
**			@a incoming_data (in which case @a _msg_buf is empty).
**
**			The message loaded into the @a _message string will then be flushed
**			(erased) from the @a [_msg_buf+incoming_data] buffer. Any trailing
**			data remaining in @a incoming_data afterward will then be moved into
**			@a _msg_buf. (Should be moved... it's c++98, so they are actually
**			copied to @a _msg_buf and erased from @a incoming_data).
**
**			Thus, the Client's @a _msg_buf buffer, upon return from this
**			function, MAY still contain additional TERMINATED messages which
**			MUST then be reaped and flushed from it by repeated get_message()
**			calls BEFORE append_to_msg_buf() is called again!
**
**			We always assume that @a _msg_buf contains the first part of an
**			incomplete message, and that @a _incoming_data contains the second
**			part of that message. Therefore, to construct @a _message we first
**			append @a _msg_buf to @a _message and then append @a _incoming_data
**			until the first crlf-termination.
**
**			If @a _msg_buf is empty, that is irrelevant, it just means that
**			@a _incoming_data contains the entire message.
**
**			If there is no crlf-terminated message, that is okay so long as the
**			accumulated bytes in @a _msg_buf are below the MSG_BUF_SIZE limit.
**			We just append the @a incoming_data to @a msg_buf and continue to
**			wait for a termination to be sent.
**
**			// MESSAGE SIZE RESTRICTION ENFORCEMENT //
**			Messages above MSG_BUF_SIZE bytes in length (including
**			crlf-termination) are not allowed. Bearing in mind that upon entry
**			to this function @a _msg_buf ALWAYS contains the first part of an
**			unterminated message, we know we have a message above MSG_BUF_SIZE
**			bytes in length if:
**
**				A crlf-termination is present in @a incoming_data BUT the
**				Client's @a _msg_buf size + the size of the remaining part of
**				the message in @a incoming_data until the first crlf-termination
**				(including crlf itself) is greater than MSG_BUF_SIZE, OR
**
**				A crlf-termination is not present in @a incoming_data and the
**				Client's @a _msg_buf would overflow (grow to more than
**				MSG_BUF_SIZE) by appending all the @a incoming_data + 2 bytes
**				(for the obligatory crlf-termination after any message).
**
**			In either such case, the message is truncated at MSG_BUF_SIZE - 2
**			bytes. In the second case, MSG_BUF_SIZE - 2 initial bytes of the
**			message are stored in the buffer until the terminating crlf is sent,
**			which will then turn it into the first case. In the first case, the
**			message is truncated at byte MSG_BUF_SIZE - 2 and crlf-terminated.
**			Coming from the second case to the first this truncation will
**			already have started in a prior call and so there will only be two
**			@a msg_buf_bytes_available left for the crlf-termination.
**
**			The crlf-terminated message, truncated or not, is loaded into the
**			@a _message string and flushed from the buffer. Any trailing
**			@a incoming_data is then copied into @a _msg_buf for future
**			retrieval and/or termination.
**
**			If the message was truncated (overflowed the buffer) at any point,
**			false is returned. This is controlled with two flags:
**
**				@a message_overflows_buffer for overflows detected on
**				crlf-termination, that is, during message extraction, and
**	
**				@a message_overflowed_buffer, a static flag for overflows
**				detected at any point before crlf was received (causing all the
**				rest of the received bytes until the crlf to be discarded).
**
**			If either of these flags is true, the return value will be false. If
**			false is returned, the caller should then send the user the
**			ERR_INPUTTOOLONG error reply. This reply must be sent before any
**			reply to the content of the truncated message itself.
**
**			While crlf-termination is expected, in practice message termination
**			with a single '\r' or '\n' is tolerated.
**
**			// TRAILING CRLF AND EMPTY LINES //
**			A trailing crlf-termination or empty line is defined here as any
**			sequence of ' ', '\r' or '\n' characters at the beginning of the
**			@a incoming_data buffer when there is NO data in the @a _msg_buf
**			buffer waiting to be terminated (when @a _msg_buf is empty).
**
**			Trailing crlf-terminations and empty lines are ignored, meaning that
**			they are deleted from the buffer without processing.
**
**			Anyway, FINALLY, if this odyssey ends with a crlf-terminated message
**			ready to be interpreted (truncated or not), we will set the client's
**			buffer state to READY.
**
** @param	data_received	Incoming message data.
** @param	nbytes			Number of bytes in incoming message data. If this is
**							not accurate, behaviour is totally undefined. You
**							monster.
** @return	false if a received message was too long (ERR_INPUTTOOLONG) and had
**			to be discarded, otherwise true
*/
bool	IRC_Server::Client::append_to_msg_buf(char const (& data_received)[MSG_BUF_SIZE], int nbytes)
{
	std::string	incoming_data(data_received, nbytes);			//incoming data register
	size_t		msg_buf_bytes_available = MSG_BUF_SIZE - _msg_buf.size() - 2;	//remaining bytes in _msg_buf available for storage - crlf-termination
	size_t		end_pos;
	static bool	message_overflowed_buffer = false;
	bool		ret = true;

	if (_msg_buf.empty() == true)								//if we don't have anything in the _msg_buf waiting for a crlf-termination
	{
		end_pos = incoming_data.find_first_not_of(" \r\n"); 							//ignore trailing crlf-termination/empty line
		incoming_data.erase(0, end_pos);												//flush trailing crlf/empty line from buffer
	}
	end_pos = incoming_data.find_first_of("\r\n");				//determine if remaining incoming data has crlf-termination.
	if (end_pos == std::string::npos) 							//incoming data does not have crlf-termination; this is ok as long it will not cause buffer overflow
	{
		if (msg_buf_bytes_available < incoming_data.size())		//if remaining data would overflow the 512-byte buffer, input is too long
		{		
			_msg_buf.append(incoming_data, 0, msg_buf_bytes_available);		//append up to msg_buf_bytes_available bytes and ignore subsequent data until crlf
			message_overflowed_buffer = true;								//message_overflowed flag indicates this message WAS truncated at some point due to buffer overflow.
		}	
		else													//otherwise, fill the buffer and wait for crlf-termination, at which point we enter the crlf-terminated message but input is too long case
			_msg_buf.append(incoming_data);									//append all incoming_data and wait for more data
		
	}
	else 														//incoming data has crlf-termination, so we extract complete message to _message and copy remaining data to _msg_buf
	{
		bool	message_overflows_buffer = (_msg_buf.size() + end_pos + 2 > MSG_BUF_SIZE);					//message_overflows_buffer indicates this message WILL now be truncated due to buffer overflow.
		_message.assign(_msg_buf);																			//first part of message in _msg_buf; if _msg_buf is empty, nothing is assigned; i'd love it to be a move, but c++98...
		_message.append(incoming_data, 0, (message_overflows_buffer ? msg_buf_bytes_available : end_pos)); 	//append last part of message from incoming_data; if message would overflow buffer, append up to available bytes in the buffer and truncate rest, otherwise append entire message
		_message.append("\r\n");																			//i have so little trust in people i don't even let the client crlf-terminate its own message xD
		if (_message[0] == ':')																				//if the message contains a source as first argument
			remove_source(_message);																		//remove it
		end_pos = incoming_data.find_first_not_of("\r\n", end_pos);											//find first character after crlf-termination, or npos if it's the end of the string
		incoming_data.erase(0, end_pos);																	//flush the part of the incoming data pertaining to the appended message (including any truncated bits)
		_msg_buf.assign(incoming_data);																		//copy remaining part of the incoming data to msg_buf; if empty, nothing will be copied, of course
		_buf_state = IRC_Server::Client::Buffer_State(READY);												//set client buffer state to READY
		ret = !(message_overflowed_buffer | message_overflows_buffer);										//in case of buffer overflow, whether detected in past or present input, return false; otherwise return true
		message_overflowed_buffer = false;																	//overflowed message is handled here, so reset static overflowed flag.
	}
	return (ret);
}

//DEPRECATED; AH, the simple, naive old days xD
// /*!
// ** @brief	Appends up to MSG_BUF_SIZE characters to message buffer, truncating
// **			any extra bytes, if the message buffer is UNREADY to be reaped.
// **
// ** @details	This function is used to add to the buffer, while get_message is
// **			used to empty the buffer. A buffer can be emptied when
// **			crlf-terminated. If a buffer is filled to MSG_BUF_SIZE, it will
// **			automatically be crlf-terminated and any extra bytes will be
// **			truncated. If calling this function leads to crlf-termination of the
// **			message buffer, the @a _buf_state changes to READY.
// **
// ** @param	msg_register	Incoming message data.
// ** @param	nbytes			Number of bytes in incoming message data.
// ** @return	true if the entire message was appended, false if it was truncated
// **			or not copied due to existing buffer that is READY to be reaped.
// */
// bool	IRC_Server::Client::append_to_msg_buf(char const (& msg_register)[MSG_BUF_SIZE], int nbytes)
// {
// 	int	bytes_remaining = MSG_BUF_SIZE - _msg_buf.size();
// 	int ret;

// 	if (_buf_state == IRC_Server::Client::Buffer_State(READY))
// 		ret = false;
// 	else if (nbytes > bytes_remaining)
// 	{
// 		_msg_buf.append(msg_register, bytes_remaining - 2);
// 		_msg_buf += "\r\n";
// 		ret = false;
// 	}
// 	else
// 	{
// 		_msg_buf.append(msg_register, nbytes);
// 		ret = true;
// 	}
// 	if (msg_buf_is_crlf_terminated())
// 		_buf_state = IRC_Server::Client::Buffer_State(READY);
// 	return (ret);
// }

/*!
** @brief	Sets Client's @a _sockfd to the value passed as an argument and
**			sets the Client's @a _serveraddr to the current address to which the
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
		_serveraddr = inet_ntoa(*(reinterpret_cast<struct in_addr *>(IRC_Server::get_in_addr(&serverIP))));	
}

/*!
** @brief	Registers Client's password validation attempt.
**
** @details	Each Client instance will accept up to MAX_PASS_ATTEMPTS password
**			validation attempts during registration, after which any further
**			attempts will lead the method to return false and the client's
**			connection should then be rejected with a very stern reprimand.
** @return	true if PASS has been sent fewer than MAX_PASS_ATTEMPTS times,
**			otherwise false.
*/
bool	IRC_Server::Client::reg_pass_attempt(void)
{
	if (_pass_attempts > MAX_PASS_ATTEMPTS)
		return (false);
	++_pass_attempts;
	return (true);
}

/*!
** @brief	Sets Client's @a _nick to the value passed as an argument.
**
** @param	nick	The string containing the Client's nickname.
*/
void	IRC_Server::Client::set_nick(std::string const & nick)
{
	_nick = nick;
	// //debug
	// std::cout << _nick << std::endl;
	// //debug
}

/*!
** @brief	Sets Client's @a _username to no more than MAX_USERNAME_SIZE bytes
**			of the value passed as a parameter, truncating if needed.
**
** @param	username	The string containing the Client's username.
*/
void	IRC_Server::Client::set_username(std::string const & username)
{
	_username.assign(username, 0, MAX_USERNAME_SIZE);
	// //debug
	// std::cout << _username << std::endl;
	// //debug
}

/*!
** @brief	Sets Client's @a _realname to no more than MAX_REALNAME_SIZE bytes
**			of the value passed as a parameter, truncating if needed.
**
** @param	username	The string containing the Client's real name.
*/
void	IRC_Server::Client::set_realname(std::string const & realname)
{
	_realname.assign(realname, 0, MAX_REALNAME_SIZE);
	// //debug
	// std::cout << _realname << std::endl;
	// //debug
}

/*!
** @brief	Sets Client's @a _clientaddr as the canonical hostname determined by
**			getaddrinfo() for the remote IP/hostname @a clientaddr passed as a
**			parameter.
**
** @details This lookup attempts to retrieve the canonical hostname for a given
**			remote IP address or hostname @a clientaddr. The canonical hostname
**			may or may not be the same as @a clientaddr.
**
**			Despite the parameter name, this lookup can be done on either an IP
**			OR a valid hostname. (i.e. "localhost" will return the local
**			canonical hostname, "www.google.com" will return the google.com
**			canonical hostname, etc.).
**
**			If this lookup fails, no the IP passed as a parameter will be set
**			as the @a _clientaddr.
**
**			If a NULL pointer is passed, no clientaddr will be set.
** @param	clientaddr	The string containing the remote IP address.
** @return	true if a clientaddr was set, otherwise false.
*/
bool	IRC_Server::Client::set_clientaddr(char const * clientaddr)
{	
	//attempt canonical name lookup
	struct addrinfo		hints;
	struct addrinfo *	remoteaddrinfo;
	
	std::memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = AF_UNSPEC;	//IPv4 or IPv6 OK
	hints.ai_flags = AI_CANONNAME;	//this flag requests canonical name lookup
	hints.ai_socktype = SOCK_STREAM;//type of connection

	if (clientaddr == NULL)
		return (false);
	else if (getaddrinfo(clientaddr, NULL, &hints, &remoteaddrinfo) != 0)
	{
		_clientaddr = remoteaddrinfo->ai_canonname;
		//debug
		std::cout << "My canonical name is? " << remoteaddrinfo->ai_canonname << std::endl; //debug
		//debug	
		freeaddrinfo(remoteaddrinfo);
	}
	else
		_clientaddr = clientaddr;
	//debug
	std::cout << "My name is: " << _hostname << std::endl;
	//debug
	return (true);
}

/*!
** @brief	Sets and unsets Client modes.
**
** @details	Supported modes are i (invisible), a (away) and o (operator). Modes
**			should be prepended by '+' or '-' as they are to be added or removed
**			and included in a string with no other delimiters.
**			Ex. (+a, -i, -o+ai). If no sign is provided, '+' will be assumed
**			until and unless a sign is found.
**
**			A '+' or '-' sign will affect all subsequent modes until an opposite
**			sign.
**
**			Unsupported modes are ignored and not applied.
** @param	modes	Modes to set or unset, preceded by a '+' or '-' sign to set
**					or unset, respectively.
** @param	applied_changes	An empty string in which all applied mode changes
**			will be recorded.
** @return	false if any mode was unsupported, otherwise true
*/
bool	IRC_Server::Client::set_modes(std::string const & modes, std::string & applied_changes)
{
	size_t	start_pos = modes.find_first_of("+-");
	size_t	end_pos = modes.find_first_of(' ');
	if (end_pos == std::string::npos)
		end_pos = modes.size();
	size_t	del;
	char	sign = '+';
	bool	ret;

	if (start_pos == std::string::npos)
		return false;
	ret = true;
	for (std::string::const_iterator it = modes.begin() + start_pos, end = modes.begin() + end_pos; it != end; ++it)
	{
		if (std::strchr("+-", *it) != NULL)								//set sign
		{
			sign = *it;
			applied_changes.push_back(*it);
		}
		else if (std::strchr(SUPPORTED_USER_MODES, *it) != NULL)		//mode is known
		{
			if (sign == '+' && _modes.find(*it) == std::string::npos)				//set requested and mode not already set
				_modes.push_back(*it);													//set mode
			else if (sign == '-' && (del = _modes.find(*it)) != std::string::npos)	//unset requested and mode not already unset
				_modes.erase(del, 1);													//unset mode
			applied_changes.push_back(*it);
		}
		else															//mode is unknown
			ret = false;
	}
	if (std::strchr("+-", applied_changes[0]) == NULL)
		applied_changes.insert(applied_changes.begin(), '+');
	return (ret);
}

/*!
** @brief	Sets Client's @a _pass_validated flag to @a state.
**
** @param	state	State to which to set Client's @a _pass_validated flag (true
**					or false).
*/
void	IRC_Server::Client::set_pass_validated(bool state)
{
	_pass_validated = state;
}

/*!
** @brief	Sets Client's state to REGISTERED.
*/
void	IRC_Server::Client::set_state_registered(void)
{
	_state = IRC_Server::Client::State(REGISTERED);
}

/*!
** @brief	Records channel membership in client object.
**
** @param	channel_iterator	An iterator to the channel of which the client
**								has become a member.
** @return	true if successful, false if unsuccessful for any reason (memory,
**			duplicate name, etc.)
*/
bool	IRC_Server::Client::set_channel_membership(IRC_Server::t_Channel_Map::iterator const & channel_iterator)
{
	bool ret;

	try
	{
		ret = _channels.insert(std::make_pair(channel_iterator->second.getChannelName(), channel_iterator)).second;
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return (false);
	}
	return (ret);
}

/*!
** @brief	Removes channel membership from client object.
**
** @param	channel_iterator	An iterator to the channel of which the client's
**								membership is being removed.
*/
void	IRC_Server::Client::remove_channel_membership(IRC_Server::t_Channel_Map::iterator const & channel_iterator)
{
	_channels.erase(channel_iterator->second.getChannelName());
}

void	IRC_Server::Client::remove_channel_membership(t_ChanMap::iterator const & channel_iterator)
{
	_channels.erase(channel_iterator);
}

/*!
** @brief	Clears all Client data.
*/
void	IRC_Server::Client::clear(void)
{
	_state = Client::State(UNREADY);
	_buf_state = Client::Buffer_State(UNREGISTERED);
	_serveraddr.clear();
	_sockfd = 0;
	_pass_attempts = 0;
	_pass_validated = false;
	_nick.clear();
	_clientaddr.clear();
	_hostname.clear();
	_username.clear();
	_realname.clear();
	_msg_buf.clear();
	_message.clear();
	//_channels.clear();
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
	int	start_pos = _message.find_first_not_of(' ');			//Tolerate leading spaces
	int	end_pos = _message.find_first_of(" \r\n", start_pos);
	return (_message.substr(start_pos, end_pos - start_pos));
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
** @brief	Client will remove itself from channel pointed to by @a channel_it.
**
** @details	NOTE: Behaviour is undefined for invalid channel_it!
** @param	channel_it	An iterator to the channel from which the client will
**						remove itself.
*/
void	IRC_Server::Client::leave_channel(t_ChanMap::iterator const & channel_it)
{
	// //debug
	// bool ret_rmember;
	// //debug
	//ret_rmember = channel_it->second->second.removeMember(get_nick());
	channel_it->second->second.removeMember(get_nick());
	_channels.erase(channel_it);
	// //debug
	// std::cout << "leave channel result: " << ret_rmember << std::endl;
	// //debug
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
	//debug
	//std::cerr << _buf_state << std::endl;
	//debug
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
	//debug
	std::cerr << msg << std::endl;
	//debug
	send(_sockfd, msg.data(), msg.size(), 0);
}

/*!
** @brief	Attempts to leave @a channel_name channel, returning true if
**			successful and false otherwise.
**
** @details	If @a channel_name is not present in the client's @a _channels map,
**			nothing is done. If the client leaves the channel, both the client
**			and channel will be	updated to reflect this.
** @param	channel_name	The name of the channel the client will leave, NOT
**							including any prefixes ('&', '#', etc.)
** @return	true if successful, false if channel_name is not present in the
**			client's @a _channels map.
*/
bool		IRC_Server::Client::leave_channel(std::string const & channel_name)
{
	t_ChanMap::iterator it = _channels.find(channel_name);
	
	if (it == _channels.end())
		return false;
	leave_channel(it);
	return true;
}

void		IRC_Server::Client::leave_all_channels(void)
{
	for (t_ChanMap::iterator it = _channels.begin(), end = _channels.end(); it != end; )
		leave_channel(it++); //map.erase() should preserve range integrity, but not deleted iterator integrity of course. unless it's a Mac and does evil magics. xD
}

/* GETTERS */

/*!
** @brief	If the Client's buffer state is READY, reaps the Client's message as
**			a string vector in which the first string is the command and the
**			rest are parameters, changing the buffer state to UNREADY if the
**			client has no more crlf-terminated strings to reap.
**
** @details This method MUST be called when the @a _buf_state is READY in order
**			to reap the message from the buffer, though there is a redundant
**			check to prevent unready message buffers from being reaped.
**
**			If there is no command in the message, a vector with an empty string
**			in position 0 is returned. If the buffer is not READY (hasn't yet
**			been crlf-terminated), an empty vector is returned.
**
**			If a crlf-terminated message was reaped and there is another
**			crlf-terminated message waiting in the Client's buffer, it will be
**			loaded to the @a _message string and flushed from @a _msg_buf and
**			the Client's buffer state will remain READY. This method should then
**			be called repeatedly so long as the Client's buffer state remains
**			READY.
**
**			If a crlf-terminated message was reaped and there is no
**			crlf-terminated message remaining in the Client's buffer, the
**			Client's buffer state will be changed to UNREADY to make way for new
**			incoming data.
**
**			If a there is another crlf-terminated message in the buffer, the
**			buffer will ALWAYS be flushed.
**
**			If there is NOT another crlf-terminated message in the buffer, the
**			buffer will NOT be flushed.
**
**			So the proper way to use this method (in pseudocode) might be:
**			while (client.msg_is_ready())
**				msg = client.get_message();
**				do_stuff_with_message(msg);
**
**			NOTE:	Once get_message() is called to reap the Client's message,
**					the Client's message buffer is cleared of the message and,
**					unless a copy is made by the caller, the return value of
**					get_message() is the ONLY remaining copy of the message!
**
**					To check the Client's message buffer WITHOUT flushing the
**					message buffer, use the see_next_message() or see_msg_buf()
**					methods instead for the next complete message ready to be
**					reaped and any trailing data in the buffer, respectively.
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
		size_t		start_pos = 0;
		size_t		end_pos;
		if (cmd.empty() == false)												//if there is a command, we retrieve command and parameters
		{
			//size_t	end_pos;

			ret.push_back(cmd);													//add command
			start_pos = _message.find(cmd);										//set start_pos to beginning of cmd
			start_pos = _message.find_first_of(" \r\n", start_pos);				//get first space or endline after cmd
			start_pos = _message.find_first_not_of(' ', start_pos);				//tolerate leading spaces
			while (_message[start_pos] != '\r' && _message[start_pos] != '\n')	//NOTHING not crlf terminated should get this far, if so fix at source!
			{	
				if (_message[start_pos] == ':')									//last param colon case
				{
					++start_pos;
					end_pos = _message.find_first_of("\r\n\0");					//strip crlf from last parameter
				}
				else															//general param case (if starting pos >= _message.size(), npos is returned, but this should NOT happen here as everything MUST be cr or lf terminated)
					end_pos = _message.find_first_of(" \r\n", start_pos);		//strip crlf from last parameter
				// //debug
				// std::cerr << "START_POS: " << start_pos << " END_POS: " << end_pos << std::endl;
				// //debug
				ret.push_back(_message.substr(start_pos, end_pos - start_pos));	//add parameter to vector; 0 bytes == empty string
				start_pos = _message.find_first_not_of(' ', end_pos);			//tolerate trailing spaces
			}
		}
		else
		{
			ret.push_back(cmd);														//we guarantee interpreters will receive argv with a command string when Client_Buffer state reports READY; if none exists, we provide an empty one
		}
		//is there another message in _msg_buf? if so, get it, otherwise, set state to unready
		end_pos = _msg_buf.find_first_of("\r\n");
		if (end_pos != std::string::npos)	//yes, there is another message
		{
			end_pos = _msg_buf.find_first_not_of("\r\n", end_pos);
			_message.assign(_msg_buf, 0, end_pos);		//end_pos == std::string::npos means copy whole buffer; should be a move, but, yaknow... c++98...
			flush_msg_buf(end_pos);					//...so we also have to flush.
		}
		else								//no, there is not another message
		{
			_message.clear();
			_buf_state = IRC_Server::Client::Buffer_State(UNREADY);	//set  buffer to wait for more incoming data
		}
	}
	// //debug
	// for (std::vector<std::string>::const_iterator it = ret.begin(), end = ret.end(); it != end; ++it)
	// 	std::cerr << *it << std::endl;
	// //debug

	return  (ret);
}

// std::vector<std::string>	IRC_Server::Client::get_message(void)
// {
// 	std::vector<std::string>	ret;

// 	if (_buf_state == IRC_Server::Client::Buffer_State(READY))
// 	{
// 		std::string	cmd = get_cmd();
// 		size_t		start_pos = 0;
// 		size_t		end_pos;
// 		if (cmd.empty() == false)												//if there is a command, we retrieve command and parameters
// 		{
// 			//size_t	end_pos;

// 			ret.push_back(cmd);													//add command
// 			start_pos = _msg_buf.find(cmd);										//set start_pos to beginning of cmd
// 			start_pos = _msg_buf.find_first_of(" \r\n", start_pos);				//get first space or endline after cmd
// 			start_pos = _msg_buf.find_first_not_of(' ', start_pos);				//tolerate leading spaces
// 			while (_msg_buf[start_pos] != '\r' && _msg_buf[start_pos] != '\n')	//NOTHING not crlf terminated should get this far, if so fix at source!
// 			{	
// 				if (_msg_buf[start_pos] == ':')									//last param colon case
// 				{
// 					++start_pos;
// 					end_pos = _msg_buf.find_first_of("\r\n\0");					//strip crlf from last parameter
// 				}
// 				else															//general param case (if starting pos >= _msg_buf.size(), npos is returned, but this should NOT happen here as everything MUST be cr or lf terminated)
// 					end_pos = _msg_buf.find_first_of(" \r\n", start_pos);		//strip crlf from last parameter
// 				// //debug
// 				// std::cerr << "START_POS: " << start_pos << " END_POS: " << end_pos << std::endl;
// 				// //debug
// 				ret.push_back(_msg_buf.substr(start_pos, end_pos - start_pos));	//add parameter to vector; 0 bytes == empty string
// 				start_pos = _msg_buf.find_first_not_of(' ', end_pos);			//tolerate trailing spaces
// 			}
// 			end_pos = _msg_buf.find_first_not_of("\r\n", end_pos);				//get the character after crlf termination; npos is OK too if there is no such character
// 		}
// 		else
// 		{
// 			end_pos = _msg_buf.find_first_of("\r\n", start_pos);
// 			end_pos = _msg_buf.find_first_not_of("\r\n", end_pos);
// 			ret[0] = cmd;														//we guarantee interpreters will receive argv with a command string when Client_Buffer state reports READY; if none exists, we provide an empty one
// 		}
// 		flush_msg_buf(end_pos); //buffer is always flushed with get_message when READY, even if there is no command
// 	}
// 	return  (ret);
// }

/*!
** @brief	Builds a source string for this Client instance in format:
**			:nickname!username@hostname
**
** @details	I might replace this with a string variable that is updated whenever
**			a new nick is set... depends how often I need this or variants.
**
*/
std::string	IRC_Server::Client::get_source(void) const
{
	std::string	msg;

	msg += ":";
	msg += get_nick();
	msg += "!";
	msg += get_username();
	msg += "@";
	msg += get_hostname();
	return (msg);
}

/*!
** @brief	Returns the number of parameters in the Client's message.
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
		i = _message.find(cmd);
	end_pos = _message.find(" :", i);
	if (end_pos != std::string::npos)						//if there is " :", that is last param	
		++end_pos;
	else							 						//if no " :", first char from end that is neither '\r', '\n' nor ' ' is endpos
		end_pos = _message.find_last_not_of(" \r\n\0") + 1;	//last param cannot be empty unless preceded by ':', so " \r\n" doesn't count;
															//the null terminator is just in case one gets in there
	while (i < end_pos && (i = _message.find_first_of(" \r\n", i)) < end_pos)
	{
		++p_count;
		i = _message.find_first_not_of(' ', i);				//tolerate trailing spaces
	}
	return (p_count);
}

std::string const &			IRC_Server::Client::get_serveraddr(void) const
{
	return(_serveraddr);
}

std::string const &			IRC_Server::Client::get_nick(void) const
{
	return(_nick);
}

std::string const &			IRC_Server::Client::get_username(void) const
{
	return (_username);
}

std::string const &			IRC_Server::Client::get_realname(void) const
{
	return (_realname);
}

//this is currently an alias for getclientaddr. :P
std::string const &			IRC_Server::Client::get_hostname(void) const
{
	//return (_hostname);
	return(get_clientaddr());
}

std::string const &			IRC_Server::Client::get_clientaddr(void) const
{
	return (_clientaddr);
}

std::string const &			IRC_Server::Client::get_modes(void)	const
{
	return (_modes);
}

int							IRC_Server::Client::get_sockfd(void) const
{
	return(_sockfd);
}

int							IRC_Server::Client::get_pass_attempts(void) const
{
	return (_pass_attempts);
}

size_t						IRC_Server::Client::get_pos(void) const
{
	return (pos);
}

bool						IRC_Server::Client::get_pass_validated(void) const
{
	return (_pass_validated);
}

/*!
** @brief	Returns a pair containing an iterator to @a channel_name as first
**			element and a bool set to true as second element if the client is a
**			member of @a channel_name. If the client is not a member, an
**			iterator to client._channels.end() will be returned, and the bool
**			will be set to false. Don't try to dereference without checking the
**			bool first.
**
** @param	channel_name	The name of the channel to check for client
**							membership.
** @return	A pair containing an iterator to @a channel_name and true if the
**			client is a member of @a channel_name, otherwise an iterator to
**			client._channels.end() and false.
*/
std::pair<IRC_Server::Client::t_ChanMap::iterator, bool>	IRC_Server::Client::get_joined_channel(std::string const & channel_name)
{
	IRC_Server::Client::t_ChanMap::iterator	it = _channels.find(channel_name);
	return (std::make_pair(it, (it == _channels.end() ? false : true)));
}

/*!
** @brief	Returns a read-only reference to the Client's next crlf-terminated
**			message ready to be reaped and parsed. @see ::get_message()
**
** @details	This must NOT be used for retrieving a message for parsing. For that
**			get the argument vector using ::get_message(). It is purely for
**			observational/debugging purposes.
**
**			The buffer and client state remain unchanged.
** @return	A read-only reference to the Client's next full unreaped message, or
**			an empty string if the Client currently has no full message.
*/
std::string const &			IRC_Server::Client::see_next_message(void) const
{
	return (_message);
}

/*!
** @brief	Returns a read-only reference to any full or partial messages in the
**			Client's message buffer AFTER the next full message as returned by
**			::see_next_message().
**
** @details	This must NOT be used for retrieving a message for parsing. For that
**			get the argument vector using ::get_message(). It is purely for
**			observational/debugging purposes.
**
**			The buffer and client state remain unchanged.
** @return	A read-only reference to the contents of the Client's message buffer
**			AFTER the next full unreaped message, or an empty string if the
**			buffer is empty.
*/
std::string const &			IRC_Server::Client::see_msg_buf(void) const
{
	return(_msg_buf);
}
