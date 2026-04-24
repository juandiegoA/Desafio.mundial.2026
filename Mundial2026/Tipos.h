#ifndef TIPOS_H
#define TIPOS_H

#include <array>
#include <cstdint>
#include <string>

enum class Confederacion : uint8_t {
    UEFA,
    CONMEBOL,
    CONCACAF,
    CAF,
    AFC,
    OFC,
    DESCONOCIDA
};

enum class Etapa : uint8_t {
    GRUPOS,
    R16,
    R8,
    QF,
    SF,
    TERCER_PUESTO,
    FINAL
};

struct Fecha {
    uint8_t dia;
    uint8_t mes;
    uint16_t anio;
};

struct EstadisticasJugadorPartido {
    uint8_t indiceJugador;
    uint8_t goles;
    uint8_t minutosJugados;
    uint8_t tarjetasAmarillas;
    uint8_t tarjetasRojas;
    uint8_t faltas;
    uint8_t asistencias;
};

struct EstadisticasEquipoPartido {
    uint8_t golesFavor;
    uint8_t golesContra;
    float posesionBalon;
    std::array<uint8_t, 11> titulares;
    std::array<EstadisticasJugadorPartido, 11> estadisticasJugadores;
};

struct FilaTablaGrupo {
    uint8_t equipoId;
    uint8_t puntos;
    uint8_t partidosJugados;
    uint8_t partidosGanados;
    uint8_t partidosEmpatados;
    uint8_t partidosPerdidos;
    uint8_t golesFavor;
    uint8_t golesContra;
    int16_t diferenciaGoles;
};

#endif
