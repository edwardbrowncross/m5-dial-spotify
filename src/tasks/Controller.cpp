#include <Arduino.h>
#define _TASK_OO_CALLBACKS
#define _TASK_STATUS_REQUEST
#include <EventHandler.h>
#include <HTTPClient.h> // Needs to be before M5Dial so that drawJpgUrl is available 
#include <M5Dial.h>
#include <TaskSchedulerDeclarations.h>

// Libraries
#include <SpotifyArduino.h>

// Fonts
#include <assets/roboto_bold_11.h>
#include <assets/roboto_regular_11.h>

#include "events.h"

enum View {
  NOW_PLAYING,
};

class NowPlayingView : public Task, public TSEvents::EventHandler {
 public:
  NowPlayingView(Scheduler& s, TSEvents::EventBus& e)
      : Task(200 * TASK_MILLISECOND, TASK_FOREVER, &s, false),
        TSEvents::EventHandler(&s, &e) {
  }

  bool OnEnable() {
    M5.Display.fillScreen(BLACK);
    return true;
  }

  bool Callback() {
    Render();
    return true;
  }

  void Render() {
    M5Dial.Display.setTextColor(WHITE);
    M5Dial.Display.setTextDatum(bottom_center);
    M5Dial.Display.drawString("Now Playing", 120, 35, &Roboto_Regular11pt7b);
    if (lastProgress == 0) {
      return;
    }
    if(cp_last.albumUri && cp.albumUri && strcmp(cp.albumUri, cp_last.albumUri) != 0) {
      Serial.println("Album changed: ");
      Serial.println(cp.albumUri);
      Serial.println(cp_last.albumUri);
      for (int i = 0; i < cp.numImages; i++) {
        if (cp.albumImages[i].width == 300) {
          Serial.print("Album art: ");
          Serial.println(cp.albumImages[i].url);
          RenderAlbumArt(cp.albumImages[i].url);
          Serial.println("Done");
          break;
        }
      }
    }
    // Serial.println("still alive");
    // Serial.println(cp.trackUri == nullptr ? "<null>" : cp.trackUri);
    // Serial.println(cp_last.trackUri == nullptr ? "<null>" : cp_last.trackUri);
    // if (cp_last.trackUri != nullptr && cp.trackUri != nullptr && strcmp(cp.trackUri, cp_last.trackUri) != 0) {
    //   Serial.print("Track: ");
    //   Serial.println(cp.trackName);
    //   RenderTrackName(cp.trackName, cp.artists[0].artistName);
    // }
    if (cp.durationMs != cp_last.durationMs || cp.progressMs < cp_last.progressMs) {
      Serial.print("Clearing ring");
      ClearProgressRing();
    }
    long actualProgress = cp.progressMs;
    if (cp.isPlaying) {
      actualProgress += (millis() - lastProgress);
    }
    RenderProgressText(actualProgress, cp.durationMs);
    RenderProgressRing(actualProgress, cp.durationMs);
    cp_last = cp;
  }

  void RenderTrackName(const char* trackName, const char* albumName) {
    M5Dial.Display.fillRect(0, 175, 240, 29, BLACK);
    M5Dial.Display.setTextColor(WHITE);
    M5Dial.Display.drawString(trackName, 120, 188, &Roboto_Regular11pt7b);
    M5Dial.Display.setTextColor(0xC638);
    M5Dial.Display.drawString(albumName, 120, 202, &Roboto_Regular11pt7b);
  }

  int msToTime(long ms, char* buffer) {
    int seconds = ms / 1000;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    return sprintf(buffer, "%d:%02d", minutes, seconds);
  }

  void RenderProgressText(long progressMs, long durationMs) {
    M5Dial.Display.fillRect(88, 206, 240, 152, BLACK);
    M5Dial.Display.setTextColor(0x9CD3);
    char text[23];
    int len = msToTime(progressMs, text);
    sprintf(text + len, " / ");
    len += 3;
    len += msToTime(durationMs, text + len);
    text[len] = '\0';
    M5Dial.Display.drawString(text, 120, 218, &Roboto_Regular11pt7b);
  }

  void ClearProgressRing() {
    M5Dial.Display.fillArc(120, 120, 103, 121, 0, 360, BLACK);
    M5Dial.Display.fillArc(120, 120, 104, 120, 0, 360, (uint32_t)0x3F3F3F);
    M5Dial.Display.fillCircle(120, 8, 8, (uint32_t)0x1DB954);
  }

  void RenderProgressRing(long progressMs, long durationMs) {
    float angle = (progressMs / (float)durationMs) * 360 - 90;
    M5Dial.Display.fillArc(120, 120, 104, 120, -90, angle, (uint32_t)0x1DB954);
    M5Dial.Display.fillCircle(120 + 112 * cos(angle * PI / 180), 120 + 112 * sin(angle * PI / 180), 8, (uint32_t)0x1DB954);
  }

  void RenderAlbumArt(const char* url) {
    M5Dial.Display.drawJpgUrl(url, 34, 34, 172, 172, 0, 0, 0.573, 0.573);
    M5Dial.Display.fillArc(120, 120, 86, 105, 0, 360, BLACK);
  }

  void HandleEvent(TSEvents::Event event) {
    switch (event.id) {
      case SPOTIFY_UPDATE: {
        CurrentlyPlaying* cp = *(CurrentlyPlaying**)event.data;
        this->cp = *cp;
        lastProgress = millis();
        break;
      }
    }
  }

 private:
  long lastProgress = 0;
  CurrentlyPlaying cp;
  CurrentlyPlaying cp_last;
};

class Controller : public Task, public TSEvents::EventHandler {
 public:
  Controller(Scheduler& s, TSEvents::EventBus& e)
      : Task(0, TASK_ONCE, &s, false),
        TSEvents::EventHandler(&s, &e) {
    nowPlayingView = new NowPlayingView(s, e);
  }

  ~Controller() {
    delete nowPlayingView;
  }

  bool OnEnable() {
    navigateTo(NOW_PLAYING);
    return false;
  }

  bool Callback() {
    return false;
  }

  void HandleEvent(TSEvents::Event event) {
    switch (event.id) {
    }
  }

  void navigateTo(View view) {
    this->view = view;
    if (viewTask != NULL) {
      viewTask->disable();
    }
    switch (view) {
      case NOW_PLAYING:
        viewTask = nowPlayingView;
        break;
      default:
        return;
    }
    viewTask->enable();
  }

 private:
  View view = NOW_PLAYING;
  Task* viewTask = NULL;

  // Views
  NowPlayingView* nowPlayingView;
};