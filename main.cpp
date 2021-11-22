#define CROW_MAIN

#include "cpp/crow_all.h"
#include "cpp/database.hpp"

int main() {
    std::ios_base::sync_with_stdio(false);
    crow::App<crow::CookieParser> app;
    Database database("classy.db");

    CROW_ROUTE(app, "/")([&app] (const crow::request& req, crow::response& res) {
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

    CROW_ROUTE(app, "/login")
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

    CROW_ROUTE(app, "/account_login")
        .methods(crow::HTTPMethod::POST)
    ([&app, &database] (const crow::request& req, crow::response& res) {
        auto url_params = crow::query_string("?" + req.body);
        const std::string user_input(url_params.get("user"));
        std::string pass_input(url_params.get("pass"));
        const bool success = database.check_user_login(user_input, pass_input);
        std::fill(pass_input.begin(), pass_input.end(), '0');

        if (success) {
            auto& session = app.get_context<crow::CookieParser>(req);
            session.set_cookie("user", user_input);
            res.code = 303;
            res.redirect("/");
            res.end();
            return;
        }

        crow::json::wvalue res_body;
        res_body["msg"] = "Username or password incorrect";
        res.code = 403;
        res.end(res_body.dump());
    });

    CROW_ROUTE(app, "/account_create")
        .methods(crow::HTTPMethod::POST)
    ([&app, &database] (const crow::request& req, crow::response& res) {
        auto url_params = crow::query_string("?" + req.body);
        const std::string user_input(url_params.get("user"));
        std::string pass_input(url_params.get("pass"));

        if (database.create_user(user_input, pass_input)) {
            auto& session = app.get_context<crow::CookieParser>(req);
            session.set_cookie("user", user_input);
            res.code = 303;
            res.redirect("/");
            res.end();
            return;
        }

        crow::json::wvalue res_body;
        res_body["msg"] = "Username taken";
        res.code = 409;
        res.end(res_body.dump());
    });

    app.multithreaded().run();
    return 0;
}
