#include "configuration.hpp"
#include "environ.hpp"

#include <cctype>
#include <algorithm>

configuration generate_default(const std::filesystem::path& from)
{
#ifdef WIN32
   std::string Path_key = "Path";
#else  // ^^ Windows | Linux vv
   std::string Path_key = "PATH";
#endif // Linux ^^

   configuration conf;
   conf.configuration_path = from;
   std::ofstream stream(from.c_str());
   if (stream)
   {
      Json::Value root;
      root["python"] = "C:\\python27\\python.exe";
      root["link_interpreter"] = false;
      conf.python = "C:\\python27\\python.exe";

      {
         Json::Value basic_env;

         basic_env["base_path"] = "";
         basic_env["var"] = "PYTHONPATH";
         basic_env["directory"].append(from.parent_path().string());
         root["env"].append(basic_env);
         conf.env["PYTHONPATH"]
             .push_back(from.parent_path().string());
      }

      {
         Json::Value basic_env;

         basic_env["base_path"] = "C:\\python27\\";
         basic_env["var"] = "PYTHONPATH";
         basic_env["directory"].append(".\\Lib");
         basic_env["directory"].append(".\\DLLs");
         root["env"].append(basic_env);
         conf.env["PYTHONPATH"].push_back("C:\\python27\\Lib");
         conf.env["PYTHONPATH"].push_back("C:\\python27\\DLLs");
      }

      {
         Json::Value basic_env;
         auto key = std::string(xts::env::path_key);
         basic_env["base_path"] = "C:\\python27\\";
         basic_env["var"] = key;
         basic_env["directory"].append("");
         basic_env["directory"].append(".\\DLLs");
         root["env"].append(basic_env);
         conf.env[key].push_back("C:\\python27\\DLLs");
         conf.env[key].push_back("C:\\python27");
      }

      root["includes"].append("");
      stream << root;
   }
   return conf;
}

bool validate_configuration(const configuration& conf)
{
   bool result = true;
   if (!std::filesystem::exists(conf.python))
   {
      std::cerr << "ERROR! : " << conf.python << " doesn't exist" << std::endl;
      result = false;
   }
   for (auto& path : conf.env)
   {
      for (auto& fullpath : path.second)
      {
         if (!std::filesystem::exists(fullpath))
         {
            std::cerr << "WARNING! : " << fullpath << " doesn't exist" << std::endl;
         }
      }
   }
   return result;
}

std::string str_to_lower(std::string val)
{
   std::transform(val.begin(), val.end(), val.begin(), [](const std::string::value_type& c) { return std::tolower(c); });
   return val;
}

bool parse_config(const std::filesystem::path& conf_path, configuration& result)
{
   std::ifstream file(conf_path.c_str());
   if (file.good())
   {
      Json::Value root;
      Json::CharReaderBuilder builder;
      builder["collectComments"] = false;
      JSONCPP_STRING errs;
      if (!Json::parseFromStream(builder, file, &root, &errs))
      {
         std::cerr << "errors in " << conf_path << std::endl
                   << errs << std::endl;
         return false;
      }

      std::filesystem::path conf_directory = conf_path.parent_path();

      result.python = root["python"].asString();
      result.link_interpreter = root.get("link_interpreter", false).asBool();

      if (!result.python.is_absolute())
         result.python = std::filesystem::absolute(conf_directory / result.python);
      for (auto& val : root["env"])
      {
         std::filesystem::path base = val["base_path"].asString();
         if (!base.is_absolute())
         {
            base = std::filesystem::absolute(conf_directory / base);
         }

		 std::string env_key = val["var"].asString();
         if (str_to_lower(env_key) == "path")
         {
            env_key.assign(xts::env::path_key.begin(), xts::env::path_key.end());
         }
         auto& vec = result.env[env_key];
         for (auto& dir : val["directory"])
         {
            vec.push_back(std::filesystem::absolute(base / dir.asString()).string());
         }
      }

      for (auto& val : root["includes"])
      {
         std::filesystem::path inc = val.asString();
		 if (!inc.is_absolute())
         {
            inc = std::filesystem::absolute(conf_directory / inc);
		 }
         result.includes.push_back(inc);
      }
      result.configuration_path = std::move(conf_path);
      return true;
   }
   std::cerr << "failed to open " << conf_path << std::endl;
   return false;
}

configuration look_for_configuration(const std::filesystem::path& cur)
{
   auto tmp = cur;
   auto fullpath = cur / config_filename;
   auto parent = cur.parent_path();

   while (tmp != std::filesystem::path())
   {
      fullpath = tmp / config_filename;
      configuration result;
      if (std::filesystem::exists(fullpath) && parse_config(fullpath, result) &&
          std::filesystem::exists(result.python) && result.python.filename() == std::filesystem::path("python.exe"))
      {
         std::cout << "config loaded: " << fullpath << std::endl;
         return result;
      }

      parent = tmp.parent_path();
      if (tmp == parent)
         tmp = std::filesystem::path();
      else
         tmp = parent;
   }

   return generate_default(cur / config_filename);
}