/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_numerics.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:54:08 by miki              #+#    #+#             */
/*   Updated: 2022/04/24 01:03:19 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_NUMERICS_H
# define IRC_NUMERICS_H

/* -- RPL NUMERICS -- */

#define RPL_WELCOME "001"
#define RPL_YOURHOST "002"
#define RPL_CREATED "003"
#define RPL_MYINFO "004"
#define RPL_ISUPPORT "005"
#define RPL_UMODEIS "221"

/* -- RPL TO COMMANDS -- */


#define RPL_LISTSTART "321"
#define RPL_LIST "322"
#define RPL_LISTEND "323"
#define RPL_TOPIC "332"
#define RPL_TOPICWHOTIME "333"
#define RPL_INVITING "341"
#define RPL_NAMREPLY "353"
#define RPL_ENDOFNAMES "366"
#define ERR_UNKNOWNMODE	"472"


/* -- ERR NUMERICS -- */

#define ERR_UNKNOWNERROR "400"
#define ERR_TOOMANYCHANNELS "405"
#define ERR_NOSUCHNICK "401"
#define ERR_NOSUCHSERVER "402"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_NORECIPIENT "411"
#define ERR_NOTEXTTOSEND "412"
#define ERR_INPUTTOOLONG "417"
#define ERR_UNKNOWNCOMMAND "421"
#define ERR_NOMOTD "422"
#define ERR_NONICKNAMEGIVEN "431"
#define ERR_ERRONEOUSNICKNAME "432"
#define ERR_NICKNAMEINUSE "433"
#define ERR_USERNOTINCHANNEL "441"
#define ERR_NOTONCHANNEL "442"
#define ERR_USERONCHANNEL "443"
#define ERR_NOTREGISTERED "451"
#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREGISTERED "462"
#define ERR_PASSWDMISMATCH "464"
#define ERR_CHANNELISFULL "471"
#define ERR_INVITEONLYCHAN "473"
#define ERR_BANNEDFROMCHAN "474"
#define ERR_BADCHANNELKEY "475"
#define ERR_BADCHANMASK	"476"
#define ERR_CHANOPRIVSNEEDED "482"
#define ERR_UMODEUNKNOWNFLAG "501"
#define ERR_USERSDONTMATCH "502"

#endif
