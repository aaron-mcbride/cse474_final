#include <Arduino_FreeRTOS.h>
#include <Arduino.h>


#define BLINK_PIN 53

void TaskBlinkExternal(void *pvParameters) {
  pinMode(BLINK_PIN, OUTPUT);

    for (;;) // A Task shall never return or exit.
  {
    digitalWrite(BLINK_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 100 / portTICK_PERIOD_MS ); // wait for one second
    digitalWrite(BLINK_PIN, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay( 200 / portTICK_PERIOD_MS ); // wait for one second
  }
}

void TaskBlink(void *pvParameters)  // This is a task.
{
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 250 / portTICK_PERIOD_MS ); // wait for one second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay( 100 / portTICK_PERIOD_MS ); // wait for one second
  }
}

void TaskAnalogRead(void *pvParameters)  // This is a task.
{
  for (;;)
  {
    // read the input on analog pin 0:
    int sensorValue = analogRead(A7);  /// modify for your input pin!
    // print out the value you read:
    Serial.println(sensorValue);
    vTaskDelay(500/portTICK_PERIOD_MS);  // 0.5 sec in between reads for stability
  }
}

static const int arrsize = 30;

static const int speaker_freq_array[] = {
    659, 659, 0,   659, // E, E, rest, E
    523, 659, 784, 0,   // C, E, G, rest
    392, 0,   523, 392, // G, rest, C, G
    329, 0,   440, 493, // E, rest, A, B
    466, 440, 0,   392, // A#, A, rest, G
    659, 784, 880, 698, // E, G, A, F
    784, 659, 523, 587, // G, E, C, D
    494, 0              // B, rest
};

// Array of song note delay
static const int speaker_delay_array[] = {
    150, 150, 100, 150, // E, E, rest, E
    150, 150, 150, 100, // C, E, G, rest
    150, 100, 150, 150, // G, rest, C, G
    150, 100, 150, 150, // E, rest, A, B
    150, 150, 100, 150, // A#, A, rest, G
    150, 150, 150, 150, // E, G, A, F
    150, 150, 150, 150, // G, E, C, D
    150, 100            // B, rest
};

void TaskPlaySpeaker(void *pvParameters)  // This is a task.
{
  int idx = 0, time;
  for (;;)
  {
    tone(33, speaker_freq_array[idx]);
    vTaskDelay((speaker_delay_array[idx] * 2)/portTICK_PERIOD_MS); 
    idx++;
    if(idx == 30){ 
      noTone(33); 
      time += 1; 
      Serial.println(time);
      if(time == 3){
        vTaskSuspend(NULL); 
      }
      idx = 0; 
      vTaskDelay(1500 / portTICK_PERIOD_MS); 
    }
  }
}

void pt1_demo() {
  
  Serial.begin(9600);
  
  while (!Serial);

  xTaskCreate(
    TaskBlinkExternal
    , "TaskBlinkExternal"
    , 128
    , NULL
    , 2
    , NULL
  );

  xTaskCreate(
    TaskBlink
    , "TaskBlink"
    , 128
    , NULL
    , 2
    , NULL
  );

  xTaskCreate(
    TaskAnalogRead
    , "TaskBlink"
    , 128
    , NULL
    , 2
    , NULL
  );

  xTaskCreate(
    TaskPlaySpeaker
    , "TaskPlaySpeaker"
    , 128
    , NULL
    , 2
    , NULL
  );

  vTaskStartScheduler();
}