#include "Grupo.h"

#include <algorithm>
#include <iostream>

#include "Utilidades.h"

Grupo::Grupo()
    : letra('?'),
    idsEquipos{0, 0, 0, 0},
    cantidadEquipos(0),
    cantidadPartidos(0) {}

Grupo::Grupo(char letra)
    : letra(letra),
    idsEquipos{0, 0, 0, 0},
    cantidadEquipos(0),
    cantidadPartidos(0) {}

char Grupo::getLetra() const {
    return letra;
}

const std::array<uint8_t, 4>& Grupo::getIdsEquipos() const {
    return idsEquipos;
}

uint8_t Grupo::getCantidadEquipos() const {
    return cantidadEquipos;
}

const std::array<Partido, 6>& Grupo::getPartidos() const {
    return partidos;
}

std::array<Partido, 6>& Grupo::getPartidos() {
    return partidos;
}

bool Grupo::puedeAgregarEquipo(const Equipo& equipo, const std::vector<Equipo>& todos) const {
    int uefa = 0;

    for (uint8_t i = 0; i < cantidadEquipos; ++i) {
        const Equipo& actual = todos[idsEquipos[i]];

        if (actual.getConfederacion() == Confederacion::UEFA) {
            ++uefa;
        }

        if (equipo.getConfederacion() != Confederacion::UEFA &&
            actual.getConfederacion() == equipo.getConfederacion()) {
            return false;
        }
    }

    if (equipo.getConfederacion() == Confederacion::UEFA && uefa >= 2) {
        return false;
    }

    return cantidadEquipos < 4;
}

bool Grupo::agregarEquipo(uint8_t equipoId) {
    if (cantidadEquipos >= 4) {
        return false;
    }

    idsEquipos[cantidadEquipos] = equipoId;
    ++cantidadEquipos;
    return true;
}

void Grupo::quitarUltimoEquipo() {
    if (cantidadEquipos > 0) {
        --cantidadEquipos;
        idsEquipos[cantidadEquipos] = 0;
    }
}

void Grupo::generarPartidos() {
    cantidadPartidos = 6;

    std::array<uint16_t, 3> arbitros{1, 2, 3};
    Fecha fechaProvisional{20, 6, 2026};

    // Jornada 1
    partidos[0].configurar(fechaProvisional, 0, 0, arbitros, Etapa::GRUPOS, idsEquipos[0], idsEquipos[1]);
    partidos[1].configurar(fechaProvisional, 0, 0, arbitros, Etapa::GRUPOS, idsEquipos[2], idsEquipos[3]);

    // Jornada 2
    partidos[2].configurar(fechaProvisional, 0, 0, arbitros, Etapa::GRUPOS, idsEquipos[0], idsEquipos[2]);
    partidos[3].configurar(fechaProvisional, 0, 0, arbitros, Etapa::GRUPOS, idsEquipos[1], idsEquipos[3]);

    // Jornada 3
    partidos[4].configurar(fechaProvisional, 0, 0, arbitros, Etapa::GRUPOS, idsEquipos[0], idsEquipos[3]);
    partidos[5].configurar(fechaProvisional, 0, 0, arbitros, Etapa::GRUPOS, idsEquipos[1], idsEquipos[2]);
}

std::array<FilaTablaGrupo, 4> Grupo::construirTabla(const std::vector<Equipo>& equipos) const {
    std::array<FilaTablaGrupo, 4> tabla{};

    for (uint8_t i = 0; i < 4; ++i) {
        tabla[i] = {idsEquipos[i], 0, 0, 0, 0, 0, 0, 0, 0};
    }

    auto buscarFila = [&](uint8_t equipoId) -> FilaTablaGrupo& {
        for (auto& fila : tabla) {
            if (fila.equipoId == equipoId) {
                return fila;
            }
        }
        return tabla[0];
    };

    for (uint8_t i = 0; i < cantidadPartidos; ++i) {
        const Partido& p = partidos[i];

        if (!p.fueJugado()) {
            continue;
        }

        auto& f1 = buscarFila(p.getEquipo1Id());
        auto& f2 = buscarFila(p.getEquipo2Id());

        ++f1.partidosJugados;
        ++f2.partidosJugados;

        f1.golesFavor += p.getStats1().golesFavor;
        f1.golesContra += p.getStats1().golesContra;

        f2.golesFavor += p.getStats2().golesFavor;
        f2.golesContra += p.getStats2().golesContra;

        if (p.getStats1().golesFavor > p.getStats2().golesFavor) {
            f1.puntos += 3;
            ++f1.partidosGanados;
            ++f2.partidosPerdidos;
        }
        else if (p.getStats2().golesFavor > p.getStats1().golesFavor) {
            f2.puntos += 3;
            ++f2.partidosGanados;
            ++f1.partidosPerdidos;
        }
        else {
            ++f1.puntos;
            ++f2.puntos;
            ++f1.partidosEmpatados;
            ++f2.partidosEmpatados;
        }
    }

    for (auto& fila : tabla) {
        fila.diferenciaGoles = static_cast<int16_t>(fila.golesFavor) - static_cast<int16_t>(fila.golesContra);
    }

    std::sort(tabla.begin(), tabla.end(),
              [&equipos](const FilaTablaGrupo& a, const FilaTablaGrupo& b) {
                  if (a.puntos != b.puntos) return a.puntos > b.puntos;
                  if (a.diferenciaGoles != b.diferenciaGoles) return a.diferenciaGoles > b.diferenciaGoles;
                  if (a.golesFavor != b.golesFavor) return a.golesFavor > b.golesFavor;
                  return equipos[a.equipoId].getPais() < equipos[b.equipoId].getPais();
              });

    return tabla;
}

void Grupo::imprimirGrupo(const std::vector<Equipo>& equipos) const {
    std::cout << "Grupo " << letra << ":\n";

    for (uint8_t i = 0; i < cantidadEquipos; ++i) {
        const Equipo& eq = equipos[idsEquipos[i]];
        std::cout << " - " << eq.getPais()
                  << " (" << Utilidades::confederacionATexto(eq.getConfederacion()) << ")\n";
    }
}

void Grupo::imprimirTabla(const std::vector<Equipo>& equipos) const {
    auto tabla = construirTabla(equipos);

    std::cout << "Tabla Grupo " << letra << ":\n";

    for (const auto& fila : tabla) {
        std::cout << equipos[fila.equipoId].getPais()
        << " | Pts: " << static_cast<int>(fila.puntos)
        << " | DG: " << fila.diferenciaGoles
        << " | GF: " << static_cast<int>(fila.golesFavor)
        << '\n';
    }
}
