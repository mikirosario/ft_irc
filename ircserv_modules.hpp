/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_modules.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:38:32 by miki              #+#    #+#             */
/*   Updated: 2022/02/16 16:26:41 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//This header contains private member functions to the IRC_Server class relating
//to client command parsing and interpreting.

#ifndef IRCMODULES_H
# define IRCMODULES_H

typedef std::string::const_iterator	string_it;

//parsing
std::vector<std::string>	get_params(std::string const & msg);
size_t						get_param_count(std::string const & msg);
bool						is_cmd_PASS(std::string const & cmd);
bool						is_cmd_NICK(std::string const & cmd);

//interpreting
bool	exec_cmd_NICK(void);
bool	register_client(int fd, std::string const & msg);

#endif
