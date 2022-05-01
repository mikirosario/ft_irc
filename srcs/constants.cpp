/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constants.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 17:34:46 by mrosario          #+#    #+#             */
/*   Updated: 2022/05/01 20:20:32 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include "../includes/constants.hpp"

char	get_mode_type(char mode)
{
	enum ChanType
	{
		A = 0,
		B,
		C,
		D
	}				type = A;
	char const *	modeptr;

	if (mode == ',' || (modeptr = std::strchr(ISUPPORT_CHANMODES, mode)) == NULL)
		return 0;
	for (size_t i = 0; ISUPPORT_CHANMODES[i] == mode; ++i)
		if (ISUPPORT_CHANMODES[i] == ',')
			type = static_cast<ChanType>(static_cast<int>(type) + 1);
	return (static_cast<int>(type) + 65);
}
