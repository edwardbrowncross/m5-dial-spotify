#include <Arduino.h>
#define _TASK_OO_CALLBACKS
#define _TASK_STATUS_REQUEST
#include <EventHandler.h>
#include <M5Dial.h>
#include <TaskSchedulerDeclarations.h>

// Libraries
#include <SpotifyArduino.h>

#include "events.h"

class DebugTask : public Task, public TSEvents::EventHandler {
 public:
  DebugTask(Scheduler& s, TSEvents::EventBus& e)
      : Task(0, TASK_ONCE, &s, false),
        TSEvents::EventHandler(&s, &e) {
    // M5Dial.Display.println("Debug task initialized");
    Serial.println("Debug task initialized");
  }

  bool OnEnable() {
    // M5Dial.Display.println("Debug task running");
    Serial.println("Debug task running");
    return false;
  }

  bool Callback() {
    return false;
  }

  void HandleEvent(TSEvents::Event event) {
    switch (event.id) {
      case WIFI_CONNECTING:
        // M5Dial.Display.println("Connecting to WiFi...");
        Serial.println("Connecting to WiFi...");
        break;
      case WIFI_CONNECTED: {
        char* ip = (char*)event.data;
        // M5Dial.Display.print("Connected: ");
        Serial.print("Connected: ");
        // M5Dial.Display.println(ip);
        Serial.println(ip);
        break;
      }
      case WIFI_DISCONNECTED:
        // M5Dial.Display.println("Disconnected");
        Serial.println("Disconnected");
        break;
      case WIFI_CONNECT_FAILED: {
        char* message = (char*)event.data;
        // M5Dial.Display.print("Connection failed: ");
        Serial.print("Connection failed: ");
        // M5Dial.Display.println(message);
        Serial.println(message);
        break;
      }
      case SPOTIFY_AUTH_ERROR:
        M5.Display.println("Spotify auth error");
        break;
      case SPOTIFY_UPDATE: {
        CurrentlyPlaying* cp = *(CurrentlyPlaying**)event.data;
        // M5Dial.Display.println(cp->trackName);
        Serial.println(cp->trackName);
        break;
      }
    }
  }
};