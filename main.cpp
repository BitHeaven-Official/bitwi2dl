#include <iostream>
#include <string>
#include <curl/curl.h>
#include <thread>
#include <unistd.h>
#include <sys/stat.h>

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
#define DEBUG 0


using namespace std;
using namespace nlohmann;


static const char* bearerToken = "Authorization: Bearer AAAAAAAAAAAAAAAAAAAAAPYXBAAAAAAACLXUNDekMxqa8h%2F40K4moUkGsoc%3DTYfbDKbT3jJPCEVnMYqilB28NHfOPqkca3qaAxGfsyKCs0wRbw";
static char* USER = new char[255];
static char* FRITTER = new char[255];
static char* OUTPATH = new char[255];
static int THREADS = 1;
static int ACTIVE_THREADS = 0;
static int TWEETS_PER_PAGE = 200;


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static void curlBase(CURL *curl, char* URL, struct curl_slist* headers, string& out, CURLcode res, long httpCode) {
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, URL);

        headers = curl_slist_append(headers, bearerToken);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
        res = curl_easy_perform(curl);
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);

#if DEBUG
        CURLRES(httpCode, out);
#endif

    }
    else {
        ERR("CURL not found!");
        exit(1);
    }
}

char* twiAuth() {
    char* twitterApi = new char[512];
    strcpy(twitterApi, "https://api.twitter.com/1.1/guest/activate.json");
    struct curl_slist* headers = NULL;

    CURL *curl;
    CURLcode res;
    long httpCode = 0;
    string readBuffer;

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");

    curlBase(curl, twitterApi, headers, readBuffer, res, httpCode);
    LOG("Buffer: " << readBuffer);
    json j = json::parse(readBuffer);
    string jstr = j["guest_token"];
    char* out = new char[512];
    strcpy(out, jstr.c_str());

    return out;
}

json getProfile(char* guestToken, char* username) {
    char* twitterApi = new char[512];
    strcpy(twitterApi, ((string)"https://api.twitter.com/graphql/4S2ihIKfF3xhp-ENxvUAfQ/UserByScreenName?variables=%7B%22screen_name%22%3A%22"
                        + (string)username + (string)"%22%2C%22withHighlightedLabel%22%3Atrue%7D").c_str());
    LOG(twitterApi);
    struct curl_slist* headers = NULL;

    CURL *curl;
    CURLcode res;
    long httpCode = 0;
    string readBuffer;

    curl = curl_easy_init();
    headers = curl_slist_append(headers, ((string)"X-Guest-Token: " + (string)guestToken).c_str());

    curlBase(curl, twitterApi, headers, readBuffer, res, httpCode);

    json j = json::parse(readBuffer);

    LOG("User RestID: " << j["data"]["user"]["rest_id"]);
    LOG("User Tweets: " << j["data"]["user"]["legacy"]["statuses_count"]);

    string preout = "{\"user_id\": \"" + (string)j["data"]["user"]["rest_id"]
                    + "\", \"tweets\": " + to_string(j["data"]["user"]["legacy"]["statuses_count"]) + "}";

    json out = json::parse(preout);

    return out;
}

json getTweets(char* guestToken, char* userId, int count) {
    char* twitterApi = new char[512];
    strcpy(twitterApi, (
            "https://api.twitter.com/2/timeline/profile/" + (string)userId
            + ".json?count=" + to_string(count)
//            + "&userId=" + (string)userId
//            + "&cursor=" + to_string(offset)
            ).c_str());
    LOG(twitterApi);
    struct curl_slist* headers = NULL;

    CURL *curl;
    CURLcode res;
    long httpCode = 0;
    string readBuffer;

    curl = curl_easy_init();
    headers = curl_slist_append(headers, ((string)"X-Guest-Token: " + (string)guestToken).c_str());

    curlBase(curl, twitterApi, headers, readBuffer, res, httpCode);

    json out = json::parse(readBuffer);
    out = out["globalObjects"]["tweets"];
//    json j2;
//    int cntr = 0;
//    for (json::iterator it = j.begin(); it != j.end(); ++it) {
//        j2 = (*it)["entities"]["media"];
//        cntr++;
//
//        for (json::iterator jt = j2.begin(); jt != j2.end(); ++jt) {
//            LOG("Tweet: " << (*jt)["id_str"]);
//            string jstr = (*jt)["id_str"];
//            strcpy(out, jstr.c_str());
//        }
//    }
//    LOG("Count: " << cntr);

    return out;
}

void help() {
    cout << "Usage: bitwi2dl <args>" << endl;
    cout << "   -h                  Help" << endl;
    cout << "   -u <username>       Twitter user" << endl;
//    cout << "   -f <filepath>       Fritter json subs" << endl;
    cout << "   -t <int>            Count of threads (Default: 1)" << endl;
    cout << "   -o <path>           Output dir (Default: ./twitter)" << endl;
    exit(0);
}

void none() {
    return;
}

void downloadFile(char* URL, char* filename) {
    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        fp = fopen(filename, "wb");
        curl_easy_setopt(curl, CURLOPT_URL, URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }
}

void dlThread(char* URL, char* filename) {

}

void userThread(char* username) {
    char* guestToken = twiAuth();

    json profile = getProfile(guestToken, username);

    string sUserId = profile["user_id"];
    char* userId = (char*)sUserId.c_str();
    int tweetsCount = (int)profile["tweets"];

    json tweets;
    tweets = getTweets(guestToken, (char*)userId, tweetsCount);

    json tweet, videoTweet, videoBitrates;
    int bitrate;
    char* mediaUrl = new char[255];
    char filename[FILENAME_MAX];
    for (json::iterator it = tweets.begin(); it != tweets.end(); ++it) {
        tweet = (*it)["extended_entities"]["media"];

        for (json::iterator jt = tweet.begin(); jt != tweet.end(); ++jt) {
            if(strstr(((string)((*jt)["type"])).c_str(), "video")) {
                bitrate = 0;

                videoBitrates = (*jt)["video_info"]["variants"];
                for (json::iterator kt = videoBitrates.begin(); kt != videoBitrates.end(); ++kt) {
                    if((*kt)["bitrate"].is_number() && (int)((*kt)["bitrate"]) > bitrate) {
                        bitrate = (int)((*kt)["bitrate"]);
                        strcpy(mediaUrl, ((string)((*kt)["url"])).c_str());
                    }
                }
                reverse(mediaUrl, mediaUrl + strlen(mediaUrl));
                strcpy(filename, mediaUrl);
                reverse(mediaUrl, mediaUrl + strlen(mediaUrl));

                strcpy(filename, strstr(filename, "4"));
                strcpy(filename, ((string)filename).substr(0, ((string)filename).find("/", 0)).c_str());
                reverse(filename, filename + strlen(filename));
            }
            else if(!strstr(((string)((*jt)["media_url"])).c_str(), "video_thumb")) {
                strcpy(mediaUrl, ((string)((*jt)["media_url"])).c_str());

                reverse(mediaUrl, mediaUrl + strlen(mediaUrl));
                strcpy(filename, mediaUrl);
                reverse(mediaUrl, mediaUrl + strlen(mediaUrl));

                strcpy(filename, ((string)filename).substr(0, ((string)filename).find("/", 0)).c_str());
                reverse(filename, filename + strlen(filename));
            }
            else {
                LOG("Skip thumbnail");
                continue;
            }
            strcpy(filename, ((string)OUTPATH + "/" + ((string)filename)).c_str());
            LOG("Filename: " << filename);
            LOG("Download: " << mediaUrl);
            downloadFile(mediaUrl, filename);
        }
    }
}

void mainThread() {
    char* buf = new char[255];
    thread userThreads[THREADS];
    thread dlThreads[THREADS];

    userThreads[0] = thread(userThread, USER);
    userThreads[0].join();

//    for(int i = 0; THREADS > i; i++) {
//        LOG("---------------- THREAD " << i);
//
//    }
//    for(int i = 0; THREADS > i; i++) {
//    }

//    char* tweets = new char[30];
//    getProfile(buf, tweets, guestToken, USER);
//    getTweets(buf, guestToken, buf, tweets);
//    LOG("Buffer: " << buf);
}

int main(int argc, char **argv) {
    string os = getOsName();

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

    mkdir((char*)OUTPATH, 0777);

    thread main(mainThread);
    main.join();

    LOG("End program. Press any key to exit.");
    getchar();

    return 0;
}
