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


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void twiAuth(char* out) {
    const char* twitterApi = "https://api.twitter.com/1.1/guest/activate.json";
    struct curl_slist* headers = NULL;

    CURL *curl;
    CURLcode res;
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
//        CURLRES(http_code, readBuffer);
#endif

        json j = json::parse(readBuffer);
        string jstr = j["guest_token"];
        strcpy(out, jstr.c_str());
    }
    else {
        ERR("CURL umer");
        exit(1);
    }
}

void getProfile(char* out, char* guestToken, char* username) {
    char* twitterApi = new char[512];
    strcpy(twitterApi, ((string)"https://api.twitter.com/graphql/4S2ihIKfF3xhp-ENxvUAfQ/UserByScreenName?variables=%7B%22screen_name%22%3A%22"
                        + (string)username + (string)"%22%2C%22withHighlightedLabel%22%3Atrue%7D").c_str());
    LOG(twitterApi);
    struct curl_slist* headers = NULL;

    CURL *curl;
    CURLcode res;
    long http_code = 0;
    string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, twitterApi);

        headers = curl_slist_append(headers, bearerToken);
        headers = curl_slist_append(headers, ((string)"X-Guest-Token: " + (string)guestToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_cleanup(curl);

#ifdef DEBUG
        CURLRES(http_code, readBuffer);
#endif

        json j = json::parse(readBuffer);
        LOG("User RestID: " << j["data"]["user"]["rest_id"]);
        string jstr = j["data"]["user"]["rest_id"];
        strcpy(out, jstr.c_str());
    }
    else {
        ERR("CURL not found!");
        exit(1);
    }
}

void getTweet(char* out, char* guestToken, char* userid, char* tweetid) {
    char* twitterApi = new char[512];
    strcpy(twitterApi, ((string)"https://api.twitter.com/2/timeline/profile/" + (string)userid + (string)".json?count=999").c_str());
    LOG(twitterApi);
    struct curl_slist* headers = NULL;

    CURL *curl;
    CURLcode res;
    long http_code = 0;
    string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, twitterApi);

        headers = curl_slist_append(headers, bearerToken);
        headers = curl_slist_append(headers, ((string)"X-Guest-Token: " + (string)guestToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_cleanup(curl);

#ifdef DEBUG
//        CURLRES(http_code, readBuffer);
#endif

        json j = json::parse(readBuffer);
        j = j["globalObjects"]["tweets"];
        json j2 = json::parse(readBuffer);
        for (json::iterator it = j.begin(); it != j.end(); ++it) {
            j2 = (*it)["entities"]["media"];
            for (json::iterator jt = j2.begin(); jt != j2.end(); ++jt) {
                LOG("Tweet: " << (*jt)["id_str"]);
                string jstr = (*jt)["id_str"];
                strcpy(out, jstr.c_str());
            }
        }
    }
    else {
        ERR("CURL not found!");
        exit(1);
    }
}

void getTweets(char* out, char* guestToken, char* userid) {
    char* twitterApi = new char[512];
    strcpy(twitterApi, ((string)"https://api.twitter.com/2/timeline/profile/" + (string)userid + (string)".json?count=601").c_str());
    LOG(twitterApi);
    struct curl_slist* headers = NULL;

    CURL *curl;
    CURLcode res;
    long http_code = 0;
    string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, twitterApi);

        headers = curl_slist_append(headers, bearerToken);
        headers = curl_slist_append(headers, ((string)"X-Guest-Token: " + (string)guestToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_cleanup(curl);

#ifdef DEBUG
//        CURLRES(http_code, readBuffer);
#endif

        json j = json::parse(readBuffer);
        j = j["globalObjects"]["tweets"];
        json j2 = json::parse(readBuffer);
        int cntr = 0;
        for (json::iterator it = j.begin(); it != j.end(); ++it) {
            j2 = (*it)["entities"]["media"];
            cntr++;

            for (json::iterator jt = j2.begin(); jt != j2.end(); ++jt) {
                LOG("Tweet: " << (*jt)["id_str"]);
                string jstr = (*jt)["id_str"];
                strcpy(out, jstr.c_str());
            }
        }
        LOG("Count: " << cntr);
    }
    else {
        ERR("CURL not found!");
        exit(1);
    }
}

void help() {
    cout << "Usage: bitwi2dl <args>" << endl;
    cout << "   -h                  Help" << endl;
    cout << "   -u <username>       Twitter user" << endl;
    cout << "   -f <filepath>       Fritter json subs" << endl;
    cout << "   -t <int>            Count of threads (Default: 1)" << endl;
    cout << "   -o <path>           Output dir (Default: ./twitter)" << endl;
    exit(0);
}

int main(int argc, char **argv) {
    char* buf = new char[255];
    char* guestToken = new char[30];
    string os = getOsName();

    char* USER = new char[255];
    int THREADS = 1;
    char* FRITTER = new char[255];
    char* OUTPATH = new char[255];
    strcpy(OUTPATH, "./twitter");

    try {
        LOG("Count args: " << argc - 1);

        for(int i = 1; i < argc; i += 2) {
            LOG("Arguments: " << argv[i] << " " << argv[i + 1]);
            if(!strcmp(argv[i], "-h"))
                help();
            else if(!strcmp(argv[i], "-u")) {
                strcpy(USER, argv[i + 1]);
                LOG("Set USER var: " << USER);
            }
            else if(!strcmp(argv[i], "-t")) {
                THREADS = atoi(argv[i + 1]);
                THREADS = THREADS > 0 ? THREADS : 1;
                LOG("Set THREADS var: " << THREADS);
            }
            else if(!strcmp(argv[i], "-f")) {
                strcpy(FRITTER, argv[i + 1]);
                LOG("Set FRITTER var: " << FRITTER);
            }
            else if(!strcmp(argv[i], "-o")) {
                strcpy(OUTPATH, argv[i + 1]);
                LOG("Set OUTPATH var: " << OUTPATH);
            }
        }
    }
    catch (const std::exception& e) {
        ERR(e.what());
    }

    LOG("User: " << USER);
    LOG("Threads: " << THREADS);
    LOG("Fritter: " << FRITTER);
    LOG("Outpath: " << OUTPATH);

    if(!USER && !FRITTER)
        help();

    LOG("System: " << os);

    thread auth(twiAuth, guestToken);
    auth.join();

    LOG(guestToken);

    getProfile(buf, guestToken, USER);
    getTweets(buf, guestToken, buf);
//    LOG("Buffer: " << buf);

    return 0;
}
