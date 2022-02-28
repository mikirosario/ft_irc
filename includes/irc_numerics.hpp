/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_numerics.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 12:54:08 by miki              #+#    #+#             */
/*   Updated: 2022/02/28 15:56:21 by miki             ###   ########.fr       */
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

/* -- RPL TO COMMANDS -- */

#define RPL_TOPIC "332"
#define RPL_TOPICWHOTIME "333"
#define RPL_NAMREPLY "353"
#define RPL_ENDOFNAMES "366"

/* -- ERR NUMERICS -- */

#define ERR_UNKNOWNERROR "400"
#define ERR_TOOMANYCHANNELS "405"
#define ERR_NOSUCHNICK "401"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_NORECIPIENT "411"
#define ERR_NOTEXTTOSEND "412"
#define ERR_INPUTTOOLONG "417"
#define ERR_UNKNOWNCOMMAND "421"
#define ERR_NONICKNAMEGIVEN "431"
#define ERR_ERRONEOUSNICKNAME "432"
#define ERR_NICKNAMEINUSE "433"
#define ERR_NOTREGISTERED "451"
#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREGISTERED "462"
#define ERR_PASSWDMISMATCH "464"
#define ERR_CHANNELISFULL "471"
#define ERR_INVITEONLYCHAN "473"
#define ERR_BANNEDFROMCHAN "474"
#define ERR_BADCHANNELKEY "475"



#endif