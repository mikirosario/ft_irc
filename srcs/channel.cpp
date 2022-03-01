#include "../includes/ircserv.hpp"

IRC_Server::Channel::Channel(void)
{}

IRC_Server::Channel::Channel(std::string const &chName) : channelName(chName)
{}

IRC_Server::Channel::Channel(std::string const &chName,std::string const &password) : channelName(chName), channelPassword(password)
{}

IRC_Server::Channel::Channel(Channel const &other)
:
    	channelName(other.channelName),
		channelPassword(other.channelPassword),
		allClients(other.allClients),
		OwnerUser(other.OwnerUser),
        topic(other.topic)
        
{}


//	This function returns a pair with the name of the user and the number of permision. If client doesn't exit, empty string and -1

std::pair<std::string, int> IRC_Server::Channel::findClient(Client const & client)
{
	(void)client; //	Error: it necesita de find en map
    std::map<std::string,int>::iterator it;
	std::string client_nick;

    it = this->allClients.begin();

    if( it == this->allClients.end())
    {
		return(std::make_pair("", CLIENT_FAILTURE));
	}
	client_nick = client.get_nick();
	for (; it != allClients.end(); it++)
	{
		if (it->first == client_nick)
			return(std::make_pair(it->first, it->second));
	};
	return(std::make_pair("", CLIENT_FAILTURE));
}

std::string const & IRC_Server::Channel::getChannelName() const
{
    return (this->channelName);
}

std::string const & IRC_Server::Channel::getTopic() const
{
    return (this->topic);
}

void IRC_Server::Channel::serTopic(std::string const &Topic)
{
    this->topic = Topic;
}

void IRC_Server::Channel::setOwner(Client const &OwnerUser2)
{
    this->OwnerUser  = OwnerUser2.get_nick();
}

std::string IRC_Server::Channel::getOwner() const
{
    return(this->OwnerUser);
};

// -miki
    // single-element insert devuelve un pair en el que 'second' es un bool que
	// indica si se ha insertado o no. si es false (0) es que ya había un string
	// con ese nombre, si es true (1) es que se ha insertado. como el map ahora
	// usa case_insensitive_less como comparador, él solo puede juzgar si ya
	// existe un cliente con ese nombre, y por lo tanto no hace falta hacer otra
	// comprobación findClients, solo llamar insert y devolver el bool que te da.
	// al castear un bool a int true == 1 y false == 0.
// Error / Debug: en PRINCIPIO bien, pero aún no he escrito tests, así que queda
// eso pendiente. ;)
int IRC_Server::Channel::addNewClient(Client const &client)
{
	if (this->channelPassword != "")
		return(INVALID_PASSWORD_RETURN);															//-miki vvvv--esto debería pasarlo la llamadora, no?
	std::pair<t_ChannelMemberMap::iterator, bool> ret = allClients.insert(std::make_pair(client.get_nick(), CLIENT_USER));
    return (ret.second);
}

// Error / Debug: en PRINCIPIO bien, pero aún no he escrito tests, así que queda
// eso pendiente. ;)
int IRC_Server::Channel::addNewClient(Client const &client, std::string const &password)
{
    if (this->channelPassword != password)
		return(INVALID_PASSWORD_RETURN);
	std::pair<t_ChannelMemberMap::iterator, bool> ret = allClients.insert(std::make_pair(client.get_nick(), CLIENT_USER));
    return (ret.second);
}

// -miki
	// map.erase tiene su propia sobrecarga de erase-by-key, que devuelve un
	// size_t que nos indica cuántos elementos se han borrado. si no existe el
	// elemento en el map, devuelve 0, de lo contrario 1 (en un map no puede
	// haber duplicados). castear cualquier entero a bool sigue la regla de
	// 0 == false; !0 == true, así que podemos devolver tranquilamente lo mismo
	// que erase-by-key y hacer un casteo implícito a bool.
	//
	// si se quejara por el casteo implícito, que no debería, pues:
	// return (static_cast<bool>(todalapesca)).
// Error / Debug: en PRINCIPIO bien, pero aún no he escrito tests, así que queda
// eso pendiente. ;)
bool IRC_Server::Channel::removeClient(Client const &client) 
{
	return (allClients.erase(client.get_nick()));
}

bool IRC_Server::Channel::removeClient(Client const &client, std::string const &msg)
{
	sendMessageToAllClients(client, msg);
	return (allClients.erase(client.get_nick()));
}


void IRC_Server::Channel::sendMessageToAllClients(Client const &client, std::string message) 
{
    message = channelName + ":"+ client.get_nick() + ":"+message;
    char buffer[255];
    bzero(buffer, 255);
    strcpy(buffer,message.c_str());

    std::map<std::string, int>::iterator it = allClients.begin();

    for (; it != allClients.end(); it++)
    {
  //      send_rpl_PRIVMSG(client, find_client_by_nick(it->first), message);
        (void)it;
    }
}
