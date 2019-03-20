#ifndef PYTHON_LNK_HPP
#define PYTHON_LNK_HPP

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

#ifdef WIN32 //Windows vv
#include <Windows.h>
#else // Windows ^^ | Linux vv
#include <dlfcn.h>
#endif // Linux ^^

class python_lnk
{
   std::filesystem::path python_dll;
   std::vector<std::string> params;
   typedef int (*python_main_type)(int, char**);
   typedef void (*python_init_type)();
   typedef void (*python_setPyHome_type)(const char*);
   python_main_type python_main_bind = nullptr;
   python_setPyHome_type python_PyHome_bind = nullptr;

   bool load_lib();
   void close_lib();

#ifdef WIN32 //Windows vv
   HMODULE dll_handle;
   bool load_lib_windows();
   void close_lib_windows();
#else  // Windows ^^ | Linux vv
   void* dll_handle;
   bool load_lib_linux();
   void close_lib_linux();
#endif // Linux ^^

 public:
   python_lnk(const std::filesystem::path& python_home, const std::vector<std::string>& params, std::unordered_map<std::string, std::vector<std::string>>& env);
   ~python_lnk();

   bool valid() const;
   int run_python(const std::filesystem::path& python_home, const std::vector<std::string>& params) const;
};
#endif //!PYTHON_LNK_HPP