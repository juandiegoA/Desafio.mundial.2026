#include <iostream>

#include "Torneo.h"

int main() {
    try {
        Torneo torneo;

        torneo.reiniciarIteraciones();
        torneo.cargarEquiposCSV("C:/Users/Admin/Downloads/selecciones_clasificadas_mundial.csv");
        torneo.medirEstadoSistema("Carga de equipos CSV");

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
        torneo.imprimirCalendarioGrupos();

        torneo.reiniciarIteraciones();
        torneo.simularFaseGrupos();
        torneo.medirEstadoSistema("Simulacion de fase de grupos");
        torneo.imprimirTablasGrupos();

        torneo.reiniciarIteraciones();
        torneo.generarR16();
        torneo.medirEstadoSistema("Transicion a R16");
        torneo.imprimirClasificadosR16();
        torneo.imprimirR16();

        torneo.reiniciarIteraciones();
        torneo.simularR16();
        torneo.medirEstadoSistema("Simulacion de R16");

        torneo.reiniciarIteraciones();
        torneo.generarR8();
        torneo.medirEstadoSistema("Transicion a R8");
        torneo.imprimirR8();

        torneo.reiniciarIteraciones();
        torneo.simularR8();
        torneo.medirEstadoSistema("Simulacion de R8");

        torneo.reiniciarIteraciones();
        torneo.generarQF();
        torneo.medirEstadoSistema("Transicion a QF");
        torneo.imprimirQF();

        torneo.reiniciarIteraciones();
        torneo.simularQF();
        torneo.medirEstadoSistema("Simulacion de QF");

        torneo.reiniciarIteraciones();
        torneo.generarSF();
        torneo.medirEstadoSistema("Transicion a SF");
        torneo.imprimirSF();

        torneo.reiniciarIteraciones();
        torneo.simularSF();
        torneo.medirEstadoSistema("Simulacion de SF");

        torneo.reiniciarIteraciones();
        torneo.generarFinalYTercerPuesto();
        torneo.medirEstadoSistema("Generacion de final y tercer puesto");
        torneo.imprimirFinalYTercerPuesto();

        torneo.reiniciarIteraciones();
        torneo.simularFinalYTercerPuesto();
        torneo.medirEstadoSistema("Simulacion de final y tercer puesto");

        torneo.generarReporteFinal();

        torneo.reiniciarIteraciones();
        torneo.guardarDatosActualizados();
        torneo.medirEstadoSistema("Persistencia de datos actualizados");

        torneo.medirEstadoSistema("Estado actual del sistema");
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    return 0;
}
