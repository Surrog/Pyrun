#include "environ.hpp"
#include "python_lnk.hpp"

#include <cassert>

python_lnk::python_lnk(const std::filesystem::path& python_home, const std::vector<std::string>& params, std::unordered_map<std::string, std::vector<std::string>>& env)
    : python_dll(), params(params)
{
   python_dll = python_home / "DLLs" / "python27_d.dll";
   if (!std::filesystem::exists(python_dll))
   {
      python_dll = python_home / "DLLs" / "python27.dll";
   }
   if (std::filesystem::exists(python_dll))
   {
      if (load_lib())
      {
         //env["PYTHONHOME"] = {python_home.string()};
		 if (xts::env::set_current_environ(env))
         {
            run_python(python_home, params);
         }
      }
   }
}

python_lnk::~python_lnk()
{
   close_lib();
}

bool python_lnk::valid() const
{
   return std::filesystem::exists(python_dll) && python_main_bind != nullptr;
}

int python_lnk::run_python(const std::filesystem::path& python_home, const std::vector<std::string>& params) const
{
   if (valid())
   {
      std::vector<char*> p;
      p.reserve(params.size() + 1);
      std::string python_exe = (python_home / "bin" / "python.exe").string();
      std::string python_home_str = python_home.string();
      p.push_back(const_cast<char*>(python_exe.c_str()));
      for (const auto& v : params)
      {
         p.push_back(const_cast<char*>(v.c_str()));	
      }
      (*python_PyHome_bind)(python_home_str.c_str());

      return (*python_main_bind)(p.size(), p.data());
   }
   throw std::logic_error("python_lnk not properly initialized");
}

bool python_lnk::load_lib()
{
#ifdef WIN32 //Windows vv
   return load_lib_windows();
#else  //Windows ^^ | Linux vv
   return load_lib_linux();
#endif //Linux ^^
}

void python_lnk::close_lib()
{
#ifdef WIN32 //Windows vv
   return close_lib_windows();
#else  //Windows ^^ | Linux vv
   return close_lib_linux();
#endif //Linux ^^
}

#ifdef WIN32 //Windows vv
bool python_lnk::load_lib_windows()
{
   dll_handle = LoadLibraryW(python_dll.c_str());
   if (dll_handle != nullptr)
   {
      python_main_bind = (python_main_type)GetProcAddress(dll_handle, "Py_Main");
      python_PyHome_bind = (python_setPyHome_type)GetProcAddress(dll_handle, "Py_SetPythonHome");
   }
   return python_main_bind != nullptr && python_PyHome_bind != nullptr;
}

void python_lnk::close_lib_windows()
{
   if (dll_handle != nullptr)
   {
      FreeLibrary(dll_handle);
      python_main_bind = nullptr;
      python_PyHome_bind = nullptr;
      dll_handle = nullptr;
   }
}
#else  // Windows ^^ | Linux vv
bool python_lnk::load_lib_linux()
{
   dll_handle = dlopen(python_dll.c_str(), RTLD_NOW);
   if (dll_handle != nullptr)
   {
      python_main_bind = (pythob_main_type)dlsym(dll_handle, "Py_Main");
      python_PyHome_bind = (python_setPyHome_type)dlsym(dll_handle, "Py_SetPythonHome");
   }
   return python_main_bind != nullptr && python_PyHome_bind != nullptr;
}

void python_lnk::close_lib_linux()
{
   if (dll_handle != nullptr)
   {
      dlclose(dll_handle);
      python_main_bind = nullptr;
      python_PyHome_bind = nullptr;
      dll_handle = nullptr;
   }
}
#endif // Linux ^^
