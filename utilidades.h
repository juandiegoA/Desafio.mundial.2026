#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <random>
#include <string>
#include <vector>

#include "Tipos.h"

namespace Utilidades {
std::vector<std::string> dividirCSV(const std::string& linea) {
    std::vector<std::string> partes;
    std::string actual;
    bool dentroComillas = false;

    for (char c : linea) {
        if (c == '"') {
            dentroComillas = !dentroComillas;
        } else if (c == ';' && !dentroComillas) {
            partes.push_back(actual);
            actual.clear();
        } else {
            actual.push_back(c);
        }
    }

    partes.push_back(actual);
    return partes;
}
}

#endif
