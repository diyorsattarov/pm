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
};

