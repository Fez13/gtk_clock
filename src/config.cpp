
#include "config.h"
#include <iostream>

void scan_path(std::vector<std::filesystem::path> &files,
               const std::filesystem::path &path) {
  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    if (entry.is_directory())
      scan_path(files, path / entry.path().filename());
    else
      files.emplace_back(entry.path());
  }
}

void Configuration::initialize(const std::string &path) {

  m_root = path;

  if (!std::filesystem::exists(m_root)) {
    std::fprintf(stderr, "Error invalid config root: %s", m_root.c_str());
    exit(1);
  }

  std::vector<std::filesystem::path> files;
  scan_path(files, m_root);

  for (auto &file_path : files) {
    file_path = std::filesystem::relative(file_path, m_root);
    const std::string root_name = file_path.begin()->string();
    if (root_name == "configuration") {
      load_config_file(file_path);
    }
  }
}

void Configuration::reload(const std::string &path){
  auto full_path = path + ".json";
  m_configs.erase(path);
  load_config_file(full_path);
}


void Configuration::load_config_file(const std::filesystem::path &path) {
  auto new_path = path;
  new_path = new_path.replace_extension();
  std::ifstream f(m_root / path);
  try {
    m_configs[new_path] = nlohmann::json::parse(f);
  } catch (...) {
    std::fprintf(stderr, "Error parsing json");
  }
}

void Configuration::save_config(const std::string &path) {
  auto [err, config] = get_config(path);

  if (err) {
    std::fprintf(stderr, "Error saving config: %s", path.c_str());
    return;
  }

  std::ofstream f(m_root /
                      std::filesystem::path(path).replace_extension(".json"),
                  std::ios::trunc);
  if (!f.is_open()) {
    std::fprintf(stderr, "Error saving config: %s", path.c_str());
    return;
  }
  f << config->dump(4).c_str();
  f.close();
}

std::vector<std::string> Configuration::get_all_configs() const {
  std::vector<std::string> strings = {};
  for (auto [s, _] : m_configs)
    strings.emplace_back(s);
  return strings;
}
