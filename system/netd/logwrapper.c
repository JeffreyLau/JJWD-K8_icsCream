/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "private/android_filesystem_config.h"
#include "cutils/log.h"

int parent(const char *tag, int parent_read) {
    int status;
    char buffer[4096];

    int a = 0;  // start index of unprocessed data
    int b = 0;  // end index of unprocessed data
    int sz;
    while ((sz = read(parent_read, &buffer[b], sizeof(buffer) - 1 - b)) > 0) {

        sz += b;
        // Log one line at a time
        for (b = 0; b < sz; b++) {
            if (buffer[b] == '\r') {
                buffer[b] = '\0';
            } else if (buffer[b] == '\n') {
                buffer[b] = '\0';

                LOG(LOG_INFO, tag, "%s", &buffer[a]);
                a = b + 1;
            }
        }

        if (a == 0 && b == sizeof(buffer) - 1) {
            // buffer is full, flush
            buffer[b] = '\0';
            LOG(LOG_INFO, tag, "%s", &buffer[a]);
            b = 0;
        } else if (a != b) {
            // Keep left-overs
            b -= a;
            memmove(buffer, &buffer[a], b);
            a = 0;
        } else {
            a = 0;
            b = 0;
        }

    }
    // Flush remaining data
    if (a != b) {
        buffer[b] = '\0';
        LOG(LOG_INFO, tag, "%s", &buffer[a]);
    }
    status = 0xAAAA;
    if (wait(&status) != -1) {  // Wait for child
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                LOG(LOG_INFO, "logwrapper", "%s terminated by exit(%d)", tag,
                        WEXITSTATUS(status));
            }
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status))
            LOG(LOG_INFO, "logwrapper", "%s terminated by signal %d", tag,
                    WTERMSIG(status));
        else if (WIFSTOPPED(status))
            LOG(LOG_INFO, "logwrapper", "%s stopped by signal %d", tag,
                    WSTOPSIG(status));
    } else
        LOG(LOG_INFO, "logwrapper", "%s wait() failed: %s (%d)", tag,
                strerror(errno), errno);
    return -EAGAIN;
}

void child(int argc, const char**argv) {
    // create null terminated argv_child array
    char* argv_child[argc + 1];
    memcpy(argv_child, argv, argc * sizeof(char *));
    argv_child[argc] = NULL;

    // XXX: PROTECT FROM VIKING KILLER
    if (execv(argv_child[0], argv_child)) {
        LOG(LOG_ERROR, "logwrapper",
            "executing %s failed: %s", argv_child[0], strerror(errno));
    }
    _exit(1);
}

int logwrap(int argc, const char* argv[], int background)
{
    pid_t pid;

    int parent_ptty;
    int child_ptty;
    char child_devname[64];  // same size as libc/unistd/ptsname_r.c

    /* Use ptty instead of socketpair so that STDOUT is not buffered */
    parent_ptty = open("/dev/ptmx", O_RDWR);
    if (parent_ptty < 0) {
	LOG(LOG_ERROR, "logwrapper", "Cannot create parent ptty");
	return -errno;
    }

    if (grantpt(parent_ptty) || unlockpt(parent_ptty) ||
            ptsname_r(parent_ptty, child_devname, sizeof(child_devname))) {
        close(parent_ptty);
	LOG(LOG_ERROR, "logwrapper", "Problem with /dev/ptmx");
	return -1;
    }

    pid = fork();
    if (pid < 0) {
        close(parent_ptty);
	LOG(LOG_ERROR, "logwrapper", "Failed to fork");
        return -errno;
    } else if (pid == 0) {
        /*
         * Child
         */
        child_ptty = open(child_devname, O_RDWR);
        if (child_ptty < 0) {
            close(parent_ptty);
	    LOG(LOG_ERROR, "logwrapper", "Problem with child ptty");
            _exit(errno < 128 ? errno : 1);  // XXX lame
        }

        // redirect stdout and stderr
        close(parent_ptty);
        dup2(child_ptty, 1);
        dup2(child_ptty, 2);
        close(child_ptty);

        if (background) {
            int fd = open("/dev/cpuctl/bg_non_interactive/tasks", O_WRONLY);
            if (fd >= 0) {
                char text[64];
                sprintf(text, "%d", getpid());
                if (write(fd, text, strlen(text)) < 0) {
                    LOG(LOG_WARN, "logwrapper",
                        "Unable to background process (%s)", strerror(errno));
                }
                close(fd);
            } else {
                LOG(LOG_WARN, "logwrapper",
                    "Unable to background process (%s)", strerror(errno));
            }
        }

        child(argc, argv);
    } else {
        /*
         * Parent
         */
        int rc = parent(argv[0], parent_ptty);
        close(parent_ptty);
        return rc;
    }

    return 0;
}

/*
 * The following is based off of bionic/libc/unistd/system.c with
 *  modifications to avoid calling /system/bin/sh -c
 */
extern char **environ;
int system_nosh(const char *command)
{
    pid_t pid;
    sig_t intsave, quitsave;
    sigset_t mask, omask;
    int pstat;
    char buffer[255];
    char *argp[32];
    char *next = buffer;
    char *tmp;
    int i = 0;

    if (!command)           /* just checking... */
        return(1);

    /*
     * The command to argp splitting is from code that was
     * reverted in Change: 11b4e9b2
     */
    if (strnlen(command, sizeof(buffer) - 1) == sizeof(buffer) - 1) {
        LOGE("command line too long while processing: %s", command);
        errno = E2BIG;
        return -1;
    }
    strcpy(buffer, command); // Command len is already checked.
    while ((tmp = strsep(&next, " "))) {
        argp[i++] = tmp;
        if (i == 32) {
            LOGE("argument overflow while processing: %s", command);
            errno = E2BIG;
            return -1;
        }
    }
    argp[i] = NULL;


    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, &omask);
    switch (pid = vfork()) {
    case -1:                        /* error */
        sigprocmask(SIG_SETMASK, &omask, NULL);
        return(-1);
    case 0:                                 /* child */
        sigprocmask(SIG_SETMASK, &omask, NULL);
        execve(argp[0], argp, environ);
        _exit(127);
    }

    intsave = (sig_t)  bsd_signal(SIGINT, SIG_IGN);
    quitsave = (sig_t) bsd_signal(SIGQUIT, SIG_IGN);
    pid = waitpid(pid, (int *)&pstat, 0);
    sigprocmask(SIG_SETMASK, &omask, NULL);
    (void)bsd_signal(SIGINT, intsave);
    (void)bsd_signal(SIGQUIT, quitsave);
    return (pid == -1 ? -1 : pstat);
}
