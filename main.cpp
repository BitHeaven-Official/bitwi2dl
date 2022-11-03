#include <iostream>
#include <string>
#include <curl/curl.h>
#include <thread>

#include "inc/getosname.h"
#include "inc/json.hpp"

#if 1
#define LOG(x) cout << "[INFO] " << x << endl
#define CURLRES(x, y) cout << "-- result" << endl << x << endl << y << endl << "-- end result" << endl
#else
#define LOG(x) ((void)0)
#define CURLRES(x, y) ((void)0)
#endif
#define ERR(x) cerr << "[ERROR] " << x << endl
#define DEBUG 1


using namespace std;
using namespace nlohmann;


static const char* bearerToken = "Authorization: Bearer AAAAAAAAAAAAAAAAAAAAAPYXBAAAAAAACLXUNDekMxqa8h%2F40K4moUkGsoc%3DTYfbDKbT3jJPCEVnMYqilB28NHfOPqkca3qaAxGfsyKCs0wRbw";


void tProcess(int i, double d, const string &s) {
    cout << i << ", " << d << ", " << s << endl;
}

CURL *curl;
CURLcode res;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void curlResult(long http_code, string readBuffer) {
    cout << "-- result" << endl << http_code << endl << readBuffer << endl << "-- end result" << endl;
}

void twiAuth(char* out) {
    const char* twitterApi = "https://api.twitter.com/1.1/guest/activate.json";
    struct curl_slist* headers = NULL;
    long http_code = 0;
    string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, twitterApi);

        headers = curl_slist_append(headers, bearerToken);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_cleanup(curl);

#ifdef DEBUG
        curlResult(http_code, readBuffer);
#endif

        json j = json::parse(readBuffer);
        string jstr = j["guest_token"];
        strcpy(out, jstr.c_str());
    }
    else {
        ERR("CURL umer");
    }
}

int main(int argc, char **argv) {
    char* guestToken = new char[30];
    string os = getOsName();

    thread auth(twiAuth, guestToken);
    LOG(guestToken);
    auth.join();

    LOG(guestToken);

//    try {
//    }
//    catch (const std::exception& e) {
//        cerr << "Request failed, error: " << e.what() << endl;
//    }

    return 0;
}
