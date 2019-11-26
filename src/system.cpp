#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

// Constructor
System::System() {
   cpu_ = Processor();
}

// Return the system's CPU
Processor& System::Cpu() {
   return cpu_;
}

// Return a container composed of the system's processes
std::vector<Process>& System::Processes() {
   processes_.clear();
   std::vector<int> pids = LinuxParser::Pids();
   for (std::size_t i = 0; i < pids.size(); i++) {
      Process p(pids[i]);
      processes_.push_back(p);
   }
   //sort vector using overloaded "less than" comparison operator
   std::sort(processes_.begin(), processes_.end());
   return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() {
   return LinuxParser::Kernel();
}

// Return the system's memory utilization
float System::MemoryUtilization() {
   return LinuxParser::MemoryUtilization();
}

// Return the operating system name
std::string System::OperatingSystem() {
   return LinuxParser::OperatingSystem(); 
}

// Return the number of processes actively running on the system
int System::RunningProcesses() {
   return LinuxParser::RunningProcesses();
}

// Return the total number of processes on the system
int System::TotalProcesses() {
   return LinuxParser::TotalProcesses();
}

// Return the number of seconds since the system started running
long int System::UpTime() {
   return LinuxParser::UpTime();
}
