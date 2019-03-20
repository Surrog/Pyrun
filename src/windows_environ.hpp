#ifndef XTS_WINDOWS_ENVIRON_HPP
#define XTS_WINDOWS_ENVIRON_HPP

#ifdef WIN32

#include <Windows.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <string_view>

namespace xts::env
{
   static const constexpr char delim = ';';
   static const constexpr std::string_view path_key = "Path";

   inline std::unordered_map<std::string, std::vector<std::string>> get_current_environ()
   {
      LPCH orig_env = GetEnvironmentStrings();
      LPCH env = orig_env;
      std::unordered_map<std::string, std::vector<std::string>> result;

      while (env && *env)
      {
         std::string_view current_value(env);
         std::size_t found = current_value.find('=');

         //if (found == 0)
         //found = current_value.find('=', 1);

         if (found != std::string_view::npos && found != 0)
         {
            std::string key(current_value.substr(0, found));
            if (key.size())
            {
               auto& values = result[key];
               std::size_t end = 0;
               do
               {
                  end = current_value.find(delim, found + 1);
                  if (end != std::string::npos)
                  {
                     if ((end - found - 1) > 0)
                        values.push_back(std::string(current_value.substr(found + 1, end - found - 1)));
                     found = end;
                  }
               } while (end != std::string::npos);
               if (found + 1 < current_value.size())
                  values.push_back(std::string(current_value.substr(found + 1)));
            }
         }
         env += current_value.size() + 1;
      }

      FreeEnvironmentStringsA(orig_env);
      return result;
   }

   inline bool set_current_environ(const std::unordered_map<std::string, std::vector<std::string>>& env)
   {
      bool result = true;
      for (const auto& keys : env)
      {
         std::string env_line = keys.first + '=';

         for (const auto& values : keys.second)
         {
            if (values.size())
            {
               if (values != keys.second[0])
                  env_line += delim;
               env_line += values;
            }
         }
         if (_putenv(env_line.c_str()) == -1)
			 return false;
      }
      return true;
   }
}

#endif //WIN32

#endif //!XTS_WINDOWS_ENVIRON_HPP