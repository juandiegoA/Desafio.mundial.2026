#ifndef MEDIDOR_RECURSOS_H
#define MEDIDOR_RECURSOS_H

#include <cstddef>
#include <cstdint>

class Jugador;
class Equipo;
class Partido;
class Grupo;
class Torneo;

class MedidorRecursos {
private:
    uint64_t iteraciones;

public:
    MedidorRecursos();

    void reiniciar();
    void sumar(uint64_t cantidad = 1);
    uint64_t obtenerIteraciones() const;

    size_t estimarMemoriaJugador(const Jugador& jugador) const;
    size_t estimarMemoriaEquipo(const Equipo& equipo) const;
    size_t estimarMemoriaPartido(const Partido& partido) const;
    size_t estimarMemoriaGrupo(const Grupo& grupo) const;
    size_t estimarMemoriaTorneo(const Torneo& torneo) const;
};

#endif
