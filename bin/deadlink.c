#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char* argv[]){
    DIR* dp;
    struct dirent *ep;
    dp = opendir("./tests/");

    if (dp != NULL) {
        while (ep = readdir(dp)) {
            printf("%s\n", ep->d_name);
        }

        closedir(dp);
    } else {
        perror("Couldn't open the directory.");
    }

    return 0;
}
