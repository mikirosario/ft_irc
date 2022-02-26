/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 17:29:13 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/26 20:16:41 by acortes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_H
# define CHANNEL_H

#include "ircserv.hpp"

class IRC_Server
{
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

			bool addNewClient(Client client);
			bool removeClient(Client client);
			std::string getChannelName()const {return channelName;};

			void setOwner(Client OwnerUser2);

			std::string getOwner()const{return OwnerUser.get_nick();};
			void sendMessageToAllClients( User user, std::string message);
			bool findClient(Client client);

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
			std::map<Client, int> allClients;
			Client OwnerUser;
	};
}

#endif
