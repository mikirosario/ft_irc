/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikiencolor <mikiencolor@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 17:29:13 by mrosario          #+#    #+#             */
/*   Updated: 2022/05/19 20:33:11 by mikiencolor      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_H
# define CHANNEL_H

#include "ircserv.hpp"

class Channel
{
	class User_Privileges
	{
		public:
								User_Privileges(void);
								User_Privileges(User_Privileges const & src);
								User_Privileges(std::string const & privileges);
								~User_Privileges(void);
			User_Privileges &	operator=(User_Privileges const & src);
			bool				set_privileges(std::string const & privileges);
			bool				remove_privileges(std::string const & privileges);
			bool				privilege_is_set(char membership_prefix) const;
		private:
			std::string			_privis;
	};
	public:

		//Channel(Client const & creator, std::string const &chName);
		Channel(Client const & creator, IRC_Server & parent_server, std::string const &chName, std::string const &password);
		Channel(Channel const &other);
		virtual ~Channel(void);

		Channel &operator=(Channel const &other);

		typedef std::set<std::string, case_insensitive_less>					t_ChannelMemberSet;

		// int addNewClient(Client const &client, std::string const & privileges);
		// int addNewClient(Client const &client, std::string const &password);
		
		/* SETTERS */
		int		addMember(Client & client, IRC_Server::t_Channel_Map::iterator & chan_it, std::string const & password, char privilege_level);
		bool	removeMember(std::string const & client_nick);
		void	removeMember(t_ChannelMemberSet::iterator const & member, t_ChannelMemberSet & member_set);
		void	removeAllMembers(void);
		//void	add_mode(char	c);
		//void	remove_mode(char c);
		//bool removeMember(Client const &client, std::string const &msg);
		//bool setNewPrivilegeLevel(Client const & member, char privilege_level);	
		void	setOwner(Client const &OwnerUser2);
		void	setTopic(std::string const &Topic);
		bool	setKey(Client const & setter, std::string const & key);
		bool	unsetKey(Client const & unsetter, std::string const & key);
		bool	banMask(std::string const & mask);
		bool	unbanMask(std::string const & mask);
		bool	isBanned(std::string const & mask);
		bool	setMode(char mode);
		bool	unsetMode(char mode);
		//bool	setModes(std::string const & modestring, std::string & applied_changes);

		bool findClient(Client const &client);
		//bool	is_empty(void) const;


		bool 	send_msg(Client const * sender, char privilege_level, std::string const & message) const;

		bool operator==(const Channel &other) const 
		{
			return case_insensitive_ascii_compare(_channelName, other._channelName);
		}
		
		bool operator!=(const Channel &other) const 
		{						
			return (_channelName != other._channelName);
		}

		size_t	size(void) const;

		//typedef std::map<std::string, User_Privileges, case_insensitive_less>	t_ChannelMemberMap; //debug //remove

		/* GETTERS */
		std::string const &			getChannelName(void) const;
		std::string const & 		getTopic(void) const;
		std::string const & 		getOwner(void) const;
		t_ChannelMemberSet const &	getChanops(void) const;
		t_ChannelMemberSet const &	getHalfops(void) const;
		t_ChannelMemberSet const &	getUsers(void) const;
		t_ChannelMemberSet const &	getBanList(void) const;
		t_ChannelMemberSet const &	getInviteList(void) const;
		std::string	const &			getModes(void) const;
		bool						isChannelOperator(Client const & client) const;
	private:
		Channel(void);

		IRC_Server &		_parent_server;
		std::string 		_channelName;
		std::string			_channelPassword;
		std::string			_modes;
		t_ChannelMemberSet	_banlist;
		//t_ChannelMemberMap allClients; //debug //remove

		std::string			_owner;
		t_ChannelMemberSet	_chanops;
		t_ChannelMemberSet	_halfops;
		t_ChannelMemberSet	_users;
		std::string 		_topic;
};

#endif
