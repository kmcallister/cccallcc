#ifndef __CCCALLCC_HPP__
#define __CCCALLCC_HPP__

#include <functional>
#include <memory>
#include <unistd.h>

template <typename T>
class cont {
public:
    static T call_cc(std::function<T (cont<T>)> f);

    void operator()(const T &x) {
        m_impl_ptr->invoke(x);
    }

private:
    class impl {
    public:
        impl(int pipe)
            : m_pipe(pipe)
            { }

        ~impl() {
            close(m_pipe);
        }

        void invoke(const T &x) {
            write(m_pipe, &x, sizeof(T));
            exit(0);
        }

    private:
        int m_pipe;
    };

    cont(int pipe)
        : m_impl_ptr(new impl(pipe))
        { }

    std::shared_ptr<impl> m_impl_ptr;
};

template <typename T>
T cont<T>::call_cc(std::function<T (cont<T>)> f) {
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

template <typename T>
static inline T call_cc(std::function<T (cont<T>)> f) {
    return cont<T>::call_cc(f);
}

#endif // !defined(__CCALLCC_HPP__)
