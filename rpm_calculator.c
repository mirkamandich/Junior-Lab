#include <stdio.h>
#include <signal.h>    //signal, SIGINT, SIGQUIT, SIGTERM
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <pthread.h>
#include <sys/time.h>  //gettimeofday, timeval

int pulseCounter_A, count;
float rpm;
pthread_t calcRPMThr;


// ISR function for the encoder counter
void counterA_ISR (void)
{
    ++pulseCounter_A;
}

// Local function declarations
/// Function controlling exit or interrupt signals
void control_event(int sig);

/// Calculate RPM Thread, runs continuously
void* calc_RPM (void* arg);

static long time_diff_us(struct timeval end , struct timeval start)
{
    return (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
}

int main (int argc, char *argv[])
{
    // Inform OS that control_event() function will be handling kill signals
    (void)signal(SIGINT, control_event);
    (void)signal(SIGQUIT, control_event);
    (void)signal(SIGTERM, control_event);
    
    if (wiringPiSetup() != 0)
    {
        // Handles error Wiring Pi initialization
        fprintf(stderr, "Unable to setup wiringPi: %s\n", strerror(errno));
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    int ret = pthread_create( &(calcRPMThr), NULL, calc_RPM, NULL);
    if( ret )
    {
        fprintf(stderr,"Error creating calcRPMThr - pthread_create() return code: %d\n",ret);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    printf("Started RPM calculator\n");
    fflush(stdout);
    
    while(1)
    {
        printf("\rRPM: %.f         ", rpm);
        fflush(stdout);
        delay(1000);
    }
}

void* calc_RPM (void* arg)
{
    pinMode(29, INPUT);
    wiringPiISR(29, INT_EDGE_FALLING, &counterA_ISR);
    int rpm_sampling_ms = 5000;
    int diff_time_us;
    struct timeval start, end;

    while(1)
    {
        pulseCounter_A = 0;
        gettimeofday (&start, NULL);
        delay(rpm_sampling_ms); //Always keep a sleep or delay in infinite loop
        count = pulseCounter_A;
        gettimeofday (&end, NULL);
        diff_time_us = time_diff_us(end, start);
        rpm = ((float)count * 60.0 * 1.0e6) / (float)diff_time_us;
    }
}

void control_event(int sig)
{
    printf("\nExiting ... \n");
    delay(100);
    printf("Done\n");
    exit(EXIT_SUCCESS);
}
