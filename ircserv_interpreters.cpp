/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_interpreters.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:43:06 by miki              #+#    #+#             */
/*   Updated: 2022/02/19 13:02:47 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include <vector>

//parsing

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
	else
		send_err_UNKNOWNCOMMAND(client, cmd, "Unknown command");
}
