#include "../includes/ircserv.hpp"



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

bool	IRC_Server::Channel::changeNick(std::string const & old_nick, std::string const & new_nick)
{
	typedef t_ChannelMemberSet::iterator member_it;
	t_ChannelMemberSet *	member_set = NULL;
	member_it				member;

		if (_owner == old_nick)
		{
			_owner = new_nick;
			return true;
		}
		if ((member = _chanops.find(old_nick)) != _chanops.end())
			member_set = &_chanops;
		else if ((member = _halfops.find(old_nick)) != _halfops.end())
			member_set = &_halfops;
		else if ((member = _users.find(old_nick)) != _users.end())
			member_set = &_users;
		if (member_set != NULL)
		{
			member_set->erase(old_nick);
			member_set->insert(new_nick);
			return true;
		}
		return false;
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
	else if (chan_it->second.isBanned(std::string(client.get_source()).erase(0, 1)))
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
}

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
	IRC_Server::Client *	recipient = NULL;

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
