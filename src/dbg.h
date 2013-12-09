#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>


/* "M" IS A STRING TO BE PRINTED TO THE USER EXPLAINING THE PROBLEM
 * "A" IS A CONDITION, VARIABLE OR FUNCTION WHICH IS MEANT TO RETURN TRUE 
 * ALL MESSAGES PRINTED TO THE STANDARD ERROR STREAM
 *
 * http://c.learncodethehardway.org/book/ex20.html
 *
 */
 

#ifdef NDEBUG
//if -DNDEBUG is set debug macro expands to nothing
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%s:%d: " M "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#endif

//finds nice error string associated with error number
#define clean_errno() (errno == 0 ? "None" : strerror(errno))

//logs messages for the end user
#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%s:%d: errno: %s) " M "\n", __FILE__, __func__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%s:%d: errno: %s) " M "\n", __FILE__, __func__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%s:%d) " M "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)

//checks condition then logs error and jumps to error section if not true
#define check(A, M, ...) if (!(A)) {log_err(M, ##__VA_ARGS__); errno=0; goto error;}
//check but for a pointer specifically
#define check_mem(A) check((A), "Out of memory.")
//check but doesn't log error if -DNDEBUG is set
#define check_debug(A, M, ...) if (!(A)) {debug(M, ##__VA_ARGS__); errno=0; goto error;}

//place in part of function which shouldn't run, like default in switch statements
#define sentinel(M, ...) {log_err(M, ##__VA_ARGS__); errno=0; goto error;}


#endif