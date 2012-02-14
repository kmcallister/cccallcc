#ifndef __CCCALLCC_FORK_HPP__
#define __CCCALLCC_FORK_HPP__

///////////////////////////////////////////
// Implementation of call_cc using fork(2).

#include <functional>
#include <memory>
#include <unistd.h>

#ifdef __GNUC__
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

// A value of type cont<T> represents a continuation accepting
// an argument of type T.
template <typename T>
class cont {
public:
    // The type of the argument to call_cc.
    typedef std::function<T (cont<T>)> call_cc_arg;

    // Static member function, serving as the "constructor"
    // for values of type cont<T>.  A free function template
    // call_cc<T> is also provided below.
    static T call_cc(call_cc_arg f);

    // Invoke the continuation.
    void operator()(const T &x) NORETURN {
        m_impl_ptr->invoke(x);
    }

    // cont<T> also provides the default copy constructor and
    // assignment operator.

private:
    // Private implementation class.
    class impl {
    public:
        impl(int pipe)
            : m_pipe(pipe)
            { }

        // Close the pipe, so the child process will exit.
        ~impl() {
            close(m_pipe);
        }

        void invoke(const T &x) NORETURN {
            write(m_pipe, &x, sizeof(T));
            exit(0);
        }

    private:
        // File descriptor of the write end of the pipe created
        // by call_cc.
        int m_pipe;
    };

    cont(int pipe)
        : m_impl_ptr(new impl(pipe))
        { }

    // A cont<T> holds a reference-counted smart pointer to a
    // cont<T>::impl.  This means cont<T> is copyable, and
    // cont<T>::impl::~impl will be invoked when the last copy of
    // some cont<T> is deleted.
    std::shared_ptr<impl> m_impl_ptr;
};

template <typename T>
T cont<T>::call_cc(call_cc_arg f) {
    int fd[2];
    pipe(fd);
    int read_fd  = fd[0];
    int write_fd = fd[1];

    if (fork()) {
        // parent: call f immediately.
        close(read_fd);
        return f( cont<T>(write_fd) );
    } else {
        // child: wait for the continuation to be invoked.
        close(write_fd);

        // read(2) will return 0 if the write end of the pipe is closed.
        // This will happen when the parent exits, or the last copy of
        // the corresponding cont<T> is deleted.
        //
        // We naively assume the value can be read in a single call to
        // read(2).
        char buf[sizeof(T)];
        if (read(read_fd, buf, sizeof(T)) < ssize_t(sizeof(T)))
            exit(0);
        close(read_fd);

        // Return (by value) what we got through the pipe.
        return *reinterpret_cast<T*>(buf);
    }
}

// Free function call_cc<T>, forwarding to the static member function
// cont<T>::call_cc.
template <typename T>
inline T call_cc(typename cont<T>::call_cc_arg f) {
    return cont<T>::call_cc(f);
}

#endif // !defined(__CCCALLCC_FORK_HPP__)
