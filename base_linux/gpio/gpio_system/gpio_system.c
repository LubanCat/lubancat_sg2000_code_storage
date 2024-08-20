#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    pid_t result;
    int i;
    for(i=0;i<10;i++){
        /*调用 system()函数*/
        result = system("gpioset 0 6=1");
        usleep(500000);
        result = system("gpioset 0 6=0");
        usleep(500000);
    }
    return result;
}
