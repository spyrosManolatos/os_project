#include <stdio.h>
#include <unistd.h>
int main(){
    for(int i = 1; i <= 5000; i++){
        usleep(100);
    }
    return 0;
}
