
TEMPLATE = lib
TARGET =  AffinePlugin
CONFIG += plugin

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
     AffinePlugin.cpp \

HEADERS += \
     AffinePlugin.h \

INCLUDEPATH +=..

DISTFILES +=

#OpenCV path
unix: !mac {
       INCLUDEPATH += /usr/local/include/opencv4
       LIBS += -L/usr/local/include/opencv4 -lopencv_core -l opencv_imgproc
   }
win32 {
       INCLUDEPATH += c:/path/to/opencv/include/opencv4
       LIBS += -lc:/path/to/opencv/lib/opencv_world
   }
