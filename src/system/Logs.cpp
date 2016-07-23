#include "Logs.h"

#include "Timing.h"

#include <ostream>
#include <streambuf>

template <class cT, class traits = char_traits<cT>> class basic_nullbuf : public basic_streambuf<cT, traits> {
    typename traits::int_type overflow(typename traits::int_type c) {
        return traits::not_eof(c); // indicate success
    }
};

template <class cT, class traits = char_traits<cT>> class basic_onullstream : public basic_ostream<cT, traits> {
  public:
    basic_onullstream() : basic_ios<cT, traits>(&m_sbuf), basic_ostream<cT, traits>(&m_sbuf) {
        this->init(&m_sbuf);
    }

  private:
    basic_nullbuf<cT, traits> m_sbuf;
};

typedef basic_onullstream<char> onullstream;
static onullstream NULL_STREAM;
static bool enabled = true;

#ifdef NDEBUG
ostream &Logs::debug(const string &) {
    return NULL_STREAM;
}
#else
ostream &Logs::debug(const string &logger) {
    if (enabled) {
        cout << "DEBUG [" << logger << "] ";
        return cout;
    } else {
        return NULL_STREAM;
    }
}
#endif

ostream &Logs::warning(const string &logger) {
    if (enabled) {
        cerr << "WARN [" << logger << "] ";
        return cerr;
    } else {
        return NULL_STREAM;
    }
}

ostream &Logs::error(const string &logger) {
    if (enabled) {
        cerr << "ERROR [" << logger << "] ";
        return cerr;
    } else {
        return NULL_STREAM;
    }
}

void Logs::debugTimestamp(const string &logger, const string &message) {
    debug(logger) << Timing::getRelativeTimeMs() << " - " << message << endl;
}

void Logs::disable() {
    enabled = false;
}
