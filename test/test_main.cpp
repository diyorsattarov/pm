#include <gtest/gtest.h>
#include <curl/curl.h>
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
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

