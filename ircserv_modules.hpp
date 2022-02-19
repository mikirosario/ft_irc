/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_modules.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:38:32 by miki              #+#    #+#             */
/*   Updated: 2022/02/19 19:44:35 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//This header contains private member functions to the IRC_Server class relating
//to client command parsing and interpreting.

#ifndef IRCMODULES_H
# define IRCMODULES_H

//parsing
bool	nick_is_valid(std::string const & str) const;

//interpreting
void	exec_cmd_PASS(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_NICK(Client & sender, std::vector<std::string> const & argv);
bool	register_client(int fd, std::string const & msg);
void	interpret_msg(Client & client);

//error replies
std::string	err_reply_start(Client const & client, char const * numeric) const;
void		err_reply_end(std::string & message, std::string const & description) const;
void		send_err_UNKNOWNCOMMAND(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_ALREADYREGISTERED(Client const & recipient, std::string const & description) const;
void		send_err_NEEDMOREPARAMS(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_NONICKNAMEGIVEN(Client const & recipient, std::string const & description) const;
void		send_err_NICKNAMEINUSE(Client const & recipient, std::string const & nick, std::string const & description) const;
void		send_err_ERRONEOUSNICKNAME(Client const & recipient, std::string const & nick, std::string const & description) const;

#endif
