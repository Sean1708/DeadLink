#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <bstrlib/bstrlib.h>


int crawl_dir(bstring dir_name);
int user_conf(const char* msg, bstring path);


static inline int is_parent_current(struct dirent* file) {
    char* name = file->d_name;

    // compare one more than the length of string to avoid false
    // positives in dotted files
    int is_current = strncmp(name, ".", 2) == 0;
    int is_parent = strncmp(name, "..", 3) == 0;

    return is_current || is_parent;
}


int OPT_CONF = 0;
int OPT_REC = 0;
int OPT_VERB = 0;


int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: deadlink [-irv] directory\n");
        return -1;
    }

    int opt_ch;
    do {
        opt_ch = getopt(argc, argv, "irv");

        if (opt_ch == -1) break;

        switch (opt_ch) {
            case 'i':
                // ask for conformation
                OPT_CONF = 1;
                break;
            case 'r':
                // follow directories
                OPT_REC = 1;
                break;
            case 'v':
                // print names of files and directories before
                // deleting or moving into them
                OPT_VERB = 1;
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
    char* dir_name_cast = (char*) dir_name->data;
    DIR* dir = opendir(dir_name_cast);

    // points to file within a directory
    struct dirent* file = NULL;

    if (dir == NULL) {
        fprintf(stderr, "Could not open directory\n\t%s\n", dir_name->data);
        return -1;
    } else {
        do {
            file = readdir(dir);

            if (file == NULL) break;

            // ignore current and parent directories
            if (is_parent_current(file)) {
                file = readdir(dir);
                continue;
            }

            // store path name rather than full file name
            bstring path = bstrcpy(dir_name);
            bcatcstr(path, file->d_name);

            // if file is a directory and program is being run recursively
            if (file->d_type == DT_DIR && OPT_REC) {
                int MOVE = 1;

                // if confirmation required
                if (OPT_CONF) MOVE = user_conf("Move into", path);

                // if user decides to move into directory
                if (MOVE) {
                    if (OPT_VERB) {
                        printf("Moving into directory: %s\n", path->data);
                    }

                    crawl_dir(path);
                } else if (OPT_VERB) {
                    printf("Skipping directory: %s\n", path->data);
                }
            // if file is a link
            } else if (file->d_type == DT_LNK) {
                // access will return -1 if link points to nothin
                char* path_cast = (char*) path->data;
                int link_is_dead = access(path_cast, F_OK) == -1;
                if (link_is_dead) {
                    int DEL = 1;  // assume file needs deleting

                    // if confirmation required
                    if (OPT_CONF) DEL = user_conf("Delete link", path);

                    if (DEL) {
                        printf("Removing link: %s\n", path->data);
                        unlink(path_cast);
                    } else if (OPT_VERB) {
                        printf("Skipping link: %s\n", path->data);
                    }
                }
            }
        } while (file != NULL);

        closedir(dir);
    }

    return 0;
}

int user_conf(const char* msg, bstring path) {
    printf("%s %s [y/n]? ", msg, path->data);

    int rv = 1;  // assume user will confirm
    int success = 0;
    while (!success) {
        int ch = getc(stdin);

        if (ch == 'y') {
            rv = 1;
            success = 1;
        } else if (ch == 'n') {
            rv = 0;
            success = 1;
        } else {
            printf("Please type 'y' or 'n': ");
        }
    }


    return rv;
}
