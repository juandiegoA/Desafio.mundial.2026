#include "Torneo.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "Utilidades.h"

namespace {

struct EquipoClasificado {
    uint8_t equipoId;
    char grupo;
    uint8_t posicion;
    uint8_t puntos;
    int16_t diferenciaGoles;
    uint8_t golesFavor;
    uint32_t boletoSorteo;
};

bool mejorClasificado(const EquipoClasificado& a, const EquipoClasificado& b) {
    if (a.puntos != b.puntos) return a.puntos > b.puntos;
    if (a.diferenciaGoles != b.diferenciaGoles) return a.diferenciaGoles > b.diferenciaGoles;
    if (a.golesFavor != b.golesFavor) return a.golesFavor > b.golesFavor;
    return a.boletoSorteo < b.boletoSorteo;
}

} // namespace

Torneo::Torneo() : generador(std::random_device{}()) {
    for (int i = 0; i < 12; ++i) {
        grupos[i] = Grupo(static_cast<char>('A' + i));
    }
}

void Torneo::cargarEquiposCSV(const std::string& archivoCSV) {
    equipos.clear();
    equipos.reserve(48);

    std::ifstream archivo(archivoCSV);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo CSV: " + archivoCSV);
    }

    std::string linea;

    std::getline(archivo, linea);
    std::getline(archivo, linea);

    uint8_t id = 0;

    while (std::getline(archivo, linea)) {
        medidor.sumar();

        if (linea.empty()) {
            continue;
        }

        auto campos = Utilidades::dividirCSV(linea);

        if (campos.size() < 10) {
            continue;
        }

        uint16_t ranking = static_cast<uint16_t>(std::stoi(campos[0]));
        std::string pais = campos[1];
        std::string directorTecnico = campos[2];
        std::string federacion = campos[3];
        Confederacion conf = Utilidades::convertirConfederacion(campos[4]);
        uint16_t gf = static_cast<uint16_t>(std::stoi(campos[5]));
        uint16_t gc = static_cast<uint16_t>(std::stoi(campos[6]));
        uint16_t pg = static_cast<uint16_t>(std::stoi(campos[7]));
        uint16_t pe = static_cast<uint16_t>(std::stoi(campos[8]));
        uint16_t pp = static_cast<uint16_t>(std::stoi(campos[9]));

        equipos.emplace_back(
            id++,
            pais,
            directorTecnico,
            federacion,
            conf,
            ranking,
            gf,
            gc,
            pg,
            pe,
            pp
            );
    }
}

void Torneo::generarJugadoresIniciales() {
    for (auto& equipo : equipos) {
        medidor.sumar();
        equipo.generarJugadoresArtificiales();
    }
}

void Torneo::crearBombos() {
    bombo1.clear();
    bombo2.clear();
    bombo3.clear();
    bombo4.clear();

    std::vector<uint8_t> idsResto;
    idsResto.reserve(equipos.size());

    bool anfitrionEncontrado = false;

    for (const auto& e : equipos) {
        medidor.sumar();

        if (e.getPais() == "United States") {
            bombo1.push_back(e.getId());
            anfitrionEncontrado = true;
        } else {
            idsResto.push_back(e.getId());
        }
    }

    if (!anfitrionEncontrado) {
        throw std::runtime_error("No se encontro al equipo anfitrion United States en la lista de equipos.");
    }

    std::sort(idsResto.begin(), idsResto.end(), [this](uint8_t a, uint8_t b) {
        return equipos[a].getRankingFIFA() < equipos[b].getRankingFIFA();
    });

    int idx = 0;

    while (bombo1.size() < 12 && idx < static_cast<int>(idsResto.size())) {
        medidor.sumar();
        bombo1.push_back(idsResto[idx++]);
    }

    while (bombo2.size() < 12 && idx < static_cast<int>(idsResto.size())) {
        medidor.sumar();
        bombo2.push_back(idsResto[idx++]);
    }

    while (bombo3.size() < 12 && idx < static_cast<int>(idsResto.size())) {
        medidor.sumar();
        bombo3.push_back(idsResto[idx++]);
    }

    while (bombo4.size() < 12 && idx < static_cast<int>(idsResto.size())) {
        medidor.sumar();
        bombo4.push_back(idsResto[idx++]);
    }
}

void Torneo::imprimirBombos() const {
    auto imprimir = [this](const std::vector<uint8_t>& bombo, const std::string& nombre) {
        std::cout << nombre << ":\n";
        for (uint8_t id : bombo) {
            std::cout << " - " << equipos[id].getPais() << "\n";
        }
    };

    imprimir(bombo1, "Bombo 1");
    imprimir(bombo2, "Bombo 2");
    imprimir(bombo3, "Bombo 3");
    imprimir(bombo4, "Bombo 4");
}

void Torneo::conformarGrupos() {
    for (int i = 0; i < 12; ++i) {
        grupos[i] = Grupo(static_cast<char>('A' + i));
    }

    std::array<std::vector<uint8_t>, 4> bombos{bombo1, bombo2, bombo3, bombo4};

    for (auto& bombo : bombos) {
        std::shuffle(bombo.begin(), bombo.end(), generador);
    }

    std::vector<uint8_t> ordenAsignacion;
    ordenAsignacion.reserve(48);

    for (int b = 0; b < 4; ++b) {
        for (uint8_t idEquipo : bombos[b]) {
            medidor.sumar();
            ordenAsignacion.push_back(idEquipo);
        }
    }

    std::function<bool(size_t)> backtrack = [&](size_t indice) -> bool {
        if (indice == ordenAsignacion.size()) {
            return true;
        }

        uint8_t idEquipo = ordenAsignacion[indice];
        const Equipo& equipoActual = equipos[idEquipo];
        int ronda = static_cast<int>(indice / 12);

        for (int g = 0; g < 12; ++g) {
            medidor.sumar();

            if (grupos[g].getCantidadEquipos() != ronda) {
                continue;
            }

            if (grupos[g].puedeAgregarEquipo(equipoActual, equipos)) {
                grupos[g].agregarEquipo(idEquipo);

                if (backtrack(indice + 1)) {
                    return true;
                }

                grupos[g].quitarUltimoEquipo();
            }
        }

        return false;
    };

    if (!backtrack(0)) {
        throw std::runtime_error("No se pudo conformar un grupo valido con las restricciones actuales.");
    }
}

void Torneo::imprimirGrupos() const {
    for (const auto& grupo : grupos) {
        grupo.imprimirGrupo(equipos);
        std::cout << '\n';
    }
}

bool Torneo::equipoDisponibleEnFecha(uint8_t equipoId,
                                     int diaCalendario,
                                     const std::vector<int>& ultimaFechaJugado) const {
    int ultimoDia = ultimaFechaJugado[equipoId];
    if (ultimoDia == -1000) {
        return true;
    }
    return (diaCalendario - ultimoDia) >= 3;
}

bool Torneo::puedeProgramarPartidoEnFecha(const Partido& partido,
                                          int diaCalendario,
                                          const std::vector<int>& ultimaFechaJugado,
                                          int partidosYaProgramadosEseDia) const {
    if (partidosYaProgramadosEseDia >= 4) {
        return false;
    }

    return equipoDisponibleEnFecha(partido.getEquipo1Id(), diaCalendario, ultimaFechaJugado) &&
           equipoDisponibleEnFecha(partido.getEquipo2Id(), diaCalendario, ultimaFechaJugado);
}

void Torneo::generarCalendarioGrupos() {
    for (auto& grupo : grupos) {
        medidor.sumar();
        grupo.generarPartidos();
    }

    Fecha fechaInicio{20, 6, 2026};
    std::array<uint16_t, 3> arbitros{1, 2, 3};

    for (int bloque = 0; bloque < 6; ++bloque) {
        medidor.sumar();

        int grupo1 = 2 * bloque;
        int grupo2 = 2 * bloque + 1;

        int diaJ1 = bloque;
        int diaJ2 = bloque + 6;
        int diaJ3 = bloque + 12;

        Fecha fechaJ1 = Utilidades::sumarDias(fechaInicio, diaJ1);
        Fecha fechaJ2 = Utilidades::sumarDias(fechaInicio, diaJ2);
        Fecha fechaJ3 = Utilidades::sumarDias(fechaInicio, diaJ3);

        {
            auto& partidos = grupos[grupo1].getPartidos();

            partidos[0].configurar(fechaJ1, 0, 0, arbitros, Etapa::GRUPOS, partidos[0].getEquipo1Id(), partidos[0].getEquipo2Id());
            partidos[1].configurar(fechaJ1, 0, 0, arbitros, Etapa::GRUPOS, partidos[1].getEquipo1Id(), partidos[1].getEquipo2Id());

            partidos[2].configurar(fechaJ2, 0, 0, arbitros, Etapa::GRUPOS, partidos[2].getEquipo1Id(), partidos[2].getEquipo2Id());
            partidos[3].configurar(fechaJ2, 0, 0, arbitros, Etapa::GRUPOS, partidos[3].getEquipo1Id(), partidos[3].getEquipo2Id());

            partidos[4].configurar(fechaJ3, 0, 0, arbitros, Etapa::GRUPOS, partidos[4].getEquipo1Id(), partidos[4].getEquipo2Id());
            partidos[5].configurar(fechaJ3, 0, 0, arbitros, Etapa::GRUPOS, partidos[5].getEquipo1Id(), partidos[5].getEquipo2Id());
        }

        {
            auto& partidos = grupos[grupo2].getPartidos();

            partidos[0].configurar(fechaJ1, 0, 0, arbitros, Etapa::GRUPOS, partidos[0].getEquipo1Id(), partidos[0].getEquipo2Id());
            partidos[1].configurar(fechaJ1, 0, 0, arbitros, Etapa::GRUPOS, partidos[1].getEquipo1Id(), partidos[1].getEquipo2Id());

            partidos[2].configurar(fechaJ2, 0, 0, arbitros, Etapa::GRUPOS, partidos[2].getEquipo1Id(), partidos[2].getEquipo2Id());
            partidos[3].configurar(fechaJ2, 0, 0, arbitros, Etapa::GRUPOS, partidos[3].getEquipo1Id(), partidos[3].getEquipo2Id());

            partidos[4].configurar(fechaJ3, 0, 0, arbitros, Etapa::GRUPOS, partidos[4].getEquipo1Id(), partidos[4].getEquipo2Id());
            partidos[5].configurar(fechaJ3, 0, 0, arbitros, Etapa::GRUPOS, partidos[5].getEquipo1Id(), partidos[5].getEquipo2Id());
        }
    }
}

void Torneo::imprimirCalendarioGrupos() const {
    struct PartidoCalendario {
        Fecha fecha;
        uint8_t equipo1Id;
        uint8_t equipo2Id;
        char grupo;
        int jornada;
    };

    std::vector<PartidoCalendario> calendario;
    calendario.reserve(72);

    for (int g = 0; g < 12; ++g) {
        char letraGrupo = static_cast<char>('A' + g);
        const auto& partidos = grupos[g].getPartidos();

        for (int p = 0; p < 6; ++p) {
            int jornada = 1;
            if (p >= 2 && p <= 3) jornada = 2;
            else if (p >= 4) jornada = 3;

            calendario.push_back({
                partidos[p].getFecha(),
                partidos[p].getEquipo1Id(),
                partidos[p].getEquipo2Id(),
                letraGrupo,
                jornada
            });
        }
    }

    std::sort(calendario.begin(), calendario.end(),
              [](const PartidoCalendario& a, const PartidoCalendario& b) {
                  if (a.fecha.anio != b.fecha.anio) return a.fecha.anio < b.fecha.anio;
                  if (a.fecha.mes != b.fecha.mes) return a.fecha.mes < b.fecha.mes;
                  if (a.fecha.dia != b.fecha.dia) return a.fecha.dia < b.fecha.dia;
                  if (a.grupo != b.grupo) return a.grupo < b.grupo;
                  return a.jornada < b.jornada;
              });

    std::cout << "===== CALENDARIO DE FASE DE GRUPOS =====\n";

    for (const auto& p : calendario) {
        std::cout << Utilidades::fechaATexto(p.fecha)
        << " | Grupo " << p.grupo
        << " | Jornada " << p.jornada
        << " | "
        << equipos[p.equipo1Id].getPais()
        << " vs "
        << equipos[p.equipo2Id].getPais()
        << '\n';
    }
}

void Torneo::simularFaseGrupos() {
    for (auto& grupo : grupos) {
        medidor.sumar();

        for (auto& partido : grupo.getPartidos()) {
            medidor.sumar();
            partido.simular(equipos, generador, true);
        }
    }
}

std::array<std::array<FilaTablaGrupo, 4>, 12> Torneo::construirTablasGrupos() const {
    std::array<std::array<FilaTablaGrupo, 4>, 12> tablas{};

    for (int i = 0; i < 12; ++i) {
        tablas[i] = grupos[i].construirTabla(equipos);
    }

    return tablas;
}

void Torneo::imprimirTablasGrupos() const {
    for (const auto& grupo : grupos) {
        grupo.imprimirTabla(equipos);
        std::cout << '\n';
    }
}

std::vector<uint8_t> Torneo::obtenerPrimeros() const {
    std::vector<uint8_t> primeros;

    for (const auto& grupo : grupos) {
        auto tabla = grupo.construirTabla(equipos);
        primeros.push_back(tabla[0].equipoId);
    }

    return primeros;
}

std::vector<uint8_t> Torneo::obtenerSegundos() const {
    std::vector<uint8_t> segundos;

    for (const auto& grupo : grupos) {
        auto tabla = grupo.construirTabla(equipos);
        segundos.push_back(tabla[1].equipoId);
    }

    return segundos;
}

std::vector<uint8_t> Torneo::obtenerMejoresTerceros() const {
    std::vector<FilaTablaGrupo> terceros;

    for (const auto& grupo : grupos) {
        auto tabla = grupo.construirTabla(equipos);
        terceros.push_back(tabla[2]);
    }

    std::sort(terceros.begin(), terceros.end(), [](const FilaTablaGrupo& a, const FilaTablaGrupo& b) {
        if (a.puntos != b.puntos) return a.puntos > b.puntos;
        if (a.diferenciaGoles != b.diferenciaGoles) return a.diferenciaGoles > b.diferenciaGoles;
        return a.golesFavor > b.golesFavor;
    });

    std::vector<uint8_t> mejores;
    for (int i = 0; i < 8 && i < static_cast<int>(terceros.size()); ++i) {
        mejores.push_back(terceros[i].equipoId);
    }

    return mejores;
}

void Torneo::imprimirClasificadosR16() const {
    auto tablas = construirTablasGrupos();

    struct EquipoClasificadoSimple {
        uint8_t equipoId;
        char grupo;
        uint8_t posicion;
        uint8_t puntos;
        int16_t diferenciaGoles;
        uint8_t golesFavor;
    };

    std::vector<EquipoClasificadoSimple> primeros;
    std::vector<EquipoClasificadoSimple> segundos;
    std::vector<EquipoClasificadoSimple> terceros;

    for (int i = 0; i < 12; ++i) {
        char letraGrupo = static_cast<char>('A' + i);

        const auto& t1 = tablas[i][0];
        const auto& t2 = tablas[i][1];
        const auto& t3 = tablas[i][2];

        primeros.push_back({t1.equipoId, letraGrupo, 1, t1.puntos, t1.diferenciaGoles, t1.golesFavor});
        segundos.push_back({t2.equipoId, letraGrupo, 2, t2.puntos, t2.diferenciaGoles, t2.golesFavor});
        terceros.push_back({t3.equipoId, letraGrupo, 3, t3.puntos, t3.diferenciaGoles, t3.golesFavor});
    }

    std::sort(terceros.begin(), terceros.end(),
              [](const EquipoClasificadoSimple& a, const EquipoClasificadoSimple& b) {
                  if (a.puntos != b.puntos) return a.puntos > b.puntos;
                  if (a.diferenciaGoles != b.diferenciaGoles) return a.diferenciaGoles > b.diferenciaGoles;
                  return a.golesFavor > b.golesFavor;
              });

    std::cout << "===== CLASIFICADOS A R16 =====\n";

    std::cout << "\n--- Primeros de grupo ---\n";
    for (const auto& e : primeros) {
        std::cout << "Grupo " << e.grupo << ": " << equipos[e.equipoId].getPais()
        << " | Pts: " << static_cast<int>(e.puntos)
        << " | DG: " << e.diferenciaGoles
        << " | GF: " << static_cast<int>(e.golesFavor) << '\n';
    }

    std::cout << "\n--- Segundos de grupo ---\n";
    for (const auto& e : segundos) {
        std::cout << "Grupo " << e.grupo << ": " << equipos[e.equipoId].getPais()
        << " | Pts: " << static_cast<int>(e.puntos)
        << " | DG: " << e.diferenciaGoles
        << " | GF: " << static_cast<int>(e.golesFavor) << '\n';
    }

    std::cout << "\n--- Terceros ordenados ---\n";
    for (size_t i = 0; i < terceros.size(); ++i) {
        const auto& e = terceros[i];
        std::cout << i + 1 << ". Grupo " << e.grupo << ": " << equipos[e.equipoId].getPais()
                  << " | Pts: " << static_cast<int>(e.puntos)
                  << " | DG: " << e.diferenciaGoles
                  << " | GF: " << static_cast<int>(e.golesFavor);

        if (i < 8) {
            std::cout << "  <-- CLASIFICA";
        }
        std::cout << '\n';
    }
}

void Torneo::generarR16() {
    partidosR16.clear();

    auto tablas = construirTablasGrupos();

    std::vector<EquipoClasificado> primeros;
    std::vector<EquipoClasificado> segundos;
    std::vector<EquipoClasificado> terceros;

    primeros.reserve(12);
    segundos.reserve(12);
    terceros.reserve(12);

    for (int i = 0; i < 12; ++i) {
        char letraGrupo = static_cast<char>('A' + i);

        const auto& t1 = tablas[i][0];
        const auto& t2 = tablas[i][1];
        const auto& t3 = tablas[i][2];

        primeros.push_back({
            t1.equipoId, letraGrupo, 1, t1.puntos, t1.diferenciaGoles, t1.golesFavor,
            static_cast<uint32_t>(Utilidades::randomInt(1, 1000000, generador))
        });

        segundos.push_back({
            t2.equipoId, letraGrupo, 2, t2.puntos, t2.diferenciaGoles, t2.golesFavor,
            static_cast<uint32_t>(Utilidades::randomInt(1, 1000000, generador))
        });

        terceros.push_back({
            t3.equipoId, letraGrupo, 3, t3.puntos, t3.diferenciaGoles, t3.golesFavor,
            static_cast<uint32_t>(Utilidades::randomInt(1, 1000000, generador))
        });
    }

    std::sort(primeros.begin(), primeros.end(), mejorClasificado);
    std::sort(segundos.begin(), segundos.end(), mejorClasificado);
    std::sort(terceros.begin(), terceros.end(), mejorClasificado);

    terceros.resize(8);

    std::vector<EquipoClasificado> cabezasVsTerceros(primeros.begin(), primeros.begin() + 8);
    std::vector<EquipoClasificado> cabezasVsPeoresSegundos(primeros.begin() + 8, primeros.end());

    std::vector<EquipoClasificado> segundosRestantes(segundos.begin(), segundos.begin() + 8);
    std::vector<EquipoClasificado> peoresSegundos(segundos.begin() + 8, segundos.end());

    std::vector<std::pair<EquipoClasificado, EquipoClasificado>> cruces1;
    std::vector<std::pair<EquipoClasificado, EquipoClasificado>> cruces2;
    std::vector<std::pair<EquipoClasificado, EquipoClasificado>> cruces3;

    std::vector<bool> usadoTerceros(terceros.size(), false);
    std::function<bool(size_t)> asignarCabezasConTerceros = [&](size_t idx) -> bool {
        if (idx == cabezasVsTerceros.size()) {
            return true;
        }

        for (size_t j = 0; j < terceros.size(); ++j) {
            medidor.sumar();

            if (usadoTerceros[j]) continue;
            if (cabezasVsTerceros[idx].grupo == terceros[j].grupo) continue;

            usadoTerceros[j] = true;
            cruces1.push_back({cabezasVsTerceros[idx], terceros[j]});

            if (asignarCabezasConTerceros(idx + 1)) {
                return true;
            }

            cruces1.pop_back();
            usadoTerceros[j] = false;
        }

        return false;
    };

    if (!asignarCabezasConTerceros(0)) {
        throw std::runtime_error("No fue posible asignar cabezas de grupo contra terceros clasificados.");
    }

    std::vector<bool> usadoPeoresSegundos(peoresSegundos.size(), false);
    std::function<bool(size_t)> asignarCabezasConPeoresSegundos = [&](size_t idx) -> bool {
        if (idx == cabezasVsPeoresSegundos.size()) {
            return true;
        }

        for (size_t j = 0; j < peoresSegundos.size(); ++j) {
            medidor.sumar();

            if (usadoPeoresSegundos[j]) continue;
            if (cabezasVsPeoresSegundos[idx].grupo == peoresSegundos[j].grupo) continue;

            usadoPeoresSegundos[j] = true;
            cruces2.push_back({cabezasVsPeoresSegundos[idx], peoresSegundos[j]});

            if (asignarCabezasConPeoresSegundos(idx + 1)) {
                return true;
            }

            cruces2.pop_back();
            usadoPeoresSegundos[j] = false;
        }

        return false;
    };

    if (!asignarCabezasConPeoresSegundos(0)) {
        throw std::runtime_error("No fue posible asignar cabezas de grupo contra los peores segundos.");
    }

    std::vector<bool> usadoSegundos(segundosRestantes.size(), false);
    std::function<bool()> emparejarSegundos = [&]() -> bool {
        int primeroLibre = -1;

        for (size_t i = 0; i < segundosRestantes.size(); ++i) {
            medidor.sumar();
            if (!usadoSegundos[i]) {
                primeroLibre = static_cast<int>(i);
                break;
            }
        }

        if (primeroLibre == -1) {
            return true;
        }

        usadoSegundos[primeroLibre] = true;

        for (size_t j = primeroLibre + 1; j < segundosRestantes.size(); ++j) {
            medidor.sumar();

            if (usadoSegundos[j]) continue;
            if (segundosRestantes[primeroLibre].grupo == segundosRestantes[j].grupo) continue;

            usadoSegundos[j] = true;
            cruces3.push_back({segundosRestantes[primeroLibre], segundosRestantes[j]});

            if (emparejarSegundos()) {
                return true;
            }

            cruces3.pop_back();
            usadoSegundos[j] = false;
        }

        usadoSegundos[primeroLibre] = false;
        return false;
    };

    if (!emparejarSegundos()) {
        throw std::runtime_error("No fue posible emparejar los segundos puestos restantes.");
    }

    std::array<uint16_t, 3> arbitros{1, 2, 3};
    Fecha fechaR16{10, 7, 2026};

    auto agregarCruce = [&](const EquipoClasificado& a, const EquipoClasificado& b) {
        Partido p;
        p.configurar(fechaR16, 0, 0, arbitros, Etapa::R16, a.equipoId, b.equipoId);
        partidosR16.push_back(p);
    };

    for (const auto& par : cruces1) {
        medidor.sumar();
        agregarCruce(par.first, par.second);
    }

    for (const auto& par : cruces2) {
        medidor.sumar();
        agregarCruce(par.first, par.second);
    }

    for (const auto& par : cruces3) {
        medidor.sumar();
        agregarCruce(par.first, par.second);
    }

    validarR16();
}

void Torneo::imprimirR16() const {
    std::cout << "===== PARTIDOS CONFIGURADOS PARA R16 =====\n";

    for (size_t i = 0; i < partidosR16.size(); ++i) {
        const Partido& p = partidosR16[i];

        std::cout << "R16-" << i + 1 << ": "
                  << equipos[p.getEquipo1Id()].getPais()
                  << " vs "
                  << equipos[p.getEquipo2Id()].getPais()
                  << " | Fecha: " << Utilidades::fechaATexto(p.getFecha())
                  << " | Hora: 00:00"
                  << " | Sede: nombreSede\n";
    }
}

void Torneo::validarR16() const {
    if (partidosR16.size() != 16) {
        throw std::runtime_error("R16 no contiene exactamente 16 partidos.");
    }

    std::vector<int> frecuencia(equipos.size(), 0);

    for (const auto& partido : partidosR16) {
        uint8_t e1 = partido.getEquipo1Id();
        uint8_t e2 = partido.getEquipo2Id();

        if (e1 == e2) {
            throw std::runtime_error("Se encontro un partido de R16 con el mismo equipo repetido.");
        }

        frecuencia[e1]++;
        frecuencia[e2]++;
    }

    int totalEquiposUsados = 0;
    for (int f : frecuencia) {
        if (f > 0) {
            totalEquiposUsados++;
        }
        if (f > 1) {
            throw std::runtime_error("Un equipo aparece mas de una vez en los cruces de R16.");
        }
    }

    if (totalEquiposUsados != 32) {
        throw std::runtime_error("R16 no utiliza exactamente 32 equipos distintos.");
    }

    for (const auto& partido : partidosR16) {
        bool mismoGrupo = false;

        for (const auto& grupo : grupos) {
            bool tiene1 = false;
            bool tiene2 = false;

            for (uint8_t id : grupo.getIdsEquipos()) {
                if (id == partido.getEquipo1Id()) tiene1 = true;
                if (id == partido.getEquipo2Id()) tiene2 = true;
            }

            if (tiene1 && tiene2) {
                mismoGrupo = true;
                break;
            }
        }

        if (mismoGrupo) {
            throw std::runtime_error("Se encontro un cruce de R16 entre equipos del mismo grupo.");
        }
    }

    std::cout << "[VALIDACION] R16 configurado correctamente.\n";
}

void Torneo::simularR16() {
    for (auto& p : partidosR16) {
        medidor.sumar();
        p.simular(equipos, generador, false);
    }
}

void Torneo::generarR8() {
    partidosR8.clear();

    if (partidosR16.size() != 16) {
        throw std::runtime_error("No se puede generar R8 porque R16 no tiene exactamente 16 partidos.");
    }

    std::vector<uint8_t> ganadoresR16;
    ganadoresR16.reserve(16);

    for (const auto& partido : partidosR16) {
        medidor.sumar();

        if (!partido.fueJugado()) {
            throw std::runtime_error("No se puede generar R8 porque aun hay partidos de R16 sin simular.");
        }

        if (partido.getGanadorId() < 0) {
            throw std::runtime_error("Se encontro un partido de R16 sin ganador valido.");
        }

        ganadoresR16.push_back(static_cast<uint8_t>(partido.getGanadorId()));
    }

    std::array<uint16_t, 3> arbitros{1, 2, 3};
    Fecha fechaR8{10, 7, 2026};

    for (size_t i = 0; i < ganadoresR16.size(); i += 2) {
        medidor.sumar();

        Partido p;
        p.configurar(
            fechaR8,
            0,
            0,
            arbitros,
            Etapa::R8,
            ganadoresR16[i],
            ganadoresR16[i + 1]
            );

        partidosR8.push_back(p);
    }

    if (partidosR8.size() != 8) {
        throw std::runtime_error("La configuracion de R8 no produjo exactamente 8 partidos.");
    }
}

void Torneo::imprimirR8() const {
    std::cout << "===== PARTIDOS CONFIGURADOS PARA R8 =====\n";

    for (size_t i = 0; i < partidosR8.size(); ++i) {
        const Partido& p = partidosR8[i];

        std::cout << "R8-" << i + 1 << ": "
                  << equipos[p.getEquipo1Id()].getPais()
                  << " vs "
                  << equipos[p.getEquipo2Id()].getPais()
                  << " | Fecha: " << Utilidades::fechaATexto(p.getFecha())
                  << " | Hora: 00:00"
                  << " | Sede: nombreSede\n";
    }
}

void Torneo::simularR8() {
    for (auto& p : partidosR8) {
        medidor.sumar();
        p.simular(equipos, generador, false);
    }
}

void Torneo::generarQF() {
    partidosQF.clear();

    if (partidosR8.size() != 8) {
        throw std::runtime_error("No se puede generar QF porque R8 no tiene exactamente 8 partidos.");
    }

    std::vector<uint8_t> ganadoresR8;
    ganadoresR8.reserve(8);

    for (const auto& partido : partidosR8) {
        medidor.sumar();

        if (!partido.fueJugado()) {
            throw std::runtime_error("No se puede generar QF porque aun hay partidos de R8 sin simular.");
        }

        if (partido.getGanadorId() < 0) {
            throw std::runtime_error("Se encontro un partido de R8 sin ganador valido.");
        }

        ganadoresR8.push_back(static_cast<uint8_t>(partido.getGanadorId()));
    }

    std::array<uint16_t, 3> arbitros{1, 2, 3};
    Fecha fechaQF{10, 7, 2026};

    for (size_t i = 0; i < ganadoresR8.size(); i += 2) {
        medidor.sumar();

        Partido p;
        p.configurar(
            fechaQF,
            0,
            0,
            arbitros,
            Etapa::QF,
            ganadoresR8[i],
            ganadoresR8[i + 1]
            );

        partidosQF.push_back(p);
    }

    if (partidosQF.size() != 4) {
        throw std::runtime_error("La configuracion de QF no produjo exactamente 4 partidos.");
    }
}

void Torneo::imprimirQF() const {
    std::cout << "===== PARTIDOS CONFIGURADOS PARA QF =====\n";

    for (size_t i = 0; i < partidosQF.size(); ++i) {
        const Partido& p = partidosQF[i];

        std::cout << "QF-" << i + 1 << ": "
                  << equipos[p.getEquipo1Id()].getPais()
                  << " vs "
                  << equipos[p.getEquipo2Id()].getPais()
                  << " | Fecha: " << Utilidades::fechaATexto(p.getFecha())
                  << " | Hora: 00:00"
                  << " | Sede: nombreSede\n";
    }
}

void Torneo::simularQF() {
    for (auto& p : partidosQF) {
        medidor.sumar();
        p.simular(equipos, generador, false);
    }
}

void Torneo::generarSF() {
    partidosSF.clear();

    if (partidosQF.size() != 4) {
        throw std::runtime_error("No se puede generar SF porque QF no tiene exactamente 4 partidos.");
    }

    std::vector<uint8_t> ganadoresQF;
    ganadoresQF.reserve(4);

    for (const auto& partido : partidosQF) {
        medidor.sumar();

        if (!partido.fueJugado()) {
            throw std::runtime_error("No se puede generar SF porque aun hay partidos de QF sin simular.");
        }

        if (partido.getGanadorId() < 0) {
            throw std::runtime_error("Se encontro un partido de QF sin ganador valido.");
        }

        ganadoresQF.push_back(static_cast<uint8_t>(partido.getGanadorId()));
    }

    std::array<uint16_t, 3> arbitros{1, 2, 3};
    Fecha fechaSF{10, 7, 2026};

    for (size_t i = 0; i < ganadoresQF.size(); i += 2) {
        medidor.sumar();

        Partido p;
        p.configurar(
            fechaSF,
            0,
            0,
            arbitros,
            Etapa::SF,
            ganadoresQF[i],
            ganadoresQF[i + 1]
            );

        partidosSF.push_back(p);
    }

    if (partidosSF.size() != 2) {
        throw std::runtime_error("La configuracion de SF no produjo exactamente 2 partidos.");
    }
}

void Torneo::imprimirSF() const {
    std::cout << "===== PARTIDOS CONFIGURADOS PARA SF =====\n";

    for (size_t i = 0; i < partidosSF.size(); ++i) {
        const Partido& p = partidosSF[i];

        std::cout << "SF-" << i + 1 << ": "
                  << equipos[p.getEquipo1Id()].getPais()
                  << " vs "
                  << equipos[p.getEquipo2Id()].getPais()
                  << " | Fecha: " << Utilidades::fechaATexto(p.getFecha())
                  << " | Hora: 00:00"
                  << " | Sede: nombreSede\n";
    }
}

void Torneo::simularSF() {
    for (auto& p : partidosSF) {
        medidor.sumar();
        p.simular(equipos, generador, false);
    }
}

void Torneo::generarFinalYTercerPuesto() {
    if (partidosSF.size() != 2) {
        throw std::runtime_error("No se puede generar final y tercer puesto porque SF no tiene exactamente 2 partidos.");
    }

    for (const auto& partido : partidosSF) {
        medidor.sumar();

        if (!partido.fueJugado()) {
            throw std::runtime_error("No se puede generar final y tercer puesto porque aun hay semifinales sin simular.");
        }

        if (partido.getGanadorId() < 0) {
            throw std::runtime_error("Se encontro una semifinal sin ganador valido.");
        }
    }

    uint8_t ganador1 = static_cast<uint8_t>(partidosSF[0].getGanadorId());
    uint8_t ganador2 = static_cast<uint8_t>(partidosSF[1].getGanadorId());

    uint8_t perdedor1 = (partidosSF[0].getEquipo1Id() == ganador1)
                            ? partidosSF[0].getEquipo2Id()
                            : partidosSF[0].getEquipo1Id();

    uint8_t perdedor2 = (partidosSF[1].getEquipo1Id() == ganador2)
                            ? partidosSF[1].getEquipo2Id()
                            : partidosSF[1].getEquipo1Id();

    std::array<uint16_t, 3> arbitros{1, 2, 3};
    Fecha fechaFinal{10, 7, 2026};

    partidoFinal = Partido();
    partidoFinal.configurar(
        fechaFinal,
        0,
        0,
        arbitros,
        Etapa::FINAL,
        ganador1,
        ganador2
        );

    partidoTercerPuesto = Partido();
    partidoTercerPuesto.configurar(
        fechaFinal,
        0,
        0,
        arbitros,
        Etapa::TERCER_PUESTO,
        perdedor1,
        perdedor2
        );
}

void Torneo::imprimirFinalYTercerPuesto() const {
    std::cout << "===== PARTIDO POR EL TERCER PUESTO =====\n";
    std::cout << equipos[partidoTercerPuesto.getEquipo1Id()].getPais()
              << " vs "
              << equipos[partidoTercerPuesto.getEquipo2Id()].getPais()
              << " | Fecha: " << Utilidades::fechaATexto(partidoTercerPuesto.getFecha())
              << " | Hora: 00:00"
              << " | Sede: nombreSede\n";

    std::cout << "===== FINAL =====\n";
    std::cout << equipos[partidoFinal.getEquipo1Id()].getPais()
              << " vs "
              << equipos[partidoFinal.getEquipo2Id()].getPais()
              << " | Fecha: " << Utilidades::fechaATexto(partidoFinal.getFecha())
              << " | Hora: 00:00"
              << " | Sede: nombreSede\n";
}

void Torneo::simularFinalYTercerPuesto() {
    medidor.sumar();
    partidoTercerPuesto.simular(equipos, generador, false);

    medidor.sumar();
    partidoFinal.simular(equipos, generador, false);
}

void Torneo::imprimirDetalleFaseGrupos() const {
    std::cout << "===== DETALLE DE PARTIDOS - FASE DE GRUPOS =====\n";

    for (const auto& grupo : grupos) {
        for (const auto& partido : grupo.getPartidos()) {
            if (partido.fueJugado()) {
                partido.imprimirDetalle(equipos);
            }
        }
    }
}

void Torneo::imprimirDetalleR16() const {
    std::cout << "===== DETALLE DE PARTIDOS - R16 =====\n";

    for (const auto& partido : partidosR16) {
        if (partido.fueJugado()) {
            partido.imprimirDetalle(equipos);
        }
    }
}

void Torneo::imprimirDetalleR8() const {
    std::cout << "===== DETALLE DE PARTIDOS - R8 =====\n";

    for (const auto& partido : partidosR8) {
        if (partido.fueJugado()) {
            partido.imprimirDetalle(equipos);
        }
    }
}

void Torneo::imprimirDetalleQF() const {
    std::cout << "===== DETALLE DE PARTIDOS - QF =====\n";

    for (const auto& partido : partidosQF) {
        if (partido.fueJugado()) {
            partido.imprimirDetalle(equipos);
        }
    }
}

void Torneo::imprimirDetalleSF() const {
    std::cout << "===== DETALLE DE PARTIDOS - SF =====\n";

    for (const auto& partido : partidosSF) {
        if (partido.fueJugado()) {
            partido.imprimirDetalle(equipos);
        }
    }
}

void Torneo::imprimirDetalleFinalYTercerPuesto() const {
    std::cout << "===== DETALLE DE PARTIDO - TERCER PUESTO =====\n";
    if (partidoTercerPuesto.fueJugado()) {
        partidoTercerPuesto.imprimirDetalle(equipos);
    }

    std::cout << "===== DETALLE DE PARTIDO - FINAL =====\n";
    if (partidoFinal.fueJugado()) {
        partidoFinal.imprimirDetalle(equipos);
    }
}

void Torneo::generarReporteFinal() const {
    std::cout << "\n=============================================\n";
    std::cout << "         REPORTE FINAL DEL TORNEO\n";
    std::cout << "=============================================\n";

    int16_t campeonId = partidoFinal.getGanadorId();
    int16_t tercerId = partidoTercerPuesto.getGanadorId();

    int16_t subcampeonId = -1;
    if (campeonId >= 0) {
        subcampeonId = (partidoFinal.getEquipo1Id() == campeonId)
        ? partidoFinal.getEquipo2Id()
        : partidoFinal.getEquipo1Id();
    }

    int16_t cuartoId = -1;
    if (tercerId >= 0) {
        cuartoId = (partidoTercerPuesto.getEquipo1Id() == tercerId)
        ? partidoTercerPuesto.getEquipo2Id()
        : partidoTercerPuesto.getEquipo1Id();
    }

    std::cout << "\n--- Ranking de los 4 primeros puestos ---\n";
    if (campeonId >= 0) std::cout << "1. " << equipos[campeonId].getPais() << '\n';
    if (subcampeonId >= 0) std::cout << "2. " << equipos[subcampeonId].getPais() << '\n';
    if (tercerId >= 0) std::cout << "3. " << equipos[tercerId].getPais() << '\n';
    if (cuartoId >= 0) std::cout << "4. " << equipos[cuartoId].getPais() << '\n';

    std::cout << "\n--- Maximo goleador del equipo campeon en esta copa ---\n";

    if (campeonId >= 0) {
        const Equipo& campeon = equipos[campeonId];
        const auto& jugadoresCampeon = campeon.getJugadores();

        const Jugador* goleadorCampeon = nullptr;

        for (const auto& jugador : jugadoresCampeon) {
            if (!goleadorCampeon || jugador.getGolesCopaActual() > goleadorCampeon->getGolesCopaActual()) {
                goleadorCampeon = &jugador;
            }
        }

        if (goleadorCampeon) {
            std::cout << "Equipo campeon: " << campeon.getPais() << '\n';
            std::cout << "Jugador: "
                      << goleadorCampeon->getNombre() << ' '
                      << goleadorCampeon->getApellido()
                      << " | Camiseta: " << static_cast<int>(goleadorCampeon->getNumeroCamiseta())
                      << " | Goles en esta copa: " << goleadorCampeon->getGolesCopaActual()
                      << '\n';
        }
    } else {
        std::cout << "No se pudo determinar el campeon.\n";
    }

    struct RegistroGoleador {
        std::string pais;
        const Jugador* jugador;
    };

    std::vector<RegistroGoleador> goleadores;
    goleadores.reserve(equipos.size() * 26);

    for (const auto& equipo : equipos) {
        for (const auto& jugador : equipo.getJugadores()) {
            goleadores.push_back({equipo.getPais(), &jugador});
        }
    }

    std::sort(goleadores.begin(), goleadores.end(),
              [](const RegistroGoleador& a, const RegistroGoleador& b) {
                  if (a.jugador->getGolesCopaActual() != b.jugador->getGolesCopaActual()) {
                      return a.jugador->getGolesCopaActual() > b.jugador->getGolesCopaActual();
                  }
                  if (a.pais != b.pais) {
                      return a.pais < b.pais;
                  }
                  return a.jugador->getNumeroCamiseta() < b.jugador->getNumeroCamiseta();
              });

    std::cout << "\n--- Tres mayores goleadores de esta copa ---\n";
    for (size_t i = 0; i < 3 && i < goleadores.size(); ++i) {
        const auto& reg = goleadores[i];
        std::cout << i + 1 << ". "
                  << reg.jugador->getNombre() << ' ' << reg.jugador->getApellido()
                  << " | Equipo: " << reg.pais
                  << " | Camiseta: " << static_cast<int>(reg.jugador->getNumeroCamiseta())
                  << " | Goles en esta copa: " << reg.jugador->getGolesCopaActual()
                  << '\n';
    }

    std::cout << "\n--- Equipo con mas goles historicos actualizados ---\n";

    const Equipo* maxEquipo = nullptr;
    for (const auto& e : equipos) {
        if (!maxEquipo || e.getGolesFavorHistoricos() > maxEquipo->getGolesFavorHistoricos()) {
            maxEquipo = &e;
        }
    }

    if (maxEquipo) {
        std::cout << maxEquipo->getPais()
        << " con " << maxEquipo->getGolesFavorHistoricos()
        << " goles historicos acumulados\n";
    }

    auto contarConfederaciones = [this](const std::vector<uint8_t>& idsEquiposEtapa,
                                        const std::string& nombreEtapa) {
        int conteoUEFA = 0;
        int conteoCONMEBOL = 0;
        int conteoCONCACAF = 0;
        int conteoCAF = 0;
        int conteoAFC = 0;
        int conteoOFC = 0;
        int conteoDESCONOCIDA = 0;

        for (uint8_t id : idsEquiposEtapa) {
            switch (equipos[id].getConfederacion()) {
            case Confederacion::UEFA: ++conteoUEFA; break;
            case Confederacion::CONMEBOL: ++conteoCONMEBOL; break;
            case Confederacion::CONCACAF: ++conteoCONCACAF; break;
            case Confederacion::CAF: ++conteoCAF; break;
            case Confederacion::AFC: ++conteoAFC; break;
            case Confederacion::OFC: ++conteoOFC; break;
            default: ++conteoDESCONOCIDA; break;
            }
        }

        std::vector<std::pair<std::string, int>> datos = {
            {"UEFA", conteoUEFA},
            {"CONMEBOL", conteoCONMEBOL},
            {"CONCACAF", conteoCONCACAF},
            {"CAF", conteoCAF},
            {"AFC", conteoAFC},
            {"OFC", conteoOFC},
            {"DESCONOCIDA", conteoDESCONOCIDA}
        };

        std::sort(datos.begin(), datos.end(),
                  [](const auto& a, const auto& b) {
                      if (a.second != b.second) return a.second > b.second;
                      return a.first < b.first;
                  });

        std::cout << nombreEtapa << " -> " << datos[0].first
                  << " con " << datos[0].second << " equipos\n";
    };

    std::vector<uint8_t> equiposR16;
    equiposR16.reserve(32);
    for (const auto& p : partidosR16) {
        equiposR16.push_back(p.getEquipo1Id());
        equiposR16.push_back(p.getEquipo2Id());
    }

    std::vector<uint8_t> equiposR8;
    equiposR8.reserve(16);
    for (const auto& p : partidosR8) {
        equiposR8.push_back(p.getEquipo1Id());
        equiposR8.push_back(p.getEquipo2Id());
    }

    std::vector<uint8_t> equiposR4;
    equiposR4.reserve(4);
    for (const auto& p : partidosSF) {
        equiposR4.push_back(p.getEquipo1Id());
        equiposR4.push_back(p.getEquipo2Id());
    }

    std::cout << "\n--- Confederacion con mayor presencia ---\n";
    contarConfederaciones(equiposR16, "R16");
    contarConfederaciones(equiposR8, "R8");
    contarConfederaciones(equiposR4, "R4");

    std::cout << "\n=============================================\n";
}

void Torneo::guardarEquiposActualizadosCSV(const std::string& rutaArchivo) const {
    std::ofstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo crear el archivo de equipos actualizados: " + rutaArchivo);
    }

    archivo << "id;pais;director_tecnico;federacion;confederacion;ranking_fifa;gf_historicos;gc_historicos;pg;pe;pp;ta;tr;faltas\n";

    for (const auto& equipo : equipos) {
        archivo << static_cast<int>(equipo.getId()) << ';'
                << equipo.getPais() << ';'
                << equipo.getDirectorTecnico() << ';'
                << equipo.getFederacion() << ';'
                << Utilidades::confederacionATexto(equipo.getConfederacion()) << ';'
                << equipo.getRankingFIFA() << ';'
                << equipo.getGolesFavorHistoricos() << ';'
                << equipo.getGolesContraHistoricos() << ';'
                << equipo.getPartidosGanadosHistoricos() << ';'
                << equipo.getPartidosEmpatadosHistoricos() << ';'
                << equipo.getPartidosPerdidosHistoricos() << ';'
                << equipo.getTarjetasAmarillasHistoricas() << ';'
                << equipo.getTarjetasRojasHistoricas() << ';'
                << equipo.getFaltasHistoricas() << '\n';
    }
}

void Torneo::guardarJugadoresActualizadosCSV(const std::string& rutaArchivo) const {
    std::ofstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo crear el archivo de jugadores actualizados: " + rutaArchivo);
    }

    archivo << "equipo_id;equipo_pais;camiseta;nombre;apellido;"
            << "partidos_historicos;goles_historicos;minutos_historicos;asistencias_historicas;ta_historicas;tr_historicas;faltas_historicas;"
            << "partidos_copa_actual;goles_copa_actual;minutos_copa_actual;asistencias_copa_actual;ta_copa_actual;tr_copa_actual;faltas_copa_actual\n";

    for (const auto& equipo : equipos) {
        for (const auto& jugador : equipo.getJugadores()) {
            archivo << static_cast<int>(equipo.getId()) << ';'
                    << equipo.getPais() << ';'
                    << static_cast<int>(jugador.getNumeroCamiseta()) << ';'
                    << jugador.getNombre() << ';'
                    << jugador.getApellido() << ';'
                    << jugador.getPartidosJugados() << ';'
                    << jugador.getGoles() << ';'
                    << jugador.getMinutosJugados() << ';'
                    << jugador.getAsistencias() << ';'
                    << jugador.getTarjetasAmarillas() << ';'
                    << jugador.getTarjetasRojas() << ';'
                    << jugador.getFaltas() << ';'
                    << jugador.getPartidosCopaActual() << ';'
                    << jugador.getGolesCopaActual() << ';'
                    << jugador.getMinutosCopaActual() << ';'
                    << jugador.getAsistenciasCopaActual() << ';'
                    << jugador.getTarjetasAmarillasCopaActual() << ';'
                    << jugador.getTarjetasRojasCopaActual() << ';'
                    << jugador.getFaltasCopaActual() << '\n';
        }
    }
}

void Torneo::guardarDatosActualizados() const {
    guardarEquiposActualizadosCSV("equipos_actualizados.csv");
    guardarJugadoresActualizadosCSV("jugadores_actualizados.csv");

    std::cout << "[PERSISTENCIA] Se generaron los archivos:\n";
    std::cout << " - equipos_actualizados.csv\n";
    std::cout << " - jugadores_actualizados.csv\n";
}

void Torneo::medirEstadoSistema(const std::string& nombreFase) const {
    std::cout << "[RECURSOS] " << nombreFase
              << " | Iteraciones: " << medidor.obtenerIteraciones()
              << " | Memoria estimada: " << medidor.estimarMemoriaTorneo(*this)
              << " bytes\n";
}

void Torneo::reiniciarIteraciones() {
    medidor.reiniciar();
}

void Torneo::sumarIteraciones(uint64_t cantidad) {
    medidor.sumar(cantidad);
}

uint64_t Torneo::obtenerIteraciones() const {
    return medidor.obtenerIteraciones();
}

const std::vector<Equipo>& Torneo::getEquipos() const {
    return equipos;
}

const std::array<Grupo, 12>& Torneo::getGrupos() const {
    return grupos;
}
