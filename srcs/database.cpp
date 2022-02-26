#include "../includes/database.hpp"

Database::Database(std::string username_file)
{
    file_name = username_file;
    loadDatabase();
}

void Database::loadDatabase() 
{
    std::ifstream file(file_name);
    std::string username, password, buffer;
    size_t positionDelimiter;

    while(std::getline(file,buffer))
	{
        Client client;
        positionDelimiter = buffer.find(";");
        username = buffer.substr(0, positionDelimiter);
        password = buffer.substr(positionDelimiter + 1, buffer.size());
        client.set_username(username);
     //   client.set_password(password);    // ¿Hay alguna funcion para introducir el password ya hecha?
        clients.push_back(client);
    }
    file.close();
}

void Database::saveDatabase()
{
    std::ofstream file(file_name, std::ofstream::out);
    std::string outputLine;

    for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++ )
	{
    //  outputLine = it->get_username() + ";" + it->get_password()+"\n";  // Lo mismo que arriba. Necesito algo para obtener la contraseña del usuario
        std::cout << outputLine;
        file<<outputLine;
    }
    file.close();
}

bool Database::addClient(Client newClient) 
{
    clients.push_back(newClient);
    saveDatabase();
    return true;
}

bool Database::checkIfClientExists(std::string clientUsername) 
{
    for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
        if (clientUsername == it->get_username())
            return true;
    return false;
}

bool Database::checkIfUserPasswordIsCorrect(std::string username, std::string password) 
{
    loadDatabase();
    if (checkIfClientExists(username))
	{
   //     for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
   //         if ((it->get_username() == username) && (it->get_password() == password))  			 // Lo mismo que arriba. Necesito algo para obtener la contraseña del usuario
   //             return true;
    }
    return false;
}