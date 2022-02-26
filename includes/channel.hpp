/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 17:29:13 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/26 15:48:36 by acortes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_H
# define CHANNEL_H

#include "client.hpp"

class IRC_Server
{
	class Channel
	{
		const int clientUser = 0;
		const int adminUser = 1;
		const int ownerUser = 2;

		public:

			Channel(void);
			Channel(std::string chName);
			Channel(Channel &other);

			Channel &Channel::operator=(Channel const &other)
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
			inline std::string getChannelName()const {return channelName;};
			inline void setOwner(Client OwnerUser2){OwnerUser = OwnerUser2;};
			inline std::string getOwner()const{return OwnerUser.get_nick();};
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
};

#endif
