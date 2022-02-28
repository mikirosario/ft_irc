/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 17:29:13 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/28 21:18:24 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_H
# define CHANNEL_H

#include "ircserv.hpp"

class Channel
{
	public:

		Channel(void);
		Channel(std::string const &chName);
		Channel(std::string const &chName,std::string const &password);
		Channel(Channel const &other);

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

		typedef std::map<std::string, int, case_insensitive_less> t_ChannelMemberMap;
	private:
		std::string channelName;
		std::string	channelPassword;
		t_ChannelMemberMap allClients;
		std::string OwnerUser;
		std::string topic;
};

#endif
