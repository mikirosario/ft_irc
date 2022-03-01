#include "../includes/ircserv.hpp"

//User_Privileges

IRC_Server::Channel::User_Privileges::User_Privileges(void)
{
	_privis.reserve(sizeof(SUPPORTED_CHANNEL_PREFIXES) - 1);
}

IRC_Server::Channel::User_Privileges::User_Privileges(User_Privileges const & src) : _privis(src._privis)
{}

IRC_Server::Channel::User_Privileges::User_Privileges(std::string const & privileges) : _privis(privileges)
{}

IRC_Server::Channel::User_Privileges::~User_Privileges(void)
{}

IRC_Server::Channel::User_Privileges &	IRC_Server::Channel::User_Privileges::operator=(User_Privileges const & src)
{
	_privis.assign(src._privis);
	return (*this);
}

bool	IRC_Server::Channel::User_Privileges::set_privileges(std::string const & privileges)
{
	if (privileges.empty() == true || privileges.size() > sizeof(SUPPORTED_CHANNEL_PREFIXES) - 1 || privileges.find_first_not_of(SUPPORTED_CHANNEL_PREFIXES) != std::string::npos)
		return false;
	for (size_t end = sizeof(SUPPORTED_CHANNEL_PREFIXES), i = 0; i < end; ++i)
	{
		if (_privis.find(SUPPORTED_CHANNEL_PREFIXES[i]) == std::string::npos
			&& privileges.find(SUPPORTED_CHANNEL_PREFIXES[i]) != std::string::npos)
			_privis.push_back(SUPPORTED_CHANNEL_PREFIXES[i]);
	}
	return true;
}

bool	IRC_Server::Channel::User_Privileges::remove_privileges(std::string const & privileges)
{
	if (privileges.empty() == true || privileges.size() > sizeof(SUPPORTED_CHANNEL_PREFIXES) - 1 || privileges.find_first_not_of(SUPPORTED_CHANNEL_PREFIXES) != std::string::npos)
		return false;
	for (size_t end = sizeof(SUPPORTED_CHANNEL_PREFIXES), i = 0; i < end; ++i)
	{
		size_t char_pos;
		if ((char_pos = _privis.find(SUPPORTED_CHANNEL_PREFIXES[i])) != std::string::npos
			&& privileges.find(SUPPORTED_CHANNEL_PREFIXES[i]) != std::string::npos)
			_privis.erase(char_pos, sizeof(char));
	}
	return true;
}

bool	IRC_Server::Channel::User_Privileges::privilege_is_set(char membership_prefix) const
{
	if (_privis.find(membership_prefix) != std::string::npos)
		return true;
	return false;
}

IRC_Server::Channel::Channel(Client const & creator, std::string const &chName) : channelName(chName), OwnerUser(creator.get_nick())
{
	addNewClient(creator, "~");
}

IRC_Server::Channel::Channel(Client const & creator, std::string const &chName, std::string const &password) : channelName(chName), channelPassword(password), OwnerUser(creator.get_nick())
{
	addNewClient(creator, password, "~");
}

IRC_Server::Channel::Channel(Channel const &other)
:
    	channelName(other.channelName),
		channelPassword(other.channelPassword),
		allClients(other.allClients),
		OwnerUser(other.OwnerUser),
        topic(other.topic)     
{}
// -miki
	// en principio esta funcionalidad ya está cubierta por el map con
	// case_insensitive_less.
// bool IRC_Server::Channel::findClient(Client const & client)
// {
// 	(void)client; //	Error: it necesita de find en map
//     std::map<std::string,int>::iterator it;

//     it = this->allClients.begin();

//     if( it == this->allClients.end())
//         return(0);
//     return(1);
// }

IRC_Server::Channel::~Channel(void)
{}

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
int IRC_Server::Channel::addNewClient(Client const &client, std::string const & privileges)
{
	if (this->channelPassword != "")
		return(INVALID_PASSWORD_RETURN);															//-miki vvvv--esto debería pasarlo la llamadora, no?
	std::pair<t_ChannelMemberMap::iterator, bool> ret = allClients.insert(std::make_pair(client.get_nick(), privileges));
    return (ret.second);
}

// // Error / Debug: en PRINCIPIO bien, pero aún no he escrito tests, así que queda
// // eso pendiente. ;)
// int IRC_Server::Channel::addNewClient(Client const &client, std::string const &password)
// {
//     if (this->channelPassword != password)
// 		return(INVALID_PASSWORD_RETURN);
// 	std::pair<t_ChannelMemberMap::iterator, bool> ret = allClients.insert(std::make_pair(client.get_nick(), std::string()));
//     return (ret.second);
// }

int IRC_Server::Channel::addNewClient(Client const &client, std::string const &password, std::string const & privileges)
{
    if (this->channelPassword != password)
		return(INVALID_PASSWORD_RETURN);
	std::pair<t_ChannelMemberMap::iterator, bool> ret = allClients.insert(std::make_pair(client.get_nick(), privileges));
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

    std::map<std::string, User_Privileges>::iterator it = allClients.begin();

    for (; it != allClients.end(); it++)
    {
        //PRIVMSG(to every user)
        (void)it;
    }
}
