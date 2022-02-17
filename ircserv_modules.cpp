/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_modules.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:43:06 by miki              #+#    #+#             */
/*   Updated: 2022/02/17 09:18:34 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include <vector>

//parsing
/*!
** @brief	Determines whether a command is the NICK command.
**
** @param msg A reference to the command to check.
** @return true if the command is NICK, otherwise false.
*/
bool	IRC_Server::is_cmd_PASS(std::string const & cmd)
{
	return (cmd.compare("NICK"));
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
