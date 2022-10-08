#include "Arduino.h"
#include "60ghzbreathheart.h"

#ifdef __AVR__
    #include <SoftwareSerial.h>
    SoftwareSerial SSerial(2, 3); // RX, TX
    #define Serial1 SSerial
#endif

void BreathHeart_60GHz::SerialInit(){
  Serial1.begin(115200);
}

// Receive data and process
void BreathHeart_60GHz::recvRadarBytes(){
  static boolean recvInProgress = false;
  static byte ndx = 0;
  byte startMarker = MESSAGE_HEAD;            //Header frame
  byte endMarker = MESSAGE_TAIL;
  byte rb; // Each frame received
  while (Serial1.available() > 0 && newData == false)
  {
    rb = Serial1.read();
    if (recvInProgress == true)
    {                     // Received header frame
      if (rb != endMarker){ // Length in range
        Msg[ndx] = rb;
        ndx++;
      }
      else{
        recvInProgress = false;
        dataLen = ndx;
        ndx = 0;
        newData = true;
      }
    }
    else if (rb == startMarker){ // Waiting for the first frame to arrive
        recvInProgress = true;
    }
  }
}

//Radar transmits data frames for display via serial port
void BreathHeart_60GHz::ShowData(byte inf[]){
  for (byte n = 0; n < dataLen+3; n++) {
    Serial.print(inf[n], HEX);
    Serial.print(' ');
  }
    Serial.println();
}


// Judgment of occupied and unoccupied, approach and distance
void BreathHeart_60GHz::Situation_judgment(byte inf[]){
  switch(inf[2]){
    case HUMAN_PSE_RADAR:
      switch(inf[3]){
        case PRESENCE_INF:
          switch(inf[6]){
            case NOONE_HERE:
              ShowData(inf);
              Serial.println("Radar detects no one.");
              Serial.println("----------------------------");
              break;
            case SOMEONE_HERE:
              ShowData(inf);
              Serial.println("Radar detects somebody.");
              Serial.println("----------------------------");
              break;
          }
          break;
        case MOVE_INF:
          switch(inf[6]){
            case NONE:
              ShowData(inf);
              Serial.println("Radar detects None.");
              Serial.println("----------------------------");
              break;
            case STATIONARY:
              ShowData(inf);
              Serial.println("Radar detects somebody stationary.");
              Serial.println("----------------------------");
              break;
            case MOVEMENT:
              ShowData(inf);
              Serial.println("Radar detects somebody in motion.");
              Serial.println("----------------------------");
              break;
          }
          break;
        case BODY_SIG:
          ShowData(inf);
          Serial.print("The radar identifies the current motion feature value is: ");
          Serial.println(inf[6]);
          Serial.println("----------------------------");
          break;
        case DISTANCE:
          ShowData(inf);
          Serial.print("The distance of the radar from the monitored person is: ");
          Serial.print(inf[6]);
          Serial.print(" ");
          Serial.print(inf[7]);
          Serial.println(" cm");
          Serial.println("----------------------------");
          break;
        case ANGLE:
          ShowData(inf);
          Serial.print("The human orientation from the monitored person is, Pitch angle: ");
          Serial.print(inf[6]);
          Serial.print(" ");
          Serial.print(inf[7]);
          Serial.println(" °");
          Serial.print(" Horizontal angle: ");
          Serial.print(inf[8]);
          Serial.print(" ");
          Serial.print(inf[9]);
          Serial.println(" °");
          Serial.println("----------------------------");
          break;
      }
      break;
  }
}

//Respiratory sleep data frame decoding
String BreathHeart_60GHz::Breath_Heart(byte inf[]){
  switch(inf[2]){
    case BREATH_RATE_RADAR:
      switch(inf[3]){
        case BREATH_DATA:
          switch(inf[6]){
            case BREATH_NORMAL:
              ShowData(inf);
              return String("Radar detects that the current breath rate is normal.");
            case BREATH_RAPID:
              ShowData(inf);
              return String("Radar detects current breath rate is too fast");
            case BREATH_SLOW:
              ShowData(inf);
              return String("Radar detects current breath rate is too slow");
            case BREATH_DETECTING:
              ShowData(inf);
              return String("Current breath rate status is detecting");
          }
          break;
        case BREATH_VAL:
          ShowData(inf);
          return String("Radar monitored the current breath rate value is " + inf[6]);
        case BREATH_INTENSITY:
          ShowData(inf);
          return String("Radar monitored the current breath intensity value is " + inf[6]);
        case BREATH_CONFIDENCE:
          ShowData(inf);
          return String("Radar monitored the current breath confidence value is " + inf[6]);
        case BREATH_WAVE:
          ShowData(inf);
          return String("The respiratory waveform has not yet been developed.");
      }
      break;
    case SLEEP_INF:
      switch(inf[3]){
        case INOUT_BED:
          switch(inf[6]){
            case OUT_BED:
              ShowData(inf);
              return String("Radar detects someone currently leaving the bed.");
            case IN_BED:
              ShowData(inf);
              return String("Radar detects that someone is currently in bed.");
          }
          break;
        case SLEEP_STATE:
          switch(inf[6]){
            case AWAKE:
              ShowData(inf);
              return String("The radar detects that the monitoring people is awake.");
            case LIGHT_SLEEP:
              ShowData(inf);
              return String("The radar detects that the monitoring people is in light sleeping.");
            case DEEP_SLEEP:
              ShowData(inf);
              return String("The radar detects that the monitoring people is in deep sleeping.");
            case SLEEP_NONE:
              ShowData(inf);
              return String("The radar detects that the monitoring people nothing.");
          }
        case AWAKE_TIME:
          ShowData(inf);
          Serial.print("Radar monitored the awake sleep time is: ");
          Serial.print(inf[6]);
          Serial.print(" ");
          Serial.println(inf[7]);
          Serial.println("----------------------------");
          break;
        case LIGHTSLEEP_TIME:
          ShowData(inf);
          Serial.print("Radar monitored the light sleep time is: ");
          Serial.print(inf[6]);
          Serial.print(" ");
          Serial.println(inf[7]);
          Serial.println("----------------------------");
          break;
        case DEEPSLEEP_TIME:
          ShowData(inf);
          Serial.print("Radar monitored the deep sleep time is: ");
          Serial.print(inf[6]);
          Serial.print(" ");
          Serial.println(inf[7]);
          Serial.println("----------------------------");
          break;
        case SLEEP_SCORE:
          ShowData(inf);
          Serial.print("Radar judgment sleep score is: ");
          Serial.println(inf[6]);
          Serial.println("----------------------------");
          break;
      }
      break;
    case HEART_INF:
      switch(inf[3]){
        case RATE_DATA:
          switch(inf[6]){
            case RATE_NORMAL:
              ShowData(inf);
              return String("Radar detects that the current heart rate is normal.");
            case RATE_RAPID:
              ShowData(inf);
              return String("Radar detects current heart rate is too fast");
            case RATE_SLOW:
              ShowData(inf);
              return String("Radar detects current heart rate is too slow");
            case RATE_DETECTING:
              ShowData(inf);
              return String("Current heart rate status is detecting");
          }
          break;
        case HEART_RATE:
          ShowData(inf);
          return String("Radar monitored the current heart rate value is " + inf[6]);
        case RATE_INTENSITY:
          ShowData(inf);
          Serial.print("Radar monitored the current heart rate intensity value is " + inf[6]);
        case RATE_CONFIDENCE:
          ShowData(inf);
          Serial.print("Radar monitored the current heart rate confidence value is " + inf[6]);
        case HEART_RATE_WAVE:
          ShowData(inf);
          return String("The heart rate waveform has not yet been developed.");
        break;
      }
      break;
    case LOCA_DET_ANOMAL:
      switch(inf[3]){
        case LOCA_DET_ANOMAL:
          switch(inf[6]){
            case OUT_OF_RANGE:
              ShowData(inf);
              return String("Radar detects that the current user is out of monitoring range.");
            case WITHIN_RANGE:
              ShowData(inf);
              return String("Radar detects that the current user is within monitoring range.");
          }
      }
      break;
  }
}




