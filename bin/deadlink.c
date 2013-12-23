#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <bstrlib/bstrlib.h>


int crawl_dir(bstring dir_name);


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

            // if file is a directory and program is being run recursively
            if (file->d_type == DT_DIR && OPT_REC) {
                int MOVE = 1;  // assume user will confirm/no confirmation

                // if confirmation required
                if (OPT_CONF) {
                    // ask for confirmation until the user gets it right
                    int success = 0;
                    while (!success) {
                        printf("Move into %s [y/n]? ", path->data);
                        int ch = getc(stdin);

                        if (ch == 'y') {
                            success = 1;
                        } else if (ch == 'n') {
                            MOVE = 0;
                            success = 1;
                        } else {
                            printf("Please type 'y' or 'n': ");
                        }
                    } 
                }

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
                int link_is_dead = access(path->data, F_OK) == -1;
                if (link_is_dead) {
                    int DEL = 1;  // assume file needs deleting

                    // if confirmation required
                    if (OPT_CONF) {
                        // ask for confirmation until user gets it right
                        int success = 0;
                        while (!success) {
                            printf("Delete link %s [y/n]? ", path->data);
                            int ch = getc(stdin);

                            if (ch == 'y') {
                                success = 1;
                            } else if (ch == 'n') {
                                DEL = 0;
                                success = 1;
                            } else {
                                printf("Please type 'y' or 'n': ");
                            }
                        }
                    }

                    if (DEL) {
                        if (OPT_VERB) printf("Removing link: %s\n", path->data);
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
