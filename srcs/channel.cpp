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

// IRC_Server::Channel::Channel(Client const & creator, std::string const &chName) : _channelName(chName), _owner(creator.get_nick())
// {
// }

IRC_Server::Channel::Channel(Client const & creator, IRC_Server & parent_server, std::string const &chName, std::string const &password) : _parent_server(parent_server), _channelName(chName), _channelPassword(password), _owner(creator.get_nick())
{
	if (_channelPassword.size() > 0)
		setMode('k');
}

IRC_Server::Channel::Channel(Channel const &other)
:
		_parent_server(other._parent_server),
    	_channelName(other._channelName),
		_channelPassword(other._channelPassword),
		_modes(other._modes),
		_banlist(other._banlist),
		_owner(other._owner),
		_chanops(other._chanops),
		_halfops(other._halfops),
		_users(other._users),
		//allClients(other.allClients),
        _topic(other._topic)     
{}

IRC_Server::Channel &	IRC_Server::Channel::operator=(Channel const &other)
{
	if (*this == other || &_parent_server != &other._parent_server)
		return(*this);
	_channelPassword = other._channelPassword;
	_modes = other._modes;
	_banlist = other._banlist;
	_channelName = other._channelName;
	//allClients.clear();
	//allClients = other.allClients;
	_owner = other._owner;
	_chanops = other._chanops;
	_halfops = other._halfops;
	_users = other._users;
	return(*this);
}

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
{
}

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

/*!
** @brief	Attempts to make @a key the channel password. If 'k' mode is not set
**			already, it will be set. If setter is not a chanop or setMode fails
**			for any reason, it will fail.
**
** @param	key	New channel password.
** @return	true if successful, otherwise false
*/
bool	IRC_Server::Channel::setKey(Client const & setter, std::string const & key)
{
	bool mode_already_set;
	if (isChannelOperator(setter) == true)
		if ((mode_already_set = _modes.find('k') != std::string::npos) == true || setMode('k') == true)
		{
			if ((_channelPassword = key) == key)
				return true;
			else if (mode_already_set == false)
				unsetMode('k');
		}
	return false;
}

/*!
** @brief	Attempts to unset the channel password and make the channel
**			passwordless. If @a key does not match the channel password, or
**			@a unsetter is not a chanop, nothing will be done.
**
** @param	key	Existing channel password.
** @
*/
bool	IRC_Server::Channel::unsetKey(Client const & unsetter, std::string const & key)
{
	if (isChannelOperator(unsetter) == true && _channelPassword == key)
	{
		_channelPassword.clear();
		unsetMode('k');
		return true;
	}
	return false;
}

/*!
** @brief Attempts to add @a mask to the ban list.
**
** @details	This function requires setMode to be successful if 'b' mode was not
**			already set.
**
**			Wildcard matching is used, so if the banlist already has a wildcard
**			that includes @a mask, then @a mask will be considered banned
**			already and the ban will not be applied.
**
**			For example, if @a mask == "piscina" and the banlist contains "p*",
**			"piscina" will be considered included in the "p*" case and so will
**			NOT be explicitly banned.
**
**			This could be further tweaked with a ban exclude list. Any project
**			partner is free to code one if they want it - I've done my bit for
**			IRC. :P
** @return	false if channel ban mode not set or unsuccessful for other reasons,
**			otherwise true
*/
bool	IRC_Server::Channel::banMask(std::string const & mask)
{
	if (_modes.find('b') != std::string::npos || setMode('b') == true)
	{
		t_ChannelMemberSet::const_iterator it = _banlist.begin(), end = _banlist.end();
		for ( ; it != end; ++it)
			if (dual_wildcard_matching_equality(*it, mask) == true)
				break ;
		if (it == end && _banlist.insert(mask).second == true)
			return true;
	}
	return false;
}

/*!
** @brief	Attempts to remove all masks matching @a mask from the ban list as
**			determined by a wildcard matching equality check.
**
** @return	false if no match was found, otherwise true
*/
bool	IRC_Server::Channel::unbanMask(std::string const & mask)
{
	t_ChannelMemberSet::iterator				it = _banlist.begin();
	t_ChannelMemberSet::const_iterator			end = _banlist.end();
	std::vector<t_ChannelMemberSet::iterator>	del_vector;

	for ( ; it != end; ++it)
		if (dual_wildcard_matching_equality(*it, mask) == true)
			del_vector.push_back(it);
	if (del_vector.size() < 1) 																									//no match found
		return false;
	for (std::vector<t_ChannelMemberSet::iterator>::iterator it = del_vector.begin(), end = del_vector.end(); it != end; ++it)	//erase all matches
		_banlist.erase(*it);
	if (_banlist.size() < 1)
		unsetMode('b');
	return true;
}

/*!
** @brief	Attempts to find a case-insensitive, wildcard-sensitive match for
**			@a mask in the banlist if the ban mode is set.
**
** @return	true if a match is found, otherwise false
*/
bool	IRC_Server::Channel::isBanned(std::string const & mask)
{
	if (getModes().find('b') != std::string::npos)
		for (t_ChannelMemberSet::const_iterator it = _banlist.begin(), end = _banlist.end(); it != end; ++it)
			if (dual_wildcard_matching_equality(*it, mask) == true)
				return true;
	return false;
}

void IRC_Server::Channel::setOwner(Client const & client)
{
    _owner = client.get_nick(); //se puede cambiar de owner? al cambiar, el antiguo owner sigue siendo miembro del canal?
}

bool	IRC_Server::Channel::setMode(char mode)
{
	if (std::strchr(SUPPORTED_CHANNEL_MODES, mode) == NULL || _modes.find_first_of(mode) != std::string::npos)
		return false;
	try
	{
		_modes.push_back(mode);
		return true;
	}
	catch (std::exception const & e)
	{
		return false;
	}
}

bool	IRC_Server::Channel::unsetMode(char mode)
{
	size_t	del_pos;
	if (std::strchr(SUPPORTED_CHANNEL_MODES, mode) == NULL || (del_pos = _modes.find_first_of(mode)) == std::string::npos)
		return false;
	_modes.erase(del_pos, 1);
	return true;
}

std::string const & IRC_Server::Channel::getOwner(void) const
{
    return(_owner);
}

IRC_Server::Channel::t_ChannelMemberSet const &	IRC_Server::Channel::getChanops(void) const
{
	return (_chanops);
}

IRC_Server::Channel::t_ChannelMemberSet const &	IRC_Server::Channel::getHalfops(void) const
{
	return (_halfops);
}

IRC_Server::Channel::t_ChannelMemberSet const &	IRC_Server::Channel::getUsers(void) const
{
	return (_users);
}

IRC_Server::Channel::t_ChannelMemberSet const &	IRC_Server::Channel::getBanList(void) const
{
	return (_banlist);
}

std::string const & 							IRC_Server::Channel::getModes(void) const
{
	return(_modes);
}

bool											IRC_Server::Channel::isChannelOperator(IRC_Server::Client const & client) const
{
	std::string const & client_nick = client.get_nick();

	if (_owner == client_nick || _chanops.find(client_nick) != _chanops.end())
		return true;
	return false;
}
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
**			If a channel is added to the client's channel list but the client
**			could not be added to the channel's client list, the channel entry
**			in the client channel list will be deleted.
** @param	client			The member to be added.
** @param	password		The password provided by the client.
** @param	privilege_level	The member's privilege level in the channel.
**							@see SUPPORTED_CHANNEL_PREFIXES
** @return	1 if the member was successfully added to the channel's member list
**			AND the channel was successfully added to the client's channel list.
+*			Otherwise, -1 if the client's password was wrong, -2 if the client's
**			privilege_level was not understood, -3 if the client is banned from
**			the channel, and 0 if the member was not successfully added for any
**			other reason.
*/
int IRC_Server::Channel::addMember(Client & client, IRC_Server::t_Channel_Map::iterator & chan_it, std::string const &password, char privilege_level)
{
    if (_channelPassword != password)
		return(INVALID_PASSWORD_RETURN);
	else if (chan_it->second.isBanned(std::string(client.get_source()).erase(0)))
		return(-3);
	else if (std::strchr(SUPPORTED_CHANNEL_PREFIXES, privilege_level) == NULL)
		return(-2); //BAD PREFIX
	else if (client.set_channel_membership(chan_it) == false)						//channel membership predicated on client member channel iterator set
		return (0);
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
	if (ret.second == false)
		client.remove_channel_membership(chan_it);
    return (ret.second);
} //MIKIMIKIMIKI aun no esta implementado el invite only, que es donde tengo que poner la excepcion si esta en el listado de invitaciones, no?

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
**			member. Destroys channel if client_nick was the last member of the
**			channel.
**
** @details	If @a client_nick is the owner, ownership is inherited as long as
**			there are still other channel members, in order of highest privilege
**			and, among members of equal privilege, alphabetical order.
**
**			Otherwise, we search for @a client in each of the privilege levels
**			and erase when we find them.
** @param	client_nick	Nick of the member to remove from the channel.
** @return	true if member was removed, false if @a client_nick was not a member
*/
bool IRC_Server::Channel::removeMember(std::string const & client_nick)
{
	IRC_Server::Channel::t_ChannelMemberSet	* pMemberSet[] = {&_chanops, &_halfops, &_users};

	bool	member_was_removed = false;
	if (_owner == client_nick)
	{
		_owner.clear();
		for (size_t i = 0; i < 3; ++i)				//replace owner with remaining channel member in order of highest privilege, and among members of equal privilege, alphabetical order
			if (pMemberSet[i]->size() > 0)
			{
				t_ChannelMemberSet::iterator new_owner = pMemberSet[i]->begin();
				_owner = *new_owner;
				pMemberSet[i]->erase(new_owner);
				break ;
			}
		member_was_removed = true;
	}
	else
	{	
		for (size_t i = 0, ret = 0; i < 3; ++i)
			if ((ret = pMemberSet[i]->erase(client_nick)) > 0)
				member_was_removed = true;
	}
	if (member_was_removed == true && size() == 0)
		_parent_server.remove_channel(getChannelName());
	return (member_was_removed);
}

/*!
** @brief	This DANGEROUS overload removes @a member from this channel.
**			Use for efficiency IF member iterator and set are already positively
**			known. If @a member is NOT really in @a member_set, UNDEFINED
**			BEHAVIOUR will result.
**
** @details This DANGEROUS overload cannot remove the owner.
** @param	member		Iterator to the member to remove from the channel.
** @param	member_set	The member set in which @a member is located.
*/
void IRC_Server::Channel::removeMember(t_ChannelMemberSet::iterator const & member, IRC_Server::Channel::t_ChannelMemberSet & member_set)
{
	member_set.erase(member);
	// if (size() == 0)
	// 	_parent_server.remove_channel(getChannelName());	//ownership is now inherited, so last channel member will always be owner.
}

void	IRC_Server::Channel::removeAllMembers(void)
{
	t_ChannelMemberSet::iterator	it;
	size_t							owneri = 0;

	for ( ; owneri < !getOwner().empty(); ++owneri)
		removeMember(getOwner());
	for (it = getChanops().begin(); it != getChanops().end(); )
		removeMember(it++, _chanops);
	for (it = getHalfops().begin(); it != getHalfops().end(); )
		removeMember(it++, _halfops);
	for (it = getUsers().begin(); it != getUsers().end(); )
		removeMember(it++, _users);
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

/*!
** @brief	If @a sender is a NULL pointer, then @a message will be sent to all
**			channel members at or above @a privilege_level, otherwise it will be
**			sent to all channel members at or above @a privilege_level EXCEPT
**			@a sender.
**
** @details	The privilege levels are '%' for half-ops and above, '&' for chanops
**			and above, '~' for owner and 0 for everyone. If an invalid
**			privilege_level is passed, nothing will be done.
** @param	sender			Address of the client instance that sent the message
**							or NULL.
** @param	privilege_level	The lowest desired privilege_level of message
**							recipients.
** @param	message			The message to send to recipients.
** @return	true if message was sent, false if privilege_level was invalid
*/
bool	IRC_Server::Channel::send_msg(IRC_Server::Client const * sender, char privilege_level, std::string const & message) const
{
	IRC_Server::Client const *	recipient = NULL;

	//debug
	std::cerr << message << std::endl;
	//debug

	switch (privilege_level)
	{
		case 0 :
			for (t_ChannelMemberSet::iterator it = _users.begin(), end = _users.end(); it != end; ++it)
				if ((recipient = _parent_server.find_client_by_nick(*it)) != NULL && recipient != sender)
					recipient->send_msg(message);
		case '%' :
			for (t_ChannelMemberSet::iterator it = _halfops.begin(), end = _halfops.end(); it != end; ++it)
				if ((recipient = _parent_server.find_client_by_nick(*it)) != NULL && recipient != sender)
					recipient->send_msg(message);
		case '@' :
			for (t_ChannelMemberSet::iterator it = _chanops.begin(), end = _chanops.end(); it != end; ++it)
				if ((recipient = _parent_server.find_client_by_nick(*it)) != NULL && recipient != sender)
					recipient->send_msg(message);
		case '~' :
			if ((recipient = _parent_server.find_client_by_nick(_owner)) != NULL && recipient != sender)
				recipient->send_msg(message);
			return true;
		default :
			return false;
	}
}

/*!
** @brief	Returns the total number of channel members.
**
** @return	The total number of channel members.
*/
size_t	IRC_Server::Channel::size(void) const
{
	size_t memberc = 0;

	memberc += _owner.empty() ? 0 : 1;
	memberc += _chanops.size();
	memberc += _halfops.size();
	memberc += _users.size();
	return memberc;
}
