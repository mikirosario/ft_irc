/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_modules.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikiencolor <mikiencolor@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:38:32 by miki              #+#    #+#             */
/*   Updated: 2022/05/28 16:38:41 by mikiencolor      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//This header contains private member functions to the IRC_Server class relating
//to client command parsing and interpreting.

#ifndef IRCMODULES_H
# define IRCMODULES_H

//parsing
static bool				nick_is_valid(std::string const & str);
static bool				username_is_valid(std::string const & username);
//static bool				hostname_is_valid(std::string const & str); ??
static bool				channel_name_is_valid(std::string const & channel_name);
static std::string &	trim(std::string & str, std::string const & unwanted_chars);
static std::string &	remove_adjacent_duplicates(std::string & str, char c);
static std::string &	preprocess_list_param(std::string & str, char delimiter);


//interpreting
	//-miki
void	exec_cmd_BAILA(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_PASS(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_NICK(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_USER(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_PRIVMSG(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_PING(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_NAMES(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_JOIN(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_MOTD(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_NOTICE(Client & sender, std::vector<std::string> const & argv);
bool	doChanModeChange(char sign, char mode, std::string const & arg, Client & recipient, Channel & channel);
bool	register_client(Client & client);
void	interpret_msg(Client & client);
void	exec_cmd_PART(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_TOPIC(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_LIST(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_INVITE(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_KICK(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_OPER(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_MODE(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_QUIT(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_KILL(Client & sender, std::vector<std::string> const & argv);
bool	execModeChanges(Channel & channel, std::string const & modestring, std::string & applied_changes);

//numeric replies
std::string	numeric_reply_start(Client const & client, char const * numeric) const;
//std::string	numeric_reply_start(Channel const & recipient, char const * numeric) const;
void		numeric_reply_end(std::string & reply, std::string const & description) const;
	//standard numeric replies

//replies to command
void		send_rpl_WELCOME(Client & recipient);
void		send_rpl_YOURHOST(Client & recipient);
void		send_rpl_CREATED(Client & recipient);
void		send_rpl_MYINFO(Client & recipient);
void		send_rpl_ISUPPORT(Client & recipient);
void		send_rpl_UMODEIS(Client & recipient);

void		send_rpl_NAMREPLY(Client & recipient, Channel const & channel);
//void		send_rpl_NAMREPLY(Channel const & recipient, Channel const & channel);
void		send_rpl_ENDOFNAMES(Client & recipient, std::string const & channel_name);
//void		send_rpl_ENDOFNAMES(Channel const & recipient, std::string const & channelName);

void		send_rpl_LISTSTART(Client & recipient);
void		send_rpl_LIST(Client & recipient, std::string const &channel_name);
void		send_rpl_LISTEND(Client & recipient);

void		send_rpl_INVITED(Client & sender, Client const & target, Channel const & channel);
void		send_rpl_INVITING(Client & sender, Client const & target, Channel const & channel);
void		send_rpl_TOPIC(Client & recipient, Channel const & channel);
void		send_rpl_NOTOPIC(Client & recipient, Channel const & channel);
void		send_rpl_CHANNELMODEIS(Client & recipient, Channel const & channel);
void		send_rpl_ENDOFBANLIST(Client & recipient, Channel const & channel, std::string const & description);
void		send_rpl_BANLIST(Client & recipient, Channel const & channel);
void		send_rpl_YOUREOPER(Client & recipient, std::string const & description);
void		send_rpl_KILL(Client & killer, Client & killed, std::string const & description);
// Esto no lo considero neceserio

/*
void		send_rpl_topicWhoTime(Client const & recipient, std::string const & command, std::string const & description)
void		send_rpl_namReply(Client const & recipient, std::string const & command, std::string const & description)
*/

	//error numeric replies

void		send_err_UNKNOWNERROR(Client & recipient, std::string const & command, std::string const & description) const;
void		send_err_UNKNOWNCOMMAND(Client & recipient, std::string const & command, std::string const & description) const;
void		send_err_ALREADYREGISTERED(Client & recipient, std::string const & description) const;
void		send_err_NEEDMOREPARAMS(Client & recipient, std::string const & command, std::string const & description) const;
void		send_err_NONICKNAMEGIVEN(Client & recipient, std::string const & description) const;
void		send_err_NICKNAMEINUSE(Client & recipient, std::string const & nick, std::string const & description) const;
void		send_err_ERRONEOUSNICKNAME(Client & recipient, std::string const & nick, std::string const & description) const;
void		send_err_PASSWDMISMATCH(Client & recipient, std::string const & description) const;
void		send_err_INPUTTOOLONG(Client & recipient, std::string const & description) const;
void		send_err_NOTREGISTERED(Client & recipient, std::string const & description) const;
void		send_err_NOTEXTTOSEND(Client & recipient, std::string const & description) const;
void		send_err_NORECIPIENT(Client & recipient, std::string const & description) const;
void		send_err_NOSUCHNICK(Client & recipient, std::string const & nick, std::string const & description) const;
void		send_err_BADCHANMASK(Client & recipient, std::string const & channel_name, std::string const & description) const;
void		send_err_BADCHANNELKEY(Client & recipient, Channel const & channel, std::string const & description) const;
void		send_err_NOTONCHANNEL(Client & recipient, Channel const & channel, std::string const & description) const;
void		send_err_USERNOTINCHANNEL(Client & recipient, Client const & target, Channel const & channel, std::string const & description) const;
void		send_err_INVITEONLYCHAN(Client & recipient, std::string const &channel_name) const;
void		send_err_USERONCHANNEL(Client & recipient, std::string const & client_name, std::string const & client_nick, Channel const & channel) const;
void		send_err_NOSUCHSERVER(Client & recipient, std::string const & server_name, std::string const & description) const;
void		send_err_NOMOTD(Client & recipient, std::string const & description) const;
void		send_err_ERR_CHANOPRIVSNEEDED(Client & recipient, Channel const & channel, std::string const & description) const;
void		send_err_USERSDONTMATCH(Client & recipient, std::string const & description) const;
void		send_err_UMODEUNKNOWNFLAG(Client & recipient, std::string const & description) const;
void		send_err_NOPRIVILEGES(Client & recipient, std::string const & description) const;
	// Me quedo aqui hoy

// Join

void		send_err_NOSUCHCHANNEL(Client & recipient, std::string const & channel_nick, std::string const & description) const;
void		send_err_TOOMANYCHANNELS(Client & recipient, std::string const & command, std::string const & description) const;

void		send_err_BANNEDFROMCHAN(Client & recipient, Channel const & channel, std::string const & description) const;
void		send_err_CHANNELISFULL (Client & recipient, std::string const & command, std::string const & description) const;
void		send_err_INVITEONLYCHAN(Client & recipient, std::string const & command, std::string const & description) const;
void		send_err_ERR_UNKNOWNMODE(Client & recipient, char const &command, std::string const & description) const;



//non-numeric replies
void		non_numeric_reply_end(std::string & reply, std::string const & last_param) const;
void		send_rpl_NICK(Client & recipient, std::string const & old_source) const;
void		send_rpl_PRIVMSG(Client & recipient, Client const & source, std::string const & message) const;
void		send_rpl_PRIVMSG(Channel const & recipient, Client const & source, std::string const & privileges, std::string const & message) const;
void		send_rpl_NOTICE(Client & recipient, Client const & source, std::string const & message) const;
void		send_rpl_JOIN(Channel const & recipient, Client const & source) const;
void		send_rpl_PART(Client & recipient, Channel const & channel, std::string const & part_message) const;
void		send_rpl_KICK(Client const & kicker, Client & recipient, Channel const & channel, std::string const & kick_message) const;
void		send_rpl_PONG(Client & recipient, std::string const & token) const;
void		send_rpl_MODE(Client & recipient, std::string const & applied_changes) const;
void		send_rpl_MODE(Client & recipient, Channel const & channel, std::string const & applied_changes) const;
void		send_rpl_QUIT(Client & quitter, std::string const & reason);

#endif
