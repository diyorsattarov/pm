#include <gtest/gtest.h>
#include <curl/curl.h>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h> // Include the file sink heade
#include <nlohmann/json.hpp>
					  // r
class JSONPlaceholderFixture : public ::testing::Test {
protected:
    CURL* curl;
    std::shared_ptr<spdlog::logger> logger;
    std::string responseBuffer;  // Buffer to store the response data

    JSONPlaceholderFixture() {
        // Initialize cURL
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();

        // Initialize the logger to write to a file
        logger = spdlog::basic_logger_mt("response_logger", "response.log");
        spdlog::set_default_logger(logger);

        // Set up cURL options
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        }
    }

    ~JSONPlaceholderFixture() {
        spdlog::drop("response_logger");
        // Cleanup cURL
        if (curl) {
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
    }

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        // This callback is called by cURL to write response data
        JSONPlaceholderFixture* self = static_cast<JSONPlaceholderFixture*>(userp);
        size_t totalSize = size * nmemb;
        self->responseBuffer.append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }
};

// Define a test case using the fixture
TEST_F(JSONPlaceholderFixture, FetchPosts) {
    if (!curl) {
        GTEST_SKIP();
    }

    // Set API URL
    std::string apiUrl = "https://jsonplaceholder.typicode.com/posts/1";

    // Set up cURL request
    curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Check if request was successful
    ASSERT_EQ(res, CURLE_OK);

    // Write the response to the logger
    spdlog::info("Response: {}", responseBuffer);
}

class JSONParsingFixture : public ::testing::Test {
protected:
    std::shared_ptr<spdlog::logger> logger;

    JSONParsingFixture() {
        // Initialize the logger to write to a file
        logger = spdlog::basic_logger_mt("response_logger", "response.log");
        spdlog::set_default_logger(logger);
    }

    ~JSONParsingFixture() {
        // Cleanup
        spdlog::drop("response_logger");
    }

    std::string PerformCurlRequest(const std::string& apiUrl) {
        CURL* curl = curl_easy_init();
        std::string response;

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                spdlog::error("Curl request failed: {}", curl_easy_strerror(res));
            }

            curl_easy_cleanup(curl);
        }

        return response;
    }

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        std::string* response = static_cast<std::string*>(userp);
        size_t totalSize = size * nmemb;
        response->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }
};

TEST_F(JSONParsingFixture, ParseAndLogJSON) {
    std::string apiUrl = "https://jsonplaceholder.typicode.com/posts/1";

    // Perform cURL request and get response
    std::string response = PerformCurlRequest(apiUrl);

    // Parse the JSON
    nlohmann::json parsedJson;
    try {
        parsedJson = nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("JSON parsing error: {}", e.what());
        return;
    }

    // Log the parsed JSON
    spdlog::info("Parsed JSON: {}", parsedJson.dump(4));  // Dump with 4-space indentation
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}