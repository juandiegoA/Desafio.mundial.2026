#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <random>
#include <string>
#include <vector>

#include "Tipos.h"

namespace Utilidades {
std::vector<std::string> dividirCSV(const std::string& linea);

std::string limpiarTexto(std::string texto);

Confederacion convertirConfederacion(const std::string& texto);
std::string confederacionATexto(Confederacion c);
std::string etapaATexto(Etapa e);
std::string fechaATexto(const Fecha& fecha);

Fecha sumarDias(const Fecha& fecha, int dias);
int diasDesdeReferencia(const Fecha& fecha);

int randomInt(int min, int max, std::mt19937& gen);
double randomReal(double min, double max, std::mt19937& gen);
bool eventoConProbabilidad(double probabilidad, std::mt19937& gen);
}

#endif
