#include <Arduino.h>
#include <HTTPClient.h> // Needs to be before M5Dial so that drawJpgUrl is available 
#include <M5Dial.h>

#define _TASK_OO_CALLBACKS
#define _TASK_STATUS_REQUEST
#include <EventBus.h>
#include <TaskScheduler.h>
#include <TaskSchedulerDeclarations.h>

#include "config.h"
#include "tasks/SpotifyTask.cpp"
#include "tasks/WifiTask.cpp"
#include "tasks/DebugTask.cpp"
#include "tasks/Controller.cpp"

Scheduler ts;
TSEvents::EventBus e;

SpotifyTask *spotifyTask;
SpotifyTask* SpotifyTask::reqInstance;
WifiTask *wifiTask;
DebugTask *debugTask;
Controller *controllerTask;

void setup() {
  Serial.begin(115200);
  M5Dial.begin();

  M5Dial.Display.setTextColor(GREEN);
  M5Dial.Display.setTextDatum(top_centre);
  M5Dial.Display.setFont(&fonts::DejaVu12);
  M5Dial.Display.setTextSize(1);
  M5Dial.Display.println("Starting up...");

  const char *configError = loadConfigFile();
  if (configError != NULL) {
    M5Dial.Display.println(configError);
    return;
  }

  M5Dial.Display.print(getConfigValue("wifiSSID"));
  M5Dial.Display.print(" ");
  M5Dial.Display.println(getConfigValue("wifiPass"));

  debugTask = new DebugTask(ts, e);
  wifiTask = new WifiTask(ts, e, getConfigValue("wifiSSID"), getConfigValue("wifiPass"));

  spotifyTask = new SpotifyTask(ts, e, getConfigValue("spotifyClientId"), getConfigValue("spotifyClientSecret"), getConfigValue("spotifyRefreshToken"));
  controllerTask = new Controller(ts, e);

  debugTask->enable();
  wifiTask->enable();
  spotifyTask->enable();
  controllerTask->enable();
}

void loop() {
  ts.execute();
  yield();
}
