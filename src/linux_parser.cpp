#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::stoi;
using std::string;
using std::to_string;
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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
  string line;
  string key;
  string value;
  float mem_total {0.0};
  float mem_free {0.0};
  float mem_buffer {0.0};
  float mem_cached {0.0};
  float mem_utilization {0.0};
  
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) 
  { 
    while(std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value; 
      if (key == "MemTotal:")
      {      
        mem_total = stof(value);
      }
      else if (key == "MemFree:")
      {          
        mem_free = stof(value);
      }
            else if (key == "Buffers:")
      {          
        mem_buffer = stof(value);
      }
            else if (key == "Cached:")
      {          
        mem_cached = stof(value);
      }
    }
  }
  mem_utilization = ((mem_total - mem_free - mem_buffer - mem_cached)/mem_total);
  return mem_utilization; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() 
{ 
  string line;
  string uptime;
  long int total_uptime {0};
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) 
  {    
     std::getline(filestream, line);
     std::istringstream linestream(line);
     linestream >> uptime;
     total_uptime = stol(uptime);
  }
  return (total_uptime); 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() 
{  
  string line;
  string key;
  string value;
  long system_jiffies {0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) 
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while(linestream >> value)
    {
      system_jiffies += stol(value);
    }
  }  
  return system_jiffies; 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() 
{
  long system_active_jiffies {0};

  vector<string> cpu_utilization = LinuxParser::CpuUtilization(); 
  system_active_jiffies = stol(cpu_utilization.at(kUser_)) + stol(cpu_utilization.at(kNice_))
                          + stol(cpu_utilization.at(kSystem_)) + stol(cpu_utilization.at(kIRQ_))
                          + stol(cpu_utilization.at(kSoftIRQ_)) + stol(cpu_utilization.at(kSteal_))
                          + stol(cpu_utilization.at(kGuest_)) + stol(cpu_utilization.at(kGuestNice_));
  return system_active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{
  long system_idle_jiffies {0};  
  vector<string> cpu_utilization = LinuxParser::CpuUtilization(); 
  system_idle_jiffies = stol(cpu_utilization.at(kIdle_)) + stol(cpu_utilization.at(kIOwait_));
  return system_idle_jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) 
{ 
 string p_id = to_string(pid);
  long process_active_jiffies {0};
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + p_id + kStatFilename);
  if (filestream.is_open()) 
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 13; i++) 
    {
      //Do nothing, this data is of no use for active jiffies calculation.
      linestream >> value; 
    }
    linestream >> value; 
    process_active_jiffies += stol(value);
    linestream >> value; 
    process_active_jiffies += stol(value);
    linestream >> value; 
    process_active_jiffies += stol(value);
    linestream >> value;
    process_active_jiffies += stol(value);
  }
  return process_active_jiffies; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() 
{ 
  vector<string> cpu_utilization;
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) 
  {
    std::getline(filestream, line) ;
    std::istringstream linestream(line);
    linestream >> value;
    while (linestream >> value) 
    {
      cpu_utilization.emplace_back(value);
    }
  }
  return cpu_utilization;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value) 
      {
        if (key == "processes") 
        {
          return stoi(value);
        }
      }
    }
  }
  return 0; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{ 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value) 
      {
        if (key == "procs_running") 
        {
          return stoi(value);
        }
      }
    }
  }
  return 0; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) 
{
  string p_id = to_string(pid);
  string cmdline = " ";
  std::ifstream filestream(kProcDirectory + p_id + kCmdlineFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, cmdline)) 
    {
        return cmdline;    
    }
  }  
  return cmdline; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) 
{ 
  string p_id = to_string(pid);
  string line;
  string key;
  string value = " ";
  long mem_MB {0};
  std::ifstream filestream(kProcDirectory + p_id + kStatusFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::replace(line.begin(), line.end(), '(', ' ');
      std::replace(line.begin(), line.end(), ')', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) 
      {
        if (key == "VmSize:") 
        {
          mem_MB = stol(value);
          return to_string(mem_MB/1024);
        }  
      }
    }
  }  
  return (value); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) 
{ 
  string p_id = to_string(pid);
  string line;
  string key;
  string value = " ";
  std::ifstream filestream(kProcDirectory + p_id + kStatusFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::replace(line.begin(), line.end(), '(', ' ');
      std::replace(line.begin(), line.end(), ')', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) 
      {
        if (key == "Uid:") 
        {
          return value;
        }  
      }
    }
  }  
  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) 
{ 
  string line;
  string usrname = " ";
  string x;
  string uid;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> usrname >> x >> uid) 
      {
        if (uid == LinuxParser::Uid(pid))
        {
          return usrname;
        }  
      }
    }
  }  
  return usrname;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) 
{ 
  string p_id = to_string(pid);
  long process_uptime {0};
  int i = 0;
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + p_id + kStatFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::replace(line.begin(), line.end(), '(', ' ');
      std::replace(line.begin(), line.end(), ')', ' ');
      std::replace(line.begin(), line.end(), '-', ' ');
      std::istringstream linestream(line);
      for (i = 0; i < 22; i++) 
      {
        value = "";
        linestream >> value;
        break;
      }
      if (i == 22) break;
    }
    process_uptime = LinuxParser::UpTime() - (stol(value)/sysconf(_SC_CLK_TCK));
  }
  return process_uptime;; 
}
