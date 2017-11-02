QT -= gui
QT += core network

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS  += \
    src/adoxabot.h \
    src/messageparser.h \
    src/network.h \
    src/toxcore.h \
    src/modules/imodule.h \
    src/modules/amodule.h \
    src/modules/networkModules/inetworkmodule.h \
    src/modules/networkModules/inetwork.h \
    src/modules/networkModules/anetworkmodule.h \
    src/modules/networkModules/anetwork.h \
    src/modules/networkModules/browser2ch/browser2ch.h \
    src/modules/networkModules/googleSearch/googlesearch.h \
    src/modules/networkModules/coinRate/coinrate.h \
    src/modules/networkModules/bitcoinRate/bitcoinrate.h \
    src/modules/networkModules/browser4ch/browser4ch.h \
    src/modules/networkModules/wikiSearch/wikisearch.h \
    src/modules/interactiveModules/cryptoHash/cryptohash.h \
    src/modules/interactiveModules/protectedControl/protectedcontrol.h \
    src/modules/interactiveModules/ainteractivemodule.h \
    src/modules/serviceModules/iservicemodule.h \
    src/modules/serviceModules/aservicemodule.h \
    src/modules/serviceModules/commandPrefix/commandprefix.h \
    src/modules/networkModules/weatherCondition/weathercondition.h \
    src/modules/interactiveModules/botInfo/botinfo.h \
    src/modules/serviceModules/contactManagement/contactmanagement.h \
    src/modules/interactiveModules/coinexchange.h

SOURCES += \
    src/main.cpp \
    src/adoxabot.cpp \
    src/messageparser.cpp \
    src/network.cpp \
    src/toxcore.cpp \
    src/modules/amodule.cpp \
    src/modules/networkModules/anetworkmodule.cpp \
    src/modules/networkModules/anetwork.cpp \
    src/modules/networkModules/browser2ch/browser2ch.cpp \
    src/modules/networkModules/googleSearch/googlesearch.cpp \
    src/modules/networkModules/coinRate/coinrate.cpp \
    src/modules/networkModules/bitcoinRate/bitcoinrate.cpp \
    src/modules/networkModules/browser4ch/browser4ch.cpp \
    src/modules/networkModules/wikiSearch/wikisearch.cpp \
    src/modules/interactiveModules/cryptoHash/cryptohash.cpp \
    src/modules/interactiveModules/protectedControl/protectedcontrol.cpp \
    src/modules/interactiveModules/ainteractivemodule.cpp \
    src/modules/serviceModules/aservicemodule.cpp \
    src/modules/serviceModules/commandPrefix/commandprefix.cpp \
    src/modules/networkModules/weatherCondition/weathercondition.cpp \
    src/modules/interactiveModules/botInfo/botinfo.cpp \
    src/modules/serviceModules/contactManagement/contactmanagement.cpp \
    src/modules/interactiveModules/coinexchange.cpp

unix:!macx: LIBS += -L$$PWD/libraries/dynamicLibLinux/ -ltoxcore.so.1.10.0

INCLUDEPATH += $$PWD/libraries/dynamicLibLinux
DEPENDPATH += $$PWD/libraries/dynamicLibLinux

unix:!macx: LIBS += -L$$PWD/libraries/dynamicLibLinux/ -ltoxav.so.1.10.0

INCLUDEPATH += $$PWD/libraries/dynamicLibLinux
DEPENDPATH += $$PWD/libraries/dynamicLibLinux

unix:!macx: LIBS += -L$$PWD/libraries/dynamicLibLinux/ -lsodium.so.23.0.0

INCLUDEPATH += $$PWD/libraries/dynamicLibLinux
DEPENDPATH += $$PWD/libraries/dynamicLibLinux
