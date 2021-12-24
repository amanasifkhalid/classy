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

    void create_user(const std::string& user, const std::string& pass);

    bool check_user_login(const std::string& user, const std::string& pass);

    bool change_password(const std::string& user, const std::string& old_pass,
        const std::string& new_pass);

    bool delete_account(const std::string& user, const std::string& pass);

    void create_category(const std::string& name, const std::string& owner);

    void rename_category(const int category_id, const std::string& name, const std::string& owner);

    void delete_category(const int category_id, const std::string& owner);

    void create_post(const std::string& content, const int category_id);

    void update_post(const int post_id, const std::string& owner, const std::string& content);

    void delete_post(const int post_id, const std::string& owner);

    soci::rowset<soci::row> get_posts(const std::string& user);

    void close() {
        this->sql.close();
    }
};

#endif
