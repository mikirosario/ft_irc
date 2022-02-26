#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ircserv.hpp"

class User
{
	public:
		User(){};
		User(std::string other_nickname){nickmame = other_nickname;};
		~User(void){};

		std::string getNick(){return(this->nickmame);}
		
		User &	operator=(User const &other)
		{
			if (*this == other)
				this->nickmame = other.nickmame;
			return(*this);
		}

		bool operator==(const User &other) const 
		{						
			return (this->nickmame == other.nickmame);
		}
	private:
		std::string nickmame;
};

#endif