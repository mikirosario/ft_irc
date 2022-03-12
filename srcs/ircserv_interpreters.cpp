/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_interpreters.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:43:06 by miki              #+#    #+#             */
/*   Updated: 2022/03/12 13:19:28 by acortes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ircserv.hpp"

/* ---- PARSING ---- */

/*!
** @brief	The easter egg command.
*/
void	IRC_Server::exec_cmd_BAILA(Client & sender, std::vector<std::string> const & argv)
{
	if (argv.size() < 2)
		send_rpl_PRIVMSG(sender, sender, "El servidor baila el chotis");
	else
		send_rpl_PRIVMSG(sender, sender, "¿Adónde voy con tanto parámetro?");
}

/*!
** @brief	Determines validity of @a nick as a nickname.
**
** @details	A nickname may contain any US-ASCII letter (A-Z, a-z) number (0-9),
**			as well as `|^_-{}[]\. A nickname may not start with a number or -.
**			A nickname must be from 1 to 9 characters long.
**
**			NOTE: Assumes "C" locale.
** @param	nick	A string proposed as a nickname.
** @return	true if @a nick is a valid nickname, otherwise false
*/
bool	IRC_Server::nick_is_valid(std::string const & nick) const
{
	if (nick.size() < 1
		|| nick.size() > 9
		|| std::isdigit(static_cast<unsigned char>(nick[0]))
		|| nick[0] == '-')
			return (false);
	else
		for (std::string::size_type i = 1, end = nick.size(); i < end; ++i)
			if (!std::isalnum(static_cast<unsigned char>(nick[i])) && std::strchr("`|^_-{}[]\\", static_cast<unsigned char>(nick[i])) == NULL)
				return (false);
	return (true);
}

/*!
** @brief	Determines validity of @a username as a username.
**
** @details	A username may contain any octet except NUL CR LF SPACE and @ and
**			between 1 and MAX_USERNAME_SIZE bytes.
** @param	user	A string proposed as a username.
** @return	true if @a user is a valid username, otherwise false
*/
bool	IRC_Server::username_is_valid(std::string const & username) const
{
	if (username.size() < 1 || username.size() > MAX_USERNAME_SIZE)
			return (false);
	else
		for (std::string::size_type i = 1, end = username.size(); i < end; ++i)
			if (std::strchr("\r\n @\0", username[i]) != NULL)
				return (false);
	return (true);
}

/*!
** @brief	Determines validity of @a channel_name as a channel name.
**
** @details	A channel name must start with '&', '#', '+' or '!' and be between
**			2 and 50 bytes long, and may not contain ' ', '\a', ',' or '0'.
** @param	channel_name	A string proposed as a channel name.
** @return	true of @a channel_name is a valid channel name, otherwise false
*/
bool	IRC_Server::channel_name_is_valid(std::string const & channel_name) const
{
	if (channel_name.size() < 2
		|| channel_name.size() > MAX_CHANNELNAME_SIZE
		|| std::strchr("&#+!", channel_name[0]) == NULL
		|| channel_name.find_first_of(" \a,") != std::string::npos)
		return false;
	return true;
}

/*!
** @brief	Trims @a str of any leading or trailing @a unwanted_chars.
** @param	str				The string to trim.
** @param	unwanted_chars	The leading and trailing characters to be
**							eliminated from @a str.
** @return	A reference to @a str.
*/
std::string	& IRC_Server::trim(std::string & str, std::string const & unwanted_chars)
{
	str.erase(str.find_last_not_of(unwanted_chars) + 1);
	str.erase(0, str.find_first_not_of(unwanted_chars));
	return str;
}

/*!
** @brief	Removes any adjacent instances of @a c from @a str.
**
** @details	A string @a bccd,,,,,dcc,,d, where ',' is the character to remove,
**			becomes: @a bccd,dcc,d .
** @param	str	The string from which to remove consecutive instances of @a c.
** @param	c	The character to be removed from @a str if next to a duplicate.
** @return	A reference to @a str.
*/
std::string & IRC_Server::remove_adjacent_duplicates(std::string & str, char c)
{
	std::string::iterator		it = str.begin();
	std::string::const_iterator end = str.end();

	while (it + 1 != end)
	{
		if (*it == c && *it == *(it + 1))
			it = str.erase(it);
		else
			++it;
	}
	return str;
}

/*!
** @brief	Trims leading, trailing and consecutive adjacent @a delimiter
**			characters from @a str. Use to preprocess any parameters sent in
**			list format (foo,{foo,}) before sending to std::getline(), as it is
**			intolerant of the errata that this function eliminates.
**
** @details	A string @a ,,,bccd,,,,,,dcc,,d, where ',' is the @a delimiter
**			becomes: @a bccd,dcc,d .
** @param	str			The string to preprocess. May not be const; const_cast
**						if needed.
** @param	delimiter	The delimiter to trim.
** @return	A reference to the trimmed @a str.
*/
std::string &	IRC_Server::preprocess_list_param(std::string & str, char delimiter)
{
	return (remove_adjacent_duplicates(trim(str, std::string(1, delimiter)), delimiter));
}

/*!
** @brief	Attempts to register @a client on the server, sending the
**			appropriate replies to the client depending on whether the attempt
**			was or was not successful.
**
** @details	The client's password validation status is irrelevant if the server
**			has no password; otherwise, it will determine whether or not
**			registration goes ahead. Client passwords are not saved, only a bool
**			indicating whether or not they were accepted. Only the last password
**			sent by the client is relevant.
** @param	client	The Client we will attempt to register.
*/
bool	IRC_Server::register_client(Client & client)
{
	//check if server has PASS; if not, ignore user PASS, if so, check if user PASS was correct
	if (_servpass.empty() == false && client.get_pass_validated() == false) 	//um... we don't accept you, what do we do now?? just kick you?¿ xD
	{
		remove_client_from_server(client);
		std::cout << "pollserver: socket " << client.get_sockfd() << " kicked from server." << std::endl;
	}
	else 										//if password was OK or there was no password requirement, we accept new client, send all the IRPLY here
	{
		client.set_state_registered();	//set client state to registered
		send_rpl_WELCOME(client);
		send_rpl_YOURHOST(client);
		send_rpl_CREATED(client);
	}

	return(true);
}

/* ---- INTERPRETING ---- */

/*!
** @brief	Executes a PASS command originating from @a sender.
**
** @details	Attempts to retrieve a password sent by @a sender in the @a argv
**			argument vector as the first parameter, for use in registration.
**
**			If there is no first parameter, an ERR_NEEDMOREPARAMS error reply is
**			returned to @a sender.
**
**			If @a sender is already registered, an ERR_ALREADYREGISTERED error
**			reply is returned to @a sender.
**
**			If both aforementioned errors are simultaneously present, only the
**			ERR_ALREADYREGISTERED error reply is returned to @a sender.
**
**			If more than MAX_PASS_ATTEMPTS calls are made to set_pass during
**			registration, an ERR_UNKNOWNERROR error reply is returned to
**			@a sender reprimanding them and they are banished from the Realm.
** @param	sender	A reference to the client who who sent the command.
** @param	argv	A reference to the message containing the command (argv[0])
**					and its arguments (argv[1,...]) in a string vector.
*/
void	IRC_Server::exec_cmd_PASS(Client & sender, std::vector<std::string> const & argv)
{
	if (sender.is_registered() == true)
		send_err_ALREADYREGISTERED(sender, "You may not reregister");
	else if (sender.get_pass_attempts() == MAX_PASS_ATTEMPTS)
	{
			send_err_UNKNOWNERROR(sender, argv[0], "You've sent too many PASS commands");
			remove_client_from_server(sender);
			std::cout << "pollserver: socket " << sender.get_sockfd() << " kicked from server." << std::endl;
	}
	else if (argv.size() < 2) //Only command argument exists
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	else
	{
		sender.reg_pass_attempt();				//register password validation attempt
		if (confirm_pass(argv[1]) == false)		//if password is incorrect send error reply PASSWD_MISMATCH
		{
			sender.set_pass_validated(false);
			send_err_PASSWDMISMATCH(sender, "Password incorrect");
		}
		else
			sender.set_pass_validated(true);
	}
}

/*!
** @brief	Executes a NICK command originating from @a sender.
**
** @details	Attempts to retrieve a nickname sent by @a sender in the @a argv
**			argument vector as the first parameter, for use in registration or
**			to change an existing nickname.
**
**			If there is no first parameter, an ERR_NONICKNAMEGIVEN error reply
**			is returned to @a sender.
**
**			If the nickname is invalid, an ERR_ERRONEOUSNICKNAME error reply is
**			returned to @a sender.
**
**			If the nickname is already in use on the server as determined by a
**			case-insensitive lookup, an ERR_NICKNAMEINUSE error reply is
**			returned to @a sender.
**
**			If @a sender is already registered and the new nickname is accepted,
**			a confirmation reply is	returned to @a sender.
**
**			If @a sender is unregistered and has sent a username before sending
**			the nickname, we will try to register @a sender after setting the
**			nickname.
**
**			If @a sender is unregistered and has already sent a nickname, we
**			will boot @a sender as our patience is not unlimited.
*/
void	IRC_Server::exec_cmd_NICK(Client & sender, std::vector<std::string> const & argv)
{
	if (argv.size() < 2)
		send_err_NONICKNAMEGIVEN(sender, "No nickname given");
	else if (nick_is_valid(argv[1]) == false)
		send_err_ERRONEOUSNICKNAME(sender, argv[1], "Erroneous nickname");
	else if (find_client_by_nick(argv[1]) != NULL)
		send_err_NICKNAMEINUSE(sender, argv[1], "Nickname is already in use");
	else if (sender.is_registered() == true)
	{
		std::string	old_source = sender.get_source();
		sender.set_nick(argv[1]);
		send_rpl_NICK(sender, old_source);	//if client is already registered, this is a nickname change, we send a reply as confirmation
	}
	else //if client is not registered, we'll check to see if we have a username; if we do, we try to register
	{
		if (sender.get_username().empty() == false)	//we have both nick and user, try to register
		{
			sender.set_nick(argv[1]);
			register_client(sender);
		}
		else if (sender.get_nick().empty() == false) //if we already have a nick and the client is just bombing us with multiple NICK commands, send them to hell
		{
			send_err_UNKNOWNERROR(sender, argv[0], "You've sent more than one NICK command during registration");
			remove_client_from_server(sender);
			std::cout << "pollserver: socket " << sender.get_sockfd() << " kicked from server." << std::endl;
		}
		else										//it's the first nick command from an unregistered client
			sender.set_nick(argv[1]);
	}
}

/*!
** @brief	Executes a USER command originating from @a sender.
**
** @details	Attempts to retrieve the username and real name parameters of the
**			USER command for use in registration.
**
**			The USER command has four parameters:
**			<username> <hostname> <servername> :<realname>
**
**			The middle two are only used in server-to-server connections and
**			seem to have some semi-unstandardized uses too, such as using the
**			hostname field to set a USER mode. They are ignored in direct
**			client-server connections, so they are ignored in this project.
**
**			The realname parameter must be last parameter as it may contain
**			spaces.
**
**			If @a sender is already registered, an ERR_ALREADYREGISTERED error
**			reply is returned to @a sender.
**
**			If there are not four parameters (five arguments in the argument
**			vector, including the command argument), an ERR_NEEDMOREPARAMS error
**			reply is returned to @a sender.
**
**			If the username does not comply with RFC 2812 (section 2.3.1)
**			regarding the user parameter, an ERR_UNKNOWNERROR error reply is
**			returned to @a sender explaining the problem.
**
**			If @a sender is unregistered but has already provided a username
**			before, @a sender will be booted from the server and an
**			ERR_UNKNOWNERROR error reply is returned to @a sender explaining
**			that spamming the server is rude.
**
**			If @a sender is unregistered and provided a nickname before, we will
**			try to register @a sender.
** @param	sender	A reference to the client who sent the command.
** @param	argv	A reference to the message containing the command (argv[0])
**					and its arguments (argv[1,...]) in a string vector.
*/
void	IRC_Server::exec_cmd_USER(Client & sender, std::vector<std::string> const & argv)
{
	if (sender.is_registered() == true)
		send_err_ALREADYREGISTERED(sender, "You may not reregister");
	else if (argv.size() < 5 || argv[1].size() < 1) //Need four params, although middle two are ignored from directly connected client; if username is empty string, this error message is also sent
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	else if (username_is_valid(argv[1]) == false)
		send_err_UNKNOWNERROR(sender, argv[0], "Username contains invalid characters (NUL, SPACE, CR, LF or @)");
	else if (sender.get_username().empty() == false) //The client is spamming us with USER commands without trying to register. The Queen is not amused. ¬¬
	{
		send_err_UNKNOWNERROR(sender, argv[0], "You've sent more than one USER command during registration");
		remove_client_from_server(sender);
		std::cout << "pollserver: socket " << sender.get_sockfd() << " kicked from server." << std::endl;
	}
	else
	{
		sender.set_username(argv[1]);
		sender.set_realname(argv[4]);
		if (sender.get_nick().empty() == false) //The client already sent a nick, so we can FINALLY try to register
			register_client(sender);
	}
}


		//(std::string() += "blah").size(); //debug //normal
		//(std::ostringstream() << "No recipient given (" << argv[0] << ")").str(); //debug //why not compiler????
/*!
** @brief	Executes a PRIVMSG command originating from @a sender.
**
** @details	Attempts to send the text at argv[2] to the targets listed at
**			argv[1], wherein targets are separated by a ','.
**
**			If less than 3 parameters (cmd, targets, message) are present in
**			@a argv a NEEDMOREPARAMS error is sent back to @a sender.
**
**			Otherwise, if there are no targets in @a argv, a NORECIPIENT error
**			is sent back to @a sender.
**
**			Otherwise, if there is no text at argv[2], a NOTEXTTOSEND error is
**			sent back to @a sender.
**
**			Otherwise, we attempt to retrieve each target from the target list
**			using std::getline() with ',' as a delimiter. If that fails for any
**			reason, an UNKNOWN error is sent back to @a sender.
**
**			Any failure of std::getline(), such as due to end of file or badbit,
**			will cause the while condition to be false, aborting any subsequent
**			retrieval attempts. Thus, only one UNKNOWN error will be sent back
**			to @a sender, even if there were more errors in the stream.
**
**			If a provided target nick or channel name is not found, a NOSUCHNICK
**			error will be sent back to @a sender. Multiple such errors may be
**			sent for a single command.
**
**			If the target is a user, the text at argv[2] will be sent directly
**			to the user as a private message.
**
**			The target is considered a channel if it is preceded by '#'. If the
**			target is a channel, the text at argv[2] will be sent to each member
**			of the channel at or above the privilege level specified by the
**			prefixes preceding '#'. No prefixes signifies the lowest privilege
**			level and will be sent to all channel members, '%' signifies halfop
**			level, '@' signifies chanop level and '~' signifies owner level.
**
**			Any other prefix will not be accepted. 				//debug //currently implemented in channel object, maybe move logic here
**
**			If multiple prefixes are sent, the one with the lowest privilege
**			level will be used. For example: "@~%#" == "%#".	//debug // currently implemented in channel object, maybe move logic here
** @param	sender	A reference to the client who sent the command.
** @param	argv	A reference to the message containing the command (argv[0])
**					and its arguments (argv[1,...]) in a string vector.
*/
void	IRC_Server::exec_cmd_PRIVMSG(Client & sender, std::vector<std::string> const & argv)
{

	if (argv.size() < 3) //we need cmd target{,target} :message
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	else if (argv[1].empty() == true) //eliminate any commas before checking this
	{
		std::ostringstream ss;
		ss << "No recipient given (" << argv[0] << ")";
		send_err_NORECIPIENT(sender, ss.str()); //debug //there HAS to be a way to one-line this xD
	}
	else if (argv[2].empty() == true)
		send_err_NOTEXTTOSEND(sender, "No text to send");
	else
	{
		std::string				msg_source = sender.get_source() + " ";
		std::string				target;
		Client * 				usr_recipient;
		t_Channel_Map::iterator	ch_recipient;
		//std::stringstream		raw_target_list(remove_adjacent_duplicates(trim(const_cast<std::string &>(argv[1]), std::string(1, ',')), ','));
		std::stringstream		raw_target_list(preprocess_list_param(const_cast<std::string &>(argv[1]), ','));

		do
		{
			std::getline(raw_target_list, target, ',');
			
			if (raw_target_list.fail() == true)
				send_err_UNKNOWNERROR(sender, argv[0], "Invalid target passed to std::getline()");
			else
			{
				size_t		hash_pos = target.find_first_of("#");
				if (hash_pos != std::string::npos) 								//it's a channel
				{
					size_t		chname_pos;
					if ((chname_pos = target.find_first_not_of("#", hash_pos)) == std::string::npos ||
						(ch_recipient = _channels.find(target.substr(chname_pos - 1))) == _channels.end())	//it's a channel, but with an empty name OR that does not exist in _channels
						send_err_NOSUCHNICK(sender, target.substr(hash_pos), "No such channel");			//Don't know why, but RFC says use NOSUCHNICK, not NOSUCHCHANNEL, in this case!
					else																				//it's a channel and it exists in _channels
					{
						std::string	prefixes = target.substr(0, hash_pos);									//get the pre-hash-pos prefixes, if any!!!!
						send_rpl_PRIVMSG(ch_recipient->second, sender, prefixes, argv[2]);
					}	
				}
				else if((usr_recipient = find_client_by_nick(target)) == NULL)	//it's a user, but no such nick
					send_err_NOSUCHNICK(sender, target, "No such nick");
				else															//it's a user, and we found nick
					send_rpl_PRIVMSG(*usr_recipient, sender, argv[2]);
			}
		}
		while (raw_target_list.eof() == false);
	}
}

/*!
** @brief	Executes a PING command originating from @a sender.
**
** @param	sender	A reference to the client who sent the command.
** @param	argv	A reference to the message containing the command (argv[0])
**					and its arguments (argv[1,...]) in a string vector.
*/
void	IRC_Server::exec_cmd_PING(Client & sender, std::vector<std::string> const & argv)
{
	if (argv.size() < 2)
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	else
		send_rpl_PONG(sender, argv[1]);
}

/****************************************
			JOIN COMMAND
*****************************************/

// DUDAS
// 	- when is NOSUCHCHANNEL thrown?? NOSUCHCHANNEL doesn't always mean CHANNEL created?
void	IRC_Server::exec_cmd_JOIN(IRC_Server::Client & sender, std::vector<std::string> const & argv)
{
	if (argv.size() < 2)
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	else if (argv[1] == "0")
		sender.leave_all_channels();
	else						//try to process
	{
			std::stringstream	raw_channel_list(preprocess_list_param(const_cast<std::string &>(argv[1]), ','));	//get raw channel list
			std::stringstream	raw_key_list;
			if (argv.size() > 2)							//get raw key list, if any
				raw_key_list << preprocess_list_param(const_cast<std::string &>(argv[2]), ',');
			do								//get channels
			{
				std::string				channel;
				std::string 			key;
				t_Channel_Map::iterator chan_it;
				int						ret = 0; // 1 == success, -1 bad password, -2 bad privilege syntax, 0 bad_alloc, bad chname or other errors

				key.clear();	//we want to clear this string after raw_key_list.eof()
				std::getline(raw_channel_list, channel, ',');
				std::getline(raw_key_list, key, ',');			//eof flag will be set when done, and key will no longer be updated
				if (raw_channel_list.fail() == true)
					send_err_UNKNOWNERROR(sender, argv[0], "Invalid target passed to std::getline()");
				else if (channel_name_is_valid(channel) == false)
					send_err_BADCHANMASK(sender, channel, ":Bad Channel Mask");
				else if ((chan_it = _channels.find(channel)) == _channels.end())	//channel doesn't exist, sender creates channel
				{
					if ((chan_it = add_channel(sender, channel, key)) == _channels.end())	//map insert failure, probably bad_alloc or something
						send_err_UNKNOWNERROR(sender, argv[0], "Server error: could not add channel");
					else
						ret = 1;															//map insert success
				}
				else if	((ret = chan_it->second.addMember(sender, chan_it, key, 0)) != 1)	//channel exists, sender attempts to join channel...
				{																	//but failed, because...
					if (ret == -1)													//it gave the wrong key
						send_err_BADCHANNELKEY(sender, chan_it->second, "Cannot join channel (+k)");
					if (ret == 0)													//garden gnomes interfered; probably memory-related
						send_err_UNKNOWNERROR(sender, argv[0], "Server error: could not add channel");
				}
				//SEND_RPL
				if (ret == 1) //somehow, some way, the client made it through that spaghetti and actually managed to join. congratulations!!!! xD
				{
					send_rpl_JOIN(chan_it->second, sender);
					send_rpl_NAMREPLY(sender, chan_it->second);
				}
					// membership restriction checks go in addMember, coded in return value; check if banned, etc.
					//key will be empty if there is none associated; 0 is for user level. 
			}
			while (raw_channel_list.eof() == false);
	}
}

/****************************************
			PART COMMAND
*****************************************/

void	IRC_Server::exec_cmd_PART(Client & sender, std::vector<std::string> const & argv)
{
	if (argv.size() < 2)
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	else
	{
		t_Channel_Map::iterator	ch_recipient;
		std::stringstream		raw_channel_list(preprocess_list_param(const_cast<std::string &>(argv[1]), ','));
		std::string				channel;
		do
		{
			
			std::getline(raw_channel_list, channel, ',');
			
			if (raw_channel_list.fail() == true)
				send_err_UNKNOWNERROR(sender, argv[0], "Invalid channel passed to std::getline()");
			
			else
			{
				size_t		hash_pos = channel.find_first_of("#");
				if (hash_pos != std::string::npos) 								//it's a channel
				{
					size_t		chname_pos;
					if ((chname_pos = channel.find_first_not_of("#", hash_pos)) == std::string::npos ||
						(ch_recipient = _channels.find(channel.substr(chname_pos - 1))) == _channels.end())	//it's a channel, but with an empty name OR that does not exist in _channels
						send_err_NOSUCHCHANNEL(sender, "PART", "No such channel");
					//else if (sender.leave_channel(ch_recipient->second.getChannelName()) == false)
					else if (sender.get_joined_channel(ch_recipient->second.getChannelName()).second == false)
						send_err_NOTONCHANNEL(sender, ch_recipient->second, "You're not on that channel");
					else																				//it's a channel and it exists in _channels
					{
						send_rpl_PART(sender, ch_recipient->second, (argv.size() > 2 ? argv[2] : sender.get_nick()));
						sender.leave_channel(ch_recipient->second.getChannelName());
					}
				}
				else
					send_err_NOSUCHCHANNEL(sender, "PART", "No such channel");
			}
		}
		while (raw_channel_list.eof() == false);
	}
}

/****************************************
			TOPIC COMMAND
*****************************************/

void	IRC_Server::exec_cmd_TOPIC(Client & sender, std::vector<std::string> const & argv)
{
	//	Aqui hacemos que part salga de los canales que pasamos por argumento. Parece sencillo

	size_t argv_size = argv.size();
	
	if (argv_size < 2)
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	
	bool existChannel = this->find_channel(argv[1]);
	if(!existChannel)
		send_err_NOSUCHCHANNEL(sender, argv[1], "Channel not found");
	else if(argv_size == 2)
		send_rpl_TOPIC(sender, argv[1], _channels.find(argv[1])->second.getTopic());
	else
	{
		// Faltaria un else if comproovando si el usuario dispone de los permisos necesarios para cambiar el topic. Es decir, que sea op/hop

		if (sender.get_joined_channel(argv[1]).second == false)
				send_err_NOTONCHANNEL(sender,  _channels.find(argv[1])->second, "You're not on that channel");
		else
		{
			std::string msg;

			for (size_t i = 3; i < argv_size; i++)
				msg += argv[i] + " ";
			_channels.find(argv[1])->second.setTopic(msg);
			//commando para mandar mensaje sobre que se cambio el topic a todos los miembros del cananl
			
		}
	}
}

/****************************************
			NAMES COMMAND
*****************************************/

/*!
** @brief	Executes a NAMES command originating from @a sender.
**
** @details	The @a sender receives a list of members of each channel listed in
**			argv[1]. Channels must be delimited by commas: channel1,channel2...
**
**			As in all lists of this kind, we preprocess the parameter to clean
**			up trailing, leading or duplicate delimiters.
**
**			If the member list for a channel does not fit in MSG_BUF_SIZE, it
**			will be split into several NAMREPLY messages. NAMREPLY will send
**			ENDOFNAMES when it has finished sending all members.
**
**			If no channel name is sent, only an ENDOFNAMES reply is sent with
**			an asterisk in place of a channel name.
**
**			If any channel name is invalid or not found, only an ENDOFNAMES
**			reply is sent with the invalid channel name.
**
**			If garden gnomes attack std::getline(), UNKNOWNERROR error reply is
**			sent.
**
**			//debug //invisible user modes and private channel modes not yet implemented!!
** @param	sender	A reference to the client who sent the command.
** @param	argv	A reference to the message containing the command (argv[0])
**					and its arguments (argv[1,...]) in a string vector.
*/
void	IRC_Server::exec_cmd_NAMES(Client & sender, std::vector<std::string> const & argv)
{
	if (argv.size() < 2)
		send_rpl_ENDOFNAMES(sender, "*");
	else if (argv.size() == 2)						
	{
			std::stringstream	raw_channel_list(preprocess_list_param(const_cast<std::string &>(argv[1]), ','));
			do
			{
				std::string				channel;
				t_Channel_Map::iterator chan_it;

				std::getline(raw_channel_list, channel, ',');
				if (raw_channel_list.fail() == true)
					send_err_UNKNOWNERROR(sender, argv[0], "Invalid target passed to std::getline()");
				else if ((chan_it = _channels.find(channel)) == _channels.end())	//didn't find channel
					send_rpl_ENDOFNAMES(sender, channel);
				else																//found channel
				{
					//debug / error // Si la lista no es invisible por las flags +p o +s, entonces retornamos el nombre de el canal
					// Si no, el caso de error correspondiente ( ¿ o simplemente mostrar como si no existiera?)

					// Adrian -->	Hacemos que continue el bucle como si no hubiera pasado nada. Me parece la opción mas limpia
					if(chan_it->second.get_mode().find('s') != std::string::npos || chan_it->second.get_mode().find('p') != std::string::npos)
						if (sender.get_joined_channel(chan_it->second.getChannelName()).second == false)
							continue;
					send_rpl_NAMREPLY(sender, chan_it->second);
				}
			}
			while (raw_channel_list.eof() == false);
	}
}

/****************************************
			LIST COMMAND
*****************************************/


//	exec_cmd_LIST imitando el uso de stringstrem en la version de JOIN de Miki

void	IRC_Server::exec_cmd_LIST(Client & sender, std::vector<std::string> const & argv)
{
	size_t argv_size = argv.size();

	if (argv_size < 2)
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	else if (argv_size == 2)
	{
		for (t_Channel_Map::iterator i = _channels.begin(); i != _channels.end(); i++)
		{
			if(i->second.get_mode().find('s') != std::string::npos || i->second.get_mode().find('p') != std::string::npos)
				if (sender.get_joined_channel(i->second.getChannelName()).second == false)
						continue;
			send_rpl_LISTSTART(sender);
			send_rpl_LIST(sender, _channels.find(i->first)->second);
			send_rpl_LISTEND(sender);
		}
	}
	else if (argv_size == 3)						
	{
			std::stringstream	raw_channel_list(argv[1]);
			do
			{
				std::string				channel;
				t_Channel_Map::iterator chan_it;
				int						ret; // 0 bad_alloc or other errors

				std::getline(raw_channel_list, channel, ',');
				if (raw_channel_list.fail() == true)
				{
					ret = 0;
					send_err_UNKNOWNERROR(sender, argv[0], "Invalid target passed to std::getline()");
				}
				else if (channel_name_is_valid(channel) == false)
				{
					ret = 0;
					send_err_BADCHANMASK(sender, channel, ":Bad Channel Mask");
				}
				else if ((chan_it = _channels.find(channel)) != _channels.end())
				{
					if (chan_it->second.get_mode().find('s') != std::string::npos || chan_it->second.get_mode().find('p') != std::string::npos)
						if (sender.get_joined_channel(chan_it->second.getChannelName()).second == false)
							continue;
					send_rpl_LISTSTART(sender);
					send_rpl_LIST(sender, chan_it->second);
					send_rpl_LISTEND(sender);
				}
				else
					send_err_NOSUCHCHANNEL(sender, channel, "Channel not found");
				
			}
			while (raw_channel_list.eof() == false);
	}
	else
	{
		//	Enviar error por demasiados argumentos
	}
}

/****************************************
			INVITE COMMAND
*****************************************/

void	IRC_Server::exec_cmd_INVITE(Client & sender, std::vector<std::string> const & argv)
{
	//	Aqui hacemos que part salga de los canales que pasamos por argumento. Parece sencillo
	(void) sender;
	(void) argv;
}

/****************************************
			KICK COMMAND
*****************************************/

void	IRC_Server::exec_cmd_KICK(Client & sender, std::vector<std::string> const & argv)
{
	if (argv.size() < 3)
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	else if (!find_channel(argv[1]))
		send_err_NOSUCHCHANNEL(sender, argv[0], "No such channel");
	else
	{

	//	Comprobacion de que el usuario pertenece al canal del que busca eliminar a alguien. Error: ERR_NOTONCHANNEL

	// Comprobacion de si el usuario tiene permisos suficientes para ejecutar el kick. Error: ERR_CHANOPRIVSNEEDED

	// Comprobacion de si el usuario usuario buscado realmente existe en el canal. Error: ERR_USERNOTINCHANNEL

	// ¡Posibilidad!	¿Tenemos que comprobar si un ops/hops esta intentando eliminar un ops/hops de mayor o igual rango?

	// Eliminamos al usuario. De tener mas de tres argumentos depuramos el mensaje al igual que hacemos en TOPIC, con la diferencia de que
	//	mandamos mensaje necesario. De no definir mensaje, usamos mensaje generico.

	}
}

/********************************************************************************

 * 									MODE
		
*********************************************************************************/

void IRC_Server::ft_add_mode(Client const &sender, std::string const &channelName, std::string const &modes)
{
	t_Channel_Map::iterator chan_it;
	const std::string valid_modes = "isp";

	chan_it = _channels.find(channelName);
	for (size_t i = 1; i < modes.size(); i++) 
	{
    	if (valid_modes.find(modes.at(i)) != std::string::npos)
		{
			if (chan_it->second.get_mode().find(modes[i]) == std::string::npos)
				chan_it->second.add_mode(modes[i]);
		}
		else
			send_err_ERR_UNKNOWNMODE(sender, modes[i], "is unknown mode char to me");
	}


}

void IRC_Server::ft_remove_mode(Client const &sender, std::string const &channelName, std::string const &modes)
{
	t_Channel_Map::iterator chan_it;
	const std::string valid_modes = "isp";

	chan_it = _channels.find(channelName);
	for(size_t i = 1; i < modes.size(); i++) 
	{
    	if(valid_modes.find(modes.at(i)) != std::string::npos)
		{
			if (chan_it->second.get_mode().find(modes[i]) != std::string::npos)
				chan_it->second.remove_mode(modes[i]);
		}
		else
			send_err_ERR_UNKNOWNMODE(sender, modes[i], "is unknown mode char to me");
	}

}

void	IRC_Server::exec_cmd_MODE(Client &sender, std::vector<std::string> const &argv)
{
	if (argv.size() < 3)
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	else if (argv[1].front() == '+' || argv[1].front() == '&')
		send_err_UNKNOWNERROR(sender, argv[0], "Unsupported channel prefixes");
	else if (argv[1].front() == '#' || argv[1].front() == '!')
	{
		if(!find_channel(argv[1]))
			send_err_NOSUCHCHANNEL(sender, argv[0], "No such channel");
		else
		{
			if (argv[2].front() == '+' )
				ft_add_mode(sender, argv[1], argv[2]);
			else if(argv[2].front() == '-')
				ft_remove_mode(sender, argv[1], argv[2]);
			else
				send_err_UNKNOWNERROR(sender, argv[0], " + or - required to give/remove modes");
		}
	}
	else if (!find_channel(argv[1]))
		send_err_NOSUCHCHANNEL(sender, argv[0], "No such channel");
}

/*!
** @brief	Takes the message from the Client as an argument vector, identifies
**			the command portion (argument at position 0) and sends the message
**			to the appropriate interpreter. If the command is unrecognized,
**			sends the Client an UNKNOWNCOMMAND reply.
**
** @details
**					
** @param	client	The Client with a message ready to be reaped.
*/
void	IRC_Server::interpret_msg(Client & client)
{
	std::vector<std::string>	argv = client.get_message();
	

	//this might work best as a cmd-method map, just need to standardize all the functions...
	if (argv.size() < 1) //if somehow the client buffer contained no command, we do nothing with the message
		return ;
	std::string &	cmd = argv[0];
	if (cmd == "PASS")
		exec_cmd_PASS(client, argv);
	else if (cmd == "NICK")
		exec_cmd_NICK(client, argv);
	else if (cmd == "USER")
		exec_cmd_USER(client, argv);
	else if (client.is_registered() == false)
		send_err_NOTREGISTERED(client, "You must register with the server first");
	else
	{
		if (cmd == "PRIVMSG")	//PRIVMSG será el comando más común probablemente, así que lo dejo el primero y fuera del map
			exec_cmd_PRIVMSG(client, argv);
		else if (cmd == "JOIN")
			exec_cmd_JOIN(client, argv);
		else if (cmd == "PART")
			exec_cmd_PART(client, argv);
		else if (cmd == "NAMES")
			exec_cmd_NAMES(client, argv);
		else if (cmd == "PING")
			exec_cmd_PING(client, argv);
		else if (cmd == "BAILA") //debug //this was originally just the first test case, might leave it in as an easter egg though ;)
			exec_cmd_BAILA(client, argv);
		
		else
			send_err_UNKNOWNCOMMAND(client, cmd, "Unknown command");
	}
}
