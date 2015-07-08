#include "param_registry.hpp"

using namespace std;

ParamRegistry::ParamRegistry(const char *filename) {

  param_db.open (filename, ios::in);
  if (! param_db.is_open()) {
    throw std::exception(); // file not found
  }
}

ParamRegistry::~ParamRegistry() {
  param_db.close();
}

template <>
void my_scanf(const std::string &line, param_unpacked<float> &result) {
  sscanf(line.c_str(), "%d %d %s %f",
         &result.mav, &result.comp, result.name, &result.val);
}

template <>
void my_scanf(const std::string &line, param_unpacked<uint32_t> &result) {
  sscanf(line.c_str(), "%d %d %s %u",
         &result.mav, &result.comp, result.name, &result.val);
}

template <>
void my_scanf(const std::string &line, param_unpacked<int32_t> &result) {
  sscanf(line.c_str(), "%d %d %s %i",
         &result.mav, &result.comp, result.name, &result.val);
}
