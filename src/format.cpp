#include <string>

#include "format.h"

/*
 * Pad string with leading character if string length < size_type
 */
template<typename T>
void Format::pad(std::basic_string<T>& s, typename std::basic_string<T>::size_type n, T c) {
   if (n > s.length())
      s.insert(s.begin(), n-s.length(), c);
}

/*
 * Convert seconds into HH:MM:SS format
 */
std::string Format::ElapsedTime(long elapsed_seconds) {
   int hrs, mins, secs;
   int wished_str_length = 2;
   char pad_with = '0';
   std::string shrs, smins, ssecs;   
   hrs = elapsed_seconds / SECONDS_IN_HOUR;
   //result of modulo is in seconds 
   mins = (elapsed_seconds % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE;
   secs = elapsed_seconds - hrs*SECONDS_IN_HOUR - mins*SECONDS_IN_MINUTE;
   //convert to string and pad with leading zero if length<2
   shrs = std::to_string(hrs);
   smins = std::to_string(mins);
   ssecs = std::to_string(secs);
   pad(shrs, wished_str_length, pad_with);
   pad(smins, wished_str_length, pad_with);
   pad(ssecs, wished_str_length, pad_with);
   return shrs + ":" + smins + ":" + ssecs;
}
