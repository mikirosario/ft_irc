#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "channel.hpp"

class IRC_Server
{
	class Database
    {
        private:

            std::string file_name;
            std::vector<Client> clients;

        public:

            Database(std::string file_name);
            void loadDatabase();
            void saveDatabase();
            std::vector<Client> find_user(std::string username);
            bool addClient(Client newClient);
            bool checkIfClientExists(std::string clientUsername);
            bool checkIfUserPasswordIsCorrect(std::string username, std::string password);
    };
};

#endif