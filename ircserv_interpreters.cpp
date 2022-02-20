/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_interpreters.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:43:06 by miki              #+#    #+#             */
/*   Updated: 2022/02/20 08:23:13 by miki             ###   ########.fr       */
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

// bool	IRC_Server::register_client(int fd, std::string const & msg)
// {
// 	//parse message for PASS command
// 	//if no PASS command, do nothing
// 	//if NICK command, register NICK and attempt to confirm password

// 	return(true);
// }

/* ---- INTERPRETING ---- */

/*!
** @brief	Executes a PASS command originating from @a sender.
**
** @details	Attempts to retrieve a password sent by @a sender in the @a argv
**			argument vector as the first parameter, for use in registration.
**			If there is no first parameter, an ERR_NEEDMOREPARAMS error reply is
**			returned to @a sender. If @a sender is already registered, an
**			ERR_ALREADYREGISTERED error reply is returned to @a sender. If both
**			errors are simultaneously present, only the ERR_ALREADYREGISTERED
**			error reply is returned to @a sender.
** @param	sender	A reference to the client who who sent the command.
** @param	argv	A reference to the message containing the command (argv[0])
**					and its arguments (argv[...]) in a string vector.
*/
void	IRC_Server::exec_cmd_PASS(Client & sender, std::vector<std::string> const & argv)
{
	if (sender.is_registered() == true)
		send_err_ALREADYREGISTERED(sender, "You may not reregister");
	else if (argv.size() < 2) //Only command argument exists
		send_err_NEEDMOREPARAMS(sender, argv[0], "Not enough parameters");
	else
		sender.set_pass(argv[1]);
}

/*!
** @brief	Executes a NICK command originating from @a sender.
**
** @details	Attempts to retrieve a nickname sent by @a sender in the @a argv
**			argument vector as the first parameter, for use in registration or
**			to change an existing nickname. If there is no first parameter, an
**			ERR_NONICKNAMEGIVEN error reply is returned to @a sender. If the
**			nickname is invalid, an ERR_ERRONEOUSNICKNAME error reply is
**			returned to @a sender. If the nickname is already in use on the
**			server as determined by a case-insensitive lookup, an
**			ERR_NICKNAMEINUSE is returned to @a sender. Otherwise, the new
**			sender's nick is set to the new nickname.
**
**			If @a sender is already registered and the new nickname is accepted,
**			a confirmation reply is	returned to @a sender.
*/
void	IRC_Server::exec_cmd_NICK(Client & sender, std::vector<std::string> const & argv)
{
	if (argv.size() < 2)
		send_err_NONICKNAMEGIVEN(sender, "No nickname given");
	else if (nick_is_valid(argv[1]) == false)
		send_err_ERRONEOUSNICKNAME(sender, argv[1], "Erroneous nickname");
	else if (find_client_by_nick(argv[1]) != NULL)
		send_err_NICKNAMEINUSE(sender, argv[1], "Nickname is already in use");
	else
	{
		// if (sender.is_registered() == true) //if client is already registered, this is a nickname change, we send a reply as confirmation
		// //The NICK message may be sent from the server to clients to acknowledge their NICK command was successful,
		// //and to inform other clients about the change of nickname. In these cases, the <source> of the message will
		// //be the old nickname [ [ "!" user ] "@" host ] of the user who is changing their nickname.
		sender.set_nick(argv[1]);
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
	else
		send_err_UNKNOWNCOMMAND(client, cmd, "Unknown command");
}
