#include "Logs.h"

#include "Time.h"

static std::ostream NULL_STREAM(0);
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
