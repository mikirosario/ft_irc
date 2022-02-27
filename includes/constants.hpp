/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constants.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/26 21:55:45 by mrosario          #+#    #+#             */
/*   Updated: 2022/02/27 14:19:06 by acortes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTANTS_H
# define CONSTANTS_H

#define INT_TO_STR( x ) static_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << x ) ).str() //itoa-like macro for std::string that saves me from loading up my functions with ugly stringstreams :p
#define MAX_CONNECTIONS 1024	//maximum number of simultaneous connections
#define MSG_BUF_SIZE 512		//maximum message length in IRC RFC including \r\n termination.
#define MAX_NICK_SIZE 9 		//maximum nickname length
#define MAX_HOSTNAME_SIZE 20	//maximum hostname size; //debug use clientaddr instead if this is too long
#define MAX_USERNAME_SIZE 25	//maximum username size
#define MAX_REALNAME_SIZE 35	//maximum real name size
#define MAX_PASS_ATTEMPTS 10	//maximum number of PASS commands allowed during registration before we reject connection

/* CHANNEL CONSTANTS */

#define CLIENT_USER 0
#define ADMIN_USER 1
#define CREATOR_USER 2

#define CLIENT_ALREADY_EXIST_RETURN	0
#define INVALID_PASSWORD_RETURN	-1



#endif
