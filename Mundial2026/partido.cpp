#include "Partido.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>

#include "Utilidades.h"

Partido::Partido()
    : fecha{1, 1, 2026},
    horaMinutos(0),
    sedeId(0),
    arbitros{1, 2, 3},
    etapa(Etapa::GRUPOS),
    equipo1Id(0),
    equipo2Id(0),
    jugado(false),
    huboProrroga(false),
    ganadorId(-1) {
    stats1 = {};
    stats2 = {};
}

void Partido::configurar(const Fecha& fecha,
                         uint16_t horaMinutos,
                         uint8_t sedeId,
                         const std::array<uint16_t, 3>& arbitros,
                         Etapa etapa,
                         uint8_t equipo1Id,
                         uint8_t equipo2Id) {
    this->fecha = fecha;
    this->horaMinutos = horaMinutos;
    this->sedeId = sedeId;
    this->arbitros = arbitros;
    this->etapa = etapa;
    this->equipo1Id = equipo1Id;
    this->equipo2Id = equipo2Id;
}

double Partido::calcularGolesEsperados(const Equipo& atacante, const Equipo& rival) const {
    constexpr double mu = 1.35;
    constexpr double alpha = 0.6;
    constexpr double beta = 0.4;

    double gf = atacante.promedioGF();
    double gc = rival.promedioGC();

    // Evitar ceros exactos que colapsen la fórmula
    if (gf <= 0.0) {
        gf = 0.01;
    }

    if (gc <= 0.0) {
        gc = 0.01;
    }

    return mu * std::pow(gf / mu, alpha) * std::pow(gc / mu, beta);
}

uint8_t Partido::convertirAGolesReales(double lambda, std::mt19937& gen) const {
    if (lambda < 0.0) lambda = 0.0;

    std::poisson_distribution<int> dist(lambda);
    int goles = dist(gen);

    if (goles < 0) goles = 0;
    if (goles > 15) goles = 15;

    return static_cast<uint8_t>(goles);
}

float Partido::calcularPosesion(const Equipo& eq1, const Equipo& eq2) const {
    double f1 = 1.0 / std::max<uint16_t>(1, eq1.getRankingFIFA());
    double f2 = 1.0 / std::max<uint16_t>(1, eq2.getRankingFIFA());

    return static_cast<float>((f1 / (f1 + f2)) * 100.0);
}

std::array<EstadisticasJugadorPartido, 11> Partido::simularJugadores(const Equipo& equipo,
                                                                     const std::array<uint8_t, 11>& titulares,
                                                                     uint8_t golesEquipo,
                                                                     uint8_t minutos,
                                                                     std::mt19937& gen) const {
    std::array<EstadisticasJugadorPartido, 11> stats{};

    for (int i = 0; i < 11; ++i) {
        stats[i] = {titulares[i], 0, minutos, 0, 0, 0, 0};

        if (Utilidades::eventoConProbabilidad(0.06, gen)) {
            stats[i].tarjetasAmarillas = 1;

            if (Utilidades::eventoConProbabilidad(0.0115, gen)) {
                stats[i].tarjetasAmarillas = 2;
                stats[i].tarjetasRojas = 1;
            }
        }

        if (Utilidades::eventoConProbabilidad(0.13, gen)) {
            stats[i].faltas += 1;

            if (Utilidades::eventoConProbabilidad(0.0275, gen)) {
                stats[i].faltas += 1;

                if (Utilidades::eventoConProbabilidad(0.007, gen)) {
                    stats[i].faltas += 1;
                }
            }
        }
    }

    uint8_t golesAsignados = 0;
    while (golesAsignados < golesEquipo) {
        int idx = Utilidades::randomInt(0, 10, gen);

        if (Utilidades::eventoConProbabilidad(0.04, gen)) {
            stats[idx].goles += 1;
            ++golesAsignados;
        }
    }

    (void)equipo;
    return stats;
}

void Partido::simular(std::vector<Equipo>& equipos, std::mt19937& gen, bool permitirEmpate) {
    Equipo& eq1 = equipos[equipo1Id];
    Equipo& eq2 = equipos[equipo2Id];

    double lambda1 = calcularGolesEsperados(eq1, eq2);
    double lambda2 = calcularGolesEsperados(eq2, eq1);

    stats1.golesFavor = convertirAGolesReales(lambda1, gen);
    stats2.golesFavor = convertirAGolesReales(lambda2, gen);

    stats1.golesContra = stats2.golesFavor;
    stats2.golesContra = stats1.golesFavor;

    stats1.posesionBalon = calcularPosesion(eq1, eq2);
    stats2.posesionBalon = 100.0f - stats1.posesionBalon;

    stats1.titulares = eq1.seleccionarTitulares(gen);
    stats2.titulares = eq2.seleccionarTitulares(gen);

    stats1.estadisticasJugadores = simularJugadores(eq1, stats1.titulares, stats1.golesFavor, 90, gen);
    stats2.estadisticasJugadores = simularJugadores(eq2, stats2.titulares, stats2.golesFavor, 90, gen);

    if (stats1.golesFavor > stats2.golesFavor) {
        ganadorId = equipo1Id;
    } else if (stats2.golesFavor > stats1.golesFavor) {
        ganadorId = equipo2Id;
    } else {
        ganadorId = -1;
        if (!permitirEmpate) {
            romperEmpatePorRanking(equipos, gen);
        }
    }

    jugado = true;
    actualizarHistoricos(equipos);
}

void Partido::romperEmpatePorRanking(std::vector<Equipo>& equipos, std::mt19937& gen) {
    Equipo& eq1 = equipos[equipo1Id];
    Equipo& eq2 = equipos[equipo2Id];

    huboProrroga = true;

    double fuerza1 = 1.0 / std::max<uint16_t>(1, eq1.getRankingFIFA());
    double fuerza2 = 1.0 / std::max<uint16_t>(1, eq2.getRankingFIFA());
    double prob1 = fuerza1 / (fuerza1 + fuerza2);

    std::bernoulli_distribution dist(prob1);
    bool gana1 = dist(gen);

    if (gana1) {
        ++stats1.golesFavor;
        stats2.golesContra = stats1.golesFavor;
        ganadorId = equipo1Id;
    } else {
        ++stats2.golesFavor;
        stats1.golesContra = stats2.golesFavor;
        ganadorId = equipo2Id;
    }

    stats1.estadisticasJugadores = simularJugadores(eq1, stats1.titulares, stats1.golesFavor, 120, gen);
    stats2.estadisticasJugadores = simularJugadores(eq2, stats2.titulares, stats2.golesFavor, 120, gen);
}

void Partido::actualizarHistoricos(std::vector<Equipo>& equipos) {
    auto sumarEquipo = [](const EstadisticasEquipoPartido& s) {
        uint8_t amarillas = 0;
        uint8_t rojas = 0;
        uint8_t faltas = 0;

        for (const auto& j : s.estadisticasJugadores) {
            amarillas += j.tarjetasAmarillas;
            rojas += j.tarjetasRojas;
            faltas += j.faltas;
        }

        return std::array<uint8_t, 3>{amarillas, rojas, faltas};
    };

    auto tot1 = sumarEquipo(stats1);
    auto tot2 = sumarEquipo(stats2);

    equipos[equipo1Id].actualizarHistoricoEquipo(
        stats1.golesFavor, stats1.golesContra,
        tot1[0], tot1[1], tot1[2],
        ganadorId == equipo1Id,
        ganadorId == -1,
        ganadorId == equipo2Id
        );

    equipos[equipo2Id].actualizarHistoricoEquipo(
        stats2.golesFavor, stats2.golesContra,
        tot2[0], tot2[1], tot2[2],
        ganadorId == equipo2Id,
        ganadorId == -1,
        ganadorId == equipo1Id
        );

    equipos[equipo1Id].actualizarJugadoresConvocados(stats1.estadisticasJugadores);
    equipos[equipo2Id].actualizarJugadoresConvocados(stats2.estadisticasJugadores);
}

uint8_t Partido::getEquipo1Id() const { return equipo1Id; }
uint8_t Partido::getEquipo2Id() const { return equipo2Id; }
int16_t Partido::getGanadorId() const { return ganadorId; }
bool Partido::fueJugado() const { return jugado; }
bool Partido::getHuboProrroga() const { return huboProrroga; }
Etapa Partido::getEtapa() const { return etapa; }
const Fecha& Partido::getFecha() const { return fecha; }
const EstadisticasEquipoPartido& Partido::getStats1() const { return stats1; }
const EstadisticasEquipoPartido& Partido::getStats2() const { return stats2; }

void Partido::imprimirDetalle(const std::vector<Equipo>& equipos) const {
    std::cout << "[" << Utilidades::etapaATexto(etapa) << "] "
              << equipos[equipo1Id].getPais() << " " << static_cast<int>(stats1.golesFavor)
              << " - " << static_cast<int>(stats2.golesFavor) << " "
              << equipos[equipo2Id].getPais()
              << " | Fecha: " << Utilidades::fechaATexto(fecha)
              << (huboProrroga ? " | Prorroga" : "")
              << '\n';
}
