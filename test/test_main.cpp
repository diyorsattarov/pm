#include <gtest/gtest.h>
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
        logger = spdlog::basic_logger_mt("response_logger", "response.log");
        spdlog::set_default_logger(logger);
    }

    ~BaseFixture() {
        spdlog::drop("response_logger");
    }

    std::string PerformHttpRequest(const std::string& host, const std::string& target) {
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        beast::tcp_stream stream(io_context);
        auto const results = resolver.resolve(host, "http");
        stream.connect(results);
        beast::http::request<beast::http::string_body> req{beast::http::verb::get, target, 11};
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::user_agent, "Beast");
        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::write(stream, req);
        beast::http::read(stream, buffer, res);
        return res.body();
    }
};


class JSONParsingFixture : public BaseFixture {
protected:
    // You don't need to redefine the logger and PerformHttpRequest here
    // since they are already available from the base class.
};

TEST_F(JSONParsingFixture, ParseAndLogJSON) {
    std::string host = "jsonplaceholder.typicode.com";
    std::string target = "/posts/1";
    std::string response = PerformHttpRequest(host, target);
    nlohmann::json parsedJson;
    try {
        parsedJson = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("JSON parsing error: {}", e.what());
        return;
    }
    spdlog::info("Parsed JSON: {}", parsedJson.dump(4));  // Dump with 4-space indentation
}

class JSONErrorHandlingFixture : public BaseFixture {
protected:
    // You can add more specific setup functions for this fixture
    // if needed, or directly use the base class setup.

    // Example of an error handling test
    void ExpectErrorOnRequest(const std::string& host, const std::string& target) {
        ASSERT_THROW({
            try {
                std::string response = PerformHttpRequest(host, target);
            } catch (const std::exception& e) {
                spdlog::info("Caught exception: {}", e.what());
                throw;
            }
        }, std::exception);
    }
};

// Test case for non-existent host
TEST_F(JSONErrorHandlingFixture, RequestToNonExistentHost) {
    ExpectErrorOnRequest("nonexistent-host.example.com", "/posts/1");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}