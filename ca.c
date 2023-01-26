#include <pthread.h>    // pthread
#include <stdio.h>      // prinftf
#include <stdlib.h>     // random
#include <unistd.h>     // sleep
#include <semaphore.h>  // semaphores
#include <stdbool.h>    // booleans 

void *doctor_actions();
void *car_actions(void *car_id);

#define NUM_CARS 5
#define MAX_QUEUE 3



sem_t semCars;
sem_t semDoctor;
pthread_mutex_t mutex; 


int carsInLine = 0;
bool dSleeping = true;

int main(int argc, char **argv) {

    int car_ids[NUM_CARS];

    pthread_t cars[NUM_CARS];
    pthread_t doctor;
    
    sem_init(&semCars, 0, 0);
    sem_init(&semDoctor, 0, 1);
    
    // create threads
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&doctor, NULL, doctor_actions, NULL);
    for (int i = 0; i < NUM_CARS; i++) {
        car_ids[i] = i;
        pthread_create(&cars[i], NULL, car_actions, (void *)&car_ids[i]);
    }

    // join threads
    pthread_join(doctor, NULL);
    for (int i = 0; i < NUM_CARS; i++) {
        pthread_join(cars[i], NULL);
    }

    return 0;
}

void *doctor_actions() {
    while (1) {
    	if(carsInLine > 0){
    		dSleeping = false;
    		sem_wait(&semCars);
    		pthread_mutex_lock(&mutex);
    		int time = rand() % 10;
    		printf("Patient is getting tested for %d seconds.\n", time);
    		carsInLine--;
    		sleep(time);
    		printf("Patient is finished testing.\n");
    		pthread_mutex_unlock(&mutex);
    		sem_post(&semDoctor);
    		
	}else{
		if(dSleeping = false){ //Avoids multiple prints when the doctor is sleeping 
			dSleeping = true;
			printf("No cars in line. Sleeping.\n");
		}
	}
    }
}

void *car_actions(void *car_id) {
    // get the id of this car
    int id_car = *(int *)car_id;

    while (1) {
    
    	int time = rand() % 10;
    	printf("Car %d is driving for %d seconds.\n", id_car, time);
    	sleep(time);
    	pthread_mutex_lock(&mutex);
    	
    	if(carsInLine < MAX_QUEUE){
    		
    		carsInLine++;
    		printf("Car %d enters the line. Cars in line = %d.\n", id_car, carsInLine);
    		pthread_mutex_unlock(&mutex);
    		if(dSleeping = true){
    			printf("Car %d honks to wake the doctor.\n", id_car);
    		}
    		sem_post(&semCars);
    		sem_wait(&semDoctor);
    	}else{
    	
    		pthread_mutex_unlock(&mutex);
    		printf("Car %d will try later.\n", id_car);
    	}
    
    }
}
