/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_modules.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:38:32 by miki              #+#    #+#             */
/*   Updated: 2022/02/28 16:39:22 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//This header contains private member functions to the IRC_Server class relating
//to client command parsing and interpreting.

#ifndef IRCMODULES_H
# define IRCMODULES_H

//parsing
bool	nick_is_valid(std::string const & str) const;
bool	username_is_valid(std::string const & username) const;

//interpreting
void	exec_cmd_PASS(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_NICK(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_USER(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_PRIVMSG(Client & sender, std::vector<std::string> const & argv);

void	exec_cmd_JOIN(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_PART(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_TOPIC(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_NAMES(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_LIST(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_INVITE(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_KICK(Client & sender, std::vector<std::string> const & argv);



void	exec_cmd_PING(Client & sender, std::vector<std::string> const & argv);

bool	register_client(Client & client);
void	interpret_msg(Client & client);

//numeric replies
std::string	numeric_reply_start(Client const & client, char const * numeric) const;
void		numeric_reply_end(std::string & reply, std::string const & description) const;
	//standard numeric replies

//replies to command
void		send_rpl_WELCOME(Client const & recipient);
void		send_rpl_YOURHOST(Client const & recipient);
void		send_rpl_CREATED(Client const & recipient);
void		send_rpl_MYINFO(Client const & recipient);
void		send_rpl_ISUPPORT(Client const & recipient);

void		send_rpl_TOPIC(Client const & recipient, std::string const & channelName, std::string const & channelTopic );

// Esto no lo considero neceserio

/*
void		send_rpl_topicWhoTime(Client const & recipient, std::string const & command, std::string const & description)
void		send_rpl_namReply(Client const & recipient, std::string const & command, std::string const & description)
void		send_rpl_endOfNames(Client const & recipient, std::string const & command, std::string const & description)
*/

	//error numeric replies

void		send_err_UNKNOWNERROR(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_UNKNOWNCOMMAND(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_ALREADYREGISTERED(Client const & recipient, std::string const & description) const;
void		send_err_NEEDMOREPARAMS(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_NONICKNAMEGIVEN(Client const & recipient, std::string const & description) const;
void		send_err_NICKNAMEINUSE(Client const & recipient, std::string const & nick, std::string const & description) const;
void		send_err_ERRONEOUSNICKNAME(Client const & recipient, std::string const & nick, std::string const & description) const;
void		send_err_PASSWDMISMATCH(Client const & recipient, std::string const & description) const;
void		send_err_INPUTTOOLONG(Client const & recipient, std::string const & description) const;
void		send_err_NOTREGISTERED(Client const & recipient, std::string const & description) const;
void		send_err_NOTEXTTOSEND(Client const & recipient, std::string const & description) const;
void		send_err_NORECIPIENT(Client const & recipient, std::string const & description) const;
void		send_err_NOSUCHNICK(Client const & recipient, std::string const & nick, std::string const & description) const;



	// Me quedo aqui hoy

// Join

void		send_err_NOSUCHCHANNEL(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_TOOMANYCHANNELS(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_BADCHANNELKEY(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_BANNEDFROMCHAN(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_CHANNELISFULL (Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_INVITEONLYCHAN(Client const & recipient, std::string const & command, std::string const & description) const;



//non-numeric replies
void		non_numeric_reply_end(std::string & reply, std::string const & last_param) const;
void		send_rpl_NICK(Client const & recipient, std::string const & old_source) const;
void		send_rpl_PRIVMSG(Client const & recipient, Client const & source, std::string const & message) const;

// Auxiliar methods

void	exec_join(IRC_Server::Client & sender, std::vector<std::string> const & argv);

#endif
