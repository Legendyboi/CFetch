#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

std::string executeCommand(const std::string& command) {
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != nullptr)
                result += buffer;
        }
        pclose(pipe);
    }
    return result;
}

std::string get_ppid(const std::string& pid) {
    std::string ppid;
    std::string proc_file = "/proc/" + (pid.empty() ? "$PPID" : pid) + "/status";
    std::ifstream file(proc_file);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("PPid:") != std::string::npos) {
                ppid = line.substr(line.find(':') + 1);
                ppid = ppid.substr(ppid.find_first_not_of(" \t"));
                break;
            }
        }
        file.close();
    }
    return ppid;
}

std::string get_process_name(const std::string& pid) {
    std::string name;
    std::string comm_file = "/proc/" + (pid.empty() ? "$PPID" : pid) + "/comm";
    std::ifstream file(comm_file);
    if (file.is_open()) {
        std::getline(file, name);
        file.close();
    }
    return name;
}

std::string getTerminal() {
    char* term = std::getenv("TERM");
    if (term != nullptr) {
        std::string termStr(term);

        // Remove "xterm-" prefix if it exists
        std::string xtermPrefix = "xterm-";
        if (termStr.find(xtermPrefix) == 0) {
            termStr = termStr.substr(xtermPrefix.length());
        }

        return termStr;
    } else {
        // Set a default value for TERM if it is not set
        return "unknown";
    }
}

std::string getShell() {
    std::string shell = executeCommand("echo $SHELL");
    shell = shell.substr(0, shell.find_last_not_of(" \n\r\t") + 1);
    return shell;
}

std::string getUsername() {
    std::string username = executeCommand("whoami");
    username = username.substr(0, username.find_last_not_of(" \n\r\t") + 1);
    return username;
}

std::string getHostname() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        return "";
    }
    return hostname;
}

std::string getKernelVersion() {
    std::string kernel_version = executeCommand("uname -r");
    kernel_version = kernel_version.substr(0, kernel_version.find_last_not_of(" \n\r\t") + 1);
    return kernel_version;
}

std::string getUptime() {
    std::string uptime = executeCommand("uptime -p");
    uptime = uptime.substr(0, uptime.find_last_not_of(" \n\r\t") + 1);

    std::string upPrefix = "up ";
    if (uptime.find(upPrefix) == 0) {
        uptime = uptime.substr(upPrefix.length());
    }

    return uptime;
}

std::string getDE() {
    std::string de;

    std::string xdg_session = executeCommand("echo $XDG_SESSION_DESKTOP");
    xdg_session = xdg_session.substr(0, xdg_session.find_last_not_of(" \n\r\t") + 1);

    if (!xdg_session.empty()) {
        de = xdg_session;
    }

    return de;
}

std::string getWM() {
    std::string wm;

    std::string x_window_manager = executeCommand("echo $XDG_CURRENT_DESKTOP");
    x_window_manager = x_window_manager.substr(0, x_window_manager.find_last_not_of(" \n\r\t") + 1);

    if (!x_window_manager.empty()) {
        wm = x_window_manager;
    } else {
        std::string window_manager = executeCommand("echo $WINDOW_MANAGER");
        window_manager = window_manager.substr(0, window_manager.find_last_not_of(" \n\r\t") + 1);
        wm = window_manager;
    }

    return wm;
}

std::string getCpuInfo() {
    std::string cpu_info = executeCommand("cat /proc/cpuinfo | grep 'model name' | uniq");
    cpu_info = cpu_info.substr(0, cpu_info.find_last_not_of(" \n\r\t") + 1);
    return cpu_info;
}

std::string getGpuInfo() {
    std::string gpu_info = executeCommand("lspci | grep -i 'vga\\|3d\\|display' | head -n 1 | awk -F ': ' '{print $2}'");
    gpu_info = gpu_info.substr(0, gpu_info.find_last_not_of(" \n\r\t") + 1);
    return gpu_info;
}

std::string getMemoryInfo() {
    std::string memory_info = executeCommand("free -h | awk '/Mem:/ {printf \"%s / %s\", $3, $2}'");
    memory_info = memory_info.substr(0, memory_info.find_last_not_of(" \n\r\t") + 1);
    return memory_info;
}

std::string getDiskInfo() {
    std::string disk_info = executeCommand("df -h / | awk 'NR==2 {printf \"%s / %s\", $3, $2}'");
    disk_info = disk_info.substr(0, disk_info.find_last_not_of(" \n\r\t") + 1);
    return disk_info;
}

int main() {
    std::string os = executeCommand("lsb_release -ds");
    std::string arch = executeCommand("uname -m");
    std::string shell = getShell();
    std::string username = getUsername();
    std::string hostname = getHostname();
    std::string kernel_version = getKernelVersion();
    std::string uptime = getUptime();
    std::string de = getDE();
    std::string wm = getWM();
    std::string terminal = getTerminal();
    std::string cpu_info = getCpuInfo();
    std::string gpu_info = getGpuInfo();
    std::string memory_info = getMemoryInfo();
    std::string disk_info = getDiskInfo();

    os = os.substr(0, os.find_last_not_of(" \n\r\t") + 1);
    if (!os.empty() && os.front() == '"' && os.back() == '"') {
        os = os.substr(1, os.length() - 2);
    }

    arch = arch.substr(0, arch.find_last_not_of(" \n\r\t") + 1);

    std::cout << username << "@" << hostname << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "Operating System: " << os << " " << arch << std::endl;
    std::cout << "Shell: " << shell << std::endl;
    std::cout << "Kernel: " << kernel_version << std::endl;
    std::cout << "Uptime: " << uptime << std::endl;
    if(!de.empty()) { std::cout << "DE: " << de << std::endl; }
    std::cout << "WM: " << wm << std::endl;
    std::cout << "Terminal Emulator: " << terminal << std::endl;
    std::cout << "CPU: " << cpu_info << std::endl;
    std::cout << "GPU: " << gpu_info << std::endl;
    std::cout << "Memory: " << memory_info << std::endl;
    std::cout << "Disk: " << disk_info << std::endl;

    return 0;
}