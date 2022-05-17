/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_modules.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikiencolor <mikiencolor@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:38:32 by miki              #+#    #+#             */
/*   Updated: 2022/05/16 17:55:41 by mikiencolor      ###   ########.fr       */
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
bool	doChanModeChange(char sign, char mode, std::string const & arg, Client const & recipient, Channel & channel);
bool	register_client(Client & client);
void	interpret_msg(Client & client);
void	exec_cmd_PART(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_TOPIC(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_LIST(Client & sender, std::vector<std::string> const & argv);
void	exec_listtoall(IRC_Server::Client const * sender, t_Channel_Map::iterator chan_it, std::vector<std::string> const & argv);
void	exec_cmd_INVITE(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_KICK(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_MODE(Client & sender, std::vector<std::string> const & argv);

void	exec_cmd_QUIT(Client & sender, std::vector<std::string> const & argv);
bool	execModeChanges(Channel & channel, std::string const & modestring, std::string & applied_changes);

//numeric replies
std::string	numeric_reply_start(Client const & client, char const * numeric) const;
//std::string	numeric_reply_start(Channel const & recipient, char const * numeric) const;
void		numeric_reply_end(std::string & reply, std::string const & description) const;
	//standard numeric replies

//replies to command
void		send_rpl_WELCOME(Client const & recipient);
void		send_rpl_YOURHOST(Client const & recipient);
void		send_rpl_CREATED(Client const & recipient);
void		send_rpl_MYINFO(Client const & recipient);
void		send_rpl_ISUPPORT(Client const & recipient);
void		send_rpl_UMODEIS(Client const & recipient);

void		send_rpl_NAMREPLY(Client const & recipient, Channel const & channel);
//void		send_rpl_NAMREPLY(Channel const & recipient, Channel const & channel);
void		send_rpl_ENDOFNAMES(Client const & recipient, std::string const & channel_name);
//void		send_rpl_ENDOFNAMES(Channel const & recipient, std::string const & channelName);

void		send_rpl_LISTSTART(Client const & recipient);
void		send_rpl_LIST(Client const & recipient, std::string const &channel_name);
void		send_rpl_LISTEND(Client const & recipient);

void		send_rpl_INVITED(Client const & sender, Client const & target, Channel const & channel);
void		send_rpl_INVITING(Client const & sender, Client const & target, Channel const & channel);
void		send_rpl_TOPIC(Client const & recipient, Channel const & channel);
void		send_rpl_NOTOPIC(Client const & recipient, Channel const & channel);
void		send_rpl_CHANNELMODEIS(Client const & recipient, Channel const & channel);
void		send_rpl_ENDOFBANLIST(Client const & recipient, Channel const & channel, std::string const & description);
void		send_rpl_BANLIST(Client const & recipient, Channel const & channel);

// Esto no lo considero neceserio

/*
void		send_rpl_topicWhoTime(Client const & recipient, std::string const & command, std::string const & description)
void		send_rpl_namReply(Client const & recipient, std::string const & command, std::string const & description)
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
void		send_err_BADCHANMASK(Client const & recipient, std::string const & channel_name, std::string const & description) const;
void		send_err_BADCHANNELKEY(Client const & recipient, Channel const & channel, std::string const & description) const;
void		send_err_NOTONCHANNEL(Client const & recipient, Channel const & channel, std::string const & description) const;
void		send_err_USERNOTINCHANNEL(Client const & recipient, Client const & target, Channel const & channel, std::string const & description) const;
void		send_err_INVITEONLYCHAN(Client const & recipient, std::string const &channel_name) const;
void		send_err_USERONCHANNEL(Client const & recipient, std::string const & client_name, std::string const & client_nick, Channel const & channel) const;
void		send_err_NOSUCHSERVER(Client const & recipient, std::string const & server_name, std::string const & description) const;
void		send_err_NOMOTD(Client const & recipient, std::string const & description) const;
void		send_err_ERR_CHANOPRIVSNEEDED(Client const & recipient, Channel const & channel, std::string const & description) const;
void		send_err_USERSDONTMATCH(Client const & recipient, std::string const & description) const;
void		send_err_UMODEUNKNOWNFLAG(Client const & recipient, std::string const & description) const;
	// Me quedo aqui hoy

// Join

void		send_err_NOSUCHCHANNEL(Client const & recipient, std::string const & channel_nick, std::string const & description) const;
void		send_err_TOOMANYCHANNELS(Client const & recipient, std::string const & command, std::string const & description) const;

void		send_err_BANNEDFROMCHAN(Client const & recipient, Channel const & channel, std::string const & description) const;
void		send_err_CHANNELISFULL (Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_INVITEONLYCHAN(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_ERR_UNKNOWNMODE(Client const & recipient, char const &command, std::string const & description) const;



//non-numeric replies
void		non_numeric_reply_end(std::string & reply, std::string const & last_param) const;
void		send_rpl_NICK(Client const & recipient, std::string const & old_source) const;
void		send_rpl_PRIVMSG(Client const & recipient, Client const & source, std::string const & message) const;
void		send_rpl_PRIVMSG(Channel const & recipient, Client const & source, std::string const & privileges, std::string const & message) const;
void		send_rpl_NOTICE(Client const & recipient, Client const & source, std::string const & message) const;
void		send_rpl_INVITE(Client const & recipient, Client const & source, std::string const & channel) const;
void		send_rpl_JOIN(Channel const & recipient, Client const & source) const;
void		send_rpl_PART(Client const & recipient, Channel const & channel, std::string const & part_message) const;
void		send_rpl_KICK(Client const & kicker, Client const & recipient, Channel const & channel, std::string const & kick_message) const;
void		send_rpl_PONG(Client const & recipient, std::string const & token) const;
void		send_rpl_MODE(Client const & recipient, std::string const & applied_changes) const;
void		send_rpl_MODE(Client const & recipient, Channel const & channel, std::string const & applied_changes) const;
void		send_rpl_QUIT(Client & quitter, std::string const & reason);

#endif
