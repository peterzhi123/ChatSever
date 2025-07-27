# ifndef USERMODEL_H
# define USERMODEL_H

# include "user.hpp"
# include "../db/MySQL.hpp"
# include<string>

using namespace std;

class UserModel
{
public:
    bool insert(User &user);

    User query(int id);

    bool updateState(User &user);
};

# endif