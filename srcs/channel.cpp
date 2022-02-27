#include "../includes/ircserv.hpp"

IRC_Server::Channel::Channel(void)
:
		channelName(),
		allClients(),
		OwnerUser(),
        topic(),
        channelPassword()
{}

IRC_Server::Channel::Channel(std::string chName) //sacar comprobaciones a llamador
:
		allClients(),
		OwnerUser()
        topic(),
        channelPassword()
{
	//size_t chName_len = chName.size();

    // if (chName_len > 50)
    //     send_err_UNKNOWNERROR(OwnerUser, chName, "Channel name to long");
    // else if (chName_len <= 1)
    //     send_err_UNKNOWNERROR(OwnerUser, chName, "Channel name to short");
    // else if (chName[0] != '&' && chName[0] != '#' && chName[0] != '+' && chName[0] != '!')
    //     send_err_UNKNOWNERROR(OwnerUser, chName, "Channel first char invalid. Use '&', '#', '+' or '!'");
    // else
    //     std::transform(chName.begin(), chName.end(), chName.begin(), asciitolower);
    // for(size_t i = 1; i < chName_len; i++)
    // {
    //     if(chName[i] == ' ' || chName[i] == ',' || chName[i] == ':')
    //          send_err_UNKNOWNERROR(OwnerUser, chName, "Invalid symbol used in the creation of the channel name");
    // }
    this->channelName = chName;
    return ;
}

IRC_Server::Channel::Channel(Channel &other)
:
    	channelName(other.channelName),
		allClients(other.allClients),
		OwnerUser(other.OwnerUser)
        topic(other.topic),
        channelPassword(other.channelPassword)
{}

bool IRC_Server::Channel::findClient(Client const & client)
{
	(void)client; //	Error: it necesita de find en map
    std::map<std::string,int>::iterator it;

    it = this->allClients.begin();

    if( it == this->allClients.end())
        return(0);
    return(1);
}

std::string IRC_Server::Channel::getChannelName() const
{
    return (this->channelName);
}

std::string IRC_Server::Channel::getTopic() const
{
    return (this->topic);
}

void IRC_Server::Channel::serTopic(std::string Topic)
{
    this->topic = Topic;
}

void IRC_Server::Channel::setOwner(Client OwnerUser2)
{
    this->OwnerUser  = OwnerUser2.get_nick();
}

std::string IRC_Server::Channel::getOwner() const
{
    return(this->OwnerUser);
};

int IRC_Server::Channel::addNewClient(Client client)
{
    if (this->channelPassword != "")
        return(INVALID_PASSWORD_RETURN);
    if (!this->findClient(client))
    {
        allClients.insert(std::make_pair(client.get_nick(), CLIENT_USER));
        return(1);
    }
    return (CLIENT_ALREADY_EXIST_RETURN);
}

int IRC_Server::Channel::addNewClient(Client client, std::string password)
{
    if (this->channelPassword != password)
        return(INVALID_PASSWORD_RETURN);
    if (!this->findClient(client))
    {
            allClients.insert(std::make_pair(client.get_nick(), CLIENT_USER));
            return(1);
    }
    return (CLIENT_ALREADY_EXIST_RETURN);
}

bool IRC_Server::Channel::removeClient(Client client) 
{
    std::map<std::string, int>::iterator it = allClients.find(client.get_nick());

    if( it == this->allClients.end())
    {
         allClients.erase(it);
          return true;
    }
    return false;
}

void IRC_Server::Channel::sendMessageToAllClients(Client const & client, std::string message) 
{
    message = channelName + ":"+ client.get_nick() + ":"+message;
    char buffer[255];
    bzero(buffer, 255);
    strcpy(buffer,message.c_str());

    std::map<std::string, int>::iterator it = allClients.begin();

    for (; it != allClients.end(); it++)
    {
        //PRIVMSG(to every user)
        (void)it;
    }
}
