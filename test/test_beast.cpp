#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h> // Include the file sink heade
#include <nlohmann/json.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;

class BaseFixture : public ::testing::Test {
protected:
    std::shared_ptr<spdlog::logger> logger;

    BaseFixture() {
        // Set up the logger as usual
        logger = spdlog::basic_logger_mt("response_logger", "response.log");
        spdlog::set_default_logger(logger);
    }

    ~BaseFixture() {
        spdlog::drop("response_logger");
    }

    static std::string PerformHttpRequest(
        const std::string& host, 
        const std::string& target, 
        const std::string& verb = "GET", 
        const std::string& user_agent = "Beast", 
        const std::string& requestBody = "", 
        const std::map<std::string, 
        std::string>& headers = {}) {
        
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        beast::tcp_stream stream(io_context);
        auto const results = resolver.resolve(host, "http");
        stream.connect(results);

        beast::http::request<beast::http::string_body> req{beast::http::string_to_verb(verb), target, 11};
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::user_agent, user_agent);

        for (const auto& header : headers) {
            req.set(header.first, header.second);
        }

        if (verb == "POST" || verb == "PUT") {
            req.body() = requestBody;
            req.prepare_payload();
        }

        beast::http::write(stream, req);

        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        return res.body();
    }
};

class GetRequestFixture : public BaseFixture {
protected:
    std::string response;

    void SetUp() override {
        std::ofstream ofs("response.log", std::ofstream::out | std::ofstream::trunc);
        ofs.close();
        // Perform a GET request to jsonplaceholder.typicode.com/posts/1
        std::string host = "jsonplaceholder.typicode.com";
        std::string target = "/posts/1";
        response = PerformHttpRequest(host, target);
    }
};

TEST_F(GetRequestFixture, TestGetRequest) {
    // Process the response or perform assertions if needed
    spdlog::info("Received response: {}", response);
    // Add your assertions here
}

class GetCommentsRequestFixture : public BaseFixture {
protected:
    std::string response;

    void SetUp() override {
        // Perform a GET request to jsonplaceholder.typicode.com/posts/1
        std::string host = "jsonplaceholder.typicode.com";
        std::string target = "/posts/1/comments";
        response = PerformHttpRequest(host, target);
    }
};

TEST_F(GetCommentsRequestFixture, TestGetRequest) {
    // Process the response or perform assertions if needed
    spdlog::info("Received response: {}", response);
    // Add your assertions here
}

class PutRequestFixture : public BaseFixture {
protected:
    std::string response;

    void SetUp() override {
        // Perform a PUT request to jsonplaceholder.typicode.com/posts/1
        std::string host = "jsonplaceholder.typicode.com";
        std::string target = "/posts/1";
        nlohmann::json requestBody = {
            {"id", 1},
            {"title", "foo"},
            {"body", "bar"},
            {"userId", 1}
        };
        response = PerformHttpRequest(host, target, "PUT", "My User Agent", requestBody.dump(), {{"Content-Type", "application/json"}});
    }
};

TEST_F(PutRequestFixture, TestPutRequest) {
    // Process the response or perform assertions if needed
    spdlog::info("Received response: {}", response);
    // Add your assertions here
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}