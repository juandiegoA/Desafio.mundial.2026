TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        equipo.cpp \
        grupo.cpp \
        jugador.cpp \
        main.cpp \
        medidorrecursos.cpp \
        partido.cpp \
        torneo.cpp \
        utilidades.cpp

HEADERS += \
    Tipos.h \
    equipo.h \
    grupo.h \
    jugador.h \
    medidorrecursos.h \
    partido.h \
    torneo.h \
    utilidades.h
