#define CROW_MAIN

#include "crow_all.h"
#include "database.hpp"
#include <exception>

int main() {
    std::ios_base::sync_with_stdio(false);
    crow::App<crow::CookieParser> app;
    Database database("classy.db");

    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)
    ([&app] (const crow::request& req, crow::response& res) {
        // Check if user is logged in
        auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
            return;
        }

        crow::mustache::context ctx;
        ctx["user"] = user;
        res.end(crow::mustache::load("index.html").render(ctx));
    });

    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)
        ([&app] (const crow::request& req, crow::response& res) {
        // Check if user is logged in
        auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (!user.empty()) {
            res.redirect("/");
            res.end();
            return;
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
        auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (!user.empty()) {
            res.redirect("/");
            res.end();
            return;
        }

        res.end(crow::mustache::load("create_account.html").render());
    });

    CROW_ROUTE(app, "/change_password")
        ([&app] (const crow::request& req, crow::response& res) {
        // Check if user is logged in
        auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
            return;
        }

        crow::mustache::context ctx;
        ctx["user"] = user;
        res.end(crow::mustache::load("change_password.html").render(ctx));
    });

    CROW_ROUTE(app, "/delete_account")
        ([&app] (const crow::request& req, crow::response& res) {
        // Check if user is logged in
        auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");

        if (user.empty()) {
            res.redirect("/login");
            res.end();
            return;
        }

        crow::mustache::context ctx;
        ctx["user"] = user;
        res.end(crow::mustache::load("delete_account.html").render(ctx));
    });

    CROW_ROUTE(app, "/account_login").methods(crow::HTTPMethod::POST)
    ([&app, &database] (const crow::request& req, crow::response& res) {
        auto url_params = crow::query_string("?" + req.body);
        const std::string user_input(url_params.get("user"));
        const std::string pass_input(url_params.get("pass"));
        bool success;

        try {
            success = database.check_user_login(user_input, pass_input);
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Database error: " << e.what() << '\n';
            success = false;
        }

        if (success) {
            auto& session = app.get_context<crow::CookieParser>(req);
            session.set_cookie("user", user_input);
            res.redirect("/");
            res.end();
            return;
        }

        crow::json::wvalue res_body;
        res_body["msg"] = "Login failed; username or password may be incorrect.";
        res.code = 403;
        res.end(res_body.dump());
    });

    CROW_ROUTE(app, "/account_create").methods(crow::HTTPMethod::POST)
    ([&app, &database] (const crow::request& req, crow::response& res) {
        auto url_params = crow::query_string("?" + req.body);
        const std::string user_input(url_params.get("user"));
        std::string pass_input(url_params.get("pass"));
        bool success;

        try {
            success = database.create_user(user_input, pass_input);
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Database error: " << e.what() << '\n';
            success = false;
        }


        if (success) {
            auto& session = app.get_context<crow::CookieParser>(req);
            session.set_cookie("user", user_input);
            res.redirect("/");
            res.end();
            return;
        }

        crow::json::wvalue res_body;
        res_body["msg"] = "Account creation failed; try another username";
        res.code = 409;
        res.end(res_body.dump());
    });

    CROW_ROUTE(app, "/account_change").methods(crow::HTTPMethod::POST)
    ([&app, &database] (const crow::request& req, crow::response& res) {
        auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");
        auto url_params = crow::query_string("?" + req.body);
        const std::string old_pass(url_params.get("old-pass"));
        const std::string new_pass(url_params.get("new-pass"));
        bool success;

        try {
            success = database.change_password(user, old_pass, new_pass);
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Database error: " << e.what() << '\n';
            success = false;
        }

        if (success) {
            res.redirect("/");
            res.end();
            return;
        }

        crow::json::wvalue res_body;
        res_body["msg"] = "Failed to change password; check your input.";
        res.code = 403;
        res.end(res_body.dump());
    });

    CROW_ROUTE(app, "/account_delete").methods(crow::HTTPMethod::POST)
    ([&app, &database] (const crow::request& req, crow::response& res) {
        auto& session = app.get_context<crow::CookieParser>(req);
        const std::string& user = session.get_cookie("user");
        auto url_params = crow::query_string("?" + req.body);
        const std::string pass(url_params.get("pass"));
        bool success;

        try {
            success = database.delete_account(user, pass);
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Database error: " << e.what() << '\n';
            success = false;
        }

        if (success) {
            session.set_cookie("user", "");
            res.redirect("/login");
            res.end();
            return;
        }

        crow::json::wvalue res_body;
        res_body["msg"] = "Failed to delete account; check your input.";
        res.code = 403;
        res.end(res_body.dump());
    });

    app.multithreaded().run();
    return 0;
}
