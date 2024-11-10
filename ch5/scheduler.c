#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
	char name[50];
	int time;
	int arrivalTime;
	int completed;
	int remainingTime;
	int completionTime;
	int firstExecutionTime;
	int isFirstExecuted;

} Process;

double calculateAvg(int numbers[], int size){

	double sum = 0;
	for(int i = 0; i < size; i++){

		sum += (double)numbers[i];
		
	}
	
	return (sum)/(size);
}

void FIFO(Process processArr[], int n){

	//Sort Arr for FIFO
	for (int i = 0; i < n - 1; i++) {
        	for (int j = i + 1; j < n; j++) {
            		if (processArr[i].arrivalTime > processArr[j].arrivalTime) {
                		Process temp = processArr[i];
                		processArr[i] = processArr[j];
                		processArr[j] = temp;
            }
        }
    }
	
	
	int currentTime = 0;
	int waitingTimeNums[n];
	int responseTimeNums[n];
	int throughput = 0;

	//Get wait and response times
	for(int i = 0; i < n; i++){

		if(currentTime < processArr[i].arrivalTime){
			currentTime = processArr[i].arrivalTime;
		}

		waitingTimeNums[i] = currentTime - processArr[i].arrivalTime;
		responseTimeNums[i] = waitingTimeNums[i];

		printf("%s	", processArr[i].name);

		for(int j = 0; j < currentTime; j++){
			printf("_");
		}
		
		for(int j = 0; j < processArr[i].time; j++){
			printf("#");
			
		}

		printf("\n");


		currentTime += processArr[i].time;

		if(currentTime < 10){
			
			if(processArr[i].time > 10){
				continue;
			}
			throughput++;
			
		}
		
	}


	//Calculate averages
	double avgWaitTime = calculateAvg(waitingTimeNums, n);
	double avgResponseTime = calculateAvg(responseTimeNums, n);
	
	printf("Average Wait Time: %f\n", avgWaitTime);
	printf("Average Response Time: %f\n", avgResponseTime);
	printf("Throughput after 10 cycles: %d\n", throughput);

	
}

void SJF(Process processArr[], int size){

	int time = 0;
	int completed = 0;
	int completedInTenSecs = 0;
	int waitingTimeNums[size];
	int responseTimeNums[size];
	int remainingTime[size];
	int tournAroundTime[size];
	int startTime[size];

	//Initialize gantt chart
    	char gantt[size][100]; 
    	for (int i = 0; i < size; i++) {
        	memset(gantt[i], '_', sizeof(gantt[i]));
        	gantt[i][99] = '\0';  
    	}

	//Set up variables for each process
	for(int i = 0; i < size; i++){
		remainingTime[i] = processArr[i].time;
		waitingTimeNums[i] = 0;
		responseTimeNums[i] = -1;
		startTime[i] = -1;
	}

	while(completed != size){
		int chkIdx = -1;
		int minRemainingTime = 9999;

		//Check if process can go based on its arrival time, if completed, and if it has taken its remaining time
		for(int i = 0; i < size; i++){
			if(processArr[i].arrivalTime <= time && !processArr[i].completed && remainingTime[i] < minRemainingTime){
				minRemainingTime = remainingTime[i];
				chkIdx = i;
			}
		}

		
		//check process that is in focus to determine remaining time left
		if(chkIdx != -1){
			
			if(responseTimeNums[chkIdx] == -1){
				responseTimeNums[chkIdx] = time - processArr[chkIdx].arrivalTime;
			}

			if(startTime[chkIdx] == -1){
				startTime[chkIdx] = time;
			}

			remainingTime[chkIdx]--;
			gantt[chkIdx][time] = '#';
			time++;

			if(remainingTime[chkIdx] == 0){
				processArr[chkIdx].completed = 1;
				completed++;
				tournAroundTime[chkIdx] = time - processArr[chkIdx].arrivalTime;
				waitingTimeNums[chkIdx] = tournAroundTime[chkIdx] - processArr[chkIdx].time;

				if (time <= 10) {
                                        completedInTenSecs++;
                                }

			}
				
			
			
		}else{
			time++;
		}
	}

	//print ganttChart
    	for (int i = 0; i < size; i++) {
        	printf("%s	%s\n", processArr[i].name, gantt[i]);
    	}

	printf("\n");

	//Calculate averages
	double avgWaitTime = calculateAvg(waitingTimeNums, size);
	double avgResponseTime = calculateAvg(responseTimeNums, size);
	
	printf("Average Wait Time: %f\n", avgWaitTime);
	printf("Average Response Time: %f\n", avgResponseTime);
	printf("Throughput over 10 seconds %d\n", completedInTenSecs);
	
}

void roundRobin(Process processArr[], int size, int quantum) {
	int time = 0;
	int completed = 0;
	int totalWaitingTime = 0;
	int totalResponseTime = 0;
	int completedInTenSecs = 0;
	int waitingTime = 0;
	int responseTime = 0;
	
	//Initialize gantt chart
    	char gantt[size][100];  
    	for (int i = 0; i < size; i++) {
        	memset(gantt[i], '_', sizeof(gantt[i]));  
        	gantt[i][99] = '\0'; 
    	}

	while(completed < size){
		//Run through each process for remaining time and subtract process's remaining time when running
		for(int i = 0; i < size; i++){
			if(processArr[i].remainingTime > 0){
				
				//Is the process's first time executing?
				if (!processArr[i].isFirstExecuted) {
                    			processArr[i].firstExecutionTime = time;
                    			processArr[i].isFirstExecuted = 1;
                		}

				//Subtract quantum time when process runs
				if(processArr[i].remainingTime > quantum){
					time += quantum;
					processArr[i].remainingTime -= quantum;
					gantt[i][time] = '#';
				} else{
					time += processArr[i].remainingTime;
					processArr[i].remainingTime = 0;
					processArr[i].completionTime = time;
					completed++;

					waitingTime = processArr[i].completionTime - processArr[i].time;
					totalWaitingTime += waitingTime;

					responseTime = processArr[i].firstExecutionTime;
					totalResponseTime += responseTime;

					if(time <= 10){
						completedInTenSecs++;
					}
				}
			}
		}
	}
	

	//Print gannt chart
    	for (int i = 0; i < size; i++) {
        	printf("%-10s %s\n", processArr[i].name, gantt[i]);
    	}
	
	printf("\n");

	//Calculate averages
	double avgWaitingTime = (double)totalWaitingTime / size;
	double avgResponseTime = (double)totalResponseTime / size;

	printf("Average Wait Time: %f\n", avgWaitingTime);
	printf("Average Response Time: %f\n", avgResponseTime);
	printf("Throughput Over Ten Seconds: %d\n", completedInTenSecs);
}

int main(){
	FILE *file = fopen("processFile.txt", "r");

	Process processes[4];
	char line[256];  
	int processCount = 0;

	//Initialize processes from text file
    	while (fgets(line, sizeof(line), file) != NULL) {
		char *name = strtok(line, " ");
        	char *timeStr = strtok(NULL, " ");
        	char *arrivalTimeStr = strtok(NULL, " ");

		strcpy(processes[processCount].name, name);
		processes[processCount].time = atoi(timeStr);
		processes[processCount].arrivalTime = atoi(arrivalTimeStr);
		processes[processCount].completed = 0;
		processes[processCount].remainingTime = processes[processCount].time;
		processCount++;
	}
	fclose(file);	

	//Get size of arr
	int size = sizeof(processes)/sizeof(processes[0]);

	//Run through algorithms
	FIFO(processes, size);
	printf("\n");
	SJF(processes, size);
	printf("\n");
	roundRobin(processes, size, 1);

	return 0;
}
