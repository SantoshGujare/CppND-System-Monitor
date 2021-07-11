#include "processor.h"
#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() 
{ 
  float total_cpu_utilization {0};
  total_cpu_utilization = (float)LinuxParser::ActiveJiffies()/LinuxParser::Jiffies();
  return (total_cpu_utilization); 
}
