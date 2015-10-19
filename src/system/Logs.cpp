#include "Logs.h"

#include "Time.h"

#include <streambuf>
#include <ostream>

template <class cT, class traits = std::char_traits<cT> >
class basic_nullbuf: public std::basic_streambuf<cT, traits> {
    typename traits::int_type overflow(typename traits::int_type c)
    {
        return traits::not_eof(c); // indicate success
    }
};

template <class cT, class traits = std::char_traits<cT> >
class basic_onullstream: public std::basic_ostream<cT, traits> {
    public:
        basic_onullstream():
        std::basic_ios<cT, traits>(&m_sbuf),
        std::basic_ostream<cT, traits>(&m_sbuf)
        {
            this->init(&m_sbuf);
        }

    private:
        basic_nullbuf<cT, traits> m_sbuf;
};

typedef basic_onullstream<char> onullstream;
static onullstream NULL_STREAM;
static bool enabled = true;

std::ostream &Logs::debug()
{
#ifdef NDEBUG
    return NULL_STREAM;
#else
    if (enabled)
    {
        std::cout << "DEBUG - ";
        return std::cout;
    }
    else
    {
        return NULL_STREAM;
    }
#endif
}

std::ostream &Logs::warning()
{
    if (enabled)
    {
        std::cerr << "WARN - ";
        return std::cerr;
    }
    else
    {
        return NULL_STREAM;
    }
}

std::ostream &Logs::error()
{
    if (enabled)
    {
        std::cerr << "ERROR - ";
        return std::cerr;
    }
    else
    {
        return NULL_STREAM;
    }
}

void Logs::debugTimestamp(const std::string &message)
{
    debug() << Time::getRelativeTimeMs() << " - " << message << std::endl;
}

void Logs::disable()
{
    enabled = false;
}
