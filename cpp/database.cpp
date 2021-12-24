#include <ctime>
#include "database.hpp"

Database::Database(const std::string& name) : hasher(), sql(soci::sqlite3, name) {}

void Database::create_user(const std::string& user, const std::string& pass) {
    const std::string& salt = std::to_string(std::time(nullptr));
    const std::string& hash = this->hasher.hash(salt + pass);
    const std::string& db_pass = salt + "$" + hash;

    // Throws error if user already exists
    this->sql << "INSERT INTO users(username, password) VALUES(:user, :pass)",
        soci::use(user), soci::use(db_pass);
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

bool Database::change_password(const std::string& user, const std::string& old_pass,
    const std::string& new_pass) {
    if (!this->check_user_login(user, old_pass)) {
        return false;
    }

    const std::string& salt = std::to_string(std::time(nullptr));
    const std::string& hash = this->hasher.hash(salt + new_pass);
    const std::string& db_pass = salt + "$" + hash;
    soci::indicator ind;

    // Throws soci_error
    this->sql << "UPDATE users SET password = :pass WHERE username = :user",
        soci::use(new_pass), soci::use(user);
    return true;
}

bool Database::delete_account(const std::string& user, const std::string& pass) {
    if (!this->check_user_login(user, pass)) {
        return false;
    }

    // Throws soci_error
    this->sql << "DELETE FROM users WHERE username = :user", soci::use(user);
    return true;
}

void Database::create_category(const std::string& name, const std::string& owner) {
    // Throws soci_error
    this->sql << "INSERT INTO categories(name, owner) VALUES(:name, :owner)",
        soci::use(name), soci::use(owner);
}

void Database::rename_category(const int category_id, const std::string& name, const std::string& owner) {
    // Throws soci_error
    this->sql << "UPDATE categories SET name = :name WHERE id = :id AND owner = :owner",
        soci::use(name), soci::use(category_id), soci::use(owner);
}

void Database::delete_category(const int category_id, const std::string& owner) {
    // Throws soci_error
    this->sql << "DELETE FROM categories WHERE id = :id AND owner = :owner",
        soci::use(category_id), soci::use(owner);
}

void Database::create_post(const std::string& content, const int category_id) {
    // Throws soci_error
    this->sql << "INSERT INTO posts(content, categoryid) "
        "VALUES(:content, :categoryid)", soci::use(content), soci::use(category_id);
}

void Database::update_post(const int post_id, const std::string& owner, const std::string& content) {
    // Throws soci_error
    this->sql << "UPDATE posts INNER JOIN categories ON "
        "posts.categoryid = categories.id SET content = :content WHERE posts.id = :id "
        "AND categories.owner = :owner", soci::use(content), soci::use(post_id),
        soci::use(owner);
}

void Database::delete_post(const int post_id, const std::string& owner) {
    // Throws soci_error
    this->sql << "DELETE FROM posts INNER JOIN categories ON posts.categoryid = categories.id "
        "WHERE posts.id = :id AND owner = :owner", soci::use(post_id), soci::use(owner);
}

soci::rowset<soci::row> Database::get_posts(const std::string& user) {
    return (this->sql.prepare << "SELECT id, content, categoryid, "
        "categories.name as category FROM posts INNER JOIN categories ON "
        "posts.categoryid = categories.id WHERE owner = :user", soci::use(user));
}
