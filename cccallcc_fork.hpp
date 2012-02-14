#ifndef __CCCALLCC_FORK_HPP__
#define __CCCALLCC_FORK_HPP__

#include <functional>
#include <memory>
#include <unistd.h>

#ifdef __GNUC__
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

template <typename T>
class cont {
public:
    typedef std::function<T (cont<T>)> call_cc_arg;

    static T call_cc(call_cc_arg f);

    void operator()(const T &x) NORETURN {
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

        void invoke(const T &x) NORETURN {
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
T cont<T>::call_cc(call_cc_arg f) {
    int fd[2];
    pipe(fd);
    int read_fd  = fd[0];
    int write_fd = fd[1];

    if (fork()) {
        // parent
        close(read_fd);
        return f( cont<T>(write_fd) );
    } else {
        // child
        close(write_fd);
        char buf[sizeof(T)];
        if (read(read_fd, buf, sizeof(T)) < ssize_t(sizeof(T)))
            exit(0);
        close(read_fd);
        return *reinterpret_cast<T*>(buf);
    }
}

template <typename T>
inline T call_cc(typename cont<T>::call_cc_arg f) {
    return cont<T>::call_cc(f);
}

#endif // !defined(__CCCALLCC_FORK_HPP__)
