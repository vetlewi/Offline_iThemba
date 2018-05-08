TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt


ROOTFLAGS = $$system( root-config --cflags )
ROOTLIBS = $$system( root-config --glibs )

QMAKE_CXXFLAGS = $$ROOTFLAGS -Wall -W -std=c++11 -fPIC -m64 -O3 -march=native
QMAKE_CFLAGS += -Wall -W -fPIC -m64 -O3 -march=native
LIBS += $$ROOTLIBS

SCRIPTDIR = $$PWD/scripts

INCLUDEPATH +=  source \
                source/export/include \
                source/core/include \
                source/system/include \
                source/userroutine/include \
                source/types/include


SOURCES += source/main.cpp \
        source/export/src/RootWriter.cpp \
        source/export/src/MamaWriter.cpp \
        source/core/src/OfflineSorting.cpp \
        source/core/src/TDRRoutine.cpp \
        source/core/src/Unpacker.cpp \
        source/system/src/aptr.ipp \
        source/system/src/RateMeter.cpp \
        source/system/src/FileReader.cpp \
        source/system/src/IOPrintf.cpp \
        source/system/src/MTFileBufferFetcher.cpp \
        source/system/src/STFileBufferFetcher.cpp \
        source/types/src/Histograms.cpp \
        source/types/src/Histogram1D.cpp \
        source/types/src/Histogram2D.cpp \
        source/types/src/Histogram3D.cpp \
    	source/types/src/Parameters.cpp \
        source/types/src/ParticleRange.cpp \
        source/userroutine/src/UserSort.cpp \
        experimentsetup.c \
        source/types/src/Event.cpp \
        source/types/src/XIA_CFD.c

HEADERS += source/DefineFile.h \
        source/export/include/RootWriter.h \
        source/export/include/MamaWriter.h \
        source/core/include/TDRRoutine.h \
        source/core/include/OfflineSorting.h \
        source/core/include/UserRoutine.h \
        source/core/include/Unpacker.h \
        source/system/include/RateMeter.h \
        source/system/include/FileReader.h \
        source/system/include/aptr.h \
        source/system/include/IOPrintf.h \
        source/system/include/BufferFetcher.h \
        source/system/include/FileBufferFetcher.h \
        source/system/include/MTFileBufferFetcher.h \
        source/system/include/STFileBufferFetcher.h \
        source/types/include/Event.h \
        source/types/include/Histograms.h \
        source/types/include/Histogram1D.h \
        source/types/include/Histogram2D.h \
        source/types/include/Histogram3D.h \
        source/types/include/Parameters.h \
        source/types/include/ParticleRange.h \
        source/userroutine/include/UserSort.h \
        source/system/include/PixieSettings.h \
        source/system/include/BufferType.h \
        experimentsetup.h \
    source/system/include/WordBuffer.h
