/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv_numeric_replies.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ineumann <ineumann@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 15:40:22 by mrosario          #+#    #+#             */
/*   Updated: 2022/05/11 18:29:43 by ineumann         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ircserv.hpp"

/*!
** @brief	Builds the initial, invariant part of any numeric reply, consisting
**			of COLON<prefix>SPACE<numeric>SPACE<client>SPACE.
**
** @param	recipient	A reference to the client to whom the reply will be sent.
** @param	numeric		The numeric. ( see @a irc_numerics.hpp ).
** @return	A string containing the initial, invariant part of a numeric reply.
*/
std::string	IRC_Server::numeric_reply_start(Client const & recipient, char const * numeric) const
{
	std::string	rpl;

	//part of the msg with controlled size:
	//: IPv4Address SPACE NUMERIC SPACE CLIENT SPACE:
	//uncontrolled part:
	//description
	//NOTE: ensure NICK size is limited during registration...

	rpl += get_source() + " ";
	rpl += numeric;
	rpl += " ";
	if (recipient.is_registered() == true) //first parameter should be client name, but if client is unregistered this hasn't yet been recorded
		rpl += recipient.get_nick() + " "; //debug //will have a get_client() for formatted replies: nick!user@userIP
	return (rpl);
}

// std::string	IRC_Server::numeric_reply_start(Channel const & recipient, char const * numeric) const
// {
// 	std::string	rpl;

// 	rpl += get_source() + " ";
// 	rpl += numeric;
// 	rpl += " ";
// 	rpl += recipient.getChannelName() + " ";
// 	return (rpl);
// }

/*!
** @brief	The @a reply will be properly ended with the @a description, or with
**			an empty argument if an empty @a description is provided, and
**			and crlf-termination.
**
** @details	This function will automatically append a @a description to a
**			numeric reply as the final parameter and properly crlf-terminate it.
**			The @a description is optional and an empty string may be sent in
**			lieu of one. If including @a description would make @a reply longer
**			than MSG_BUF_SIZE, @a description will be truncated as needed to
**			fit.
**
**			This function will correctly terminate @a reply leaving it ready to
**			be sent to the client **IF** the calling function correctly filled
**			in the parameters for the given numeric. It does NOT guarantee
**			correctness of the string PRECEDING the final parameter, though it
**			WILL eliminate trailing spaces, NUL characters or premature
**			crlf-termination.
**
**			If @a reply was already crlf-terminated or terminated with a space
**			or space and colon, or null-terminated, all such existing
**			terminations will be erased and replaced.
** @param	reply		A reference to the numeric reply to be ended.
** @param	description	A reference to the description to append to the end of
**						the numeric reply as the final parameter.
*/
void		IRC_Server::numeric_reply_end(std::string & reply, std::string const & description) const
{
	size_t	bytes_remaining;
	size_t	pos;

	if ((pos = reply.find_last_not_of(" \r\n\0")) != std::string::npos)
		reply.erase(pos + 1, std::string::npos);
	reply += " :";
	bytes_remaining = MSG_BUF_SIZE - reply.size() - 2;
	reply.append(description, 0, bytes_remaining);
	reply += "\r\n";
}

void		IRC_Server::send_rpl_WELCOME(Client const & recipient)
{
	std::string msg = numeric_reply_start(recipient, RPL_WELCOME); 

	std::string	welcome_msg;

	welcome_msg += "Welcome to the ";
	welcome_msg += _networkname;
	welcome_msg += " Network, ";
	welcome_msg += recipient.get_nick(); //debug //might put this longname/mask into static memory
	welcome_msg += "!";
	welcome_msg += recipient.get_username();
	welcome_msg += "@";
	welcome_msg += recipient.get_hostname();

	numeric_reply_end(msg, welcome_msg);
	recipient.send_msg(msg);
}

void		IRC_Server::send_rpl_YOURHOST(Client const & recipient)
{
	std::string msg = numeric_reply_start(recipient, RPL_YOURHOST); 

	std::string	yourhost_msg;

	yourhost_msg += "Your host is ";
	yourhost_msg += _servername;
	yourhost_msg += ", running version ";
	yourhost_msg += VERSION; //debug //currently derived from git tag and passed through clang
	numeric_reply_end(msg, yourhost_msg);
	recipient.send_msg(msg);
}

void		IRC_Server::send_rpl_CREATED(Client const & recipient)
{
	std::string msg = numeric_reply_start(recipient, RPL_CREATED);
	std::string created_msg;

	created_msg += "This server was created ";
	created_msg += _server_creation_time;
	numeric_reply_end(msg, created_msg);
	recipient.send_msg(msg);
}

//debug //finish these
void		IRC_Server::send_rpl_MYINFO(Client const & recipient)
{
	std::string msg = numeric_reply_start(recipient, RPL_MYINFO); 
	msg += _servername + " ";
	msg += VERSION;
	msg += " o";

	numeric_reply_end(msg, "qoh");
	recipient.send_msg(msg);
}

void		IRC_Server::send_rpl_ISUPPORT(Client const & recipient)
{
	std::ostringstream	ss1;
	std::ostringstream	ss2;
	std::string msg_start = numeric_reply_start(recipient, RPL_ISUPPORT); 
	std::string msg;

	
	std::string 		isupport_msg;
	
	ss1	<< "AWAYLEN=" << AWAYLEN_MAX << " "
		<< "CASEMAPPING=ascii "
		<< "CHANLIMIT=" << SUPPORTED_CHANNEL_PREFIXES << ": "
		<< "CHANNELLEN=" << MAX_CHANNELNAME_SIZE << " "
		<< "CHANTYPES=" << ISUPPORT_CHANTYPES << " "
		<< "CHANMODES=" << ISUPPORT_CHANMODES << " "
		<< "ELIST= "
		<< "EXTBAN= "
		<< "KICKLEN=" << KICKLEN_MAX << " "
		<< "MAXLIST= "
		<< "PREFIX=(" << SUPPORTED_CHANNEL_MODES << ")" << SUPPORTED_CHANNEL_PREFIXES << " "
		<< "NICKLEN=" << MAX_NICK_SIZE << " "
		<< "HOSTLEN=" << MAX_HOSTNAME_SIZE << " ";
		
	ss2 << "USERLEN=" << MAX_USERNAME_SIZE
		<< "TOPICLEN=" << TOPICLEN_MAX << " "
		<< "STATUSMSG=" << SUPPORTED_CHANNEL_PREFIXES << " ";

	msg = msg_start + ss1.str();
	numeric_reply_end(msg, "are supported by this server");
	recipient.send_msg(msg);
	msg = msg_start + ss2.str();
	numeric_reply_end(msg, "are supported on this server");
	recipient.send_msg(msg);
	// //debug
	// std::cerr << "imprime " << msg << std::endl;
	// //debug
}

void		IRC_Server::send_rpl_UMODEIS(Client const & recipient)
{
	std::string msg = numeric_reply_start(recipient, RPL_UMODEIS);

	msg += recipient.get_modes();
	numeric_reply_end(msg, std::string());
	recipient.send_msg(msg);
}

//debug  //finish these

// Join: replies to command


void		IRC_Server::send_rpl_TOPIC(Client const & recipient, Channel const & channel)
{
	std::string msg = numeric_reply_start(recipient, RPL_TOPIC); 

	msg += channel.getChannelName() + " ";
	numeric_reply_end(msg, channel.getTopic());
	recipient.send_msg(msg);
}

void		IRC_Server::send_rpl_NOTOPIC(Client const & recipient, Channel const & channel)
{
	std::string msg = numeric_reply_start(recipient, RPL_NOTOPIC); 

	msg += channel.getChannelName() + " ";
	numeric_reply_end(msg, "No topic is set");
	recipient.send_msg(msg);
}

void		IRC_Server::send_rpl_CHANNELMODEIS(Client const & recipient, Channel const & channel)
{
	std::string msg = numeric_reply_start(recipient, RPL_CHANNELMODEIS);
	msg += channel.getChannelName() + " ";
	msg += channel.getModes();
	//debug //+ arguments??
	numeric_reply_end(msg, std::string());
	recipient.send_msg(msg);
}

void		IRC_Server::send_rpl_ENDOFBANLIST(Client const & recipient, Channel const & channel, std::string const & description)
{
	std::string msg = numeric_reply_start(recipient, RPL_ENDOFBANLIST);
	msg += channel.getChannelName() + " ";
	numeric_reply_end(msg, description);
	recipient.send_msg(msg);
}

void		IRC_Server::send_rpl_BANLIST(Client const & recipient, Channel const & channel)
{
	std::string msg_start = numeric_reply_start(recipient, RPL_BANLIST);
	msg_start += channel.getChannelName() + " ";
	Channel::t_ChannelMemberSet banlist = channel.getBanList();

	for (Channel::t_ChannelMemberSet::const_iterator banmask = banlist.begin(), end = banlist.end(); banmask != end; ++banmask)
	{
		std::string msg;

		msg = msg_start + *banmask;
		numeric_reply_end(msg, std::string());
		recipient.send_msg(msg);
	}
	send_rpl_ENDOFBANLIST(recipient, channel, "End of channel ban list");	
}

//debug //if user invisibility is implemented, we will need to account for this!!
/*!
** @brief	Builds and sends a NAMREPLY reply to @a recipient containing a list
**			of all members of @a channel and terminates with an ENDOFNAMES reply
**			when finished.
**
** @details If the NAMREPLY reply would overflow MSG_BUF_SIZE bytes, it will be
**			split into separate replies fitting into MSG_BUF_SIZE bytes.
**
** @param	recipient	The client to whom the reply will be sent.
** @param	channel		The channel regarding which member information was sent.
*/
void		IRC_Server::send_rpl_NAMREPLY(Client const & recipient, Channel const & channel)
{
	typedef IRC_Server::Channel::t_ChannelMemberSet::const_iterator chan_it;

	if (channel.size() > 0)
	{
		std::string msg = numeric_reply_start(recipient, RPL_NAMREPLY); 
		std::string member_list;

			//debug //will want channel to give us this
		msg += "= "; //debug //get channel status (= public, @ secret, *private), currently unimplemented so all channels are public
		msg += channel.getChannelName();

		member_list.reserve(MSG_BUF_SIZE * 2);
		for (size_t memberc = channel.size(); memberc > 0; ) //main loop
		{
			std::string	msg_cpy;
			size_t		bytes_used;
			size_t		owneri = 0;
			chan_it		chanops_it = channel.getChanops().begin();
			chan_it		halfops_it = channel.getHalfops().begin();
			chan_it 	user_it = channel.getUsers().begin();

			msg_cpy = msg;
			bytes_used = msg.size() + 4; //bytes used out of message buffer are current bytes occupied plus 4 for prepending " :" and appending "crlf".
			for ( ; owneri < !channel.getOwner().empty() && bytes_used + channel.getOwner().size() + 2 < MSG_BUF_SIZE; ++owneri, bytes_used += channel.getOwner().size() + 2, --memberc)
				member_list += "~" + channel.getOwner() + " ";
			for ( ; chanops_it != channel.getChanops().end() && bytes_used + chanops_it->size() + 2 < MSG_BUF_SIZE; ++chanops_it, bytes_used += chanops_it->size() + 2, --memberc)
				member_list += "@" + *chanops_it + " ";
			for ( ; halfops_it != channel.getHalfops().end() && bytes_used + halfops_it->size() + 2 < MSG_BUF_SIZE; ++halfops_it, bytes_used += halfops_it->size() + 2, --memberc)
				member_list += "%" + *halfops_it + " ";
			for ( ; user_it != channel.getUsers().end() && bytes_used + user_it->size() + 1 < MSG_BUF_SIZE; ++user_it, bytes_used += user_it->size() + 1, --memberc)
				member_list += *user_it + " ";
			numeric_reply_end(msg_cpy, member_list);
			recipient.send_msg(msg_cpy);
			//msg_cpy.erase(msg_cpy.begin() + msg.size(), msg_cpy.end()); alternative, but is it more efficient than msg_cpy = msg?? ;)
		}
	}
	send_rpl_ENDOFNAMES(recipient, channel.getChannelName());
}

void		IRC_Server::send_rpl_ENDOFNAMES(Client const & recipient, std::string const & channelName)
{
	std::string msg = numeric_reply_start(recipient, RPL_ENDOFNAMES); 

	msg += channelName;
	numeric_reply_end(msg, "End of /NAMES list");
	recipient.send_msg(msg);
}

void		IRC_Server::send_rpl_LISTSTART(Client const & recipient)
{
	std::string msg = numeric_reply_start(recipient, RPL_LISTSTART);
	numeric_reply_end(msg, "Channel :Users  Name");
	recipient.send_msg(msg);
}

void		IRC_Server::send_rpl_LIST(Client const & recipient, std::string const & channelName)
{
	
	 //"<client> <channel> <client count> :<topic>"

	std::string msg = numeric_reply_start(recipient, RPL_LIST);
	std::string copy_msg;

	copy_msg += channelName + " ";
	copy_msg += INT_TO_STR(_channels.find(channelName)->second.size());
	copy_msg += " " + _channels.find(channelName)->second.getTopic() + " ";
	numeric_reply_end(msg, copy_msg);
	recipient.send_msg(msg);
}

void		IRC_Server::send_rpl_LISTEND(Client const & recipient)
{
	std::string msg = numeric_reply_start(recipient, RPL_LISTSTART);
	numeric_reply_end(msg, "End of /LIST");
	recipient.send_msg(msg);
}

void	IRC_Server::send_rpl_INVITED(Client const & sender, Client const & target, Channel const & channel)
{
	std::string msg = numeric_reply_start(sender, RPL_INVITED);
	msg += channel.getChannelName() + " ";
	msg += target.get_username()  + " ";
	msg += sender.get_username()  + " : ";
	msg += target.get_username() + " has been invited by ";
	msg += sender.get_username();
	numeric_reply_end(msg, "Invited message");
	channel.send_msg(NULL, 0, msg); 
}

void	IRC_Server::send_rpl_INVITING(Client const & sender, Client const & target , Channel const & channel)
{
	std::string msg = numeric_reply_start(sender, RPL_INVITING);
	msg += target.get_nick() + " ";
	msg += channel.getChannelName();
	sender.send_msg(msg);
}