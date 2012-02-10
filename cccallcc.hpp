#ifndef __CCCALLCC_HPP__
#define __CCCALLCC_HPP__

#include <functional>
#include <unistd.h>

template <typename T>
class cont {
public:
    static T call_cc(std::function<T (cont<T>)> f) {
        int fd[2];
        pipe(fd);
        int read_fd  = fd[0];
        int write_fd = fd[1];

        pid_t pid = fork();
        if (pid) {
            // parent
            close(read_fd);
            return f( cont<T>(write_fd) );
        } else {
            // child
            close(write_fd);
            char buf[sizeof(T)];
            if (read(read_fd, buf, sizeof(T)) < sizeof(T))
                exit(0);
            close(read_fd);
            return *reinterpret_cast<T*>(buf);
        }
    }

    void operator()(const T &x) {
        write(m_pipe, &x, sizeof(T));
        exit(0);
    }

private:
    cont<T>(int pipe)
        : m_pipe(pipe) { }

    int m_pipe;
};

#endif // !defined(__CCALLCC_HPP__)
