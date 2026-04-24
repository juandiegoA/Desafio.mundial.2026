#include <iostream>

#include "Torneo.h"

int main() {
    try {
        Torneo torneo;

        torneo.reiniciarIteraciones();
        torneo.cargarEquiposCSV("C:/Users/Admin/Downloads/selecciones_clasificadas_mundial.csv");
        torneo.medirEstadoSistema("Carga de equipos CSV");

        const auto& equipos = torneo.getEquipos();
        std::cout << "Cantidad de equipos cargados: " << equipos.size() << "\n";

        for (size_t i = 0; i < equipos.size() && i < 5; ++i) {
            equipos[i].imprimirResumen();
        }

        torneo.reiniciarIteraciones();
        torneo.generarJugadoresIniciales();
        torneo.medirEstadoSistema("Generacion de jugadores");

        torneo.reiniciarIteraciones();
        torneo.crearBombos();
        torneo.medirEstadoSistema("Creacion de bombos");
        torneo.imprimirBombos();

        torneo.reiniciarIteraciones();
        torneo.conformarGrupos();
        torneo.medirEstadoSistema("Conformacion de grupos");
        torneo.imprimirGrupos();

        torneo.reiniciarIteraciones();
        torneo.generarCalendarioGrupos();
        torneo.medirEstadoSistema("Generacion del calendario de grupos");

        torneo.reiniciarIteraciones();
        torneo.simularFaseGrupos();
        torneo.medirEstadoSistema("Simulacion de fase de grupos");
        torneo.imprimirTablasGrupos();

        torneo.reiniciarIteraciones();
        torneo.generarR16();
        torneo.medirEstadoSistema("Generacion de R16");

        torneo.reiniciarIteraciones();
        torneo.simularR16();
        torneo.medirEstadoSistema("Simulacion de R16");

        torneo.generarReporteFinal();
        torneo.medirEstadoSistema("Estado actual del sistema");
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    return 0;
}
