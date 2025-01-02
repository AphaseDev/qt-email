##
# @file qt-email_Static.pri
# @author Alexander Nassian <nassian@bitshift-dynamics.com>
# @date 2020-06-08
#
# @brief The qt-email_Static project file. A platform independant wrapper to compose an email
# and open it with the default mail client.
# @note This file can be used to load the qt-email source files statically in your project or library.
#
# Copyright (c) 2012 bitshift dynamics. All rights reserved.
##

# General
SOURCES += \
    $$PWD/email.cpp \
    $$PWD/mimetypemanager.cpp

HEADERS += \
    $$PWD/email.h \
    $$PWD/email_p.h \
    $$PWD/mimetypemanager.h

OTHER_FILES += \
    $$PWD/mime.types

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

RESOURCES += \
    $$PWD/qtemailresources.qrc



