/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_modules.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:43:06 by miki              #+#    #+#             */
/*   Updated: 2022/02/16 17:16:33 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include <vector>
// //utils


//get_cmd_params? maybe return a list?
/*!
** @brief	Extract command from message containing command.
**
** @param	msg Client message.
** @return	A substring containing the command.
*/
// static std::string	get_cmd(std::string const & msg)
// {
// 	int	end_pos = msg.find_first_of(' ');
// 	return(msg.substr(0, end_pos));
// }

//parsing
/*!
** @brief	Returns number of parameters in a messsage.
**
** @details	All parameters are preceded by SPACE. A parameter preceded by SPACE
**			and COLON is the last parameter, and all subsequent spaces are
**			interpreted as part of the parameter proper.
** @param	msg Client message.
** @return	The number of parameters in the message.
*/
size_t	IRC_Server::get_param_count(std::string const & msg)
{
	size_t	end_pos;
	size_t	p_count = 0;

	end_pos = msg.find(" :");
	if (end_pos != std::string::npos) //found " :"
		end_pos = end_pos + 1;
	else
		end_pos = msg.find_first_of("\r\n");
	size_t i = msg.find_first_of(" \r\n");
	while (i < end_pos)
	{
		i = msg.find_first_not_of(' ', i);
		i = msg.find_first_of(" :\r\n", i);
		++p_count;
	}
	return (p_count);
}

// /*!
// ** @brief	Determines whether or not a character marks the end of a message.
// **
// ** @details	The standard requires all lines to terminate in '\r\n', but we will be
// **			fault-tolerant and accept either '\r' or '\n' as the end of a line.
// ** @param	c A character.
// ** @return	true if end of line, otherwise false.
// **/
// static bool	is_endmsg(char const c)
// {
// 	return (c == '\r' || c == '\n');
// }


/*! @brief	Returns a vector containing all parameters in @a msg WITHOUT crlf.
**			Make sure to add crlf back in to any messages being echoed!
**
** @param	msg A reference to the message to parse.
** @return	A vector of strings containing the message parameters in the same
**			order as in the message.
*/
std::vector<std::string>	IRC_Server::get_params(std::string const & msg)
{
	std::vector<std::string>	ret;
	size_t	start_pos = 0;
	size_t	end_pos;

	start_pos = msg.find_first_of(" \r\n", start_pos);				//get first space or endline
	start_pos = msg.find_first_not_of(' ', start_pos);				//tolerate leading spaces
	while (msg[start_pos] != '\r' && msg[start_pos] != '\n')		//NOTHING not crlf terminated should get this far, if so fix at source!
	{	
		if (msg[start_pos] == ':')									//last param colon case
		{
			++start_pos;
			end_pos = msg.find_last_not_of("\r\n") + 1;				//strip crlf from last parameter
		}
		else														//general param case (if starting pos >= msg.size(), npos is returned, but this should NOT happen here as everything MUST be cr or lf terminated)
			end_pos = msg.find_first_of(" \r\n", start_pos);		//strip crlf from last parameter
		ret.push_back(msg.substr(start_pos, end_pos - start_pos));	//add parameter to vector; 0 bytes == empty string
		start_pos = msg.find_first_not_of(' ', end_pos);			//tolerate trailing spaces
	}
	return (ret);
}

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
