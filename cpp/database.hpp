#include "SHA512.h"
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

#ifndef DATABASE_HPP
#define DATABASE_HPP

class Database {
    SHA512 hasher;
    soci::session sql;

public:
    Database(const std::string& name);

    bool create_user(const std::string& user, const std::string& pass);

    bool check_user_login(const std::string& user, const std::string& pass);

    bool change_password(const std::string& user, const std::string& old_pass,
        const std::string& new_pass);

    bool delete_account(const std::string& user, const std::string& pass);

    void close() {
        this->sql.close();
    }
};

#endif
