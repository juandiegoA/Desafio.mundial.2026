#ifndef EQUIPO_H
#define EQUIPO_H

#include <array>
#include <iostream>
#include <random>
#include <string>

#include "Jugador.h"
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
    uint16_t partidosGanados;
    uint16_t partidosEmpatados;
    uint16_t partidosPerdidos;
    uint16_t tarjetasAmarillas;
    uint16_t tarjetasRojas;
    uint16_t faltas;

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
           uint16_t partidosGanados,
           uint16_t partidosEmpatados,
           uint16_t partidosPerdidos);

    uint8_t getId() const;
    const std::string& getPais() const;
    const std::string& getDirectorTecnico() const;
    const std::string& getFederacion() const;
    Confederacion getConfederacion() const;
    uint16_t getRankingFIFA() const;
    uint16_t getGolesFavorHistoricos() const;
    uint16_t getGolesContraHistoricos() const;
    uint16_t getPartidosGanados() const;
    uint16_t getPartidosEmpatados() const;
    uint16_t getPartidosPerdidos() const;
    uint16_t getTarjetasAmarillas() const;
    uint16_t getTarjetasRojas() const;
    uint16_t getFaltas() const;

    std::array<Jugador, 26>& getJugadores();
    const std::array<Jugador, 26>& getJugadores() const;

    void generarJugadoresArtificiales();
    std::array<uint8_t, 11> seleccionarTitulares(std::mt19937& gen) const;

    void actualizarHistoricoEquipo(uint8_t golesFavorPartido,
                                   uint8_t golesContraPartido,
                                   uint8_t amarillasPartido,
                                   uint8_t rojasPartido,
                                   uint8_t faltasPartido,
                                   bool victoria,
                                   bool empate,
                                   bool derrota);

    void actualizarJugadoresConvocados(const std::array<EstadisticasJugadorPartido, 11>& estadisticasJugadores);

    float promedioGF() const;
    float promedioGC() const;

    void imprimirResumen() const;
    void imprimirJugadores() const;
};

#endif
