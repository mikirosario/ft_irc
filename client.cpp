/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 22:02:27 by miki              #+#    #+#             */
/*   Updated: 2022/02/12 14:40:46 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

IRC_Server::Client::Client(void) : _state(IRC_Server::Client::State(UNREGISTERED))
{}

IRC_Server::Client &	IRC_Server::Client::operator=(Client const & src)
{
	_nick = src._nick;
	_user_profile = src._user_profile;
	return (*this);
}

IRC_Server::Client::~Client(void)
{}

/*!
** @brief	Searches the registered user map of the @a server for the @a nick and
**			associates the Client instance with it if it exists.
*/
void	IRC_Server::Client::find_nick(std::string const & nick, IRC_Server & server)
{
	IRC_Server::Client::t_user_ptr user = server._reg_users.upper_bound(nick);

	if ((*user).first == nick)
		this->_user_profile = user;
	//always create new user profile?
	// if ((*user).first != nick)
	// 	user = server._reg_users.insert(user, std::make_pair(nick, User(nick)));
	// this->_user_profile = user;
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