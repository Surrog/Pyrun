
#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "json/json.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

struct configuration
{
   std::filesystem::path python;
   bool link_interpreter;
   std::unordered_map<std::string, std::vector<std::string>> env;
   std::filesystem::path configuration_path;
   std::vector<std::filesystem::path> includes;
};

constexpr std::string_view config_filename = "pyrun.json";

configuration generate_default(const std::filesystem::path& from);

bool validate_configuration(const configuration& conf);

bool parse_config(const std::filesystem::path& conf_path, configuration& result);

configuration look_for_configuration(const std::filesystem::path& cur);

#endif //!CONFIGURATION_HPP