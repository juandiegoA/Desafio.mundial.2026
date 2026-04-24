#ifndef JUGADOR_H
#define JUGADOR_H

#include <cstdint>
#include <iostream>
#include <string>

class Jugador {
private:
    uint8_t numeroCamiseta;
    std::string nombre;
    std::string apellido;

    uint16_t partidosJugados;
    uint16_t goles;
    uint32_t minutosJugados;
    uint16_t asistencias;
    uint16_t tarjetasAmarillas;
    uint16_t tarjetasRojas;
    uint16_t faltas;

public:
    Jugador();
    Jugador(uint8_t numeroCamiseta, const std::string& nombre, const std::string& apellido);

    uint8_t getNumeroCamiseta() const;
    const std::string& getNombre() const;
    const std::string& getApellido() const;
    uint16_t getPartidosJugados() const;
    uint16_t getGoles() const;
    uint32_t getMinutosJugados() const;
    uint16_t getAsistencias() const;
    uint16_t getTarjetasAmarillas() const;
    uint16_t getTarjetasRojas() const;
    uint16_t getFaltas() const;

    void sumarGoles(uint8_t cantidad);
    void actualizarEstadisticas(uint8_t golesPartido,
                                uint8_t minutosPartido,
                                uint8_t amarillasPartido,
                                uint8_t rojasPartido,
                                uint8_t faltasPartido,
                                uint8_t asistenciasPartido);

    void imprimir() const;
};

#endif
