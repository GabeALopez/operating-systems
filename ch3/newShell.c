#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>

//Envrionment Vars
struct EnvVars{
	char currentCommand[200];
	char currentWorkingDir[200];
};

//Message of the Day
void motd(){
	printw("Another day, another Towle assignment\n");
}

//Function to execute commands
void executeCommand(char *command, char *args[], struct EnvVars *envVars) {

    //Check for whether or not vim is being used
    bool isFullScreenApp = (strcmp(command, "vim") == 0);

    char *currentWorkingDirBuffer = envVars->currentWorkingDir;
    size_t currentWorkingDirSize = sizeof(envVars->currentWorkingDir);

    if (isFullScreenApp) {
        endwin();
    }

    //Handle cd command
    if (strcmp(command, "cd") == 0) {
	printw("-----Starting Program-----\n");
        if (args[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd failed");
            } else {
                if (getcwd(currentWorkingDirBuffer, currentWorkingDirSize) == NULL) {
                } else {
                    //perror("getcwd error");
                }
            }
        }
	printw("-----Program Ended-----\n");
        return;
    }

    //Create pipe to transfer data to curses shell
    int cursesTermPipe[2];
    if (!isFullScreenApp && pipe(cursesTermPipe) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    //Run user commmand
    if (pid == 0) {
        if (!isFullScreenApp) {
	    printw("-----Program Starting-----\n");
            close(cursesTermPipe[0]);
            dup2(cursesTermPipe[1], STDOUT_FILENO);
            close(cursesTermPipe[1]);
        }

        if (execvp(command, args) == -1) {
            fprintf(stderr, "Error: Failed to execute command '%s'\n", command);
            exit(EXIT_FAILURE);
        }
    } else {
        if (!isFullScreenApp) {
	    printw("-----Program Starting-----\n");
            close(cursesTermPipe[1]);

            char outputPipeBuffer[1024];
            ssize_t count;
            while (1) {

		count = read(cursesTermPipe[0], outputPipeBuffer, sizeof(outputPipeBuffer) - 1);

    		if (count <= 0) { 
        		break;
    		}	

                outputPipeBuffer[count] = '\0';
                printw("%s", outputPipeBuffer);
		printw("-----Program Ended-----\n");
                refresh();
            }

            close(cursesTermPipe[0]);
        }

        wait(NULL);

        if (isFullScreenApp) {
            initscr();
            start_color();
            noecho();
        }
    }
}


//Main function
int main(){

	//Initialize
	initscr();          
    	start_color();      
    	//cbreak();           
    	noecho();           

	init_pair(1, COLOR_CYAN, COLOR_BLACK);  
	init_pair(2, COLOR_GREEN, COLOR_BLACK);

	//Call message of the day and set base vars
	motd();
	const char *exitStr = "exit";
	const char *clearStr = "clear";
	struct EnvVars envVars;
	//Main loop
	while (1){
		attron(COLOR_PAIR(1));
        	printw("> ");           
        	attroff(COLOR_PAIR(1));

		attron(COLOR_PAIR(2));
		echo();

		getnstr(envVars.currentCommand, sizeof(envVars.currentCommand) - 1);
		
		envVars.currentCommand[strcspn(envVars.currentCommand, "\n")] = 0;
		attroff(COLOR_PAIR(2));

		noecho();
		
		if(strcmp(envVars.currentCommand, exitStr) == 0){
			break;
		}

		if (strcmp(envVars.currentCommand, clearStr) == 0) {
            		clear();
            		motd();
            		continue;
        	}

		//Run command logic
		char *args[200];
        	char *token;
        	int i = 0; 

		token = strtok(envVars.currentCommand, " ");
        	while (token != NULL) {
            		args[i++] = token;
            		token = strtok(NULL, " ");
        	}
		
		 args[i] = NULL;

		
        	if (i > 0) {
            		executeCommand(args[0], args, &envVars);
        	}
		
	}

	endwin();
	return EXIT_SUCCESS;

}
