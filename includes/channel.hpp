/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 17:29:13 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/28 13:02:30 by acortes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_H
# define CHANNEL_H

#include "ircserv.hpp"

class Channel
{
	public:

		Channel(void);
		Channel(std::string chName);
		Channel(std::string chName,std::string password);
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

		int addNewClient(Client client);
		int addNewClient(Client client, std::string password);
		bool removeClient(Client client);
		std::string const & getChannelName() const;
		std::string const & getTopic() const;

		void setOwner(Client OwnerUser2);
		void serTopic(std::string Topic);
		std::string getOwner() const;
		void sendMessageToAllClients(Client const & client, std::string message);
		bool findClient(Client const & client);

		bool operator==(const Channel &other) const 
		{						
			return (this->channelName == other.channelName);
		}
		
		bool operator!=(const Channel &other) const 
		{						
			return (this->channelName != other.channelName);
		}

	private:
		std::string channelName;
		std::string	channelPassword;
		std::map<std::string, int> allClients;
		std::string OwnerUser;
		std::string topic;
};

#endif
