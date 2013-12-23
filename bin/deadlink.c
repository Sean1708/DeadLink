#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <bstrlib/bstrlib.h>


int crawl_dir(bstring dir_name);
int file_exists(char* path_name);


/* FILE->D_NAME DOES NOT RETURN A PATHNAME WHICH IS CAUSING THE ERROR */
int main(int argc, char* argv[]) {
    bstring b = bfromcstr("./tests/");
    crawl_dir(b);
    bdestroy(b);
    return 0;
}


int crawl_dir(bstring dir_name) {
    // contains files in a directory
    DIR* dir = opendir(dir_name->data);

    // points to file within a directory
    struct dirent* file = NULL;

    if (dir == NULL) {
        fprintf(stderr, "Could not open directory\n\t%s\n", dir_name->data);
    } else {
        file = readdir(dir);

        while (file != NULL) {
            if (file->d_type == DT_DIR) {
                printf("Directory: %s\n", file->d_name);
            } else if (file->d_type == DT_LNK) {
                // copy directory name
                bstring path = bstrcpy(dir_name);
                // store path name rather than full file name
                bcatcstr(path, file->d_name);

                // access will return -1 if link points to nothin
                int link_is_dead = access(path->data, F_OK) == -1;
                if (link_is_dead) printf("Dead Link: %s\n", file->d_name);
            }

            file = readdir(dir);
        }

        closedir(dir);
    }

    return 0;
}
