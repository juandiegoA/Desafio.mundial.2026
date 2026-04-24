#include "Equipo.h"

#include <algorithm>

Equipo::Equipo()
    : id(0),
    pais(""),
    directorTecnico(""),
    federacion(""),
    confederacion(Confederacion::DESCONOCIDA),
    rankingFIFA(0),
    golesFavorHistoricos(0),
    golesContraHistoricos(0),
    partidosGanados(0),
    partidosEmpatados(0),
    partidosPerdidos(0),
    tarjetasAmarillas(0),
    tarjetasRojas(0),
    faltas(0) {}

Equipo::Equipo(uint8_t id,
               const std::string& pais,
               const std::string& directorTecnico,
               const std::string& federacion,
               Confederacion confederacion,
               uint16_t rankingFIFA,
               uint16_t golesFavorHistoricos,
               uint16_t golesContraHistoricos,
               uint16_t partidosGanados,
               uint16_t partidosEmpatados,
               uint16_t partidosPerdidos)
    : id(id),
    pais(pais),
    directorTecnico(directorTecnico),
    federacion(federacion),
    confederacion(confederacion),
    rankingFIFA(rankingFIFA),
    golesFavorHistoricos(golesFavorHistoricos),
    golesContraHistoricos(golesContraHistoricos),
    partidosGanados(partidosGanados),
    partidosEmpatados(partidosEmpatados),
    partidosPerdidos(partidosPerdidos),
    tarjetasAmarillas(0),
    tarjetasRojas(0),
    faltas(0) {}

uint8_t Equipo::getId() const { return id; }
const std::string& Equipo::getPais() const { return pais; }
const std::string& Equipo::getDirectorTecnico() const { return directorTecnico; }
const std::string& Equipo::getFederacion() const { return federacion; }
Confederacion Equipo::getConfederacion() const { return confederacion; }
uint16_t Equipo::getRankingFIFA() const { return rankingFIFA; }
uint16_t Equipo::getGolesFavorHistoricos() const { return golesFavorHistoricos; }
uint16_t Equipo::getGolesContraHistoricos() const { return golesContraHistoricos; }
uint16_t Equipo::getPartidosGanados() const { return partidosGanados; }
uint16_t Equipo::getPartidosEmpatados() const { return partidosEmpatados; }
uint16_t Equipo::getPartidosPerdidos() const { return partidosPerdidos; }
uint16_t Equipo::getTarjetasAmarillas() const { return tarjetasAmarillas; }
uint16_t Equipo::getTarjetasRojas() const { return tarjetasRojas; }
uint16_t Equipo::getFaltas() const { return faltas; }
std::array<Jugador, 26>& Equipo::getJugadores() { return jugadores; }
const std::array<Jugador, 26>& Equipo::getJugadores() const { return jugadores; }

void Equipo::generarJugadoresArtificiales() {
    const uint16_t golesBase = golesFavorHistoricos / 26;
    const uint16_t residuo = golesFavorHistoricos % 26;

    for (uint8_t i = 0; i < 26; ++i) {
        const uint8_t numero = i + 1;
        jugadores[i] = Jugador(numero,
                               "nombre" + std::to_string(numero),
                               "apellido" + std::to_string(numero));

        uint8_t golesIniciales = static_cast<uint8_t>(golesBase);
        if (i < residuo) {
            ++golesIniciales;
        }
        jugadores[i].sumarGoles(golesIniciales);
    }
}

std::array<uint8_t, 11> Equipo::seleccionarTitulares(std::mt19937& gen) const {
    std::array<uint8_t, 26> indices{};
    for (uint8_t i = 0; i < 26; ++i) {
        indices[i] = i;
    }
    std::shuffle(indices.begin(), indices.end(), gen);

    std::array<uint8_t, 11> titulares{};
    for (uint8_t i = 0; i < 11; ++i) {
        titulares[i] = indices[i];
    }
    return titulares;
}

void Equipo::actualizarHistoricoEquipo(uint8_t golesFavorPartido,
                                       uint8_t golesContraPartido,
                                       uint8_t amarillasPartido,
                                       uint8_t rojasPartido,
                                       uint8_t faltasPartido,
                                       bool victoria,
                                       bool empate,
                                       bool derrota) {
    golesFavorHistoricos += golesFavorPartido;
    golesContraHistoricos += golesContraPartido;
    tarjetasAmarillas += amarillasPartido;
    tarjetasRojas += rojasPartido;
    faltas += faltasPartido;

    if (victoria) ++partidosGanados;
    if (empate) ++partidosEmpatados;
    if (derrota) ++partidosPerdidos;
}

void Equipo::actualizarJugadoresConvocados(const std::array<EstadisticasJugadorPartido, 11>& estadisticasJugadores) {
    for (const auto& stats : estadisticasJugadores) {
        if (stats.indiceJugador < jugadores.size()) {
            jugadores[stats.indiceJugador].actualizarEstadisticas(
                stats.goles,
                stats.minutosJugados,
                stats.tarjetasAmarillas,
                stats.tarjetasRojas,
                stats.faltas,
                stats.asistencias
                );
        }
    }
}

float Equipo::promedioGF() const {
    const uint16_t partidosTotales = partidosGanados + partidosEmpatados + partidosPerdidos;
    if (partidosTotales == 0) return 0.0f;
    return static_cast<float>(golesFavorHistoricos) / static_cast<float>(partidosTotales);
}

float Equipo::promedioGC() const {
    const uint16_t partidosTotales = partidosGanados + partidosEmpatados + partidosPerdidos;
    if (partidosTotales == 0) return 0.0f;
    return static_cast<float>(golesContraHistoricos) / static_cast<float>(partidosTotales);
}

void Equipo::imprimirResumen() const {
    std::cout << "ID: " << static_cast<int>(id)
    << " | Pais: " << pais
    << " | DT: " << directorTecnico
    << " | Ranking FIFA: " << rankingFIFA
    << " | GF: " << golesFavorHistoricos
    << " | GC: " << golesContraHistoricos
    << " | PG: " << partidosGanados
    << " | PE: " << partidosEmpatados
    << " | PP: " << partidosPerdidos
    << " | TA: " << tarjetasAmarillas
    << " | TR: " << tarjetasRojas
    << " | Faltas: " << faltas
    << '\n';
}

void Equipo::imprimirJugadores() const {
    std::cout << "Jugadores de " << pais << ":\n";
    for (const auto& jugador : jugadores) {
        jugador.imprimir();
    }
}
