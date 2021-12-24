#define CROW_MAIN

#include "database.hpp"
#include <crow.h>
#include <crow/middlewares/cookie_parser.h>
#include <exception>
#include <unordered_map>
#include <vector>

int main() {
    std::ios_base::sync_with_stdio(false);
    crow::App<crow::CookieParser> app;
    Database database("classy.db");

    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)
    ([&app] (const crow::request& req, crow::response& res) {
        // Check if user is logged in
        const auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
        }

        crow::mustache::context ctx;
        ctx["user"] = user;
        res.end(crow::mustache::load("index.html").render(ctx));
    });

    CROW_ROUTE(app, "/posts")
    ([&app, &database] (const crow::request& req, crow::response& res) {
        // Check if user is logged in
        const auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
        }

        const soci::rowset<soci::row>& rows = database.get_posts(user);
        std::unordered_map<std::string, std::vector<crow::json::wvalue> > cat_map;

        for (soci::rowset<soci::row>::const_iterator it = rows.begin();
            it != rows.end(); ++it) {
            const std::string& content = it->get<std::string>(0);
            const std::string& category = it->get<std::string>(1);
            cat_map[category].emplace_back(content);
        }

        crow::json::wvalue posts;

        for (auto it = cat_map.cbegin(); it != cat_map.cend(); ++it) {
            posts[it->first] = crow::json::wvalue(it->second);
        }

        res.end(posts.dump());
    });

    CROW_ROUTE(app, "/category").methods(crow::HTTPMethod::POST)
    ([&app, &database] (const crow::request& req, crow::response& res) {
        // Check if user is logged in
        const auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
        }

        try {
            const std::string cat_name(req.url_params.get("name"));
            database.create_category(cat_name, user);
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << e.what();
            res.code = 400;
        }

        res.end();
    });

    CROW_ROUTE(app, "/category/<int>").methods(crow::HTTPMethod::PATCH)
    ([&app, &database] (const crow::request& req, crow::response& res, const int cat_id) {
        // Check if user is logged in
        const auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
        }

        try {
            const std::string cat_name(req.url_params.get("name"));
            database.rename_category(cat_id, cat_name, user);
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << e.what();
            res.code = 400;
        }

        res.end();
    });

    CROW_ROUTE(app, "/category/<int>").methods(crow::HTTPMethod::DELETE)
    ([&app, &database] (const crow::request& req, crow::response& res, const int cat_id) {
        // Check if user is logged in
        const auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
        }

        try {
            database.delete_category(cat_id, user);
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << e.what();
            res.code = 400;
        }

        res.end();
    });

    CROW_ROUTE(app, "/post").methods(crow::HTTPMethod::POST)
    ([&app, &database] (const crow::request& req, crow::response& res) {
        // Check if user is logged in
        const auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
        }

        try {
            const std::string content(req.url_params.get("content"));
            const int category_id = std::atoi(req.url_params.get("cat-id"));
            database.create_post(content, category_id);
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << e.what();
            res.code = 400;
        }

        res.end();
    });

    CROW_ROUTE(app, "/post/<int>").methods(crow::HTTPMethod::PATCH)
    ([&app, &database] (const crow::request& req, crow::response& res, const int post_id) {
        // Check if user is logged in
        const auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
        }

        try {
            const std::string content(req.url_params.get("content"));
            database.update_post(post_id, user, content);
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << e.what();
            res.code = 400;
        }

        res.end();
    });

    CROW_ROUTE(app, "/post/<int>").methods(crow::HTTPMethod::DELETE)
    ([&app, &database] (const crow::request& req, crow::response& res, const int post_id) {
        // Check if user is logged in
        const auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
        }

        try {
            database.delete_post(post_id, user);
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << e.what();
            res.code = 400;
        }

        res.end();
    });

    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)
    ([&app] (const crow::request& req, crow::response& res) {
        // Check if user is logged in
        const auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (!user.empty()) {
            res.redirect("/");
            res.end();
        }

        res.end(crow::mustache::load("login.html").render());
    });

    CROW_ROUTE(app, "/logout")
    ([&app] (const crow::request& req, crow::response& res) {
        auto& session = app.get_context<crow::CookieParser>(req);
        session.set_cookie("user", "");
        res.redirect("/login");
        res.end();
    });

    CROW_ROUTE(app, "/create_account")
    ([&app] (const crow::request& req, crow::response& res) {
        // Check if user is logged in
        const auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (!user.empty()) {
            res.redirect("/");
            res.end();
        }

        res.end(crow::mustache::load("create_account.html").render());
    });

    CROW_ROUTE(app, "/change_password")
    ([&app] (const crow::request& req, crow::response& res) {
        // Check if user is logged in
        const auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
        }

        crow::mustache::context ctx;
        ctx["user"] = user;
        res.end(crow::mustache::load("change_password.html").render(ctx));
    });

    CROW_ROUTE(app, "/delete_account")
    ([&app] (const crow::request& req, crow::response& res) {
        // Check if user is logged in
        const auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
        }

        crow::mustache::context ctx;
        ctx["user"] = user;
        res.end(crow::mustache::load("delete_account.html").render(ctx));
    });

    CROW_ROUTE(app, "/account_login").methods(crow::HTTPMethod::POST)
    ([&app, &database] (const crow::request& req, crow::response& res) {
        try {
            const auto url_params = crow::query_string("?" + req.body);
            const std::string user_input(url_params.get("user"));
            const std::string pass_input(url_params.get("pass"));

            if (database.check_user_login(user_input, pass_input)) {
                auto& session = app.get_context<crow::CookieParser>(req);
                session.set_cookie("user", user_input);
                res.redirect("/");
                res.end();
            }
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << e.what();
        }

        crow::mustache::context ctx;
        ctx["msg"] = "Login failed; username or password may be incorrect.";
        ctx["back"] = "/login";
        res.end(crow::mustache::load("error.html").render(ctx));
    });

    CROW_ROUTE(app, "/account_create").methods(crow::HTTPMethod::POST)
    ([&app, &database] (const crow::request& req, crow::response& res) {
        try {
            const auto url_params = crow::query_string("?" + req.body);
            const std::string user_input(url_params.get("user"));
            const std::string pass_input(url_params.get("pass"));
            database.create_user(user_input, pass_input);

            auto& session = app.get_context<crow::CookieParser>(req);
            session.set_cookie("user", user_input);
            res.redirect("/");
            res.end();
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << e.what();
        }

        crow::mustache::context ctx;
        ctx["msg"] = "Account creation failed; try another username.";
        ctx["back"] = "/create_account";
        res.end(crow::mustache::load("error.html").render(ctx));
    });

    CROW_ROUTE(app, "/account_password").methods(crow::HTTPMethod::POST)
    ([&app, &database] (const crow::request& req, crow::response& res) {
        try {
            const auto& session = app.get_context<crow::CookieParser>(req);
            const std::string& user = session.get_cookie("user");
            const auto url_params = crow::query_string("?" + req.body);
            const std::string old_pass(url_params.get("old-pass"));
            const std::string new_pass(url_params.get("new-pass"));

            if (database.change_password(user, old_pass, new_pass)) {
                res.redirect("/");
                res.end();
            }
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << e.what();
        }

        crow::mustache::context ctx;
        ctx["msg"] = "Failed to change password; check your input.";
        ctx["back"] = "/change_password";
        res.end(crow::mustache::load("error.html").render(ctx));
    });

    CROW_ROUTE(app, "/account_delete").methods(crow::HTTPMethod::POST)
    ([&app, &database] (const crow::request& req, crow::response& res) {
        try {
            auto& session = app.get_context<crow::CookieParser>(req);
            const std::string& user = session.get_cookie("user");
            const auto url_params = crow::query_string("?" + req.body);
            const std::string pass(url_params.get("pass"));

            if (database.delete_account(user, pass)) {
                session.set_cookie("user", "");
                res.redirect("/login");
                res.end();
            }
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << e.what();
        }

        crow::mustache::context ctx;
        ctx["msg"] = "Failed to delete account; check your input.";
        ctx["back"] = "/delete_account";
        res.end(crow::mustache::load("error.html").render(ctx));
    });

    app.multithreaded().run();
    database.close();
    return 0;
}
