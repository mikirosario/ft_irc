/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 22:02:27 by miki              #+#    #+#             */
/*   Updated: 2022/02/11 23:14:30 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

IRC_Server::Client::Client(void)
{}

IRC_Server::Client::Client(std::string const & user_info)
{
	(void)user_info;
	//search User set for existing profile; if exists, save address to User profile, or iterator, or whatever, if not create one (maybe)
}

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
