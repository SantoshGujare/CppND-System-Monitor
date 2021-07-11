#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
#define SEC_IN_HOUR 3600
#define SEC_IN_MINUTE 60
string Format::ElapsedTime(long seconds) 
{
  int HH;
  int MM;
  int SS;
  string hh_digit ="";
  string mm_digit ="";
  string ss_digit ="";
  HH = (seconds / SEC_IN_HOUR);
  MM = ((seconds % SEC_IN_HOUR) / SEC_IN_MINUTE);
  SS = (((seconds % SEC_IN_HOUR) % SEC_IN_MINUTE));
  if(SS < 10) { ss_digit = "0";};
  if(MM < 10) { mm_digit = "0";};
  if(HH < 10) { hh_digit = "0";};
  string time = hh_digit + to_string(HH) + ":" + mm_digit + to_string(MM) + ":" + ss_digit + to_string(SS);
  return time; 
}
