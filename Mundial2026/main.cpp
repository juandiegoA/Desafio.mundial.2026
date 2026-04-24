#include <iostream>
#include "Torneo.h"
int main() {
    try {
        Torneo torneo;

        torneo.cargarEquiposCSV("C:/Users/Admin/Downloads/selecciones_clasificadas_mundial.csv");

        const auto& equipos = torneo.getEquipos();
        std::cout << "Cantidad de equipos cargados: " << equipos.size() << "\n";

        for (size_t i = 0; i < equipos.size() && i < 5; ++i) {
            equipos[i].imprimirResumen();
        }

        torneo.generarJugadoresIniciales();
        torneo.crearBombos();
        torneo.imprimirBombos();

        torneo.conformarGrupos();
        torneo.imprimirGrupos();

        torneo.generarCalendarioGrupos();
        torneo.simularFaseGrupos();
        torneo.imprimirTablasGrupos();

        torneo.generarR16();
        torneo.simularR16();

        torneo.generarReporteFinal();
        torneo.medirEstadoSistema("Estado actual del sistema");
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    return 0;
}
