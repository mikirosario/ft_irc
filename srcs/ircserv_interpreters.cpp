/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_interpreters.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikiencolor <mikiencolor@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:43:06 by miki              #+#    #+#             */
/*   Updated: 2022/05/12 12:19:44 by mikiencolor      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utility>
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
bool	IRC_Server::nick_is_valid(std::string const & nick)
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
bool	IRC_Server::username_is_valid(std::string const & username)
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
** @details	A channel name must start with '#' and be between 2 and 50 bytes			// debug //deprecated A channel name must start with '&', '#', '+' or '!' and be between
**			long, and may not contain ' ', '\a', ',' or '0'.
** @param	channel_name	A string proposed as a channel name.
** @return	true of @a channel_name is a valid channel name, otherwise false
*/
bool	IRC_Server::channel_name_is_valid(std::string const & channel_name)
{
	if (channel_name.size() < 2
		|| channel_name.size() > MAX_CHANNELNAME_SIZE
		|| std::strchr("#", channel_name[0]) == NULL
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
		send_rpl_MYINFO(client);
		send_rpl_ISUPPORT(client);
		send_err_NOMOTD(client, "MOTD file is missing"); //debug //temporary, no MOTD implemented yet
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
		//std::string				msg_source = sender.get_source() + " ";
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

/*!
** @brief	Executes an MOTD command originating from @a sender.
**
** @param	sender	A reference to the client who sent the command.
** @param	argv	A reference to the message containing the command (argv[0])
**					and its arguments (argv[1,...]) in a string vector.
*/
void	IRC_Server::exec_cmd_MOTD(Client & sender, std::vector<std::string> const & argv)
{
	if (argv.size() > 1 && argv[1].compare(_servername) != 0)
		send_err_NOSUCHSERVER(sender, argv[1], "No such server");
	else	//debug //add MOTD at some point :p
		send_err_NOMOTD(sender, "MOTD file is missing");
}

/*!
** @brief	Executes a NOTICE command originating from @a sender.
**
** @param	sender	A reference to the client who sent the command.
** @param	argv	A reference to the message containing the command (argv[0])
**					and its arguments (argv[1,...]) in a string vector.
*/
void	IRC_Server::exec_cmd_NOTICE(Client & sender, std::vector<std::string> const & argv)
{
	std::string				target;
	Client * 				usr_recipient;
	std::stringstream		raw_target_list(preprocess_list_param(const_cast<std::string &>(argv[1]), ','));

	if (argv.size() < 3)
		return ;
	do	
	{
		std::getline(raw_target_list, target, ',');
		
		if (raw_target_list.fail() == false) //valid target passed to std::getline
		{
			size_t		hash_pos = target.find_first_of("#");
			if (hash_pos == std::string::npos && (usr_recipient = find_client_by_nick(target)) != NULL) //it's not a channel and the user exists
				send_rpl_NOTICE(*usr_recipient, sender, argv[2]);
		}
	}
	while (raw_target_list.eof() == false);
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
					else if (ret == -3)
						send_err_BANNEDFROMCHAN(sender, chan_it->second, "Cannot join channel (+b)");
					else if (ret == 0)												//garden gnomes interfered; probably memory-related
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
	//size_t argv_size = argv.size();
	//std::vector<std::string> stringVector;
	
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
					// if ((chname_pos = channel.find_first_not_of("#", hash_pos)) == std::string::npos ||
					// 	(ch_recipient = _channels.find(channel.substr(chname_pos - 1))) == _channels.end())	//it's a channel, but with an empty name OR that does not exist in _channels
					if ((chname_pos = channel.find_first_not_of("#", hash_pos)) == std::string::npos ||
						(ch_recipient = _channels.find(channel)) == _channels.end())	//it's a channel, but with an empty name OR that does not exist in _channels
						send_err_NOSUCHCHANNEL(sender, channel, "No such channel");
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
					send_err_NOSUCHCHANNEL(sender, channel, "No such channel");
			}
		}
		while (raw_channel_list.eof() == false);
	}
	// for (std::vector<std::string>::iterator it = stringVector.begin(); it != stringVector.end(); it++)
	// {
	// 	std::string password = "";
	// 	std::string expectsString(*it);

	// 	//	Error encontrado. Find channel nos devuelve siempre 0 si ejecutamos "PART canalAleatorioInventado"
		
	// 	bool existChannel = find_channel(expectsString);
	// 	if(!existChannel)
	// 		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	// 	else
	// 	{
	// 		//send_err_NOSUCHCHANNEL(sender, argv[0], "Debug error");
	// 		if (argv_size == 2)
	// 			remove_user_from_channel(sender, expectsString);
	// 		// else
	// 		// {
	// 		// 	size_t i = 2;
	// 		// 	std::string tmp_msg;

	// 		// 	while (i < argv_size)
	// 		// 	{
	// 		// 		tmp_msg += argv[i];
	// 		// 		tmp_msg += " ";
	// 		// 		i++;
	// 		// 	}
	// 		// 	remove_user_from_channel(sender, expectsString, tmp_msg);
	// 		// }
	// 	}
	// }
}

/****************************************
			TOPIC COMMAND
*****************************************/

void	IRC_Server::exec_cmd_TOPIC(Client & sender, std::vector<std::string> const & argv)
{
	size_t argv_size = argv.size();
	
	if (argv_size < 2)
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");

	t_Channel_Map::iterator	target_channel = get_channel_by_name(argv[1]);	
	if (target_channel == _channels.end())
		send_err_NOSUCHCHANNEL(sender, argv[1], "No such channel");	
	else if (target_channel->second.isChannelOperator(sender) == false && argv_size > 2)								//sender lacks needed permissions
		send_err_ERR_CHANOPRIVSNEEDED(sender, target_channel->second, "You're not a channel operator");
	else if(argv_size == 2)
		send_rpl_TOPIC(sender, target_channel->first, target_channel->second.getTopic());
	else
	{
		std::string	new_topic;

		for(size_t i = 2; i < argv_size; i++)
			new_topic += argv[i];
		target_channel->second.setTopic(new_topic);
		send_rpl_TOPIC(sender, target_channel->first, target_channel->second.getTopic());
	}
}

/****************************************
			NAMES COMMAND
*****************************************/


/*

	Con names, cualquier usuario puede ver los usuarios conectados a un canal siempre que no esten en modo invisible (+i)

*/


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
	size_t 	argc = argv.size();
	if (argc < 2)
		send_err_NEEDMOREPARAMS(sender, "MODE", "Not enough parameters");
	else
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
					if (sender.get_joined_channel(channel).second == true || chan_it->second.getModes().find('i') == std::string::npos)
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
	size_t 	argc = argv.size();

	if (argc == 1)
	{
		send_rpl_LISTSTART(sender);
		for (IRC_Server::t_Channel_Map::iterator i = _channels.begin();	i != _channels.end(); i++)
		{
				if (sender.get_joined_channel(i->second.getChannelName()).second == true || i->second.getModes().find('i') == std::string::npos)
					send_rpl_LIST(sender, i->first);
		}
		send_rpl_LISTEND(sender);
	}
	else if (argc == 2)						
	{
			std::stringstream	raw_channel_list(argv[1]);
			send_rpl_LISTSTART(sender);
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
					send_err_NOSUCHCHANNEL(sender, channel, "No such channel");	
				}
				else if ((chan_it = _channels.find(channel)) != _channels.end())
				{
					if (sender.get_joined_channel(channel).second == true || chan_it->second.getModes().find('i') == std::string::npos)
						send_rpl_LIST(sender, chan_it->first);
				}
				else
					send_err_NOSUCHCHANNEL(sender, channel, "No such channel");	
			}
			while (raw_channel_list.eof() == false);
			send_rpl_LISTEND(sender);
	}
	else
		send_err_UNKNOWNERROR(sender, argv[0], "To many arguments");
}

/****************************************
			INVITE COMMAND
*****************************************/

void	IRC_Server::exec_cmd_INVITE(Client & sender, std::vector<std::string> const & argv)
{
	size_t argc = argv.size();

	if (argc < 3)
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	t_Channel_Map::iterator	target_channel = get_channel_by_name(argv[1]);	
	if (target_channel == _channels.end())
		send_err_NOSUCHCHANNEL(sender, argv[2], "No such channel");	
	else if (sender.get_joined_channel(argv[2]).second == false)									//sender is not on affected channel
		send_err_NOTONCHANNEL(sender, target_channel->second, "You're not on that channel");
	else if (target_channel->second.isChannelOperator(sender) == false)								//sender lacks needed permissions
		send_err_ERR_CHANOPRIVSNEEDED(sender, target_channel->second, "You're not a channel operator");
	else
	{
		std::stringstream	raw_target_list(preprocess_list_param(const_cast<std::string &>(argv[1]), ','));
		do
		{
			std::string				target_nick;
			Channel &				channel = target_channel->second;
			Client *				target;

			std::getline(raw_target_list, target_nick, ',');

			if (raw_target_list.fail() == true)												//weird getline error
				send_err_UNKNOWNERROR(sender, argv[0], "Invalid target passed to std::getline()");
			else if ((target = find_client_by_nick(target_nick)) == NULL)					//target user does not exist
				send_err_UNKNOWNERROR(sender, argv[0], "Target user does not exist");
			else if (target->get_joined_channel(channel.getChannelName()).second == false)	//target user is not on channel
				send_err_USERNOTINCHANNEL(sender, *target, channel, "They aren't on that channel");
			else if (sender.get_nick() != channel.getOwner() && channel.isChannelOperator(*target))	//only channel owner can kick other operators
				send_err_UNKNOWNERROR(sender, argv[0], "Target user is also a Channel Operator");
			else
			{
				target->leave_channel(channel.getChannelName());
				send_rpl_JOIN(channel, sender);
				send_rpl_NAMREPLY(sender, channel);
				send_rpl_INVITED(sender, target->get_hostname(), target->get_nick(), channel);
			}
		}
		while (raw_target_list.eof() == false);
	}
}

/****************************************
			KICK COMMAND
*****************************************/

void	IRC_Server::exec_cmd_KICK(Client & sender, std::vector<std::string> const & argv)
{
	size_t argc = argv.size();

	if (argc < 3)
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");

	t_Channel_Map::iterator	target_channel = get_channel_by_name(argv[1]);	
	if (target_channel == _channels.end())
		send_err_NOSUCHCHANNEL(sender, argv[1], "No such channel");	
	else if (sender.get_joined_channel(argv[1]).second == false)									//sender is not on affected channel
		send_err_NOTONCHANNEL(sender, target_channel->second, "You're not on that channel");
	else if (target_channel->second.isChannelOperator(sender) == false)								//sender lacks needed permissions
		send_err_ERR_CHANOPRIVSNEEDED(sender, target_channel->second, "You're not a channel operator");
	else
	{
		std::stringstream	raw_target_list(preprocess_list_param(const_cast<std::string &>(argv[2]), ','));
		do
		{
			std::string				target_nick;
			Channel &				channel = target_channel->second;
			Client *				target;

			std::getline(raw_target_list, target_nick, ',');

			if (raw_target_list.fail() == true)												//weird getline error
				send_err_UNKNOWNERROR(sender, argv[0], "Invalid target passed to std::getline()");
			else if ((target = find_client_by_nick(target_nick)) == NULL)					//target user does not exist
				send_err_UNKNOWNERROR(sender, argv[0], "Target user does not exist");
			else if (target->get_joined_channel(channel.getChannelName()).second == false)	//target user is not on channel
				send_err_USERNOTINCHANNEL(sender, *target, channel, "They aren't on that channel");
			else if (sender.get_nick() != channel.getOwner() && channel.isChannelOperator(*target))	//only channel owner can kick other operators
				send_err_UNKNOWNERROR(sender, argv[0], "Target user is also a Channel Operator");
			else
			{
				target->leave_channel(channel.getChannelName());
				send_rpl_KICK(sender, *target, channel, (argc > 3 ? argv[3] : std::string("Kicked from the channel")));
			}
		}
		while (raw_target_list.eof() == false);
	}

	//	Comprobacion de que el usuario pertenece al canal del que busca eliminar a alguien. Error: ERR_NOTONCHANNEL

	// Comprobacion de si el usuario tiene permisos suficientes para ejecutar el kick. Error: ERR_CHANOPRIVSNEEDED

	// Comprobacion de si el usuario usuario buscado realmente existe en el canal. Error: ERR_USERNOTINCHANNEL

	// ¡Posibilidad!	¿Tenemos que comprobar si un ops/hops esta intentando eliminar un ops/hops de mayor o igual rango?

	// Eliminamos al usuario. De tener mas de tres argumentos depuramos el mensaje al igual que hacemos en TOPIC, con la diferencia de que
	//	mandamos mensaje necesario. De no definir mensaje, usamos mensaje generico.
}

/*!
** @brief	Validates a modestring received by exec_cmd_MODE().
**
** @details	A modestring is valid if it has at least one character before any
**			trailing space.
** @param	modestring	The modestring to validate.
** @return	the position of the first valid character in the modestring, or
**			std::string::npos if none was found
*/
static bool	validate_modestring(std::string const & modestring)
{
	size_t	end_modes = modestring.find_first_of(' ');
	//there is a space and a non-space before the space, orthere is no space but there as at least one non-space character
	return ((end_modes != std::string::npos && end_modes > 0) ||(end_modes == std::string::npos && modestring.size() > 0));
}

/*!
**	@brief		Validates a mode change request in the MODE command. For mode
**				changes that require an argument, the argument is returned in
**				the arg parameter and the next_arg iterator is updated for the
**				caller. This is meant to be called for each mode in modestring.
**
**	@details	This is an auxiliary function for exec_cmd_MODE(). It determines
**				whether a given mode change request is or is not valid. If valid
**				then true is returned, otherwise false is returned.
**
**				For valid mode changes that require an argument, the argument is
**				stored in the @a arg string passed by reference. For valid mode
**				changes with no argument, an the @a arg string is left empty.
**
**				Mode change rules are as follows:
**				- Type A MAY have an argument or not. If an argument is
**				available, it is assumed to have that argument. Type A changes
**				are always valid.
**				- Type B MUST have an argument. If an argument is unavailable,
**				the change is invalid.
**				- Type C MUST NOT have an argument if the mode is being unset
**				and MUST have an argument if the mode is being set. If being
**				set and no argument is available, the change is invalid.
**				Type C unsets are always valid, as any arguments are ignored.
**				- Type D MUST NOT have an argument. Type D changes are always
**				valid, as any arguments are ignored.
**
**				Of course, the proposed @a mode MUST also be a supported channel
**				mode.
**	@param		mode		Proposed mode.
**	@param		sign		'+' for set or '-' for unset (proposed change).
**	@param		next_arg	An iterator to the start of the next argument
**							substring in the arguments string.
**	@param		end_args	An iterator to the end of the arguments string.
**	@param		arg			A reference to a writable string where the argument
**							substring will be stored.
**	@return		true if the mode change is validated, otherwise false.
*/
static bool	validateChanModeChange(char mode, char sign, std::string::const_iterator & next_arg, std::string::const_iterator const & end_args, std::string & arg)
{
	if (std::strchr(SUPPORTED_CHANNEL_MODES, mode) == NULL)
		return false;

	char	type = get_mode_type(mode);
	bool	has_arg = (next_arg != end_args);

	if ((type == 'D') || (type == 'C' && sign == '-'))		//Type D MUST NEVER have arg and Type C MUST NOT have arg when being unset, so we ignore any args in those cases and validate
	{
		arg.clear();
		return true;
	}
	if ((type == 'B' || type == 'C') && has_arg == false)	//Type C MUST have arg when being set and Type B MUST ALWAYS have arg, so change is invalid for these types if there is no arg
		return false;
	//Type A may either have arg or not; if there is an arg, we get it

	std::string::const_iterator arg_end = next_arg;
	while (arg_end != end_args && *arg_end != ' ')
		++arg_end;
	// if (next_arg != arg_end)								//get arg, if there is one, or empty arg otherwise
	// 	arg.assign(next_arg, arg_end);
	// else
	// 	arg.clear();
	arg.assign(next_arg, arg_end);	
	while (arg_end != end_args && *arg_end == ' ')			//find next arg or end of args
		++arg_end;
	next_arg = arg_end;										//update next_arg for caller
	return true;
}

// /*!
// **	@brief	Sets or unsets all modes in @a modes parameter if they were not
// **			already set or unset, respectively, and indicates which changes were
// **			applied in the @a applied_changes string. Modes requiring an
// **			argument to be set/unset will only be set/unset if an argument is
// **			available and valid. 
// **
// ** @param	modestring		Modes to set in the channel, and their arguments, if
// **							any, in format [['sign''modes'...] ['arg'...]].
// ** @param	applied_changes	An empty writable string where applied changes with
// **							their arguments will be returned.
// ** @return	false if any modes could not be set, otherwise true
// */
// bool	IRC_Server::execModeChanges(Channel & channel, std::string const & modestring, std::string & applied_changes)
// {
// 	std::string	applied_args;
// 	size_t		start_pos = modestring.find_first_of("+-");
// 	size_t		first_arg_pos;
// 	size_t		del;
// 	char		sign;
// 	bool		ret;

// 	if (start_pos == std::string::npos)
// 		return false;
// 	first_arg_pos = modestring.find_first_of(' ', start_pos);
// 	std::string::const_iterator next_arg = (first_arg_pos == std::string::npos ? modestring.end() : modestring.begin() + ++first_arg_pos);
// 	std::string::const_iterator end_args = modestring.end();
// 	ret = true;
// 	for (std::string::const_iterator it = modestring.begin() + start_pos, end = modestring.end(); it != end; ++it)
// 	{
// 		if (std::strchr("+-", *it) != NULL)								//set sign
// 		{
// 			sign = *it;
// 			applied_changes.push_back(*it);
// 		}
// 		else if (std::strchr(SUPPORTED_CHANNEL_MODES, *it) != NULL)		//mode is known
// 		{
// 			std::string	arg;
// 			if (sign == '+'
// 				&&	validateModeChange(get_mode_type(*it), sign, next_arg, end_args, arg) == true) //set requested and mode not already set and mode change validated
// 				{
// 					channel.setMode(*it); //try to set mode
// 					//add to banlist, etc.
// 					//add applied arg if applicable
// 				}
// 			else if (sign == '-'
// 					&& validateModeChange(get_mode_type(*it), sign, next_arg, end_args, arg) == true)	//unset requested and mode not already unset and mode change validated
// 				_modes.erase(del, 1);					//unset mode //debug // duda, si quitas ban mode sin arg, el banlist se borra o se guarda???
// 			applied_changes.push_back(*it);
// 		}
// 		else															//mode is unknown
// 			ret = false;
// 	}
// 	return (ret);
// }

// static void	getChanModeIterators(std::string::iterator & first_mode, std::string::iterator & end_modes, std::string::iterator & first_arg, std::string::iterator & end_args)
// {
	
// }


static bool	validateMask(std::string const & mask_candidate)
{
	std::string::const_iterator	begin = mask_candidate.begin();
	std::string::const_iterator	end = mask_candidate.end();
	size_t						excl_pos;
	size_t						at_pos;

	if (std::count(begin, end, '!') == 1 && std::count(begin, end, '@') == 1							//One '!' and one '@'
		&& (excl_pos = mask_candidate.find_first_of('!')) < (at_pos = mask_candidate.find_first_of('@'))//'!' comes before '@'
		&& excl_pos > 0																					//Any character comes before '!'
		&& mask_candidate.find_first_not_of('!', excl_pos) < at_pos										//Any character(s) separate '!' and '@'
		&& mask_candidate.find_first_not_of('@', at_pos) < std::string::npos)							//Any character(s) come after '@'
			return true;
	return false;
}

bool	IRC_Server::doChanModeChange(char sign, char mode, std::string const & arg, Client const & recipient, Channel & channel)
{
	if (std::strchr("+-", sign) == NULL || std::strchr(SUPPORTED_CHANNEL_MODES, mode) == NULL)
		return false;

	char	type = get_mode_type(mode);
	bool	ret = false;
	if (type == 'A')
	{
		if (arg.size() == 0)	//type A with no arg == return list// these are consultations, not changes, so all should return false
		{
			if (mode == 'b')	//return ban list
				send_rpl_BANLIST(recipient, channel); //wheeee returning the ban list
			//else if...		//hypothetical future type A modes without arg here (who the hell am I kidding? xD)
			//ret = false; //según Oráculo de IRC rnavarre, consulta no cuenta como change, por lo tanto return false
		}
		else if (mode == 'b' && validateMask(arg) == true) //apply ban
			ret = sign == '+' ? channel.banMask(arg) : channel.unbanMask(arg);
		//else if //hypothetical future type A modes with arg here (not if I can help it)
		// else
		// 	ret = false;
	}
	else if (type == 'B' && arg.size() > 0)
	{
		if (mode == 'k')	//apply key mode
			ret = sign == '+' ? channel.setKey(recipient, arg) : channel.unsetKey(recipient, arg);		
	}
	else if (type == 'D')
		if (mode == 'i')	//apply invite-only mode
			ret = sign == '+' ? channel.setMode(mode) : channel.unsetMode(mode);
	//no type C modes supported
	return ret;
}

/*!
** @brief	Executes a MODE command originating from @a sender.
**
** @details	The expected argv for this command is:
**
**			MODE <target> [<modestring> [<mode arguments>...]]
**
**			The command has two behaviours, depending on whether <target> is a
**			user or a channel.
**
**			If <modestring> is omitted, the command replies with <target>'s
**			current active modes.
**
**			If <modestring> contains no sign ('+' or '-'), or only contains
**			contains signs, it will be interpreted as no modestring given.
** @param	sender	A reference to the client who sent the command.
** @param	argv	A reference to the message containing the command (argv[0])
**					and its arguments (argv[1,...]) in a string vector.
*/
void	IRC_Server::exec_cmd_MODE(Client & sender, std::vector<std::string> const & argv)
{
	size_t 	argc = argv.size();
	if (argc < 2)
		send_err_NEEDMOREPARAMS(sender, "MODE", "Not enough parameters");
	size_t		hash_pos = argv[1].find_first_of("#");
	if (hash_pos == std::string::npos) 					//it's a user (all supported user modes are type D, so no args expected)
	{
		Client *						target = find_client_by_nick(argv[1]);

		if (target == NULL)																//user does not exist
			send_err_NOSUCHNICK(sender, argv[1], "No such nick");
		else if (case_insensitive_ascii_compare(sender.get_nick(), argv[1]) == false)	//sender is not the same as target
			send_err_USERSDONTMATCH(sender, "Can't change mode for other users");
		else if (argc < 3 ||
				(validate_modestring(argv[2])) == false)	//no modestring given; si fuera c++11 haría un lambda
			send_rpl_UMODEIS(sender);
		else		//debug //shouldn't o be privileged????? :p pregunta a raul
		{
			std::string	applied_changes;
			bool		all_modes_supported;

			all_modes_supported = target->set_modes(argv[2], applied_changes);
			send_rpl_MODE(sender, applied_changes);
			if (all_modes_supported == false)
				send_err_UMODEUNKNOWNFLAG(sender, "Unknown mode flag");
		}			
	}
	else												//it's a channel
	{
		t_Channel_Map::iterator target = get_channel_by_name(argv[1]);

		if (target == _channels.end())													//channel does not exist
			send_err_NOSUCHCHANNEL(sender, argv[1], "No such channel");
		// else if (argc < 3 ||
		// 		(first_sign_pos = argv[2].find_first_of("+-")) == std::string::npos ||
		// 		argv[2].find_first_not_of("+-", first_sign_pos) == std::string::npos)	//no modestring given; si fuera c++11 haría un lambda
		else if (argc < 3 ||
				(validate_modestring(argv[2])) == false)	//no modestring given; si fuera c++11 haría un lambda
			send_rpl_CHANNELMODEIS(sender, target->second);
		else if (target->second.isChannelOperator(sender) == false)						//sender is not channel operator
			send_err_ERR_CHANOPRIVSNEEDED(sender, target->second, "You're not a channel operator");
		else
		{
			//ok... fork it, i'm going with a pair... tired of this bs...
			std::pair<std::string, std::string>	modesandargs;
			size_t	end_modes_pos = argv[2].find_first_of(' ');
			size_t	start_args_pos = argv[2].size();
			size_t	tmp;	//in c++17 this would not have to exist because conditional declarations :p
			if (end_modes_pos == std::string::npos)
				end_modes_pos = argv[2].size();
			else if ((tmp = argv[2].find_first_not_of(' ', end_modes_pos)) != std::string::npos)
				start_args_pos = tmp;
			
			std::string arg;
			
			char	sign = '+';
			//for each mode in modestring
			for (std::string::const_iterator mode_it = argv[2].begin(),
				modes_end = argv[2].begin() + end_modes_pos,
				next_arg = argv[2].begin() + start_args_pos; mode_it != modes_end; ++mode_it)
			{
				if (*mode_it == '+' || *mode_it == '-')
				{
					sign = *mode_it;
					modesandargs.first += sign;
				}
				else if (	validateChanModeChange(*mode_it, sign, next_arg, argv[2].end(), arg) == true
						&&	doChanModeChange(sign, *mode_it, arg, sender, target->second) == true)
					modesandargs.first += *mode_it, modesandargs.second += arg;
			}
			if (modesandargs.first.size() == 1 && std::strchr("+-", modesandargs.first[0]) != NULL)
				modesandargs.first.clear();
			else if (modesandargs.first.size() > 0 && std::strchr("+-", modesandargs.first[0]) == NULL) //debug //do no funciona con -b y cuando no hay cambios queda un signo sin nada al principio
				modesandargs.first.insert(modesandargs.first.begin(), '+');
			std::string applied_changes = modesandargs.first + " ";
			applied_changes += modesandargs.second;
			send_rpl_MODE(sender, target->second, applied_changes);

			
			//for each mode in modestring
			//	get set/unset flag
			//	get mode type
			//	get mode argument as applicable
			//	react as appropriate
			//mode does not exist -> modeunknown ???
			//type b or c mode exists but lacks required argument -> ignore
			//type a mode exists but lacks argument -> send mode list to user
		}
		
		
	}
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
	
	//debug
	for (std::vector<std::string>::iterator it = argv.begin(), end = argv.end(); it != end; ++it)
		std::cerr << *it << " ";
	std::cerr << std::endl;
	//debug

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
		else if (cmd == "MOTD")
			exec_cmd_MOTD(client, argv);
		else if (cmd == "NOTICE")
			exec_cmd_NOTICE(client, argv);
		else if (cmd == "KICK")
			exec_cmd_KICK(client, argv);
		else if (cmd == "MODE")
			exec_cmd_MODE(client, argv);
		else
			send_err_UNKNOWNCOMMAND(client, cmd, "Unknown command");
	}
}
