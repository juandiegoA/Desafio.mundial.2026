#ifndef TORNEO_H
#define TORNEO_H

#include <array>
#include <random>
#include <string>
#include <vector>

#include "Grupo.h"
#include "MedidorRecursos.h"

class Torneo {
private:
    std::vector<Equipo> equipos;
    std::array<Grupo, 12> grupos;

    std::vector<uint8_t> bombo1;
    std::vector<uint8_t> bombo2;
    std::vector<uint8_t> bombo3;
    std::vector<uint8_t> bombo4;

    std::vector<Partido> partidosR16;
    std::vector<Partido> partidosR8;
    std::vector<Partido> partidosQF;
    std::vector<Partido> partidosSF;

    Partido partidoTercerPuesto;
    Partido partidoFinal;

    MedidorRecursos medidor;
    std::mt19937 generador;

public:
    Torneo();

    void cargarEquiposCSV(const std::string& archivoCSV);
    void generarJugadoresIniciales();

    void crearBombos();
    void imprimirBombos() const;

    void conformarGrupos();
    void imprimirGrupos() const;

    void generarCalendarioGrupos();
    void simularFaseGrupos();

    std::array<std::array<FilaTablaGrupo, 4>, 12> construirTablasGrupos() const;
    void imprimirTablasGrupos() const;

    std::vector<uint8_t> obtenerPrimeros() const;
    std::vector<uint8_t> obtenerSegundos() const;
    std::vector<uint8_t> obtenerMejoresTerceros() const;

    void generarR16();
    void imprimirR16() const;
    void simularR16();

    void generarR8();
    void imprimirR8() const;
    void simularR8();

    void generarQF();
    void imprimirQF() const;
    void simularQF();

    void generarSF();
    void imprimirSF() const;
    void simularSF();

    void generarFinalYTercerPuesto();
    void imprimirFinalYTercerPuesto() const;
    void simularFinalYTercerPuesto();

    void generarReporteFinal() const;
    void medirEstadoSistema(const std::string& nombreFase) const;

    void reiniciarIteraciones();
    void sumarIteraciones(uint64_t cantidad = 1);
    uint64_t obtenerIteraciones() const;

    const std::vector<Equipo>& getEquipos() const;
    const std::array<Grupo, 12>& getGrupos() const;
};

#endif
