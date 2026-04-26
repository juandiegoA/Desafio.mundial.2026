#include "equipo.h"

#include <iostream>

Equipo::Equipo()
    : id(0),
    pais(""),
    directorTecnico(""),
    federacion(""),
    confederacion(Confederacion::DESCONOCIDA),
    rankingFIFA(0),
    golesFavorHistoricos(0),
    golesContraHistoricos(0),
    partidosGanadosHistoricos(0),
    partidosEmpatadosHistoricos(0),
    partidosPerdidosHistoricos(0),
    tarjetasAmarillasHistoricas(0),
    tarjetasRojasHistoricas(0),
    faltasHistoricas(0) {}

Equipo::Equipo(uint8_t id,
               const std::string& pais,
               const std::string& directorTecnico,
               const std::string& federacion,
               Confederacion confederacion,
               uint16_t rankingFIFA,
               uint16_t golesFavorHistoricos,
               uint16_t golesContraHistoricos,
               uint16_t partidosGanadosHistoricos,
               uint16_t partidosEmpatadosHistoricos,
               uint16_t partidosPerdidosHistoricos)
    : id(id),
    pais(pais),
    directorTecnico(directorTecnico),
    federacion(federacion),
    confederacion(confederacion),
    rankingFIFA(rankingFIFA),
    golesFavorHistoricos(golesFavorHistoricos),
    golesContraHistoricos(golesContraHistoricos),
    partidosGanadosHistoricos(partidosGanadosHistoricos),
    partidosEmpatadosHistoricos(partidosEmpatadosHistoricos),
    partidosPerdidosHistoricos(partidosPerdidosHistoricos),
    tarjetasAmarillasHistoricas(0),
    tarjetasRojasHistoricas(0),
    faltasHistoricas(0) {}

uint8_t Equipo::getId() const {
    return id;
}

const std::string& Equipo::getPais() const {
    return pais;
}

const std::string& Equipo::getDirectorTecnico() const {
    return directorTecnico;
}

const std::string& Equipo::getFederacion() const {
    return federacion;
}

Confederacion Equipo::getConfederacion() const {
    return confederacion;
}

uint16_t Equipo::getRankingFIFA() const {
    return rankingFIFA;
}

uint16_t Equipo::getGolesFavorHistoricos() const {
    return golesFavorHistoricos;
}

uint16_t Equipo::getGolesContraHistoricos() const {
    return golesContraHistoricos;
}

uint16_t Equipo::getPartidosGanadosHistoricos() const {
    return partidosGanadosHistoricos;
}

uint16_t Equipo::getPartidosEmpatadosHistoricos() const {
    return partidosEmpatadosHistoricos;
}

uint16_t Equipo::getPartidosPerdidosHistoricos() const {
    return partidosPerdidosHistoricos;
}

uint16_t Equipo::getTarjetasAmarillasHistoricas() const {
    return tarjetasAmarillasHistoricas;
}

uint16_t Equipo::getTarjetasRojasHistoricas() const {
    return tarjetasRojasHistoricas;
}

uint16_t Equipo::getFaltasHistoricas() const {
    return faltasHistoricas;
}

const std::array<Jugador, 26>& Equipo::getJugadores() const {
    return jugadores;
}

std::array<Jugador, 26>& Equipo::getJugadores() {
    return jugadores;
}

double Equipo::promedioGF() const {
    uint16_t partidosTotales = partidosGanadosHistoricos
                               + partidosEmpatadosHistoricos
                               + partidosPerdidosHistoricos;

    if (partidosTotales == 0) {
        return 0.0;
    }

    return static_cast<double>(golesFavorHistoricos) /
           static_cast<double>(partidosTotales);
}

double Equipo::promedioGC() const {
    uint16_t partidosTotales = partidosGanadosHistoricos
                               + partidosEmpatadosHistoricos
                               + partidosPerdidosHistoricos;

    if (partidosTotales == 0) {
        return 0.0;
    }

    return static_cast<double>(golesContraHistoricos) /
           static_cast<double>(partidosTotales);
}

void Equipo::generarJugadoresArtificiales() {
    for (uint8_t i = 0; i < 26; ++i) {
        jugadores[i] = Jugador(
            static_cast<uint8_t>(i + 1),
            "nombre" + std::to_string(i + 1),
            "apellido" + std::to_string(i + 1)
            );
    }
}

void Equipo::actualizarEstadisticas(uint8_t golesFavorPartido,
                                    uint8_t golesContraPartido,
                                    uint8_t tarjetasAmarillasPartido,
                                    uint8_t tarjetasRojasPartido,
                                    uint8_t faltasPartido) {
    golesFavorHistoricos += golesFavorPartido;
    golesContraHistoricos += golesContraPartido;
    tarjetasAmarillasHistoricas += tarjetasAmarillasPartido;
    tarjetasRojasHistoricas += tarjetasRojasPartido;
    faltasHistoricas += faltasPartido;

    if (golesFavorPartido > golesContraPartido) {
        ++partidosGanadosHistoricos;
    } else if (golesFavorPartido == golesContraPartido) {
        ++partidosEmpatadosHistoricos;
    } else {
        ++partidosPerdidosHistoricos;
    }
}

std::array<uint8_t, 11> Equipo::seleccionarTitulares(std::mt19937& gen) const {
    (void)gen;

    std::array<uint8_t, 11> titulares{};

    for (uint8_t i = 0; i < 11; ++i) {
        titulares[i] = i;
    }

    return titulares;
}

void Equipo::actualizarHistoricoEquipo(uint8_t golesFavorPartido,
                                       uint8_t golesContraPartido,
                                       uint8_t tarjetasAmarillasPartido,
                                       uint8_t tarjetasRojasPartido,
                                       uint8_t faltasPartido,
                                       bool victoria,
                                       bool empate,
                                       bool derrota) {
    golesFavorHistoricos += golesFavorPartido;
    golesContraHistoricos += golesContraPartido;

    tarjetasAmarillasHistoricas += tarjetasAmarillasPartido;
    tarjetasRojasHistoricas += tarjetasRojasPartido;
    faltasHistoricas += faltasPartido;

    if (victoria) {
        ++partidosGanadosHistoricos;
    } else if (empate) {
        ++partidosEmpatadosHistoricos;
    } else if (derrota) {
        ++partidosPerdidosHistoricos;
    }
}

void Equipo::actualizarJugadoresConvocados(
    const std::array<EstadisticasJugadorPartido, 11>& estadisticasJugadores) {

    for (const auto& est : estadisticasJugadores) {
        uint8_t idx = est.indiceJugador;

        if (idx >= jugadores.size()) {
            continue;
        }

        jugadores[idx].actualizarEstadisticas(
            est.goles,
            est.minutosJugados,
            est.tarjetasAmarillas,
            est.tarjetasRojas,
            est.faltas,
            est.asistencias
            );
    }
}

void Equipo::imprimirResumen() const {
    std::cout << *this
              << " | TA historicas: " << tarjetasAmarillasHistoricas
              << " | TR historicas: " << tarjetasRojasHistoricas
              << " | Faltas historicas: " << faltasHistoricas
              << '\n';
}

std::ostream& operator<<(std::ostream& os, const Equipo& equipo) {
    os << "ID: " << static_cast<int>(equipo.getId())
    << " | Pais: " << equipo.getPais()
    << " | DT: " << equipo.getDirectorTecnico()
    << " | Federacion: " << equipo.getFederacion()
    << " | Ranking FIFA: " << equipo.getRankingFIFA()
    << " | GF historicos: " << equipo.getGolesFavorHistoricos()
    << " | GC historicos: " << equipo.getGolesContraHistoricos()
    << " | PG: " << equipo.getPartidosGanadosHistoricos()
    << " | PE: " << equipo.getPartidosEmpatadosHistoricos()
    << " | PP: " << equipo.getPartidosPerdidosHistoricos();

    return os;
}
