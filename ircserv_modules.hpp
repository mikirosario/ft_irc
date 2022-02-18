/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_modules.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:38:32 by miki              #+#    #+#             */
/*   Updated: 2022/02/18 18:00:31 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//This header contains private member functions to the IRC_Server class relating
//to client command parsing and interpreting.

#ifndef IRCMODULES_H
# define IRCMODULES_H

//parsing
bool						is_cmd_PASS(std::string const & cmd);
bool						is_cmd_NICK(std::string const & cmd);

//interpreting
bool	exec_cmd_NICK(void);
bool	register_client(int fd, std::string const & msg);
void	exec_cmd(Client & client);

//error replies
std::string	err_reply_start(Client const & client, char const * numeric) const;
void		err_reply_end(std::string & message, std::string const & description) const;
void		send_err_UNKNOWNCOMMAND(Client const & client, std::string const & command, std::string const & description = std::string()) const;

#endif
