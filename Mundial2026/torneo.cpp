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
    uint8_t posicion;   // 1, 2 o 3
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

void Torneo::generarCalendarioGrupos() {
    for (auto& grupo : grupos) {
        medidor.sumar();
        grupo.generarPartidos();
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
