#include <Arduino.h>
#define _TASK_OO_CALLBACKS
#define _TASK_STATUS_REQUEST
#include <EventHandler.h>
#include <M5Dial.h>
#include <TaskSchedulerDeclarations.h>

#include "events.h"
#include "encoder.h"

class InputTask : public Task, public TSEvents::EventEmitter {
 public:
  InputTask(Scheduler& s, TSEvents::EventBus& e)
      : Task(100*TASK_MILLISECOND, TASK_FOREVER, &s, false),
        TSEvents::EventEmitter(&e) {
  }

  bool OnEnable() {
    init_encoder();
    return true;
  }

  bool Callback() {
    M5Dial.update();

    // Button handling
    if (M5Dial.BtnA.wasPressed()) {
      buttonPressed = true;
      lastButtonPress = millis();
      dispatch(BUTTON_PRESS);
    }
    if (M5Dial.BtnA.wasReleased()) {
      if (buttonPressed) {
        dispatch(BUTTON_RELEASE);
      }
      buttonPressed = false;
    }
    if (buttonPressed && millis() - lastButtonPress > 1500) {
      dispatch(BUTTON_LONG_PRESS);
      // No release event for long press
      buttonPressed = false;
    }
    
    // Dial handling
    // long position = M5Dial.Encoder.read();
    int16_t position = get_encoder();
    if (position != lastPosition) {
      int16_t delta = position - lastPosition;
      dispatch(DIAL_MOVE, &delta, sizeof(delta));
      lastPosition = position;
    }
    return true;
  }

  private:
    bool buttonPressed = false;
    ulong lastButtonPress = 0;
    int16_t lastPosition = 0;
};