// Copyright (c) 2005 - 2010
// Seweryn Habdank-Wojewodzki
//
// Distributed under the Boost Software License, Version 1.0.
// ( copy at http://www.boost.org/LICENSE_1_0.txt )

#include "trilogger.h"

#if !defined(CLEANTLOG)

#   if defined(FTLOG)

#       include <fstream>

#   else

#       include <iostream>
#       include "nullstream.h"

#   endif

namespace trivial_logger {

    namespace implementation {

        typedef class tri_logger_impl sealed
        {

        public:
            // true if logger is active
            static bool _is_active;

            // auto pointer helps manage resources;
            static ::std::unique_ptr<::std::ostream> _outstream_helper_ptr;

            // pointer to the output stream of the logger
            static ::std::ostream *_outstream;

        } tri_logger_impl;

        // activate logger by default
        bool tri_logger_impl::_is_active = true;

        void init_tri_logger_impl ();
    }


    ::std::unique_ptr<implementation::tri_logger_impl> 
        tri_logger::_tl_impl (::std::unique_ptr<implementation::tri_logger_impl> (new implementation::tri_logger_impl ()));


    tri_logger::tri_logger ()
    {
        if (NULL == _tl_impl.get ())
        {
            tri_logger::_tl_impl.reset (new implementation::tri_logger_impl ());
        }
        implementation::init_tri_logger_impl ();
    }

    tri_logger::~tri_logger ()
    {}

    bool tri_logger::is_active ()
    {
        return _tl_impl->_is_active;
    }

    void tri_logger::activate (bool active)
    {
        _tl_impl->_is_active = active;
    }

    ::std::ostream*& tri_logger::ostream_ptr ()
    {
        return _tl_impl->_outstream;
    }

#   if defined(OTLOG)

    // set auto pointer to the null stream
    // reason: ::std::cout can not be created in runtime, so
    // the auto pointer has nothing to do with its resources
    ::std::unique_ptr<::std::ostream> implementation::tri_logger_impl::_outstream_helper_ptr =
        ::std::unique_ptr<::std::ostream> (new ::std::null_stream ());
    ::std::ostream *implementation::tri_logger_impl::_outstream = &std::cout;

    void implementation::init_tri_logger_impl ()
    { 
        if (NULL == implementation::tri_logger_impl::_outstream_helper_ptr.get ())
        {
            implementation::tri_logger_impl::_outstream_helper_ptr.reset (new ::std::null_stream ());
        }
        implementation::tri_logger_impl::_outstream = &std::cout;
    }

#   elif defined (ETLOG)

    // set auto pointer to the null stream
    // reason: ::std::cerr can not be created in runtime, so
    // the auto pointer has nothing to do with its resources
    ::std::unique_ptr<::std::ostream> implementation::tri_logger_impl::_outstream_helper_ptr =
        ::std::unique_ptr<::std::ostream> (new ::std::null_stream ());
    ::std::ostream *implementation::tri_logger_impl::_outstream = &std::cerr;

    void implementation::init_tri_logger_impl ()
    { 
        if (NULL == implementation::tri_logger_impl::_outstream_helper_ptr.get ())
        {
            implementation::tri_logger_impl::_outstream_helper_ptr.reset (new ::std::null_stream ());
        }
        implementation::tri_logger_impl::_outstream = &std::cerr;
    }


#   elif defined (FTLOG)

#       include <cctype>

#       define XSTR(s) STR(s)
#       define STR(s) #s

#       define MIN(x1, x2) ((x1) < (x2) ? (x1) : (x2))
#       define MAX(x1, x2) ((x1) > (x2) ? (x1) : (x2))

    typedef char char_type;

    namespace implementation {

        /// Function calculates length of C string
        /// It can be used with wide characters
        template < typename Char_type >
        size_t const str_len (const Char_type s[])
        {
            size_t length = 0;
            while (*s)
            {
                ++s;
                ++length;
            }
            return length;
        }

        /// Function paste rhs C string to the lhs C string.
        /// lhs should be long enough for that operation.
        /// Additionally coping is stared from the point which
        /// points lhs.
        template < typename Char_type >
        size_t const str_cat (Char_type *&lhs, const Char_type *rhs)
        {
            size_t length = 0;
            while (*rhs)
            {
                *lhs = *rhs;
                ++rhs;
                ++lhs;
                ++length;
            }
            return length;
        }

        /// Function copy rhs C string in to the lhs.
        /// It do not check size of target C string
        /// It starts to copy from the beginning of the C string,
        /// but it begins put characters at the point where lhs points,
        /// so there can be a problem when lhs points on the end of lhs
        /// C string.
        template < typename Char_type >
        size_t const str_cpy (Char_type *&lhs, const Char_type *rhs)
        {
            size_t length = 0;
            while (*rhs)
            {
                *lhs = *rhs;
                ++rhs;
                ++lhs;
                ++length;
            }
            *lhs = '\0';
            return length + 1;
        }

        /// Function converts existing file name to the file name
        /// which has no non-printable signs and 
        /// at the end is added extension.
        /// The space sign in file name is converted to the underscore.
        /// Lengths of C strings has to be proper.
        template<typename Char_type>
        const size_t
            create_filename (
            Char_type filename[],
            const Char_type fn_log[],
            const Char_type ext_log[],
            const Char_type fn_def[])
        {
            size_t length = 0; 

            if (str_len (fn_log) > 1)
            {
                while (*fn_log)
                {
                    // check if characters have grapnical
                    // reprasentation
                    if (0 != ::isgraph (unsigned char (*fn_log)))
                    {
                        *filename = *fn_log;
                        ++filename;
                        ++length;
                    }
                    else
                    {
                        // convert space to underscore
                        if (' ' == *fn_log)
                        {
                            *filename = '_';
                            ++filename;
                            ++length;
                        }
                    }
                    ++fn_log;
                }
            }
            else
            {
                //filename = &filename[0];
                length   = str_cpy (filename, fn_def);
            }

            // add extension
            str_cat (filename, ext_log);
            *filename = '\0';

            return length;
        }

        //template<typename T>
        //T const min (T const x1, T const x2) { return (x1 < x2 ? x1 : x2); }

        //template<typename T>
        //T const max (T const x1, T const x2) { return (x1 > x2 ? x1 : x2); }

        const char_type* get_fn_log()   { return XSTR(FTLOG); }
        const char_type* get_fn_def()   { return "log_eng"; }
        // extension C string
        const char_type* get_ext_log()  { return ".txt"; }

    }

    // convert definition of the FTLOG to the C string
    const char_type *fn_log  = implementation::get_fn_log();
    const char_type *fn_def  = implementation::get_fn_def();
    const char_type *ext_log = implementation::get_ext_log();

    // container for final file name
    char_type filename[(MAX (sizeof (fn_log), sizeof(fn_def)) + sizeof (ext_log)) / sizeof(char_type)];
    // create file name
    size_t const length = implementation::create_filename (filename, implementation::get_fn_log(), implementation::get_ext_log(), implementation::get_fn_def());

#       undef STR
#       undef XSTR
#       undef MIN
#       undef MAX

    // new file is opened and its destruction is managed by unique_ptr
    ::std::unique_ptr<::std::ostream> implementation::tri_logger_impl::_outstream_helper_ptr =
        ::std::unique_ptr<::std::ostream> (new ::std::ofstream (filename, ::std::ios_base::out | ::std::ios_base::app));
    // set pointer output stream
    ::std::ostream *implementation::tri_logger_impl::_outstream = _outstream_helper_ptr.get ();

    void implementation::init_tri_logger_impl ()
    { 
        if (NULL == implementation::tri_logger_impl::_outstream_helper_ptr.get ())
        {
            implementation::tri_logger_impl::_outstream_helper_ptr.reset (new ::std::ofstream (filename, ::std::ios_base::out | ::std::ios_base::app));
            // set pointer output stream
            implementation::tri_logger_impl::_outstream =
                implementation::tri_logger_impl::_outstream_helper_ptr.get ();
        }
    }

    // here is a place for user defined output stream and flag

    //

#   else

    ::std::unique_ptr<::std::ostream> implementation::tri_logger_impl::_outstream_helper_ptr =
        ::std::unique_ptr<::std::ostream> (new ::std::null_stream ());
    ::std::ostream *implementation::tri_logger_impl::_outstream =
        implementation::tri_logger_impl::_outstream_helper_ptr.get ();

    void implementation::init_tri_logger_impl ()
    {
        if (NULL == implementation::tri_logger_impl::_outstream_helper_ptr.get ())
        {
            implementation::tri_logger_impl::_outstream_helper_ptr.reset (new ::std::null_stream ());
            implementation::tri_logger_impl::_outstream =
                implementation::tri_logger_impl::_outstream_helper_ptr.get ();
        }
    }

#   endif

    ::std::unique_ptr<tri_logger> implementation::tri_logger_out_ptr (new tri_logger ());

    tri_logger& instance ()
    {
        if (NULL == implementation::tri_logger_out_ptr.get ())
        {
            implementation::tri_logger_out_ptr.reset (new tri_logger ());
        }
        return *(implementation::tri_logger_out_ptr);
    }

}

#endif // !CLEANTLOG