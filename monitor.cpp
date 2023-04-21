#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <unistd.h>
#include <proc/readproc.h>
#include <cstring>
#include <sys/sysinfo.h>

double calculateCpuUsage(const proc_t& procInfo, unsigned long long total_time) {
    double total_cpu_time = procInfo.utime + procInfo.stime;
    double cpu_usage = (total_cpu_time * 100.0) / total_time;
    return cpu_usage;
}

bool logUsage(const std::string& filename, double cpuThreshold, double memThreshold) {
    std::ofstream logFile;
    logFile.open(filename, std::ios_base::app);

    if (!logFile.is_open()) {
        std::cerr << "Error opening log file." << std::endl;
        return false;
    }

    PROCTAB *proc = openproc(PROC_FILLSTAT | PROC_FILLSTATUS);

    proc_t procInfo;
    memset(&procInfo, 0, sizeof(proc_t));

    struct sysinfo sys_info;
    sysinfo(&sys_info);
    unsigned long long total_time = sys_info.uptime * sysconf(_SC_NPROCESSORS_ONLN);

    bool foundHighUsage = false;
    while (readproc(proc, &procInfo) != nullptr) {
        double cpuPercent = calculateCpuUsage(procInfo, total_time);
        double memPercent = (procInfo.vm_rss * 100.0) / sysconf(_SC_PHYS_PAGES);

        if (cpuPercent > cpuThreshold || memPercent > memThreshold) {
            foundHighUsage = true;
            break;
        }
    }

    if (foundHighUsage) {
        // Get current date and time
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        logFile << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << std::endl;

        logFile << "PID\tCPU%\tMEM%\tCMD" << std::endl;

        closeproc(proc); // Close the process table
        proc = openproc(PROC_FILLSTAT | PROC_FILLSTATUS); // Reopen the process table to reset its position
        memset(&procInfo, 0, sizeof(proc_t));
        while (readproc(proc, &procInfo) != nullptr) {
            double cpuPercent = calculateCpuUsage(procInfo, total_time);
            double memPercent = (procInfo.vm_rss * 100.0) / sysconf(_SC_PHYS_PAGES);

            if (cpuPercent > cpuThreshold || memPercent > memThreshold) {
                logFile << procInfo.tid << "\t" << cpuPercent << "\t" << memPercent << "\t" << procInfo.cmd << std::endl;
            }
        }

        logFile << std::endl;  // Add a new line after adding logs
    }

    logFile.close();
    closeproc(proc);
    return foundHighUsage;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <log_filename> <cpu_threshold> <mem_threshold> <interval_sec>" << std::endl;
        return 1;
    }

    std::string logFilename       = argv[1];
    double cpuThreshold = std::stod(argv[2]);
    double memThreshold = std::stod(argv[3]);
    int intervalSeconds = std::stod(argv[4]);

    while (true) {
        if (logUsage(logFilename, cpuThreshold, memThreshold)) {
            std::cout << "Logged high usage processes" << std::endl;
        }
        sleep(intervalSeconds);
    }

    return 0;
}

