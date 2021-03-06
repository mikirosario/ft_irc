/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constants.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikiencolor <mikiencolor@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/26 21:55:45 by mrosario          #+#    #+#             */
/*   Updated: 2022/06/13 22:51:56 by mikiencolor      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTANTS_H
# define CONSTANTS_H

#define INT_TO_STR( x ) static_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << x ) ).str() //itoa-like macro for std::string that saves me from loading up my functions with ugly stringstreams :p
#define MAX_CONNECTIONS 1024	//maximum number of simultaneous connections
#define MSG_BUF_SIZE 512		//maximum message length in IRC RFC including \r\n termination.
#define OUT_BUF_SIZE 1000448	//maximum output buffer length before user is kicked (about 1MB, multiple of 512 bytes)
#define MAX_NICK_SIZE 9 		//maximum nickname length
#define MAX_HOSTNAME_SIZE 20	//maximum hostname size;
#define MAX_USERNAME_SIZE 25	//maximum username size
#define MAX_REALNAME_SIZE 35	//maximum real name size
#define MAX_PASS_ATTEMPTS 10	//maximum number of PASS commands allowed during registration before we reject connection
#define AWAYLEN_MAX 200
#define KICKLEN_MAX 200
#define TOPICLEN_MAX 307

/* CHANNEL CONSTANTS */

#define CLIENT_USER 0
#define ADMIN_USER 1
#define CREATOR_USER 2
#define MAX_CHANNELNAME_SIZE 50
#define ISUPPORT_CHANTYPES "#"
#define ISUPPORT_CHANMODES "b,ko,,i"
#define SUPPORTED_CHANNEL_PREFIXES "~@%" //we support Founder, Operator, HalfOp || //Founder (q)~ // Operator (o)@ // HalfOp(h)%
#define SUPPORTED_CHANNEL_MODES "biko" 
#define SUPPORTED_USER_MODES "oi"
#define UNSUPPORTED_CHANNEL_PREFIXES "&+" //we don't support Protected or Voice
#define INVALID_PASSWORD_RETURN	-1

/*!
** @brief	Returns channel mode type of @a mode.
**
** @details	Channel mode types are A, B, C and D.
**
**			Types B and C have mandatory parameters.
**
**			Type A are modes associated with lists (like a ban list) with
**			optional parameters.
**
**          Type D has no parameters.
** @param	mode	Channel mode.
** @return	Mode type of @a mode.
*/
char	get_mode_type(char mode);

#endif
