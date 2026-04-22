#ifndef PARTIDO_H
#define PARTIDO_H

#include <array>
#include <random>
#include <vector>

#include "Equipo.h"
#include "Tipos.h"

class Partido {
private:
    Fecha fecha;
    uint16_t horaMinutos;
    uint8_t sedeId;
    std::array<uint16_t, 3> arbitros;
    Etapa etapa;

    uint8_t equipo1Id;
    uint8_t equipo2Id;

    EstadisticasEquipoPartido stats1;
    EstadisticasEquipoPartido stats2;

    bool jugado;
    bool huboProrroga;
    int16_t ganadorId;

    double calcularGolesEsperados(const Equipo& atacante, const Equipo& rival) const;
    uint8_t convertirAGolesReales(double lambda, std::mt19937& gen) const;
    float calcularPosesion(const Equipo& eq1, const Equipo& eq2) const;
    std::array<EstadisticasJugadorPartido, 11> simularJugadores(const Equipo& equipo,
                                                                const std::array<uint8_t, 11>& titulares,
                                                                uint8_t golesEquipo,
                                                                uint8_t minutos,
                                                                std::mt19937& gen) const;

public:
    Partido();

    void configurar(const Fecha& fecha,
                    uint16_t horaMinutos,
                    uint8_t sedeId,
                    const std::array<uint16_t, 3>& arbitros,
                    Etapa etapa,
                    uint8_t equipo1Id,
                    uint8_t equipo2Id);

    void simular(std::vector<Equipo>& equipos,
                 std::mt19937& gen,
                 bool permitirEmpate);

    void romperEmpatePorRanking(std::vector<Equipo>& equipos, std::mt19937& gen);
    void actualizarHistoricos(std::vector<Equipo>& equipos);

    uint8_t getEquipo1Id() const;
    uint8_t getEquipo2Id() const;
    int16_t getGanadorId() const;
    bool fueJugado() const;
    bool getHuboProrroga() const;
    Etapa getEtapa() const;
    const Fecha& getFecha() const;

    const EstadisticasEquipoPartido& getStats1() const;
    const EstadisticasEquipoPartido& getStats2() const;

    void imprimirDetalle(const std::vector<Equipo>& equipos) const;
};

#endif
