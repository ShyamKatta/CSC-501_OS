#define RANDOMSCHED 1
#define LINUXSCHED 2
#define XINUSCHED 0

int process_to_schedule();

int getProcess();

void setschedclass(int);

int getschedclass();
