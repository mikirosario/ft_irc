/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 17:29:13 by mrosario          #+#    #+#             */
/*   Updated: 2022/03/02 22:24:46 by acortes-         ###   ########.fr       */
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

		Channel(Client const & creator, std::string const &chName);
		Channel(Client const & creator, std::string const &chName,std::string const &password);
		Channel(Channel const &other);
		~Channel(void);

		Channel &operator=(Channel const &other)
		{
			if (*this == other)
				return(*this);
			_channelPassword = other._channelPassword;
			_channelName = other._channelName;
			//allClients.clear();
			//allClients = other.allClients;
			_owner = other._owner;
			_chanops = other._chanops;
			_halfops = other._halfops;
			_users = other._users;
			return(*this);
		};

		// int addNewClient(Client const &client, std::string const & privileges);
		// int addNewClient(Client const &client, std::string const &password);
		int	addMember(Client & client, std::string const & password, char privilege_level);
		bool removeMember(std::string const & client_nick);
		//bool removeMember(Client const &client, std::string const &msg);
		//bool setNewPrivilegeLevel(Client const & member, char privilege_level);
		std::string const & getChannelName() const;
		std::string const & getTopic() const;

		void setOwner(Client const &OwnerUser2);
		void setTopic(std::string const &Topic);
		std::string getOwner() const;
		bool findClient(Client const &client);
		bool	is_empty(void) const;


		bool 	send_msg(char privilege_level, std::string const & message, IRC_Server const & parent) const;
		// - miki
			//USA case_insensitive_ascii_compare()!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			
		bool operator==(const Channel &other) const 
		{						
			return (_channelName == other._channelName);
		}
		
		bool operator!=(const Channel &other) const 
		{						
			return (_channelName != other._channelName);
		}

		//typedef std::map<std::string, User_Privileges, case_insensitive_less>	t_ChannelMemberMap; //debug //remove
		typedef std::set<std::string, case_insensitive_less>					t_ChannelMemberSet;
	private:
		Channel(void);

		std::string 		_channelName;
		std::string			_channelPassword;
		//t_ChannelMemberMap allClients; //debug //remove

		std::string			_owner;
		t_ChannelMemberSet	_chanops;
		t_ChannelMemberSet	_halfops;
		t_ChannelMemberSet	_users;
		std::string 		_topic;
};

#endif
