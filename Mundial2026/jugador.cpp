#include "Jugador.h"

Jugador::Jugador()
    : numeroCamiseta(0),
    nombre(""),
    apellido(""),
    partidosJugados(0),
    goles(0),
    minutosJugados(0),
    asistencias(0),
    tarjetasAmarillas(0),
    tarjetasRojas(0),
    faltas(0) {}

Jugador::Jugador(uint8_t numeroCamiseta, const std::string& nombre, const std::string& apellido)
    : numeroCamiseta(numeroCamiseta),
    nombre(nombre),
    apellido(apellido),
    partidosJugados(0),
    goles(0),
    minutosJugados(0),
    asistencias(0),
    tarjetasAmarillas(0),
    tarjetasRojas(0),
    faltas(0) {}

uint8_t Jugador::getNumeroCamiseta() const { return numeroCamiseta; }
const std::string& Jugador::getNombre() const { return nombre; }
const std::string& Jugador::getApellido() const { return apellido; }

void Jugador::sumarGoles(uint8_t cantidad) { goles += cantidad; }

void Jugador::actualizarEstadisticas(uint8_t golesPartido,
                                     uint8_t minutosPartido,
                                     uint8_t amarillasPartido,
                                     uint8_t rojasPartido,
                                     uint8_t faltasPartido,
                                     uint8_t asistenciasPartido) {
    partidosJugados += 1;
    goles += golesPartido;
    minutosJugados += minutosPartido;
    tarjetasAmarillas += amarillasPartido;
    tarjetasRojas += rojasPartido;
    faltas += faltasPartido;
    asistencias += asistenciasPartido;
}

void Jugador::imprimir() const {
}
