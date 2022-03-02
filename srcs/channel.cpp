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

/*!
** @brief	Sets the privileges indicated in @a privileges for the
**			User_Privileges instance.
**
** @details	'~' is Owner/Founder, '@' is Operator and '%' is HalfOp. Additional
**			supported prefixes in @a SUPPORTED_CHANNEL_PREFIXES.
**
**			If the requested privileges are already set, nothing is done for
**			those privileges. If @a privileges contains characters not present
**			in SUPPORTED_CHANNEL_PARAMETERS, or more characters than those
**			present in SUPPORTED_CHANNEL_PARAMETERS, or is empty, nothing is
**			done and the method returns false.
** @param	privileges	The privileges you want to set from among
**						@a SUPPORTED_CHANNEL_PREFIXES.
** @return	true if @a privileges argument was valid, false otherwise
*/
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

/*!
** @brief	Removes the privileges indicated in @a privileges for the
**			User_Privileges instance.
**
** @details '~' is Owner/Founder, '@' is Operator and '%' is HalfOp. Additional
**			supported prefixes in @a SUPPORTED_CHANNEL_PREFIXES.
**
**			If the requested privileges are not set, nothing is done for those
**			privileges. If @a privileges contains characters not present in
**			SUPPORTED_CHANNEL_PARAMETERS, or more characters than those present
**			in SUPPORTED_CHANNEL_PARAMETERS, or is empty, nothing is done and
**			the method returns false.
** @param	privileges	The privileges you want to remove from among
**						@a SUPPORTED_CHANNEL_PREFIXES.
** @return	true if @a privileges argument was valid, false otherwise
*/
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

/*!
** @brief	Determines whether the privilege corresponding to
**			@a membership_prefix is or is not set.
**
** @details	'~' is Owner/Founder, '@' is Operator and '%' is HalfOp. Additional
**			supported prefixes in @a SUPPORTED_CHANNEL_PREFIXES.
** @param	membership_prefix	The prefix representing the privilege level.
** @return	true if the privilege is set, otherwise false
*/
bool	IRC_Server::Channel::User_Privileges::privilege_is_set(char membership_prefix) const
{
	if (_privis.find(membership_prefix) != std::string::npos)
		return true;
	return false;
}

IRC_Server::Channel::Channel(Client const & creator, std::string const &chName) : _channelName(chName), _owner(creator.get_nick())
{
}

IRC_Server::Channel::Channel(Client const & creator, std::string const &chName, std::string const &password) : _channelName(chName), _channelPassword(password), _owner(creator.get_nick())
{
}

IRC_Server::Channel::Channel(Channel const &other)
:
    	_channelName(other._channelName),
		_channelPassword(other._channelPassword),
		_owner(other._owner),
		_chanops(other._chanops),
		_halfops(other._halfops),
		_users(other._users),
		//allClients(other.allClients),
        _topic(other._topic)     
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

IRC_Server::Channel::~Channel(void)
{}

std::string const & IRC_Server::Channel::getChannelName() const
{
    return (_channelName);
}

std::string const & IRC_Server::Channel::getTopic() const
{
    return (_topic);
}

void IRC_Server::Channel::setTopic(std::string const & topic)
{
    _topic = topic;
}

void IRC_Server::Channel::setOwner(Client const & client)
{
    _owner = client.get_nick(); //se puede cambiar de owner? al cambiar, el antiguo owner sigue siendo miembro del canal?
}

std::string IRC_Server::Channel::getOwner() const
{
    return(_owner);
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
// int IRC_Server::Channel::addMember(Client const &client, std::string const & privileges)
// {
// 	if (this->channelPassword != "")
// 		return(INVALID_PASSWORD_RETURN);															//-miki vvvv--esto debería pasarlo la llamadora, no?
// 	std::pair<t_ChannelMemberMap::iterator, bool> ret = allClients.insert(std::make_pair(client.get_nick(), privileges));
//     return (ret.second);
// }

// // Error / Debug: en PRINCIPIO bien, pero aún no he escrito tests, así que queda
// // eso pendiente. ;) De momento los client no saben a qué canales están suscritos

/*!
** @brief	Adds @a client to the channel with @a privilege_level if @a password
**			matches the channel password.
**
** @details	Clients will be added to one of three sets of members in the channel
**			object depending on @a privilege_level.
**
**			@a privilege_level '0' is the lowest, and linked to the _users set.
**			@a privilege_level '%' is the next, and linked to the _halfops set.
**			@a privilege_level '@' is the next, and linked to the _chanops set.
**			@a privilege_level '~' is the highest, and linked to _owner.
**
**			There can only be one _owner, so a replaced _owner is downgraded to
**			_chanop.
**
** @param	client			The member to be added.
** @param	password		The password provided by the client.
** @param	privilege_level	The member's privilege level in the channel.
**							@see SUPPORTED_CHANNEL_PREFIXESÇ
** @return	1 if the member was successfully added. Otherwise, -1 if the
**			client's password was wrong, -2 if the client's privilege_level was
**			not understood, and 0 if the member was not successfully added for
**			any other reason.
*/
int IRC_Server::Channel::addMember(Client & client, std::string const &password, char privilege_level)
{
    if (_channelPassword != password)
		return(INVALID_PASSWORD_RETURN);
	else if (std::strchr(SUPPORTED_CHANNEL_PREFIXES, privilege_level) == NULL)
		return(-2); //BAD PREFIX
	std::pair<t_ChannelMemberSet::iterator, bool> ret;
	ret.second = false;
	if (privilege_level == 0)														//lowest privilege level
		ret = _users.insert(client.get_nick());
	else if (privilege_level == '%')												//halfops privilege level
		ret = _halfops.insert(client.get_nick());
	else if (privilege_level == '@')												//chanops privilege level
		ret = _chanops.insert(client.get_nick());
	else if (privilege_level == '~' &&
			((ret.second = _owner.empty()) == true || (ret = _chanops.insert(_owner)).second == 1))	//owner privilege level; if there is no owner, or existing owner is successfully downgraded to chanops, replace owner
		_owner = client.get_nick();
	
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

/*!
** @brief	Removes @a client_nick from this channel, if @a client_nick is a
**			member.
**
** @details	If @a client_nick is the owner, the channel becomes ownerless.
**			Otherwise, we search for @a client in each of the privilege levels
**			and erase when we find them.
** @param	client_nick	Nick of the member to remove from the channel.
** @return	true if member was removed, false if @a client_nick was not a member
*/
bool IRC_Server::Channel::removeMember(std::string const & client_nick) 
{
	if (_owner == client_nick)
	{
		_owner.clear();
		return (true);
	}
	else
	{
		IRC_Server::Channel::t_ChannelMemberSet	* pMemberSet[] = {&_chanops, &_halfops, &_users};
	
		for (size_t i = 0, ret = 0; i < 3; ++i)
			if ((ret = pMemberSet[i]->erase(client_nick)) > 0)
				return (true);
		return (false);
	}
}

// bool IRC_Server::Channel::removeClient(Client const &client, std::string const &msg)
// {
// 	sendMessageToAllClients(client, msg);
// 	return (allClients.erase(client.get_nick()));
// }


// void IRC_Server::Channel::sendMessageToAllClients(Client const &client, std::string message) 
// {
//     message = channelName + ":"+ client.get_nick() + ":"+message;
//     char buffer[255];
//     bzero(buffer, 255);
//     strcpy(buffer,message.c_str());

//     std::map<std::string, User_Privileges>::iterator it = allClients.begin();

//     for (; it != allClients.end(); it++)
//     {
//         //PRIVMSG(to every user)
//         (void)it;
//     }
// }

bool	IRC_Server::Channel::send_msg(char privilege_level, std::string const & message, IRC_Server const & parent) const
{
	IRC_Server::Client const *	recipient = NULL;
	switch (privilege_level)
	{
		case 0 :
			for (t_ChannelMemberSet::iterator it = _users.begin(), end = _users.end(); it != end; ++it)
				if ((recipient = parent.find_client_by_nick(*it)) != NULL)
					recipient->send_msg(message);
		case '%' :
			for (t_ChannelMemberSet::iterator it = _halfops.begin(), end = _halfops.end(); it != end; ++it)
				if ((recipient = parent.find_client_by_nick(*it)) != NULL)
					recipient->send_msg(message);
		case '@' :
			for (t_ChannelMemberSet::iterator it = _chanops.begin(), end = _chanops.end(); it != end; ++it)
				if ((recipient = parent.find_client_by_nick(*it)) != NULL)
					recipient->send_msg(message);
		case '~' :
			if ((recipient = parent.find_client_by_nick(_owner)) != NULL)
				recipient->send_msg(message);
			return true;
		default :
			return false;
	}
}

