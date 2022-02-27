#include "../includes/ircserv.hpp"

IRC_Server::Channel::Channel(void)
{}

IRC_Server::Channel::Channel(std::string chName) : channelName(chName)
{}

IRC_Server::Channel::Channel(Channel const &other)
:
    	channelName(other.channelName),
		channelPassword(other.channelPassword),
		allClients(other.allClients),
		OwnerUser(other.OwnerUser),
        topic(other.topic)
        
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
