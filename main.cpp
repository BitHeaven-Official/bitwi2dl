#include <iostream>
#include <string>
#include <curl/curl.h>
#include <thread>
#include <unistd.h>
#include <sys/stat.h>
#include <future>
#include <chrono>
#include <fstream>
#include <regex>

#include "inc/getosname.h"
#include "inc/json.hpp"

#define LOG(x) cout << "[INFO] " << x << endl
#define CURLRES(x, y) cout << "-- result" << endl << x << endl << y << endl << "-- end result" << endl
#define ERR(x) cout << "\033[1;31m" << "[ERROR] " << x << "\033[0m" << endl
#define DEBUG 0


using namespace std;
using namespace nlohmann;


static const char* bearerToken = "Authorization: Bearer AAAAAAAAAAAAAAAAAAAAAPYXBAAAAAAACLXUNDekMxqa8h%2F40K4moUkGsoc%3DTYfbDKbT3jJPCEVnMYqilB28NHfOPqkca3qaAxGfsyKCs0wRbw";
static char* USER = new char[255];
static char* FRITTER = new char[255];
static char* OUTPATH = new char[255];
static int THREADS = 1;
static int ACTIVE_THREADS = 0;
static int USER_THREADS = 0;
static int RUNNING = 1;
static int TWEETS_PER_CURSOR = 200;


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

    if(j["errors"].is_array()) {
        ERR(j["errors"][0]["message"]);
        exit(1);
    }

    string jstr = j["guest_token"];
    char* out = new char[512];
    strcpy(out, jstr.c_str());

    return out;
}

void getLoad() {
    while(RUNNING) {
        this_thread::sleep_for(chrono::milliseconds(500));
        LOG("Threads: " << ACTIVE_THREADS << "/" << THREADS);
    }
}

void getActiveUsers() {
    while(RUNNING) {
        this_thread::sleep_for(chrono::milliseconds(2500));
        LOG("User threads: " << USER_THREADS << "/" << THREADS);
    }
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

    if(j["errors"].is_array()) {
        ERR(j["errors"][0]["message"]);
        exit(1);
    }

    LOG("User RestID: " << j["data"]["user"]["rest_id"]);
    LOG("User Tweets: " << j["data"]["user"]["legacy"]["statuses_count"]);

    string preout = "{\"user_id\": \"" + (string)j["data"]["user"]["rest_id"]
                    + "\", \"tweets\": " + to_string(j["data"]["user"]["legacy"]["statuses_count"]) + "}";

    json out = json::parse(preout);

    return out;
}

json getTweets(char* guestToken, char* userId, int count, string sCursor = "") {
    char* twitterApi = new char[512];
    char* cursor = new char[32];
    strcpy(cursor, sCursor.c_str());
    LOG("Cursor: " << cursor);

    int curCount = count;
    int useCount = curCount;
    if(curCount > TWEETS_PER_CURSOR) {
        useCount = TWEETS_PER_CURSOR;
        curCount -= TWEETS_PER_CURSOR;
    }
    else {
        curCount = 0;
    }

    if(strcmp(cursor, "") != 0) {
        sCursor = regex_replace(sCursor, regex("\\+"), "%2B");
        strcpy(cursor, ("&cursor=" + sCursor).c_str());
    }

    strcpy(twitterApi, (
            "https://api.twitter.com/2/timeline/profile/" + (string)userId
            + ".json?count=" + to_string(useCount)
            + "&userId=" + (string)userId
            + (string)cursor
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

    if(out["errors"].is_array()) {
        ERR(out["errors"][0]["message"]);
        exit(1);
    }

    for (json::iterator it = out["timeline"]["instructions"].begin(); it != out["timeline"]["instructions"].end(); ++it) {

        for (json::iterator jt = (*it).begin(); jt != (*it).end(); ++jt) {
            if((*jt)["addEntries"].is_array()) {

                for (json::iterator kt = (*jt)["addEntries"]["entries"].begin(); kt != (*jt)["addEntries"]["entries"].end(); ++kt) {
                    if((*kt)["content"]["operation"]["cursor"]["cursorType"].is_string()) {
                        if(!((string)(*kt)["content"]["operation"]["cursor"]["cursorType"]).compare("Bottom")) {
                            sCursor = (*kt)["content"]["operation"]["cursor"]["value"];
                        }
                    }
                }
            }
            else if((*jt)["entries"].is_array()) {
                for (json::iterator kt = (*jt)["entries"].begin(); kt != (*jt)["entries"].end(); ++kt) {
                    if((*kt)["content"]["operation"]["cursor"]["cursorType"].is_string()) {
                        if(!((string)(*kt)["content"]["operation"]["cursor"]["cursorType"]).compare("Bottom")) {
                            sCursor = (*kt)["content"]["operation"]["cursor"]["value"];
                        }
                    }
                }
            }
        }
    }

    out = out["globalObjects"];
    out["count"] = curCount;
    out["cursor"] = (string)sCursor;

    return out;
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

void none() {
    return;
}

void downloadFile(char* URL, char* filename) {
    CURL *curl;
    FILE *fp;
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }
}

inline bool fileExists (const char* name) {
    struct stat buffer;
    return (stat (name, &buffer) == 0);
}

void dlThread(string URL, string filename) {
    char* iFilename = (char*)filename.c_str();
    char* iURL = (char*)URL.c_str();
    downloadFile(iURL, iFilename);
    LOG("End thread");
    ACTIVE_THREADS--;
}

void userThread(string sUsername) {
    char* guestToken = twiAuth();
    vector<future<void>> tasks;

    char* username = (char*)sUsername.c_str();
    json profile = getProfile(guestToken, username);

    mkdir((char*)((string)OUTPATH + "/" + (string)username).c_str(), 0777);

    string sUserId = profile["user_id"];
    char* userId = (char*)sUserId.c_str();
    int tweetsCount = (int)profile["tweets"];

    if(!tweetsCount) {
        ERR("User " << username << " no have tweets!");
        return;
    }

    json tweets;
    tweets = getTweets(guestToken, (char*)userId, tweetsCount);

    int totalCount = (int)tweets["count"];
    char* cursor = new char[32];
    strcpy(cursor, ((string)tweets["cursor"]).c_str());

    tweets = tweets["tweets"];

    while(totalCount > 0) {
        json tweet, videoTweet, videoBitrates;
        int bitrate;
        char *mediaUrl = new char[255];
        char filename[FILENAME_MAX];
        for (json::iterator it = tweets.begin(); it != tweets.end(); ++it) {
            tweet = (*it)["extended_entities"]["media"];

            for (json::iterator jt = tweet.begin(); jt != tweet.end(); ++jt) {
                if (strstr(((string) ((*jt)["type"])).c_str(), "video")) {
                    bitrate = 0;

                    videoBitrates = (*jt)["video_info"]["variants"];
                    for (json::iterator kt = videoBitrates.begin(); kt != videoBitrates.end(); ++kt) {
                        if ((*kt)["bitrate"].is_number() && (int) ((*kt)["bitrate"]) > bitrate) {
                            bitrate = (int) ((*kt)["bitrate"]);
                            strcpy(mediaUrl, ((string) ((*kt)["url"])).c_str());
                        }
                    }
                    reverse(mediaUrl, mediaUrl + strlen(mediaUrl));
                    strcpy(filename, mediaUrl);
                    reverse(mediaUrl, mediaUrl + strlen(mediaUrl));

                    strcpy(filename, strstr(filename, "4pm."));
                    strcpy(filename, ((string) filename).substr(0, ((string) filename).find("/", 0)).c_str());
                    reverse(filename, filename + strlen(filename));
                } else if (!strstr(((string) ((*jt)["media_url"])).c_str(), "video_thumb")) {
                    strcpy(mediaUrl, ((string) ((*jt)["media_url"])).c_str());

                    reverse(mediaUrl, mediaUrl + strlen(mediaUrl));
                    strcpy(filename, mediaUrl);
                    reverse(mediaUrl, mediaUrl + strlen(mediaUrl));

                    strcpy(filename, ((string) filename).substr(0, ((string) filename).find("/", 0)).c_str());
                    reverse(filename, filename + strlen(filename));
                } else {
                    LOG("Skip thumbnail");
                    continue;
                }

                strcpy(filename, ((string) OUTPATH + "/" + (string) username + "/" + ((string) filename)).c_str());

                if (fileExists(filename)) {
                    LOG("File exists, skipping");
                    continue;
                }

                while (1) {
                    this_thread::sleep_for(chrono::milliseconds(10));

                    if (ACTIVE_THREADS < THREADS) {
                        LOG("Filename: " << filename);
                        LOG("Download: " << mediaUrl);
                        ACTIVE_THREADS++;
                        tasks.push_back(async(dlThread, (string) mediaUrl, (string) filename));
                        break;
                    }
                }
            }
        }

        tweets = getTweets(guestToken, (char*)userId, totalCount, (string)cursor);
        totalCount = (int)tweets["count"];
        strcpy(cursor, ((string)tweets["cursor"]).c_str());
        tweets = tweets["tweets"];
    }

    USER_THREADS--;
    return;
}

void mainThread() {
    auto load = async(getLoad);
    auto usrs = async(getActiveUsers);

    if(strcmp(FRITTER, "")) {
        ifstream fritter(FRITTER);
        vector<future<void>> tasks;
        json jFritter;
        fritter >> jFritter;
        char* username = new char[32];

        jFritter = jFritter["subscriptions"];

        for (json::iterator it = jFritter.begin(); it != jFritter.end(); ++it) {
            strcpy(username, ((string)((*it)["screen_name"])).c_str());

            while(1) {
                this_thread::sleep_for(chrono::milliseconds(10));

                if(USER_THREADS < 1) {
                    LOG("Username: " << username);
                    USER_THREADS++;
                    tasks.push_back(async(userThread, (string)username));

                    this_thread::sleep_for(chrono::milliseconds(10000));
                    break;
                }
            }
        }
    }
    else {
        USER_THREADS++;
        userThread((string)USER);
    }

    RUNNING = 0;

    return;
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

    if(!strcmp(USER, "") && !strcmp(FRITTER, ""))
        help();

    LOG("System: " << os);

    mkdir((char*)OUTPATH, 0777);

    thread main(mainThread);
    main.join();

//    while(RUNNING) this_thread::sleep_for(chrono::milliseconds(100));

    ERR("End program. Press Enter to exit.");
    getchar();

    return 0;
}
