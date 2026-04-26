#ifndef EQUIPO_H
#define EQUIPO_H

#include <array>
#include <cstdint>
#include <ostream>
#include <random>
#include <string>

#include "jugador.h"
#include "Tipos.h"

class Equipo {
private:
    uint8_t id;
    std::string pais;
    std::string directorTecnico;
    std::string federacion;
    Confederacion confederacion;
    uint16_t rankingFIFA;

    uint16_t golesFavorHistoricos;
    uint16_t golesContraHistoricos;
    uint16_t partidosGanadosHistoricos;
    uint16_t partidosEmpatadosHistoricos;
    uint16_t partidosPerdidosHistoricos;
    uint16_t tarjetasAmarillasHistoricas;
    uint16_t tarjetasRojasHistoricas;
    uint16_t faltasHistoricas;

    std::array<Jugador, 26> jugadores;

public:
    Equipo();

    Equipo(uint8_t id,
           const std::string& pais,
           const std::string& directorTecnico,
           const std::string& federacion,
           Confederacion confederacion,
           uint16_t rankingFIFA,
           uint16_t golesFavorHistoricos,
           uint16_t golesContraHistoricos,
           uint16_t partidosGanadosHistoricos,
           uint16_t partidosEmpatadosHistoricos,
           uint16_t partidosPerdidosHistoricos);

    // Constructor de copia y operador de asignacion.
    Equipo(const Equipo& otro) = default;
    Equipo& operator=(const Equipo& otro) = default;

    uint8_t getId() const;
    const std::string& getPais() const;
    const std::string& getDirectorTecnico() const;
    const std::string& getFederacion() const;
    Confederacion getConfederacion() const;
    uint16_t getRankingFIFA() const;

    uint16_t getGolesFavorHistoricos() const;
    uint16_t getGolesContraHistoricos() const;
    uint16_t getPartidosGanadosHistoricos() const;
    uint16_t getPartidosEmpatadosHistoricos() const;
    uint16_t getPartidosPerdidosHistoricos() const;
    uint16_t getTarjetasAmarillasHistoricas() const;
    uint16_t getTarjetasRojasHistoricas() const;
    uint16_t getFaltasHistoricas() const;

    const std::array<Jugador, 26>& getJugadores() const;
    std::array<Jugador, 26>& getJugadores();

    double promedioGF() const;
    double promedioGC() const;

    void generarJugadoresArtificiales();

    void actualizarEstadisticas(uint8_t golesFavorPartido,
                                uint8_t golesContraPartido,
                                uint8_t tarjetasAmarillasPartido,
                                uint8_t tarjetasRojasPartido,
                                uint8_t faltasPartido);

    std::array<uint8_t, 11> seleccionarTitulares(std::mt19937& gen) const;

    void actualizarHistoricoEquipo(uint8_t golesFavorPartido,
                                   uint8_t golesContraPartido,
                                   uint8_t tarjetasAmarillasPartido,
                                   uint8_t tarjetasRojasPartido,
                                   uint8_t faltasPartido,
                                   bool victoria,
                                   bool empate,
                                   bool derrota);

    void actualizarJugadoresConvocados(
        const std::array<EstadisticasJugadorPartido, 11>& estadisticasJugadores);

    void imprimirResumen() const;
};

std::ostream& operator<<(std::ostream& os, const Equipo& equipo);

#endif
