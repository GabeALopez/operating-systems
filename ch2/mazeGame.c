#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/random.h>

#define BUFFER_SIZE 2048

#define PROC_NAME "mazeGame"
#define HEIGHT 9
#define WIDTH 9
#define WALL 0
#define PATH 1

/**
	Name: Gabriel Lopez
	Date: 9/16/24
	Description: Template protype from hello.c
 */
ssize_t produceMaze(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

/**
	Name: Gabriel Lopez
	Date: 9/16/24
	Description: Template struct from hello.c
 */

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = produceMaze,
};

/**
	Name: Gabriel Lopez
	Date: 9/17/24
	Description: A struct to hold the value of coordinates of a single cell
 */
typedef struct {
    int x, y;
} Cell;

/**
	Name: Gabriel Lopez
	Date: 9/17/24
	Description: Generates random numbers between 0 and 99. Was based off of professors rand function
 */
unsigned int produceRandNum(void) {
    unsigned int i;
    int lessThan100;
    get_random_bytes(&i, sizeof(i));
    lessThan100 = i % 100;
    return lessThan100;
}

/**
	Name: Gabriel Lopez
	Date: 9/17/24
	Description: Produces the random number for coordinates at the boarder
 */
unsigned int produceBoarderNumber(void) {
	unsigned int i;
	int lessThan100, x, y, temp;
	get_random_bytes(&i, sizeof(i));
	x = i % WIDTH;		
	return x;
}

/**
	Name: Gabriel Lopez
	Date: 9/17/24
	Description: Initializes the maze
 */

void initMaze(int maze[HEIGHT][WIDTH]) {
    printk(KERN_INFO "Initialize maze");
    int y, x;
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            maze[y][x] = WALL;
        }
    }
}

/**
	Name: Gabriel Lopez
	Date: 9/17-19/24
	Description: Function to check if a coordinate is in bounds
 */

int inBounds(int x, int y) {
    return (x > 0 && x < WIDTH && y > 0 && y < HEIGHT);
}

/**
	Name: Gabriel Lopez
	Date: 9/17/24
	Description: Function to print the maze to kernel for debug
 */
void printMaze(int maze[HEIGHT][WIDTH]) {
    int y, x;
    printk(KERN_INFO "Current Maze State:\n");
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (maze[y][x] == WALL) {
                printk(KERN_CONT "#");
            } else {
                printk(KERN_CONT " ");
            }
        }
        printk(KERN_CONT "\n");
    }
    printk(KERN_CONT "\n");
}

/**
	Name: Gabriel Lopez
	Date: 9/17-20/24
	Description: Function to create the maze
 */
void createMaze(int maze[HEIGHT][WIDTH]) {
    int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    int visited[HEIGHT][WIDTH] = {0};  // Initialize visited to 0
    Cell walls[WIDTH * HEIGHT];
    int wallCount = 0;
    unsigned int randNum;
    int i, j, nx, ny, px, py, mx, my;

    // Start position
    randNum = produceRandNum();
    int startX = randNum % WIDTH;
    int startY = randNum % HEIGHT;

    printk(KERN_INFO "startX: %d\n", startX);
    printk(KERN_INFO "startY: %d\n", startY);

    maze[startY][startX] = PATH;
    visited[startY][startX] = 1;

    // Add surrounding walls to the wall list
    for (i = 0; i < 4; i++) {
        nx = startX + directions[i][0] * 2;
        ny = startY + directions[i][1] * 2;
        if (inBounds(nx, ny) && !visited[ny][nx]) {
            walls[wallCount++] = (Cell){nx, ny};
            visited[ny][nx] = 1;  // Mark as visited when added to walls
        }
    }

    printk(KERN_INFO "Maze after initial setup:\n");
    printMaze(maze);

    while (wallCount > 0) {
        int wallIndex = produceRandNum() % wallCount;
        Cell wall = walls[wallIndex];

        printk(KERN_INFO "Processing wall at (%d, %d)\n", wall.x, wall.y);

        int wallProcessed = 0;

        for (i = 0; i < 4; i++) {
            px = wall.x + directions[i][0] * 2;
            py = wall.y + directions[i][1] * 2;

            if (inBounds(px, py) && maze[py][px] == PATH && maze[wall.y][wall.x] == WALL) {
                maze[wall.y][wall.x] = PATH;

                mx = (wall.x + px) / 2;
                my = (wall.y + py) / 2;

                maze[my][mx] = PATH;  // Clear the wall in between

                visited[wall.y][wall.x] = 1;
                visited[my][mx] = 1;  // Update visited

                for (j = 0; j < 4; j++) {
                    nx = wall.x + directions[j][0] * 2;
                    ny = wall.y + directions[j][1] * 2;
                    if (inBounds(nx, ny) && maze[ny][nx] == WALL && !visited[ny][nx]) {
                        walls[wallCount++] = (Cell){nx, ny};
                        visited[ny][nx] = 1; 
                    }
                }

                wallProcessed = 1;
                break;
            }
        }

        if (wallProcessed) {
            for (i = wallIndex; i < wallCount - 1; i++) {
                walls[i] = walls[i + 1];
            }
            wallCount--;
        } else {
            printk(KERN_INFO "Wall at (%d, %d) could not be processed\n", wall.x, wall.y);
            for (i = wallIndex; i < wallCount - 1; i++) {
                walls[i] = walls[i + 1];
            }
            wallCount--;
        }

        printk(KERN_INFO "Maze state after processing a wall:\n");
        printMaze(maze);
    }
	int tempX;
	while(1){
		tempX = produceBoarderNumber();
		if(maze[tempX][1] == PATH){
			maze[tempX][0] = PATH;
			break;
		}
		else{
			break;
		}
	}
	
	while(1){
		tempX = produceBoarderNumber();
		if(maze[tempX][HEIGHT - 2] == PATH){
			maze[tempX][HEIGHT - 1] = PATH;
			break;
		}
		else{
			break;
		}
		
	}
	
}

/**
	Name: Gabriel Lopez
	Date: 9/16/24
	Description: Init function based off of the hello.c
 */
int initProgramFunc(void) {
    // creates the /proc/mazeGame entry
    proc_create(PROC_NAME, 0, NULL, &proc_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

/**
	Name: Gabriel Lopez
	Date: 9/16/24
	Description: Exit function based off of the hello.c
 */
void exitFunction(void) {
    // removes the /proc/mazeGame entry
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
	Name: Gabriel Lopez
	Date: 9/16-18/24
	Description: Function to print the maze to the to the terminal
 */
ssize_t produceMaze(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    char buffer[BUFFER_SIZE];
    static int completed = 0;

    int maze[HEIGHT][WIDTH];
    initMaze(maze);
    createMaze(maze);

    if (completed) {
        completed = 0;
        return 0;
    }

    completed = 1;

    int offset = 0;
    int y, x;

    offset += sprintf(buffer + offset, "Maze Game\n");

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (maze[y][x] == WALL) {
                offset += sprintf(buffer + offset, "#");
            } else {
                offset += sprintf(buffer + offset, " ");
            }
        }
        offset += sprintf(buffer + offset, "\n");
    }

    if (offset > count) {
        offset = count;
    }

    // copies the contents of buffer to userspace usr_buf
    if (copy_to_user(usr_buf, buffer, offset)) {
        return -EFAULT;
    }

    return offset;
}

/* Macros for registering module entry and exit points. */
module_init(initProgramFunc);
module_exit(exitFunction);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Maze Game Module");
MODULE_AUTHOR("Gabriel Lopez");
