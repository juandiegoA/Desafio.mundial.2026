#include "Utilidades.h"

#include <iomanip>
#include <sstream>

namespace Utilidades {

std::vector<std::string> dividirCSV(const std::string& linea) {
    std::vector<std::string> partes;
    std::string actual;
    bool dentroComillas = false;

    for (char c : linea) {
        if (c == '"') {
            dentroComillas = !dentroComillas;
        } else if (c == ',' && !dentroComillas) {
            partes.push_back(actual);
            actual.clear();
        } else {
            actual.push_back(c);
        }
    }
    partes.push_back(actual);
    return partes;
}

Confederacion convertirConfederacion(const std::string& texto) {
    if (texto == "UEFA") return Confederacion::UEFA;
    if (texto == "CONMEBOL") return Confederacion::CONMEBOL;
    if (texto == "CONCACAF") return Confederacion::CONCACAF;
    if (texto == "CAF") return Confederacion::CAF;
    if (texto == "AFC") return Confederacion::AFC;
    if (texto == "OFC") return Confederacion::OFC;
    return Confederacion::DESCONOCIDA;
}

std::string confederacionATexto(Confederacion c) {
    switch (c) {
    case Confederacion::UEFA: return "UEFA";
    case Confederacion::CONMEBOL: return "CONMEBOL";
    case Confederacion::CONCACAF: return "CONCACAF";
    case Confederacion::CAF: return "CAF";
    case Confederacion::AFC: return "AFC";
    case Confederacion::OFC: return "OFC";
    default: return "DESCONOCIDA";
    }
}

std::string etapaATexto(Etapa e) {
    switch (e) {
    case Etapa::GRUPOS: return "GRUPOS";
    case Etapa::R16: return "R16";
    case Etapa::R8: return "R8";
    case Etapa::QF: return "QF";
    case Etapa::SF: return "SF";
    case Etapa::TERCER_PUESTO: return "TERCER PUESTO";
    case Etapa::FINAL: return "FINAL";
    default: return "DESCONOCIDA";
    }
}

std::string fechaATexto(const Fecha& fecha) {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << static_cast<int>(fecha.dia) << "/"
        << std::setw(2) << static_cast<int>(fecha.mes) << "/"
        << fecha.anio;
    return oss.str();
}

int diasDesdeReferencia(const Fecha& fecha) {
    return static_cast<int>(fecha.anio) * 365 + static_cast<int>(fecha.mes) * 30 + static_cast<int>(fecha.dia);
}

Fecha sumarDias(const Fecha& fecha, int dias) {
    int total = diasDesdeReferencia(fecha) + dias;
    Fecha nueva{};
    nueva.anio = total / 365;
    total %= 365;
    nueva.mes = total / 30;
    total %= 30;
    nueva.dia = total;

    if (nueva.dia == 0) nueva.dia = 1;
    if (nueva.mes == 0) nueva.mes = 1;
    return nueva;
}

int randomInt(int min, int max, std::mt19937& gen) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

double randomReal(double min, double max, std::mt19937& gen) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(gen);
}

bool eventoConProbabilidad(double probabilidad, std::mt19937& gen) {
    std::bernoulli_distribution dist(probabilidad);
    return dist(gen);
}

}
