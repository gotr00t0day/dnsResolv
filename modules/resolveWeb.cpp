#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "resolveWeb.h"
#include "../includes/httplib.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define WHITE   "\033[37m"

std::mutex printMutex;

std::string getIP(const std::string& hostname) {
    std::string host = hostname;
    if (host.find("http://") == 0) host = host.substr(7);
    if (host.find("https://") == 0) host = host.substr(8);

    struct addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), nullptr, &hints, &res) != 0)
        return "Unable to resolve";

    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &((struct sockaddr_in*)res->ai_addr)->sin_addr,
              ipStr, sizeof(ipStr));
    freeaddrinfo(res);
    return std::string(ipStr);
}


void worker(std::queue<std::string>& jobs, std::mutex& jobMutex) {
    while (true) {
        std::string url;
        {
            std::lock_guard<std::mutex> lock(jobMutex);
            if (jobs.empty()) return;
            url = jobs.front();
            jobs.pop();
        }

        std::string domain = url;
        bool useSSL = false;
        if (domain.find("https://") == 0) { domain = domain.substr(8); useSSL = true; }
        else if (domain.find("http://") == 0) { domain = domain.substr(7); }

        std::string ip = getIP(domain);

        if (useSSL) {
            httplib::SSLClient sslCli(domain.c_str());
            sslCli.set_connection_timeout(5, 0);
            sslCli.set_read_timeout(10, 0);
            sslCli.set_follow_location(true);

            if (auto res = sslCli.Get("/")) {
                std::lock_guard<std::mutex> lock(printMutex);
                if (res->status >= 200 && res->status < 300)
                    std::cout << GREEN << "[+] " << WHITE << url << CYAN << " [" << ip << "] "
                              << MAGENTA << "-> " << GREEN << res->status << RESET << "\n";
                else if (res->status >= 300 && res->status < 400 && res->status != 304)
                    std::cout << YELLOW << "[*] " << WHITE << url << CYAN << " [" << ip << "] "
                              << MAGENTA << "-> " << YELLOW << res->status << " (redirected)" << RESET << "\n";
                else
                    std::cout << RED << "[-] " << WHITE << url << CYAN << " [" << ip << "] "
                              << MAGENTA << "-> " << RED << res->status << RESET << "\n";
            } else {
                std::lock_guard<std::mutex> lock(printMutex);
                std::cout << RED << "[-] " << WHITE << url << CYAN << " [" << ip << "] "
                          << MAGENTA << "-> " << RED << "Request failed!" << RESET << "\n";
            }
        } else {
            httplib::Client cli(domain.c_str());
            cli.set_connection_timeout(5, 0);
            cli.set_read_timeout(10, 0);
            cli.set_follow_location(true);

            if (auto res = cli.Get("/")) {
                std::lock_guard<std::mutex> lock(printMutex);
                if (res->status >= 200 && res->status < 300)
                    std::cout << GREEN << "[+] " << WHITE << url << CYAN << " [" << ip << "] "
                              << MAGENTA << "-> " << GREEN << res->status << RESET << "\n";
                else if (res->status >= 300 && res->status < 400 && res->status != 304)
                    std::cout << YELLOW << "[*] " << WHITE << url << CYAN << " [" << ip << "] "
                              << MAGENTA << "-> " << YELLOW << res->status << " (redirected)" << RESET << "\n";
                else
                    std::cout << RED << "[-] " << WHITE << url << CYAN << " [" << ip << "] "
                              << MAGENTA << "-> " << RED << res->status << RESET << "\n";
            } else {
                std::lock_guard<std::mutex> lock(printMutex);
                std::cout << RED << "[-] " << WHITE << url << CYAN << " [" << ip << "] "
                          << MAGENTA << "-> " << RED << "Request failed!" << RESET << "\n";
            }
        }
    }
}

// Resolve function
std::vector<std::string> Resolve(const std::string& file) {
    std::vector<std::string> urls;
    std::ifstream checkFile(file);
    if (!checkFile) { std::cerr << "File not found\n"; return urls; }

    std::string line;
    while (std::getline(checkFile, line))
        if (!line.empty()) urls.push_back(line);

    std::queue<std::string> jobs;
    for (auto& u : urls) jobs.push(u);

    std::mutex jobMutex;
    std::vector<std::thread> threads;
    int numWorkers = std::min(8, (int)urls.size());
    for (int i = 0; i < numWorkers; i++)
        threads.emplace_back(worker, std::ref(jobs), std::ref(jobMutex));

    for (auto& t : threads) t.join();
    return urls;
}
