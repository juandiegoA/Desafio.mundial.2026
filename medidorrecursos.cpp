#include "MedidorRecursos.h"

#include "Equipo.h"
#include "Grupo.h"
#include "Jugador.h"
#include "Partido.h"
#include "Torneo.h"

MedidorRecursos::MedidorRecursos() : iteraciones(0) {}

void MedidorRecursos::reiniciar() {
    iteraciones = 0;
}

void MedidorRecursos::sumar(uint64_t cantidad) {
    iteraciones += cantidad;
}

uint64_t MedidorRecursos::obtenerIteraciones() const {
    return iteraciones;
}

size_t MedidorRecursos::estimarMemoriaJugador(const Jugador& jugador) const {
    return sizeof(jugador)
    + jugador.getNombre().capacity()
        + jugador.getApellido().capacity();
}

size_t MedidorRecursos::estimarMemoriaEquipo(const Equipo& equipo) const {
    size_t total = sizeof(equipo)
    + equipo.getPais().capacity()
        + equipo.getDirectorTecnico().capacity()
        + equipo.getFederacion().capacity();

    for (const auto& j : equipo.getJugadores()) {
        total += estimarMemoriaJugador(j);
    }
    return total;
}

size_t MedidorRecursos::estimarMemoriaPartido(const Partido& partido) const {
    (void)partido;
    return sizeof(Partido);
}

size_t MedidorRecursos::estimarMemoriaGrupo(const Grupo& grupo) const {
    (void)grupo;
    return sizeof(Grupo);
}

size_t MedidorRecursos::estimarMemoriaTorneo(const Torneo& torneo) const {
    size_t total = sizeof(Torneo);
    for (const auto& equipo : torneo.getEquipos()) {
        total += estimarMemoriaEquipo(equipo);
    }
    return total;
}
