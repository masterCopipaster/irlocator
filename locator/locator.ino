#include <TimerOne.h>

#define ROUND_SENSOR_SIZE 12
#define TAG_COUNT 6 
#define START_BORDER 0

#define WAIT_TIMER_PERIOD 1100
#define CAPTURE_TIMER_PERIOD 5000
#define PRECAPTURE_TIMER_PERIOD 18500

uint8_t round_sensor_image[ROUND_SENSOR_SIZE];
uint8_t round_sensor_count;
uint8_t round_sensor_pins[ROUND_SENSOR_SIZE] = {3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14};
uint8_t inframe_elem_count = 0;
float frame_angles[TAG_COUNT];
unsigned long timelable;
float calculated_sensor_angle;


//FRAME STRUCTURE
// .....|START| 1 | 2 | 3 |.....|STOP|
//   0     1    2   3   4  ......  8
//      |  on | .. data ..      | off| 

void data_capture_isr()
{
  read_round_sensor();
  /*
  Serial.println(inframe_elem_count);
  sensor_image_visualise();
  */
  if(inframe_elem_count == 0)
  {
    if(round_sensor_count > START_BORDER)
    {
      inframe_elem_count++;
      Timer1.initialize(PRECAPTURE_TIMER_PERIOD);
    }
    return;
  } 
  if(inframe_elem_count == 1)
  {
    Timer1.initialize(CAPTURE_TIMER_PERIOD);
  }
  calculate_angle();
  //Serial.println(millis());
  frame_angles[inframe_elem_count - 1] = calculated_sensor_angle;
  inframe_elem_count++;
  if(inframe_elem_count == (TAG_COUNT + 2))
  {
    inframe_elem_count = 0;
    timelable = micros() / 1000;
    Timer1.initialize(WAIT_TIMER_PERIOD);
    return;
  } 
}

void setup() {
  init_round_sensor_pins();
  Timer1.initialize(WAIT_TIMER_PERIOD);
  Timer1.attachInterrupt(data_capture_isr);
  Serial.begin(9600);
}

void loop() {
  //noInterrupts();
  //read_round_sensor();
  sensor_frame_visualise();
  //Serial.println(sizeof(frame_angles));
  delay(100);
}


inline void init_round_sensor_pins()
{
  for(int i = 0; i < ROUND_SENSOR_SIZE; i++)
  {
    pinMode(round_sensor_pins[i], INPUT_PULLUP);
  }
}

inline void read_round_sensor()
{
  round_sensor_count = 0;
  for(int i = 0; i < ROUND_SENSOR_SIZE; i++)
  {
    round_sensor_image[i] = !digitalRead(round_sensor_pins[i]);
    round_sensor_count += round_sensor_image[i];
  }
}

inline void sensor_image_visualise()
{
  Serial.print("{");
  for(int i = 0; i < ROUND_SENSOR_SIZE; i++)
  {
    Serial.print(round_sensor_image[i] ? '|' : ' '); 
  }
  Serial.print("}");
  Serial.println(round_sensor_count);
}

inline void sensor_frame_visualise()
{
  Serial.print("{");
  for(int i = 0; i < TAG_COUNT; i++)
  {
    Serial.print(frame_angles[i]); 
    Serial.print('\t'); 
  }
  Serial.print("}");
  Serial.println(timelable);
}


inline void calculate_angle()
{
  float r1w = 0;
  float r2w = 0;
  for(uint8_t i = 0; i < ROUND_SENSOR_SIZE; i++) r1w += (float)round_sensor_image[i] * i * (360.0 / ROUND_SENSOR_SIZE); 
  //for(int8_t i = ROUND_SENSOR_SIZE - 1; i >= 0; i--) r2w += (float)round_sensor_image[i] * (ROUND_SENSOR_SIZE - 1 - i) * (360.0 / ROUND_SENSOR_SIZE);
  calculated_sensor_angle = round_sensor_count > 0 ? r1w / round_sensor_count : -1;
}
