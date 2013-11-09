#QMAKE_CXXFLAGS = -std=c++11 -stdlib=libc++
#QMAKE_LFLAGS = -std=c++11 -stdlib=libc++
CONFIG(release, debug|release): DEFINES += NDEBUG

