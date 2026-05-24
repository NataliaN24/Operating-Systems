
//ps -eo pid -U uid | xargs kill

const char filename[] = "foo.log";

int main(int argc, char* argv[])
{
    if(argc <= 1) { err(1, "Not enough argumenets"); }

    int log = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if(log < 0) { err(2, "Open"); }

    uid_t uid = getuid();
    char uidStr[8];
    snprintf(uidStr, sizeof(uidStr), "%u", uid);

    struct timeval tv;
    if(gettimeofday(&tv, NULL) < 0 ) { err(3, "gettimeofday"); }

    const struct tm* time = localtime(&tv.tv_sec);
    if(time == NULL) { err(4, "Localtime"); }

    char date[32];
    strftime(date, sizeof(date), "%F %T", time);
    char fullDate[64];
    snprintf(fullDate, sizeof(fullDate), "%s.%06ld", date, tv.tv_usec);

    struct passwd* usr_info = getpwuid(uid);
    if(usr_info == NULL) { err(5, "Getpwuid"); }
    char in = ' ';

    if(write(log, fullDate, strlen(fullDate)) < 0 || write(log, &in, sizeof(in)) < 0 ||
       write(log, usr_info->pw_name, strlen(usr_info->pw_name)) < 0 || write(log, &in, sizeof(in)) < 0) { err(6, "Write"); }

    for(int i = 1; i < argc; i++)
    {
        if(write(log, argv[i], strlen(argv[i])) < 0 || write(log, &in, sizeof(in)) < 0) { err(7, "Write"); }
    }

    char el = '\n';
    if(write(log, &el, sizeof(el)) < 0) { err(8, "Write"); }

    int pwd = fork();
    if(pwd < 0) { err(9, "Fork"); }
    if(pwd == 0)
    {
        execlp("passwd", "passwd", "-l", usr_info->pw_name, (char*)NULL);
        err(10, "Execlp");
    }

    wait(NULL);

    int pids[2];
    if(pipe(pids) < 0) { err(11, "Pipe"); }
    int ps = fork();
    if(ps < 0) { err(12, "Ps"); }
    if(ps == 0)
    {
        if(close(pids[0]) < 0) { err(13, "Close"); }

        if(dup2(pids[1], 1) < 0) { err(14, "Dup2"); }
        if(close(pids[1]) < 0) { err(15, "Dup2"); }

        execlp("ps", "ps", "-eo", "pid", "--no-headers", "-U", uidStr, (char*)NULL);
        err(13, "Execlp");
    }

    if(close(pids[1]) < 0) { err(16, "Close"); }

    int kill = fork();
    if(kill < 0) { err(17, "Fork"); }
    if(kill == 0)
    {
        if(dup2(pids[0], 0) < 0) { err(18, "Dup2"); }
        if(close(pids[0]) < 0) { err(19, "Close"); }

        execlp("xargs", "xargs", "kill", (char*)NULL);
        err(20, "Execlp xargs and kill");
    }

    if(close(pids[0]) < 0) { err(21, "Close"); }

    wait(NULL);
    wait(NULL);

    return 0;
}
