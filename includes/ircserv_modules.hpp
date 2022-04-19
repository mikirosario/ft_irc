/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_modules.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:38:32 by miki              #+#    #+#             */
/*   Updated: 2022/04/19 22:45:39 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//This header contains private member functions to the IRC_Server class relating
//to client command parsing and interpreting.

#ifndef IRCMODULES_H
# define IRCMODULES_H

//parsing
bool					nick_is_valid(std::string const & str) const;
bool					username_is_valid(std::string const & username) const;
bool					channel_name_is_valid(std::string const & channel_name) const;
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
bool	register_client(Client & client);
void	interpret_msg(Client & client);

	//-adrian
void	exec_cmd_PART(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_TOPIC(Client & sender, std::vector<std::string> const & argv);

void	exec_cmd_LIST(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_INVITE(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_KICK(Client & sender, std::vector<std::string> const & argv);
void	exec_cmd_MODE(Client & sender, std::vector<std::string> const & argv);



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
void		send_rpl_NAMREPLY(Client const & recipient, Channel const & channel);
void		send_rpl_ENDOFNAMES(Client const & recipient, std::string const & channel_name);

void		send_rpl_LISTSTART(Client const & recipient);
void		send_rpl_LIST(Client const & recipient, std::string const &channel_name);
void		send_rpl_LISTEND(Client const & recipient);

void		send_rpl_INVITED(Client const & sender, std::string const &client_name, std::string const &client_nick, Channel const & channel);
void		send_rpl_TOPIC(Client const & recipient, std::string const & channelName, std::string const & channelTopic );


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
	// Me quedo aqui hoy

// Join

void		send_err_NOSUCHCHANNEL(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_TOOMANYCHANNELS(Client const & recipient, std::string const & command, std::string const & description) const;

void		send_err_BANNEDFROMCHAN(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_CHANNELISFULL (Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_INVITEONLYCHAN(Client const & recipient, std::string const & command, std::string const & description) const;
void		send_err_ERR_UNKNOWNMODE(Client const & recipient, char const &command, std::string const & description) const;



//non-numeric replies
void		non_numeric_reply_end(std::string & reply, std::string const & last_param) const;
void		send_rpl_NICK(Client const & recipient, std::string const & old_source) const;
void		send_rpl_PRIVMSG(Client const & recipient, Client const & source, std::string const & message) const;
void		send_rpl_PRIVMSG(Channel const & recipient, Client const & source, std::string const & privileges, std::string const & message) const;
void		send_rpl_NOTICE(Client const & recipient, Client const & source, std::string const & message) const;
void		send_rpl_JOIN(Channel const & recipient, Client const & source) const;
void		send_rpl_PART(Client const & recipient, Channel const & channel, std::string const & part_message) const;
void		send_rpl_KICK(Client const & kicker, Client const & recipient, Channel const & channel, std::string const & kick_message) const;
void		send_rpl_PONG(Client const & recipient, std::string const & token) const;

// Auxiliar methods

void	exec_join(IRC_Server::Client & sender, std::vector<std::string> const & argv);
void 	ft_add_mode(Client const &sender, std::string const &channelName, std::string const &modes);
void	ft_remove_mode(Client const &sender, std::string const &channelName, std::string const &modes);

#endif
