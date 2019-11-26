#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
   string line, key, value;
   float mem_total, mem_free;
   std::ifstream filestream(kProcDirectory + kMeminfoFilename);
   if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
	std::istringstream linestream(line);
	while (linestream >> key >> value) {
	   if (key == "MemTotal:") {
	      mem_total = std::stof(value);
	   }
           if (key == "MemFree:") {
	      mem_free = std::stof(value);
	   }
        }
      }
      return (mem_total - mem_free) / mem_total;
   }
   return -1.0;    
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return std::stol(uptime);
  }
  return -1;
}

// Read and return the jiffies (i.e., clock ticks) for the system. Guest not included
vector<long> LinuxParser::Jiffies() {
   string line, key;
   long user, nice, system, idle, iowait, irq, softirq, steal;
   vector<long> jiffies;
   std::ifstream filestream(kProcDirectory + kStatFilename);
   if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
         std::istringstream linestream(line);
         if (linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal) {
            if (key == "cpu") {
	       jiffies = {user, nice, system, idle, iowait, irq, softirq, steal};
	       return jiffies;
	    }
         }
      } 
   }
   return {};
}

// Read and return the jiffies of a process relative to CPU time
vector<long> LinuxParser::Jiffies(int pid) {
   string line, value;
   int counter = 1;
   // CPU time fields
   vector<int> fields = {14, 15, 16, 17, 22};
   vector<long> jiffies;
   std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/" + kStatFilename);
   if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      while (linestream >> value) {
         if (std::find(fields.begin(), fields.end(), counter) != fields.end()) {
            jiffies.push_back(std::stol(value));
	 }
         counter++;
      }
      return jiffies;
   }
   return {};
}

// Read and return CPU utilization since boot. Result is in range [0.0, 1.0]
float LinuxParser::CpuUtilization() {
   long total_cpu_time, total_cpu_idle_time, total_cpu_usage_time;
   vector<long> js = Jiffies();
   if (!js.empty()) {
      // 0:user, 1:nice, 2:system, 3:idle, 4:iowait, 5:irq, 6:softirq, 7:steal
      total_cpu_time = js[0] + js[1] + js[2] + js[3] + js[4] + js[5] + js[6] + js[7];
      total_cpu_idle_time = js[3] + js[4];
      total_cpu_usage_time = total_cpu_time - total_cpu_idle_time;
      return static_cast<float>(total_cpu_usage_time) / total_cpu_time;
   }
   return -1.0; 
}

// Read and return CPU utilization of a process
float LinuxParser::CpuUtilization(int pid) { 
   long total_cpu_time, lseconds;
   vector<long> js = Jiffies(pid);
   // Check if process still exists
   if (!js.empty()) {
      // 0:utime, 1:stime, 2:cutime, 3:cstime, 4:starttime
      total_cpu_time = js[0] + js[1] + js[2] + js[3];
      lseconds = LinuxParser::UpTime() - (js[4] / sysconf(_SC_CLK_TCK));
      return static_cast<float>(total_cpu_time) / sysconf(_SC_CLK_TCK) / lseconds;
   }
   return -1.0;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
   string line, key, value;
   std::ifstream filestream(kProcDirectory + kStatFilename);
   if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
         std::istringstream linestream(line);
         if (linestream >> key >> value) {
  	    if (key == "processes") {
               return std::stoi(value);
	    }
         }
     } 
   }
   return -1;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
   string line, key, value;
   std::ifstream filestream(kProcDirectory + kStatFilename);
   if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
         std::istringstream linestream(line);
         if (linestream >> key >> value) {
            if (key == "procs_running") {
               return std::stoi(value);
	    }
         }
     } 
   }
   return -1;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
   string line;
   std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/" + kCmdlineFilename);
   if (filestream.is_open()) {
      if (std::getline(filestream, line)) {
         return line;
      }
   }
   return string();
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/" + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
	if (key == "VmSize:") {
          return value;
        }
      }
    }
  }
  return string();
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/" + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
	if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return string();
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string line, user, placeholder, uid;
  string searched_uid = LinuxParser::Uid(pid);  
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> placeholder >> uid) {
	if (uid == searched_uid) {
          return user;
        }
      }
    }
  }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
   string line, uptime;
   int uptime_field = 22;
   int counter = 1;
   std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/" + kStatFilename);
   if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      while (linestream >> uptime) {
	if (counter == uptime_field) {
	   //divide by sysconf(_SC_CLK_TCK) to convert clock ticks into seconds
	   return std::stol(uptime) / sysconf(_SC_CLK_TCK);
	}
	counter++;
      }
   }
   return -1;
}
