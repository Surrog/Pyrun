
#include "aprocess.hpp"
#include "configuration.hpp"
#include "environ.hpp"
#include "python_lnk.hpp"
#include <algorithm>
#include <array>
#include <filesystem>

std::string quote_this(std::string_view view)
{
   if (view[0] != '"')
   {
      return '"' + std::string(view) + '"';
   }
   return std::string(view);
}

std::vector<std::string> params(const configuration& conf, int argc, char** argv)
{
   std::vector<std::string> result;
   std::string_view extension = ".py";
   std::filesystem::path curdir = std::filesystem::current_path();

   for (int i = 1; i < argc; i++)
   {
      std::string arg = argv[i];
      if (std::search(arg.begin(), arg.end(), extension.begin(), extension.end()) != arg.end() && !std::filesystem::exists(curdir / arg))
      {
         for (auto& inc : conf.includes)
         {
            auto fullpath = inc / arg;
            if (std::filesystem::exists(fullpath))
            {
               arg = fullpath.string();
            }
         }
      }
      if (arg.find(' ') != std::string::npos)
         arg = quote_this(arg);
      result.push_back(std::move(arg));
   }

   return result;
}

std::vector<std::string> env(const std::unordered_map<std::string, std::vector<std::string>>& env)
{
   std::vector<std::string> result;
   for (const auto& keys : env)
   {
      std::string env_line = keys.first + '=';
      for (const auto& values : keys.second)
      {
         if (values != keys.second[0])
            env_line += xts::env::delim;
         env_line += values;
      }

      result.push_back(env_line);
   }
   return result;
}

bool test_equal(const std::unordered_map<std::string, std::vector<std::string>>& rval,
                const std::unordered_map<std::string, std::vector<std::string>>& lval)
{
   if (rval.size() != lval.size())
   {
      return false;
   }
   for (const auto& pair : rval)
   {
      auto f = lval.find(pair.first);
      if (f == lval.end())
      {
         return false;
      }
      if (f->second.size() != pair.second.size())
      {
         return false;
      }
      for (const auto& vec_val : pair.second)
      {
         if (std::find(f->second.begin(), f->second.end(), vec_val) == f->second.end())
         {
            return false;
         }
      }
   }
   return true;
}

int main(int argc, char** argv)
{
#ifdef _DEBUG
   for (int i = 0; i < argc; i++)
      std::cout << argv[i] << std::endl;
#endif //_DEBUG

   configuration conf = look_for_configuration(std::filesystem::current_path());
   if (validate_configuration(conf))
   {
      auto local_env = xts::env::get_current_environ();
      for (auto& val : conf.env)
      {
         auto it = local_env.find(val.first);
         if (it != local_env.end() && it->second.size())
         {
            val.second.insert(val.second.end(), it->second.begin(), it->second.end());
         }
      }

      if (!conf.link_interpreter)
      { //we use expected python interpreter
         xts::aprocess proc(conf.python, params(conf, argc, argv), conf.env);
         proc.join();
      }
      else
      { //Loading up dll interpreter
         python_lnk lnker(conf.python.parent_path().parent_path(), params(conf, argc, argv), conf.env);
      }
   }
   return 0;
}
