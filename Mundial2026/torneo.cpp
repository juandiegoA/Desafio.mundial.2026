#include "Torneo.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>

#include "Utilidades.h"

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

    // Saltar primera línea: título
    std::getline(archivo, linea);

    // Saltar segunda línea: encabezados
    std::getline(archivo, linea);

    uint8_t id = 0;

    while (std::getline(archivo, linea)) {
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
        bombo1.push_back(idsResto[idx++]);
    }

    while (bombo2.size() < 12 && idx < static_cast<int>(idsResto.size())) {
        bombo2.push_back(idsResto[idx++]);
    }

    while (bombo3.size() < 12 && idx < static_cast<int>(idsResto.size())) {
        bombo3.push_back(idsResto[idx++]);
    }

    while (bombo4.size() < 12 && idx < static_cast<int>(idsResto.size())) {
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

    // Primero todos los del bombo 1, luego todos los del 2, etc.
    for (int b = 0; b < 4; ++b) {
        for (uint8_t idEquipo : bombos[b]) {
            ordenAsignacion.push_back(idEquipo);
        }
    }

    std::function<bool(size_t)> backtrack = [&](size_t indice) -> bool {
        if (indice == ordenAsignacion.size()) {
            return true;
        }

        uint8_t idEquipo = ordenAsignacion[indice];
        const Equipo& equipoActual = equipos[idEquipo];

        // Qué "ronda" de llenado vamos
        // 0 -> primer equipo de cada grupo (bombo 1)
        // 1 -> segundo equipo de cada grupo (bombo 2)
        // 2 -> tercero (bombo 3)
        // 3 -> cuarto (bombo 4)
        int ronda = static_cast<int>(indice / 12);

        for (int g = 0; g < 12; ++g) {
            // Solo se puede agregar a grupos que estén exactamente
            // en la ronda actual
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

void Torneo::generarCalendarioGrupos() {
    for (auto& grupo : grupos) {
        grupo.generarPartidos();
    }
}

void Torneo::simularFaseGrupos() {
    for (auto& grupo : grupos) {
        for (auto& partido : grupo.getPartidos()) {
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

void Torneo::generarR16() {
    partidosR16.clear();

    auto primeros = obtenerPrimeros();
    auto segundos = obtenerSegundos();
    auto terceros = obtenerMejoresTerceros();

    std::array<uint16_t, 3> arbitros{1, 2, 3};
    Fecha f{10, 7, 2026};

    int n = 0;

    for (size_t i = 0; i < primeros.size() && i < terceros.size(); ++i) {
        Partido p;
        p.configurar(f, 0, 0, arbitros, Etapa::R16, primeros[i], terceros[i]);
        partidosR16.push_back(p);
        ++n;
        if (n == 8) break;
    }

    for (size_t i = 0; i + 1 < segundos.size() && n < 16; i += 2) {
        Partido p;
        p.configurar(f, 0, 0, arbitros, Etapa::R16, segundos[i], segundos[i + 1]);
        partidosR16.push_back(p);
        ++n;
    }
}

void Torneo::simularR16() {
    for (auto& p : partidosR16) {
        p.simular(equipos, generador, false);
    }
}

void Torneo::generarReporteFinal() const {
    std::cout << "===== REPORTE FINAL =====\n";

    const Equipo* maxEquipo = nullptr;

    for (const auto& e : equipos) {
        if (!maxEquipo || e.getGolesFavorHistoricos() > maxEquipo->getGolesFavorHistoricos()) {
            maxEquipo = &e;
        }
    }

    if (maxEquipo) {
        std::cout << "Equipo con mas goles historicos: "
                  << maxEquipo->getPais()
                  << " (" << maxEquipo->getGolesFavorHistoricos() << ")\n";
    }
}

void Torneo::medirEstadoSistema(const std::string& nombreFase) const {
    std::cout << "[RECURSOS] " << nombreFase
              << " | Iteraciones: " << medidor.obtenerIteraciones()
              << " | Memoria estimada: " << medidor.estimarMemoriaTorneo(*this)
              << " bytes\n";
}

const std::vector<Equipo>& Torneo::getEquipos() const {
    return equipos;
}

const std::array<Grupo, 12>& Torneo::getGrupos() const {
    return grupos;
}
