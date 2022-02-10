/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/10 03:18:04 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/10 06:39:10 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

IRC_Server::IRC_Server(void) {}

IRC_Server::IRC_Server(std::string const & arg)
{
	init(arg);
}

IRC_Server::~IRC_Server(void) {}

IRC_Server &	IRC_Server::operator=(std::string const & arg)
{
	this->init(arg);
	return (*this);
}

/**
 * @brief	Parses the program argument for network info and copies it to the
 * 			IRC_Server network info variables if it exists.
 * 
 * @details	A string is considered to have network info if its first character
 * 			is an opening bracket '[' and it contains a closing bracket ']' and
 * 			there are two colons ':' between the brackets. The network host,
 *			port and password will be the substrings before, between and after
 *			the colons, respectively. Empty strings will be copied as empty.
 * @param arg	A reference to the string to parse.
 * @return		If network info has been collected, true is returned, otherwise
 * 				false is returned.
*/
bool	IRC_Server::get_network_info(std::string const & arg)
{
	std::string	*				netinfo[3] = {&_nethost, &_netport, &_netpass};
	std::string::const_iterator	end = arg.end(),
								network_info_begin = arg.begin(),
								network_info_end = std::find(network_info_begin, end, ']'),
								token_begin,
								token_end;

	//does network info exist?
	if	(*network_info_begin == '[' && network_info_end != end									//first character is '[' and there is a ']'
		&& std::count(network_info_begin, network_info_end, ':') == 2)							//there are 2 ':' betweeen '[' and ']'
	{
		int i = 0;
		token_begin = network_info_begin + 1;
		do
		{
			token_end = std::find(token_begin, network_info_end, ':');
			(*netinfo[i++]).assign(token_begin, token_end);
			token_begin = token_end + 1;
		}
		while (token_end != network_info_end);
		
		//debug
		for (int x = 0; x < 3; ++x)
			std::cout << *netinfo[x] << std::endl;
		//debug
		
		return (true);
	}
	return (false);	
}

void	IRC_Server::init(std::string const & arg) throw(BadArgumentException)
{
	//is there an argument to parse?
	if (arg.size() == 0)
	{
		throw BadArgumentException();
		return ;
	}
	//skip white spaces
	
	if (get_network_info(arg))
	{
		std::cout << "Has network info"	<< std::endl;
	}
}
