#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;

// Constructor
Process::Process(int id) : id_(id) {}

// Return this process's ID
int Process::Pid() const {
   return id_; 
}

// Return this process's CPU utilization
float Process::CpuUtilization() const {
   return LinuxParser::CpuUtilization(id_);
}

// Return the command that generated this process
string Process::Command() {
   return LinuxParser::Command(id_); 
}

// Return this process's memory utilization
string Process::Ram() {
   return LinuxParser::Ram(id_); 
}

//  Return the user (name) that generated this process
string Process::User() {
   return LinuxParser::User(id_); 
}

// Return the age of this process (in seconds)
long int Process::UpTime() {
   return LinuxParser::UpTime(id_); 
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
   return a.CpuUtilization() < this->CpuUtilization();
}
