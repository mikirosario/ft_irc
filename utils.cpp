/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 18:56:09 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/04 20:45:04 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

/*! @brief Prints an error message to standard error.
**
** @param msg The error message.
*/
void	print_err(std::string const & msg)
{
	std::cerr << msg << std::endl;
}

/*! @brief	Attempts to close an open connection. Prints message to standard
**			error in case of failure.
**
** @param fd Socket file descriptor to close.
*/
void	close_connection(int const fd)
{
	if (close(fd) == -1)
		std::cerr << "Close connection " << fd << " failed." << std::endl;
}
