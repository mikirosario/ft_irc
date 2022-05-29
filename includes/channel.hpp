/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 17:29:13 by mrosario          #+#    #+#             */
/*   Updated: 2022/05/29 14:19:22 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_H
# define CHANNEL_H

#include "ircserv.hpp"

class Channel
{
	public:
		Channel(Client const & creator, IRC_Server & parent_server, std::string const &chName, std::string const &password);
		Channel(Channel const &other);
		virtual ~Channel(void);
		Channel &operator=(Channel const &other);

		typedef std::set<std::string, case_insensitive_less>					t_ChannelMemberSet;
		
		/* SETTERS */
		int		addMember(Client & client, IRC_Server::t_Channel_Map::iterator & chan_it, std::string const & password, char privilege_level);
		bool	removeMember(std::string const & client_nick);
		void	removeMember(t_ChannelMemberSet::iterator const & member, t_ChannelMemberSet & member_set);
		void	removeAllMembers(void);
		void	setOwner(Client const &OwnerUser2);
		void	setTopic(std::string const &Topic);
		bool	setKey(Client const & setter, std::string const & key);
		bool	unsetKey(Client const & unsetter, std::string const & key);
		bool	banMask(std::string const & mask);
		bool	unbanMask(std::string const & mask);
		bool	isBanned(std::string const & mask);
		bool	setMode(char mode);
		bool	unsetMode(char mode);
		bool	changeNick(std::string const & old_nick, std::string const & new_nick);
		bool 	send_msg(Client const * sender, char privilege_level, std::string const & message) const;
		bool	operator==(const Channel &other) const 
		{
			return case_insensitive_ascii_compare(_channelName, other._channelName);
		}	
		bool 	operator!=(const Channel &other) const 
		{						
			return (_channelName != other._channelName);
		}

		/* GETTERS */
		size_t						size(void) const;
		std::string const &			getChannelName(void) const;
		std::string const & 		getTopic(void) const;
		std::string const & 		getOwner(void) const;
		t_ChannelMemberSet const &	getChanops(void) const;
		t_ChannelMemberSet const &	getHalfops(void) const;
		t_ChannelMemberSet const &	getUsers(void) const;
		t_ChannelMemberSet const &	getBanList(void) const;
		std::string	const &			getModes(void) const;
		bool						isChannelOperator(Client const & client) const;
	private:
		Channel(void);

		IRC_Server &		_parent_server;
		std::string 		_channelName;
		std::string			_channelPassword;
		std::string			_modes;
		t_ChannelMemberSet	_banlist;
		std::string			_owner;
		t_ChannelMemberSet	_chanops;
		t_ChannelMemberSet	_halfops;
		t_ChannelMemberSet	_users;
		std::string 		_topic;
};

#endif
