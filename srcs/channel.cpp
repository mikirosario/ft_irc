#include "../includes/channel.hpp"

IRC_Server::Channel::Channel(void)
:
		channelName(),
		allClients(),
		OwnerUser()
{}

char asciitolower(char in) 
{
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

IRC_Server::Channel::Channel(std::string chName)
:
		allClients(),
		OwnerUser()
{
    size_t chName_len = chName.size();

    if (chName_len > 50)
        send_err_UNKNOWNERROR(OwnerUser, chName, "Channel name to long");
    else if (chName_len <= 1)
        send_err_UNKNOWNERROR(OwnerUser, chName, "Channel name to short");
    else if (chName[0] != '&' && chName[0] != '#' && chName[0] != '+' && chName[0] != '!')
        send_err_UNKNOWNERROR(OwnerUser, chName, "Channel first char invalid. Use '&', '#', '+' or '!'");
    else
        std::transform(chName.begin(), chName.end(), chName.begin(), asciitolower);
    for(size_t i = 1; i < chName_len; i++)
    {
        if(chName[i] == ' ' || chName[i] == ',' || chName[i] == ':')
             send_err_UNKNOWNERROR(OwnerUser, chName, "Invalid symbol used in the creation of the channel name");
    }
    this->channelName = chName;
    return ;
}

IRC_Server::Channel::Channel(Channel &other)
:
    	channelName(other.channelName),
		allClients(other.allClients),
		OwnerUser(other.OwnerUser)
{}

bool IRC_Server::Channel::findClient(Client client)
{
    std::map<Client,int>::iterator it;

    it = this->allClients.begin();

    if( it == this->allClients.end())
        return(0);
    return(1);
}


bool IRC_Server::Channel::addNewClient(Client client)
{
    if (!this->findClient(client))
    {
        allClients.insert(std::make_pair(client, clientUser));
        return(true);
    }
    return (false);
}

bool IRC_Server::Channel::removeClient(Client client) 
{
    std::map<Client, int>::iterator it = allClients.find(client);

    if( it == this->allClients.end())
    {
         allClients.erase(it);
          return true;
    }
    return false;
}

void IRC_Server::Channel::sendMessageToAllClients(User user, std::string message) 
{
    message = channelName + ":"+ user.getNick() + ":"+message;
    char buffer[255];
    bzero(buffer, 255);
    strcpy(buffer,message.c_str());

    std::map<Client, int>::iterator it = allClients.begin();

    for (; it != allClients.end(); it++)
    {
        //PRIVMSG(to every user)
        (void)it;
    }
}
