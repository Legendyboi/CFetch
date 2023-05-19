#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <unistd.h>

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

std::string executeCommand(const std::string& command){
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if(pipe) {
        char buffer[128];
        while(!feof(pipe)){
            if(fgets(buffer, 128, pipe) != nullptr){
                result += buffer;
            }
        }
        pclose(pipe);
    }
    return result;
}

std::string getFormattedDataTime(){
    time_t now = time(nullptr);
    char formattedDataTime[50];
    strftime(formattedDataTime, sizeof(formattedDataTime), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return formattedDataTime;
}

int main(){

    std::string os = executeCommand("lsb_release -ds");
    std::string hostname;
    char hostnameBuffer[128];
    gethostname(hostnameBuffer, sizeof(hostnameBuffer));
    hostname = hostnameBuffer;
    std::string kernel = executeCommand("uname -r");
    std::string uptime = executeCommand("uptime -p");
    std::string shell = executeCommand("echo $SHELL");
    std::string DE_WM = executeCommand("echo $XDG_CURRENT");
    std::string terminal = executeCommand("echo $TERM");


    return 0;
}
