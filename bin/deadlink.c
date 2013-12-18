#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <bstrlib/bstrlib.h>


int crawl_dir(bstring main_dir_name);
int file_exists(char* path_name);


int main(int argc, char* argv[]){
    printf("file_exists(\"./tests/directory/file\") = %d\n",
            file_exists("./tests/directory/file"));
    printf("file_exists(\"./tests/link\") = %d\n\n",
            file_exists("./tests/link"));

    bstring b = bfromcstr("./tests/");
    crawl_dir(b);
    bdestroy(b);
    return 0;
}


int crawl_dir(bstring main_dir_name) {
    // declare char* to avoid the problem of signedness
    char* to_open = (char*) main_dir_name->data;
    DIR* dir = opendir(to_open);

    // points to file within a directory
    struct dirent* file = NULL;

    if (dir == NULL) {
        fprintf(stderr, "Could not open directory\n\t%s\n", main_dir_name->data);
    } else {
        file = readdir(dir);

        while (file != NULL) {
            if (file->d_type == DT_DIR) {
                printf("Directory: %s\n", file->d_name);
            } else if (file->d_type == DT_LNK) {
                // access will return -1 if link points to nothin
                int link_is_dead = file_exists(file->d_name) == 0;

                if (link_is_dead) printf("Dead Link: %s\n", file->d_name);
            }

            file = readdir(dir);
        }

        closedir(dir);
    }

    return 0;
}

int file_exists(char* path_name) {
    struct stat s;
    return stat(path_name, &s) == 0;
}
