#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>

StaticJsonDocument<512> config;

const char* configSpiffsError = "Spiffs Error";
const char* configFileNotFound = "File not found";

const char* loadConfigFile() {
  if (!SPIFFS.begin(false)) {
    return configSpiffsError;
  }
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    return configFileNotFound;
  }
  DeserializationError error = deserializeJson(config, configFile);
  if (error) {
    return error.c_str();
  }
  return NULL;
}

const char* getConfigValue(const char* key) {
  return config[key];
}
const int getConfigIntValue(const char* key) {
  return config[key];
}