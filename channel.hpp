/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 17:29:13 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/25 19:43:53 by mrosario         ###   ########.fr       */
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
		Channel(Channel &other);

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

		bool addNewClient(IRC_Server::Client client);
		bool removeClient(IRC_Server::Client client);
		std::string getChannelName() const;

		void setOwner(IRC_Server::Client OwnerUser2);
		IRC_Server::Client getOwner() const;
		void sendMessageToAllClients( User user, std::string message);
		bool findClient(IRC_Server::Client client);

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
		std::map<IRC_Server::Client, int> allClients;
		IRC_Server::Client OwnerUser;
};

#endif
