#include <Arduino.h>
#define _TASK_OO_CALLBACKS
#define _TASK_STATUS_REQUEST
#include <EventHandler.h>
#include <M5Dial.h>
#include <TaskSchedulerDeclarations.h>

// Libraries

#include "spotify.h"
#include "events.h"

class DebugTask : public Task, public TSEvents::EventHandler {
 public:
  DebugTask(Scheduler& s, TSEvents::EventBus& e)
      : Task(10*TASK_SECOND, TASK_FOREVER, &s, false),
        TSEvents::EventHandler(&s, &e) {
    // M5Dial.Display.println("Debug task initialized");
    Serial.println("Debug task initialized");
  }

  bool OnEnable() {
    // M5Dial.Display.println("Debug task running");
    Serial.println("Debug task running");
    return true;
  }

  bool Callback() {
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT); // internal RAM, memory capable to store data or to create new task
    Serial.print("Free heap: ");
    Serial.println(info.total_free_bytes);
    Serial.print("Minimum free heap: ");
    Serial.println(info.minimum_free_bytes);
    Serial.print("Largest free block: ");
    Serial.println(info.largest_free_block);
    return true;
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
        SpotifyState* ss = *(SpotifyState**)event.data;
        // M5Dial.Display.println(cp->trackName);
        Serial.println(ss->trackName);
        break;
      }
    }
  }
};