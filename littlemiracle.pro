QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += auto_visibility_controller.cpp  kcolorbutton.cpp  main.cpp  main_window.cpp  star_widget.cpp douglas_peucker.cpp sharpness_line_edit.cpp

HEADERS += auto_visibility_controller.h  chart_elements.h  enum_map.h  kcolorbutton.h  main_window.h  star_widget.h  visibility_flags.h douglas_peucker.h sharpness_line_edit.h

FORMS += main_window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
