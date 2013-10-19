//#pragma once
#ifndef IO_LOGGER_H_
#define IO_LOGGER_H_

#include <iostream>
#include "tiebuffer.h"
#include "Time.h"


// Singleton I/O logger class
typedef class IOLogger sealed
{

private:
    std::ofstream _fstm;
    std::tie_buf  _inbuf;
    std::tie_buf  _outbuf;
    std::string   _fn_log;

protected:

    // Constructor should be protected !!!
    IOLogger (std::string fn_log)
        : _inbuf (std::cin.rdbuf (), &_fstm)
        , _outbuf (std::cout.rdbuf (), &_fstm)
        , _fn_log (fn_log)
    {}

    // Don't forget to declare these functions.
    // Want to make sure they are unaccessable & non-copyable
    // otherwise may accidently get copies of singleton.
    // Don't Implement these functions.
    IOLogger ();

    IOLogger (IOLogger const &);
    template<class T>
    T& operator= (IOLogger const &);

public:

    ~IOLogger ()
    {
        stop ();
    }

    static IOLogger& instance ()
    {
        // Guaranteed to be destroyed.
        // Instantiated on first use.
        static IOLogger _instance ("log_io.txt");
        return _instance;
    }

    void start ()
    {
        if (!_fstm.is_open ())
        {
            _fstm.open (_fn_log, std::ios_base::out | std::ios_base::app);
            _fstm << "[" << Time::to_string (Time::now ()) << "] ->" << std::endl;

            std::cin.rdbuf (&_inbuf);
            std::cout.rdbuf (&_outbuf);
        }
    }

    void stop ()
    {
        if (_fstm.is_open ())
        {
            std::cout.rdbuf (_outbuf.sbuf ());
            std::cin.rdbuf (_inbuf.sbuf ());

            _fstm << "[" << Time::to_string (Time::now ()) << "] <-" << std::endl;
            _fstm.close ();
        }
    }

} IOLogger;

inline void log_io (bool on)
{
    on ?
        IOLogger::instance ().start () :
        IOLogger::instance ().stop () ;
}

#endif
