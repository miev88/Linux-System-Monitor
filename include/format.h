#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
   const int SECONDS_IN_HOUR = 3600;
   const int SECONDS_IN_MINUTE = 60;
   template<typename T> void pad(std::basic_string<T>& s, typename std::basic_string<T>::size_type n, T c);
   std::string ElapsedTime(long elapsed_seconds); 
};                                    

#endif
