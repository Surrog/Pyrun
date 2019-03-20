#ifndef XTS_WINDOWS_PROCESS_HPP
#define XTS_WINDOWS_PROCESS_HPP
#ifdef WIN32

#define NOMINMAX
#include <Windows.h>
#undef NOMINMAX

#include "environ.hpp"
#include <filesystem>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <system_error>

namespace xts
{
   class windows_process
   {
      std::string quote_this(std::string_view arg)
      {
         if (arg.size() && arg[0] != '"' && arg.find(' ') != std::string::npos)
         {
            return '"' + std::string(arg) + '"';
         }
         return std::string(arg);
      }

      std::system_error run_process(std::filesystem::path filename, const std::vector<std::string>& argv, const std::unordered_map<std::string, std::vector<std::string>>& env)
      {
         std::memset(&si, 0, sizeof(si));
         std::memset(&pi, 0, sizeof(pi));
         std::memset(&proc_sec_attr, 0, sizeof(proc_sec_attr));
         std::memset(&thread_sec_attr, 0, sizeof(thread_sec_attr));


		 si.cb = sizeof(si);
		 proc_sec_attr.nLength = sizeof(proc_sec_attr);
         proc_sec_attr.bInheritHandle = TRUE;
         thread_sec_attr.nLength = sizeof(thread_sec_attr);
         thread_sec_attr.bInheritHandle = TRUE;

         command_line = filename.string();
         for (const auto& s : argv)
         {
            command_line += " " + quote_this(s);
         }

#ifdef _DEBUG
         std::cout << command_line << std::endl;
#endif

         if (xts::env::set_current_environ(env))
         {
            auto result = ::CreateProcessA(NULL, command_line.data(), &proc_sec_attr, &thread_sec_attr,
                                           TRUE, NULL, nullptr, running_dir.data(), &si, &pi);
            if (result > 0)
            {
               attached = true;
            }
            else
            {
               return std::system_error(std::error_code(GetLastError(), std::system_category()));
            }
         }
         else
         {
            return std::system_error(std::error_code(GetLastError(), std::system_category()));
         }
         return std::system_error(std::make_error_code(std::errc(0)));
      }

    public:
      windows_process(const std::filesystem::path& filename, const std::vector<std::string>& argv = {}, const std::unordered_map<std::string, std::vector<std::string>>& env = {})
      {
         std::system_error ec = run_process(filename, argv, env);
         if (ec.code().value() != 0)
         {
            throw ec;
         }
      }

      windows_process(const std::filesystem::path& filename, const std::vector<std::string>& argv, const std::unordered_map<std::string, std::vector<std::string>>& env, std::system_error& ec)
      {
         ec = run_process(filename, argv, env);
      }

      bool join()
      {
         if (pi.hProcess)
         {
            bool result = ::WaitForSingleObject(
                              pi.hProcess, std::numeric_limits<DWORD>::max()) != WAIT_FAILED;
            if (result)
               attached = false;
         }
         return false;
      }

      bool kill() const
      {
         if (pi.hProcess)
            return ::TerminateProcess(pi.hProcess, -1);
         return false;
      }

      void detach()
      {
         attached = false;
      }

      DWORD pid() const
      {
         return pi.dwProcessId;
      }

      ~windows_process()
      {
         if (attached)
         {
            kill();
         }
         if (pi.hProcess)
         {
            CloseHandle(pi.hProcess);
         }
         if (pi.hThread)
         {
            CloseHandle(pi.hThread);
         }
      }

    private:
      std::string command_line;
      std::string env;
      std::string running_dir = std::filesystem::current_path().string();
      bool attached;
      PROCESS_INFORMATION pi;
      STARTUPINFO si;
      SECURITY_ATTRIBUTES proc_sec_attr;
      SECURITY_ATTRIBUTES thread_sec_attr;
   };
}
#endif
#endif // !XTS_WINDOWS_PROCESS_HPP