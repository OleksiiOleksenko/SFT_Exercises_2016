int fd[2];  // pipe fd

int backup() {
    int restarts = 0, ret = 0;

    for (;; restarts++) {

        if (pipe(fd) < 0) {
            fd[0] = -1;
            fd[1] = -1;
            printf("Error creating pipe\n");
            ret = -1;
        } else {
            /* integer overflow -> trigger graceful degradation */
            if (restarts < 0)
                ret = -1;
            else ret = fork();
        }

        if (ret == 0) {// child returns
            close(fd[1]);
            printf("Restart %d\n", restarts);
            printf("New echo server with PID: %d\n", (int) getpid());
            return restarts;
        }

        if ( ret < 0 ) {
            printf("Graceful degradation .. running without backup\n");
            return ret;
        }

        close(fd[0]);
        printf("Waiting %d for %d\n", (int) getpid(), ret);
        while (ret != waitpid(ret,0,0));
    }
    return 0;
}

/*
 * This function returns true (as int: 1) if the pipe between both
 * processes has been closed.
 */
int isPipeClosed() {
    struct pollfd pfds;
    int time = 0;

    pfds.fd = fd[0];
    pfds.events = POLLHUP | POLLERR | POLLIN; // POLLIN might be enough..
    pfds.revents = 0;

    if(poll(&pfds, 1, time) < 0) {
        printf("Poll failed\n");
    }
    if (pfds.revents) {
        printf("Pipe failed (%d)\n", pfds.revents);
        close (fd[0]);
        return 1;
    }
    return 0;
}


/* add the following to the end of the main loop */

/* if parent process disappeared make new backup */
// if (ft < 0 || isPipeClosed()) { ft = backup(); }
