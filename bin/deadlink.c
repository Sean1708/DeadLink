#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <bstrlib/bstrlib.h>


int crawl_dir(bstring dir_name);


int rflag = 0;
int vflag = 0;


int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: deadlink [-rv] directory\n");
        return -1;
    }

    int opt_ch;
    do {
        opt_ch = getopt(argc, argv, "rv");

        if (opt_ch == -1) break;

        switch (opt_ch) {
            case 'r':
                // follow directories
                rflag = 1;
                break;
            case 'v':
                // print names of files and directories before
                // deleting or moving into them
                vflag = 1;
                break;
        }
    } while (opt_ch != -1);

    bstring b = bfromcstr(argv[argc - 1]);
    crawl_dir(b);
    bdestroy(b);
    return 0;
}


int crawl_dir(bstring dir_name) {
    // ensure dir_name ends in '/'
    if (dir_name->data[dir_name->slen - 1] != '/') bconchar(dir_name, '/');

    // contains files in a directory
    DIR* dir = opendir(dir_name->data);

    // points to file within a directory
    struct dirent* file = NULL;

    if (dir == NULL) {
        fprintf(stderr, "Could not open directory\n\t%s\n", dir_name->data);
    } else {
        do {
            file = readdir(dir);

            if (file == NULL) break;

            // ignore current and parent directories
            if (strncmp(file->d_name, ".", 1) == 0 ||
                    strncmp(file->d_name, "..", 2) == 0) {
                file = readdir(dir);
                continue;
            }

            // store path name rather than full file name
            bstring path = bstrcpy(dir_name);
            bcatcstr(path, file->d_name);

            if (file->d_type == DT_DIR && rflag) {
                if (vflag) printf("Moving into directory: %s\n", path->data);
                crawl_dir(path);
            } else if (file->d_type == DT_LNK) {
                // access will return -1 if link points to nothin
                int link_is_dead = access(path->data, F_OK) == -1;
                if (link_is_dead) {
                    if (vflag) printf("Removing link: %s\n", path->data);
                }
            }
        } while (file != NULL);

        closedir(dir);
    }

    return 0;
}
