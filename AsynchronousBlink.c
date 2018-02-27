//Program to blink 2 LEDs with different frequencies:

#include <wiringPi.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

pthread_t faster_thread;
void* Blinkfast(void* arg);

int main(void) {
  wiringPiSetup();
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  int create_success;
  
  create_success = pthread_create(&faster_thread, NULL,
                          Blinkfast, NULL);
  if(create_success==0)
  {
	while (1)
	{
		digitalWrite(2, HIGH);
		delay(750);
		digitalWrite(2, LOW);
		delay(500);
    }
  }
  else{
	  return 1;
  }

  return 0;
}

void* Blinkfast(void* arg) {
	while (1){
      digitalWrite(0, HIGH);
      delay(500);
      digitalWrite(0, LOW);
      delay(400);
    }
}
