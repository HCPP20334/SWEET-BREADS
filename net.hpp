#pragma once
#include <future>
#include <string>
#include <stdint.h>
#include "httplib.h"
#include "json.hpp"
#include <filesystem>
#include "obxrac32b64.hpp"
#include "logger.hpp"
struct nethttp
{
    using json = nlohmann::json;
    std::future<std::string> sFtable;
    std::future<std::string> sFGetTable;
    std::future<std::string> sFTestProxy;
    std::string sServerUrl = "127.0.0.1";
    int iServerPort = 4435;
    std::string sProxyHost = "";
    std::string sTestProxyURL = "nginx16218.hostkey.in";
    int iProxyPort = 0;
    std::vector<std::string> sVuser, sVscore, sVTimestump, sVdiff;
    Logger ln;
    void log(std::string data) {
        ln.send("   net "+data +"\n");
    }
    void cleanup() {
        sVuser.clear();
        sVscore.clear();
        sVTimestump.clear(); 
        sVdiff.clear();
    }
    bool bReq = false;
    std::string out = "";

    std::array<uint8_t, 128> keyRaw = {
    'b','c','1','d','a','7','4','a','5','2',
    'a','8','b','3','f','7','2','3','b','4','4',
    'a','0','2','0','5','c','b','6','5','d','e',
    'a','1','a','9','d','7','0','0','1','3','b',
    '6','e','0','2','d','8','8','6','6','5','1',
    '7','5','5','e','5','8','7','c','3','7','6',
    '3','f','b','1','e','a','1','c','7','2','a',
    'c','1','2','f','f','f','f','6','5','0','2',
    '1','0','d','f','c','7','1','7','d','8','1',
    '8','7','0','8','1','f','3','9','c','2','2',
    'e','6','1','4','d','3','e','f','1','8','d',
    '1','2','7','a','d','2','2','3',
    };
    std::string outKey = "";
    std::string key() {
        for (int k = 0; k < keyRaw.size(); k++) {
            outKey += keyRaw[k];
        }
        return outKey;
    }
    void isConnected(bool* flag, bool data) {
        if (flag != nullptr) {
            *flag = data;
            log("edit flag " + std::format("{} to {}", *flag, data));
        }
    }
    void sendResultToGetHttp(std::string sUser,uint64_t u64Score,std::string sDiff) {
        log(" call sendResultToGetHttp()");
        int iProxyPortSwp = iProxyPort;
        std::string sProxyHostSwp = sProxyHost;
        std::string sServerUrlSwp = sServerUrl;
        int iServerPortSwp = iServerPort;
        std::string data_0 = obxrac32b64(false, sUser, key()), data_1 = obxrac32b64(false, std::to_string(u64Score), key()), data_2 = obxrac32b64(false, sDiff, key());
        log(std::format(" send data [{},{},{}]",
            obxrac32b64(false, sUser, key()),
            obxrac32b64(false, std::to_string(u64Score), key()),
            obxrac32b64(false, sDiff, key())));
        sFGetTable = std::async(std::launch::async, [this,data_0, data_1,data_2, iProxyPortSwp, sProxyHostSwp, sServerUrlSwp, iServerPortSwp]() {
            httplib::Client cliOut(sServerUrlSwp, iServerPortSwp);
            if (!sProxyHostSwp.empty() && iProxyPortSwp != 0) {
                cliOut.set_proxy(sProxyHostSwp, iProxyPortSwp);
            }
            std::cout << data_0 << " " << data_1 << " " << data_2 << "\n";
            auto resOut = cliOut.Get("/table_write?user=" + data_0 + "&score=" + data_1 + "&diff="+data_2);
            std::cout << "resOut: " << &resOut << (resOut != nullptr) << "\n";
            if (resOut) {
                isConnected(&isConnectedsSendResultToGetHttpFlag, true);
                std::cout << " connected "<< resOut->status << resOut->body  << "\n";
                return resOut->body;
            }
            else {
                isConnected(&isConnectedsSendResultToGetHttpFlag, false);
            }
            int status_code = resOut ? resOut->status : -1;
            return std::string("Error: " + std::to_string(status_code));
        });
    }
    bool isConnectedGetHttpUpdateTableFlag = false;
    bool isConnectedsSendResultToGetHttpFlag = false;
   
    void getHttp() {
        log(" call getHttp()");
        sFtable = std::async(std::launch::async, [=]() {
            httplib::Client cli(sServerUrl, iServerPort);
            cli.set_connection_timeout(2, 0);
            if (!sProxyHost.empty() && iProxyPort != 0) {
                cli.set_proxy(sProxyHost, iProxyPort);
            }

            auto res = cli.Get("/table");
            if (res && res->status == 200) {
                log(" connected!");
                isConnected(&isConnectedGetHttpUpdateTableFlag, true);
                json j = json::parse(res->body);
                for (auto& val : j) {
                    sVuser.push_back(val.value("user", ""));
                    sVscore.push_back(val.value("score", ""));
                    sVdiff.push_back(val.value("diff", ""));
                    sVTimestump.push_back(val.value("timestamp", ""));
                }
                return res->body;
            }
            else {
                isConnected(&isConnectedGetHttpUpdateTableFlag, false);
                log("error connecting to server");
            }
            int status_code = res ? res->status : -1;
            return std::string("Error: " + std::to_string(status_code));
            });
    }
    bool bIsProxyValid = false;
    void TestProxy() {
        sFTestProxy = std::async(std::launch::async, [=,this]() {
            log(" (lnethttp) TestProxy() launch");
            httplib::Client cli(sTestProxyURL, 4444);
            cli.set_connection_timeout(2, 0);
            if (!sProxyHost.empty() && iProxyPort != 0) {
                log(" (lnethttp) TestProxy() proxy enabled");
                cli.set_proxy(sProxyHost, iProxyPort);
            }
            auto res = cli.Get("/getid");
            if (res) {
                log("(lnethttp) TestProxy() proxy: [nginx16218.hostkey.in/getid] connected");
                isConnected(&bIsProxyValid, true);
            }
            else {
                log("(lnethttp) TestProxy() proxy: [nginx16218.hostkey.in/getid] not connected");
                isConnected(&bIsProxyValid, false);
            }
            int status_code = res ? res->status : -1;
            return std::string("Error: " + std::to_string(status_code));
            });
    }
    void fmtResultToString(std::string& output,auto& u,auto& s, auto& d,auto& t){

        if (!u.empty() && !s.empty() && !d.empty() && !t.empty()) {
            for (uint64_t i = 0; i < u.size(); i++) {
                output += std::format("{} | {} | {} | {}\n", u[i], s[i], d[i], t[i]);
            }
        }
        else {
            output = "Данных нет!";
        }

    }
    void sendScore(std::string& sScore, std::string& sUser);
    std::string fmtTime() {
        SYSTEMTIME* lptime = new SYSTEMTIME();
        GetLocalTime(lptime);
        return std::format("{}:{}:{} | {} {} {}", lptime->wHour,
            lptime->wMinute,
            lptime->wSecond,
            lptime->wDay,
            lptime->wMonth,
            lptime->wYear);
    }
};