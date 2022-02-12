/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_modules.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:43:06 by miki              #+#    #+#             */
/*   Updated: 2022/02/12 14:42:21 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

// //utils
// static bool	is_endline(char const c)
// {
// 	return (c == '\r' || c == '\n');
// }

//get_cmd_params? maybe return a list?
/*!
** @brief	Extract command from message containing command.
**
** @param	msg Client message
** @return	A substring containing the command.
*/
// static std::string	get_cmd(std::string const & msg)
// {
// 	int	end_pos = msg.find_first_of(' ');
// 	return(msg.substr(0, end_pos));
// }

//parsing
/*!
** @brief	Determines whether a client message contains a command.
**
** @param msg client message
** @return	true if the message contains a command, otherwise false.
*/
bool	IRC_Server::is_cmd(std::string const & msg)
{
	return (msg[0] != ':');
}

/*!
** @brief	Determines whether a command is the NICK command.
**
** @param msg A reference to the command to check.
** @return true if the command is NICK, otherwise false.
*/
bool	IRC_Server::is_cmd_NICK(std::string const & cmd)
{
	return (cmd.compare("NICK"));
}

// bool	IRC_Server::register_client(int fd, std::string const & msg)
// {
// 	//parse message for PASS command
// 	//if no PASS command, do nothing
// 	//if NICK command, register NICK and attempt to confirm password

// 	return(true);
// }
