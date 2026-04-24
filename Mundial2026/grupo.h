#ifndef GRUPO_H
#define GRUPO_H

#include <array>
#include <vector>

#include "Partido.h"

class Grupo {
private:
    char letra;
    std::array<uint8_t, 4> idsEquipos;
    uint8_t cantidadEquipos;

    std::array<Partido, 6> partidos;
    uint8_t cantidadPartidos;

public:
    Grupo();
    Grupo(char letra);

    char getLetra() const;
    const std::array<uint8_t, 4>& getIdsEquipos() const;
    uint8_t getCantidadEquipos() const;

    const std::array<Partido, 6>& getPartidos() const;
    std::array<Partido, 6>& getPartidos();

    bool puedeAgregarEquipo(const Equipo& equipo, const std::vector<Equipo>& todos) const;
    bool agregarEquipo(uint8_t equipoId);
    void quitarUltimoEquipo();

    void generarPartidos();
    std::array<FilaTablaGrupo, 4> construirTabla(const std::vector<Equipo>& equipos) const;

    void imprimirGrupo(const std::vector<Equipo>& equipos) const;
    void imprimirTabla(const std::vector<Equipo>& equipos) const;
};

#endif
