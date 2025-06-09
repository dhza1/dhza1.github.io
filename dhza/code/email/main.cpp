#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/Statement.h>
#include <iostream>
#include "crow_all.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cctype>
#include <random>

std::string generate_random_string(size_t length) {
    static const std::string charset =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    static thread_local std::mt19937 rng{std::random_device{}()};
    static thread_local std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);

    std::string result;
    result.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        result += charset[dist(rng)];
    }

    return result;
}

std::string url_decode(const std::string &value) {
    std::string result;
    char ch;
    int i, ii;
    for (i = 0; i < value.length(); i++) {
        if (value[i] == '%') {
            sscanf(value.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            result += ch;
            i += 2;
        } else if (value[i] == '+') {
            result += ' ';
        } else {
            result += value[i];
        }
    }
    return result;
}

std::vector<std::string> split_string(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    for (char c : s) {
        if (c == delimiter) {
            if (!token.empty()) tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

std::string read_file(const std::string& path) {
    std::ifstream in(path);
    if (!in) return "";
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

std::string domains = read_file("domains.txt");
std::vector<std::string> valid_domains = split_string(domains, '\n');

bool validate_token(SQLite::Database& db, std::string token) {
    SQLite::Statement query(db, "SELECT 1 FROM accounts WHERE token = ?");
    query.bind(1, token);

    return query.executeStep();
}


bool validate_domain(std::string to) {
    std::cout << split_string(to, '$').size() << std::endl;
    std::string domain = split_string(to, '$')[1];
    for (const auto d : valid_domains) {
        if (domain == d) {
            return true;
        }
    }
    return false;
}

crow::response send_email(SQLite::Database& db, std::string from, std::string to, std::string msg) {
    //  emails ('to' TEXT, 'from' TEXT, 'body' TEXT, 'timestamp' DATETIME DEFAULT CURRENT_TIMESTAMP)
    if (validate_domain(to) != true) {
        crow::response res;
        res.code = 69;
        res.body = "not a valid domain";

        return res;
    }

    SQLite::Statement query(db, "INSERT INTO emails ('to', 'from', 'body') VALUES (?, ?, ?)");

    query.bind(1, to);
    query.bind(2, from);
    query.bind(3, msg);

    query.exec();  // This runs the query and inserts the row

    crow::response res;
    res.code = 200;
    res.body = "sent";

    return res;
}

bool has_special_characters(const std::string& str) {
    for (char c : str) {
        if (!std::isalnum(static_cast<unsigned char>(c))) {
            return true;
        }
    }
    return false;
}

std::string get_param(const std::string& body, const std::string& key) {
    size_t start = body.find(key + "=");
    if (start == std::string::npos) return "";
    start += key.length() + 1;
    size_t end = body.find('&', start);
    if (end == std::string::npos) end = body.length();
    return url_decode(body.substr(start, end - start));
}


crow::response get_emails(SQLite::Database& db, std::string token) {
    SQLite::Statement q(db, "SELECT name FROM accounts WHERE token = ?");
    q.bind(1, token);
    if (!q.executeStep()) {
        crow::response res;
        res.code = 40;
        res.body = "account not found!";
        return res;
    }
    std::string name = q.getColumn(0); // 0 = name

    SQLite::Statement q_find_emails(db, "SELECT from, body FROM emails WHERE 'to' = ?");
    q_find_emails.bind(1, name);
    std::string body = "";

    while (q_find_emails.executeStep()) {
        std::string email_body = q_find_emails.getColumn(0).getText();
        std::string from_email = q_find_emails.getColumn(1).getText();

        body += email_body + ";" + from_email + "\n";
    }
    crow::response res;
    res.code = 200;
    res.body = body;
    return res;
}

crow::response signup(SQLite::Database& db, std::string username, std::string password) {
    SQLite::Statement query(db, "INSERT INTO accounts ('name', 'password', 'token') VALUES (?, ?, ?)");
    std::string token = generate_random_string(30);

    username = username + "$zmail.gg";
    if (!validate_domain(username)) {
        crow::response res;
        res.code = 1212;
        res.body = "invalid domain, why did you input a domain in username?";
        return res;
    }
    query.bind(1, username);
    query.bind(2, password);
    query.bind(3, token);
    if (!has_special_characters(username)) {
        crow::response res;
        res.code = 69;
        res.body = "Dont put special characters in your username!";
        return res;
    }
    query.exec();

    crow::response res;
    res.code = 200;
    res.body = "Your token is: " + token + "\nusername:" + username + "\nDont loose it!";
    return res;
}

int main() {
    SQLite::Database db("emails.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    std::cout << "Initializing db..." << std::endl;

    db.exec("CREATE TABLE IF NOT EXISTS accounts ('name' TEXT, 'password' TEXT, 'domain' TEXT, 'token' TEXT)");
    db.exec("CREATE TABLE IF NOT EXISTS emails ('to' TEXT, 'from' TEXT, 'body' TEXT, 'timestamp' DATETIME DEFAULT CURRENT_TIMESTAMP)");
    std::cout << "Initializing http server" << std::endl;

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        std::string content = read_file("index.html");

        crow::response res{content};
        res.add_header("Content-Type", "text/html");
        return res;
    });

    CROW_ROUTE(app, "/api")([](){
        return "<h1>Dont look here</h1>";
    });

    CROW_ROUTE(app, "/api/get_emails").methods(crow::HTTPMethod::POST)([&db](const crow::request& req){
        std::string token = get_param(req.body, "token");
        return get_emails(db, token);
    });


    CROW_ROUTE(app, "/api/send_email").methods(crow::HTTPMethod::POST)([&db](const crow::request& req){
        std::string body = req.body;

        std::string to, from, message;
        auto kvs = split_string(body, '&');
        for (const auto& kv : kvs) {
            auto pair = split_string(kv, '=');
            if (pair.size() != 2) continue;
            std::string key = url_decode(pair[0]);
            std::string value = url_decode(pair[1]);

            if (key == "to") to = value;
            else if (key == "from") from = value;
            else if (key == "message") message = value;
        }

        return send_email(db, from, to, message);
    });


    CROW_ROUTE(app, "/send")([](){
        std::string content = read_file("send.html");

        crow::response res{content};
        res.add_header("Content-Type", "text/html");
        return res;
    });

    CROW_ROUTE(app, "/signup")([](){
        std::string content = read_file("signup.html");

        crow::response res{content};
        res.add_header("Content-Type", "text/html");
        return res;
    });

    CROW_ROUTE(app, "/api/signup").methods(crow::HTTPMethod::POST)([&db](const crow::request& req) {
        std::string username = get_param(req.body, "username");
        std::string password = get_param(req.body, "password");

        return signup(db, username, password);
    });

    CROW_ROUTE(app, "/login")([](){
        std::string content = read_file("login.html");

        crow::response res{content};
        res.add_header("Content-Type", "text/html");
        return res;
    });

    CROW_ROUTE(app, "/token_login")([](){
        std::string content = read_file("token_login.html");

        crow::response res{content};
        res.add_header("Content-Type", "text/html");
        return res;
    });

    CROW_ROUTE(app, "/api/token_login").methods(crow::HTTPMethod::POST)([&db](crow::request& req, crow::response& res) {
        std::string token = get_param(req.body, "token");

        std::cout << "found login with " << token << std::endl;

        if (validate_token(db, token)) {
            res.add_header("Set-Cookie", "session_token=" + token + "; Path=/");

            res.write("logged in!");
        } else {
            res.code = 401;
            res.write("something went wrong!");
        }
        res.end();
    });

    CROW_ROUTE(app, "/inbox")([](){
        std::string content = read_file("inbox.html");

        crow::response res{content};
        res.add_header("Content-Type", "text/html");
        return res;
    });

    app.port(18080).multithreaded().run();
}
