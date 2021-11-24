#include <ctime>
#include "database.hpp"

Database::Database(const std::string& name) : hasher(), sql(soci::sqlite3, name) {}

bool Database::create_user(const std::string& user, const std::string& pass) {
    const std::string salt = std::to_string(std::time(nullptr));
    const std::string hash = this->hasher.hash(salt + pass);
    const std::string db_pass = salt + "$" + hash;

    try {
        this->sql << "INSERT INTO users(username, password) VALUES(:user, :pass)",
            soci::use(user), soci::use(db_pass);
    } catch (soci::soci_error &e) {
        // Username already exists in database
        return false;
    }

    return true;
}

bool Database::check_user_login(const std::string& user, const std::string& pass) {
    std::string db_pass;
    soci::indicator ind;
    this->sql << "SELECT password FROM users WHERE username = :user",
        soci::into(db_pass, ind), soci::use(user);

    if (ind != soci::i_ok) {
        // User does not exist
        return false;
    }

    std::size_t split_index = db_pass.find('$');
    const std::string salt(db_pass, 0, split_index);
    const std::string db_hash(db_pass, split_index + 1);
    const std::string user_hash = this->hasher.hash(salt + pass);

    return user_hash == db_hash;
}
