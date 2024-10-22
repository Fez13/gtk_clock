#pragma once
#include "../include/json/json.hpp"
#include <filesystem>
#include <paths.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

class Configuration {
public:
  void initialize(const std::string &path);

  static Configuration &get() {
    static Configuration s_instance;
    return s_instance;
  }
  std::pair<bool, nlohmann::json *>
  get_config(const std::string &path) {
    if (m_configs.count(path) == 0)
      return {true, nullptr};
    return {false, &m_configs[path]};
  }

  void save_config(const std::string &path);
  void reload(const std::string &path);

  std::vector<std::string> get_all_configs() const;

private:
  void load_config_file(const std::filesystem::path &);
  std::unordered_map<std::string, nlohmann::json> m_configs;

  std::string m_root = "";
  Configuration() = default;
  Configuration(const Configuration &);
  Configuration &operator=(const Configuration &) = delete;
};
