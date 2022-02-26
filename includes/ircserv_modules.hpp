/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_modules.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:38:32 by miki              #+#    #+#             */
/*   Updated: 2022/02/26 15:46:38 by acortes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//This header contains private member functions to the IRC_Server class relating
//to client command parsing and interpreting.

#ifndef IRCMODULES_H
# define IRCMODULES_H

#include "ircserv.hpp"
#include "client.hpp"

//parsing
bool	nick_is_valid(std::string const & str);
bool	username_is_valid(std::string const & username);

//interpreting
void	exec_cmd_PASS(Client &sender, std::vector<std::string> const & argv);
void	exec_cmd_NICK(Client &sender, std::vector<std::string> const & argv);
void	exec_cmd_USER(Client &sender, std::vector<std::string> const & argv);
bool	register_client(Client &client);
void	interpret_msg(Client &client);

//numeric replies
std::string	numeric_reply_start(Client const & client, char const * numeric);
void		numeric_reply_end(std::string & reply, std::string const & description);
	//standard numeric replies
void		send_rpl_WELCOME(Client const & recipient);
void		send_rpl_YOURHOST(Client const & recipient);
void		send_rpl_CREATED(Client const & recipient);
void		send_rpl_MYINFO(Client const & recipient);
void		send_rpl_ISUPPORT(Client const & recipient);

	//error numeric replies
void		send_err_UNKNOWNERROR(Client const & recipient, std::string const & command, std::string const & description);
void		send_err_UNKNOWNCOMMAND(Client const & recipient, std::string const & command, std::string const & description);
void		send_err_ALREADYREGISTERED(Client const & recipient, std::string const & description);
void		send_err_NEEDMOREPARAMS(Client const & recipient, std::string const & command, std::string const & description);
void		send_err_NONICKNAMEGIVEN(Client const & recipient, std::string const & description);
void		send_err_NICKNAMEINUSE(Client const & recipient, std::string const & nick, std::string const & description);
void		send_err_ERRONEOUSNICKNAME(Client const & recipient, std::string const & nick, std::string const & description);
void		send_err_PASSWDMISMATCH(Client const & recipient, std::string const & description);
void		send_err_INPUTTOOLONG(Client const & recipient, std::string const & description);

//non-numeric replies
void		non_numeric_reply_end(std::string & reply, std::string const & last_param);
void		send_rpl_NICK(Client const & recipient, std::string const & old_source);

#endif
