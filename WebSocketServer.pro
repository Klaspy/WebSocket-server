QT = core websockets

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        src/connection.cpp \
        src/guidedobject.cpp \
        src/jsonparser.cpp \
        src/manager.cpp \
        src/server.cpp \
        src/worker.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/connection.h \
    src/guidedobject.h \
    src/jsonparser.h \
    src/manager.h \
    src/server.h \
    src/worker.h
