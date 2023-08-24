#include <gtest/gtest.h>
#include <curl/curl.h>
#include <spdlog/spdlog.h>
#include <string>

class JSONPlaceholderFixture : public ::testing::Test {
protected:
    CURL* curl;

    JSONPlaceholderFixture() {
        // Initialize cURL
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
    }

    ~JSONPlaceholderFixture() {
        // Cleanup cURL
        if (curl) {
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
    }
};

// Callback function to handle the received data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// Define a test case using the fixture
TEST_F(JSONPlaceholderFixture, FetchPosts) {
    if (!curl) {
        GTEST_SKIP();
    }

    // Set API URL
    std::string apiUrl = "https://jsonplaceholder.typicode.com/posts/1";

    // Set up cURL request
    curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());

    // Set the callback function to capture the response
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Check if request was successful
    ASSERT_EQ(res, CURLE_OK);

    // Use spdlog to log the captured response
    spdlog::info("Response: {}", response);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

