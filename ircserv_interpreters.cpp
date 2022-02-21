/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_interpreters.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:43:06 by miki              #+#    #+#             */
/*   Updated: 2022/02/21 20:42:48 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

/* ---- PARSING ---- */

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
**					and its arguments (argv[...]) in a string vector.
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
**					and its arguments (argv[...]) in a string vector.
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
	if (cmd == "BAILA") //debug //this was just a test case, might leave it in as an easter egg though ;)
		std::cout << "El servidor baila el chotis" << std::endl;
	else if (cmd == "PASS")
		exec_cmd_PASS(client, argv);
	else if (cmd == "NICK")
		exec_cmd_NICK(client, argv);
	else if (cmd == "USER")
		exec_cmd_USER(client, argv);
	else
		send_err_UNKNOWNCOMMAND(client, cmd, "Unknown command");
}
