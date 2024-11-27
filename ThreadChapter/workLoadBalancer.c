#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

//global semaphore array
sem_t semArr[3];

//struct to hold data of thread
typedef struct {

        int id;
        int work;

} threadData;

//function used for sorting function to tell which value is higher
int compare(const void *pA, const void *pB) {

        int a = *(int *)pA;
        int b = *(int *)pB;

        return a - b;
}

void* workFunction(void* arg){

        //get thread data for thead
        threadData* data = (threadData*)arg;

        //set up numOfSemChosen and semIndex array to keep track of semahpores and how many are being used
        int semIDs[3] = {0, 0, 0};
        int numOfSemChosen = 0;


        while(data->work > 0){

                //choose random number of semaphores
                numOfSemChosen = rand() % 3 + 1;

                //for each semaphore add thier ids' to an array
                for (int i = 0; i < numOfSemChosen; i++){
                        int randArrIndex;

                        do{
                                randArrIndex = rand() % 3;
                                int isUnique = 1;
                                for (int j = 0; j < i; j++) {
                                        if (semIDs[j] == randArrIndex) {
                                                isUnique = 0;
                                                break;
                                        }
                                }

                                if (isUnique) {
                                        semIDs[i] = randArrIndex;
                                        break;
                                }
                        } while(1);
                }

                //sort semaphore ids in ascending order to have the semaphore going to be used at the top
                qsort(semIDs, numOfSemChosen, sizeof(int), compare);

                //for semaphores being used put a "lock" on them by waiting
                int semAcquired = 1;
                for (int i = 0; i < numOfSemChosen; i++) {
                        if (sem_wait(&semArr[semIDs[i]]) != 0) {
                                semAcquired = 0;
                                break;
                        }
                }

                //if semaphore is aquired do work for a random time and then release it
                if (semAcquired) {
                        data->work--;
                        printf("Thread %d: Work decreased, current work = %d\n", data->id, data->work);

                        int sleepTime = rand() % 10000;
                        usleep(sleepTime);

                        for (int i = 0; i < numOfSemChosen; i++) {
                                sem_post(&semArr[semIDs[i]]);
                                printf("Thread %d released semaphore %d\n", data->id, semIDs[i]);
                        }

                }else {
                //if all semaphores can't be aquired then try again in the loop

                        for (int i = 0; i < numOfSemChosen; i++) {
                                sem_post(&semArr[semIDs[i]]);
                        }
                        printf("Thread %d failed to acquire all semaphores. Retrying...\n", data->id);
                }

        }

        //print message that all threads have finished
        printf("Thread %d has finished all work.\n", data->id);
        pthread_exit(NULL);
}

int main(){
        //set up threads and thread data arrays
        pthread_t threads[5];
        threadData threadDataArr[5];

        //initialize semaphores
        for (int i = 0; i < 3; i++) {
                if (sem_init(&semArr[i], 0, 1) != 0) {
                    perror("sem_init failed");
                    exit(EXIT_FAILURE);
                }
            }

        //fire off threads
        for(int i = 0; i < 5; i++){
                threadDataArr[i].work = 10;
                threadDataArr[i].id = i + 1;

                pthread_create(&threads[i], NULL, workFunction, &threadDataArr[i]);
        }

        //join threads together when finishing
        for (int i = 0; i < 5; i++) {
                pthread_join(threads[i], NULL);
        }


        return 0;

}