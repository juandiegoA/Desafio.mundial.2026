#ifndef JUGADOR_H
#define JUGADOR_H

#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>

class Jugador {
private:
    uint8_t numeroCamiseta;
    std::string nombre;
    std::string apellido;

    // Estadisticas historicas acumuladas
    uint16_t partidosJugados;
    uint16_t goles;
    uint32_t minutosJugados;
    uint16_t asistencias;
    uint16_t tarjetasAmarillas;
    uint16_t tarjetasRojas;
    uint16_t faltas;

    // Estadisticas exclusivas de la copa actual
    uint16_t partidosCopaActual;
    uint16_t golesCopaActual;
    uint32_t minutosCopaActual;
    uint16_t asistenciasCopaActual;
    uint16_t tarjetasAmarillasCopaActual;
    uint16_t tarjetasRojasCopaActual;
    uint16_t faltasCopaActual;

public:
    Jugador();
    Jugador(uint8_t numeroCamiseta, const std::string& nombre, const std::string& apellido);

    // Constructor de copia y operador de asignacion.
    Jugador(const Jugador& otro) = default;
    Jugador& operator=(const Jugador& otro) = default;

    uint8_t getNumeroCamiseta() const;
    const std::string& getNombre() const;
    const std::string& getApellido() const;

    // Getters de historico total
    uint16_t getPartidosJugados() const;
    uint16_t getGoles() const;
    uint32_t getMinutosJugados() const;
    uint16_t getAsistencias() const;
    uint16_t getTarjetasAmarillas() const;
    uint16_t getTarjetasRojas() const;
    uint16_t getFaltas() const;

    // Getters de copa actual
    uint16_t getPartidosCopaActual() const;
    uint16_t getGolesCopaActual() const;
    uint32_t getMinutosCopaActual() const;
    uint16_t getAsistenciasCopaActual() const;
    uint16_t getTarjetasAmarillasCopaActual() const;
    uint16_t getTarjetasRojasCopaActual() const;
    uint16_t getFaltasCopaActual() const;

    void sumarGoles(uint8_t cantidad);

    void actualizarEstadisticas(uint8_t golesPartido,
                                uint8_t minutosPartido,
                                uint8_t amarillasPartido,
                                uint8_t rojasPartido,
                                uint8_t faltasPartido,
                                uint8_t asistenciasPartido);

    void imprimir() const;
};

std::ostream& operator<<(std::ostream& os, const Jugador& jugador);

#endif
