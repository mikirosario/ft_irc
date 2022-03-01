/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 17:29:13 by mrosario          #+#    #+#             */
/*   Updated: 2022/03/01 16:37:14 by mrosario         ###   ########.fr       */
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
			channelName = other.channelName;
			allClients.clear();
			allClients = other.allClients;
			OwnerUser = other.OwnerUser;
			return(*this);
		};

		int addNewClient(Client const &client);
		int addNewClient(Client const &client, std::string const &password);
		int	addNewClient(Client const &client, std::string const &password, std::string const & privileges);
		bool removeClient(Client const &client);
		bool removeClient(Client const &client, std::string const &msg);
		std::string const & getChannelName() const;
		std::string const & getTopic() const;

		void setOwner(Client const &OwnerUser2);
		void serTopic(std::string const &Topic);
		std::string getOwner() const;
		void sendMessageToAllClients(Client const &client, std::string message);
		bool findClient(Client const &client);

		// - miki
			//USA case_insensitive_ascii_compare()!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		bool operator==(const Channel &other) const 
		{						
			return (this->channelName == other.channelName);
		}
		
		bool operator!=(const Channel &other) const 
		{						
			return (this->channelName != other.channelName);
		}

		typedef std::map<std::string, User_Privileges, case_insensitive_less> t_ChannelMemberMap;
	private:
		Channel(void);

		std::string channelName;
		std::string	channelPassword;
		t_ChannelMemberMap allClients;
		std::string OwnerUser;
		std::string topic;
};

#endif
