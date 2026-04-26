#include "jugador.h"

Jugador::Jugador()
    : numeroCamiseta(0),
    nombre(""),
    apellido(""),
    partidosJugados(0),
    goles(0),
    minutosJugados(0),
    asistencias(0),
    tarjetasAmarillas(0),
    tarjetasRojas(0),
    faltas(0),
    partidosCopaActual(0),
    golesCopaActual(0),
    minutosCopaActual(0),
    asistenciasCopaActual(0),
    tarjetasAmarillasCopaActual(0),
    tarjetasRojasCopaActual(0),
    faltasCopaActual(0) {}

Jugador::Jugador(uint8_t numeroCamiseta, const std::string& nombre, const std::string& apellido)
    : numeroCamiseta(numeroCamiseta),
    nombre(nombre),
    apellido(apellido),
    partidosJugados(0),
    goles(0),
    minutosJugados(0),
    asistencias(0),
    tarjetasAmarillas(0),
    tarjetasRojas(0),
    faltas(0),
    partidosCopaActual(0),
    golesCopaActual(0),
    minutosCopaActual(0),
    asistenciasCopaActual(0),
    tarjetasAmarillasCopaActual(0),
    tarjetasRojasCopaActual(0),
    faltasCopaActual(0) {}

uint8_t Jugador::getNumeroCamiseta() const {
    return numeroCamiseta;
}

const std::string& Jugador::getNombre() const {
    return nombre;
}

const std::string& Jugador::getApellido() const {
    return apellido;
}

uint16_t Jugador::getPartidosJugados() const {
    return partidosJugados;
}

uint16_t Jugador::getGoles() const {
    return goles;
}

uint32_t Jugador::getMinutosJugados() const {
    return minutosJugados;
}

uint16_t Jugador::getAsistencias() const {
    return asistencias;
}

uint16_t Jugador::getTarjetasAmarillas() const {
    return tarjetasAmarillas;
}

uint16_t Jugador::getTarjetasRojas() const {
    return tarjetasRojas;
}

uint16_t Jugador::getFaltas() const {
    return faltas;
}

uint16_t Jugador::getPartidosCopaActual() const {
    return partidosCopaActual;
}

uint16_t Jugador::getGolesCopaActual() const {
    return golesCopaActual;
}

uint32_t Jugador::getMinutosCopaActual() const {
    return minutosCopaActual;
}

uint16_t Jugador::getAsistenciasCopaActual() const {
    return asistenciasCopaActual;
}

uint16_t Jugador::getTarjetasAmarillasCopaActual() const {
    return tarjetasAmarillasCopaActual;
}

uint16_t Jugador::getTarjetasRojasCopaActual() const {
    return tarjetasRojasCopaActual;
}

uint16_t Jugador::getFaltasCopaActual() const {
    return faltasCopaActual;
}

void Jugador::sumarGoles(uint8_t cantidad) {
    goles += cantidad;
    golesCopaActual += cantidad;
}

void Jugador::actualizarEstadisticas(uint8_t golesPartido,
                                     uint8_t minutosPartido,
                                     uint8_t amarillasPartido,
                                     uint8_t rojasPartido,
                                     uint8_t faltasPartido,
                                     uint8_t asistenciasPartido) {
    // Historico total
    ++partidosJugados;
    goles += golesPartido;
    minutosJugados += minutosPartido;
    asistencias += asistenciasPartido;
    tarjetasAmarillas += amarillasPartido;
    tarjetasRojas += rojasPartido;
    faltas += faltasPartido;

    // Copa actual
    ++partidosCopaActual;
    golesCopaActual += golesPartido;
    minutosCopaActual += minutosPartido;
    asistenciasCopaActual += asistenciasPartido;
    tarjetasAmarillasCopaActual += amarillasPartido;
    tarjetasRojasCopaActual += rojasPartido;
    faltasCopaActual += faltasPartido;
}

void Jugador::imprimir() const {
    std::cout << *this << '\n';
}

std::ostream& operator<<(std::ostream& os, const Jugador& jugador) {
    os << "Camiseta: " << static_cast<int>(jugador.getNumeroCamiseta())
    << " | Nombre: " << jugador.getNombre() << ' ' << jugador.getApellido()
    << " | PJ historicos: " << jugador.getPartidosJugados()
    << " | Goles historicos: " << jugador.getGoles()
    << " | Min historicos: " << jugador.getMinutosJugados()
    << " | Goles copa actual: " << jugador.getGolesCopaActual()
    << " | Min copa actual: " << jugador.getMinutosCopaActual();

    return os;
}
