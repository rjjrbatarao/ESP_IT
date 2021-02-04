#ifndef __TASK_SCHED_H
#define __TASK_SCHED_H




#define _TASK_TIMECRITICAL
#define _TASK_TIMEOUT
#include <TaskScheduler.h>
Scheduler ts;

//Schedulers
void scheduleDynamicTask(uint8_t timeout);

void schedule0Callback();
void schedule0OnDisable();

void schedule1Callback();
void schedule1OnDisable();

void schedule2Callback();
void schedule2OnDisable();

void schedule3Callback();
void schedule3OnDisable();

void schedule4Callback();
void schedule4OnDisable();


Task schedule0(SCHEDULE_0_INTERVAL * TASK_SECOND, TASK_FOREVER, &schedule0Callback, &ts, false, NULL, &schedule0OnDisable);
Task schedule1(SCHEDULE_1_INTERVAL * TASK_SECOND, TASK_FOREVER, &schedule1Callback, &ts, false, NULL, &schedule1OnDisable);
Task schedule2(SCHEDULE_2_INTERVAL * TASK_SECOND, TASK_FOREVER, &schedule2Callback, &ts, false, NULL, &schedule2OnDisable);
Task schedule3(SCHEDULE_3_INTERVAL * TASK_SECOND, TASK_FOREVER, &schedule3Callback, &ts, false, NULL, &schedule3OnDisable);
Task schedule4(SCHEDULE_4_INTERVAL * TASK_SECOND, TASK_FOREVER, &schedule4Callback, &ts, false, NULL, &schedule4OnDisable);


void ICACHE_FLASH_ATTR scheduleDynamicTask(uint8_t timeout, uint8_t index_time, uint8_t timer_interval) {
  flexibleTimer[index_time].interval = timer_interval;
  switch (index_time) {
    case 0:
      schedule0.setInterval(timer_interval * TASK_SECOND);
      schedule0.setIterations(timeout * timer_interval);

      schedule0.setTimeout(timeout * timer_interval * TASK_SECOND);

      schedule0.enable();
      break;
    case 1:
      schedule1.setInterval(timer_interval * TASK_SECOND);
      schedule1.setIterations(timeout * timer_interval);

      schedule1.setTimeout(timeout * timer_interval * TASK_SECOND);

      schedule1.enable();
      break;
    case 2:
      schedule2.setInterval(timer_interval * TASK_SECOND);
      schedule2.setIterations(timeout * timer_interval);

      schedule2.setTimeout(timeout * timer_interval * TASK_SECOND);

      schedule2.enable();
      break;
    case 3:
      schedule3.setInterval(timer_interval * TASK_SECOND);
      schedule3.setIterations(timeout * timer_interval);

      schedule3.setTimeout(timeout * timer_interval * TASK_SECOND);

      schedule3.enable();
      break;
    case 4:
      schedule4.setInterval(timer_interval * TASK_SECOND);
      schedule4.setIterations(timeout * timer_interval);

      schedule4.setTimeout(timeout * timer_interval * TASK_SECOND);

      schedule4.enable();
      break;
  }

}

void ICACHE_FLASH_ATTR timerDisable(uint8_t timer_index) {
  switch (timer_index) {
    case 0:
      schedule0.disable();
      break;
    case 1:
      schedule1.disable();
      break;
    case 2:
      schedule2.disable();
      break;
    case 3:
      schedule3.disable();
      break;
    case 4:
      schedule4.disable();
      break;
  }
}

void ICACHE_FLASH_ATTR timerResponse(uint8_t name_index) {
  long time_response;
  switch (name_index) {
    case 0:
      time_response = schedule0.untilTimeout() / 1000;
      break;
    case 1:
      time_response = schedule1.untilTimeout() / 1000;
      break;
    case 2:
      time_response = schedule2.untilTimeout() / 1000;
      break;
    case 3:
      time_response = schedule3.untilTimeout() / 1000;
      break;
    case 4:
      time_response = schedule4.untilTimeout() / 1000;
      break;
  }
  Serial.printf("%s:%d\n", flexibleTimer[name_index].name , time_response / flexibleTimer[name_index].interval);
}

void schedule0Callback() {

  timerResponse(0);
}
void schedule0OnDisable() {
  Serial.printf("%s:%s\n", flexibleTimer[0].name , "disabled");
}


void schedule1Callback() {

  timerResponse(1);
}
void schedule1OnDisable() {
  Serial.printf("%s:%s\n", flexibleTimer[1].name , "disabled");
}


void schedule2Callback() {

  timerResponse(2);
}
void schedule2OnDisable() {
  Serial.printf("%s:%s\n", flexibleTimer[2].name , "disabled");
}


void schedule3Callback() {

  timerResponse(3);
}
void schedule3OnDisable() {
  Serial.printf("%s:%s\n", flexibleTimer[3].name , "disabled");
}


void schedule4Callback() {

  timerResponse(4);
}
void schedule4OnDisable() {
  Serial.printf("%s:%s\n", flexibleTimer[4].name , "disabled");
}



#endif
