#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

/*int alarmed = 0;
void sigh(int signum) {
    alarmed = 1;
}
int main(void){
    signal(SIGALRM, &sigh);
    alarm(1);
    while(!alarmed) {
        printf("A");
    }
}*/

int show_status (double x);

int main()
{
    int taskcompleted = 0;
    double numtasks = 76;

    for(; taskcompleted <= numtasks; taskcompleted++)
    {
        show_status((taskcompleted / numtasks) * 100);
    }

    return (EXIT_SUCCESS);

}

int show_status (double percent)
{
    int x;
    for(x = 0; x < percent; x++)
    {
        printf("|");

    }
    printf("%.2f%%\r", percent);
    fflush(stdout);
    usleep(200000);


    return(EXIT_SUCCESS);
}