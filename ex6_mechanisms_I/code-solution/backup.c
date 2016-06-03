int backup() {
    int restarts = 0, ret;

    for (;; restarts++) {
        ret = fork();

        // child returns -> return from backup() and do some request handling
        if (ret == 0) {
            printf("Restart %d\n", restarts);
            printf("New echo server with PID: %d\n", (int) getpid());
            return restarts;
        }

        // parent returns -> wait until child is dead
        printf("Waiting %d for %d\n", (int) getpid(), ret);
        while (ret != waitpid(ret,0,0)) /* noop */;
    }
    return 0;
}
