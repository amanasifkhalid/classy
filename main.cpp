#define CROW_MAIN

#include "crow.h"
#include "crow/middlewares/cookie_parser.h"

int main() {
    crow::App<crow::CookieParser> app;

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
        res.body = crow::mustache::load("index.html").render(ctx);
        res.end();
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

        res.body = crow::mustache::load("login.html").render();
        res.end();
    });


    CROW_ROUTE(app, "/account_login")
        .methods(crow::HTTPMethod::POST)
    ([&app] (const crow::request& req, crow::response& res) {
        auto url_params = crow::query_string("?" + req.body);
        const std::string user(url_params.get("user"));
        const std::string pass(url_params.get("pass"));

        if (user.empty() || pass.empty()) {
            res.code = 400;
            res.end();
            return;
        }

        auto& session = app.get_context<crow::CookieParser>(req);
        session.set_cookie("user", user);
        res.code = 303;
        res.redirect("/");
        res.end();
    });

    // CROW_ROUTE(app, "/account_create")
    //     .methods(crow::HTTPMethod::POST)
    // ([] (const crow::request& req) {

    // });

    app.multithreaded().run();
    return 0;
}
