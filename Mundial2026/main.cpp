#include <iostream>
#include <limits>
#include <string>

#include "Torneo.h"

void limpiarEntrada() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void mostrarMenu(bool mostrarDetallePartidos) {
    std::cout << "\n=============================================\n";
    std::cout << "        MENU PRINCIPAL - TORNEO MUNDIAL\n";
    std::cout << "=============================================\n";
    std::cout << "1.  Cargar equipos desde CSV\n";
    std::cout << "2.  Generar jugadores iniciales\n";
    std::cout << "3.  Crear e imprimir bombos\n";
    std::cout << "4.  Conformar e imprimir grupos\n";
    std::cout << "5.  Generar e imprimir calendario de grupos\n";
    std::cout << "6.  Simular fase de grupos\n";
    std::cout << "7.  Generar e imprimir R16\n";
    std::cout << "8.  Simular R16\n";
    std::cout << "9.  Generar y simular R8\n";
    std::cout << "10. Generar y simular QF\n";
    std::cout << "11. Generar y simular SF\n";
    std::cout << "12. Generar y simular final y tercer puesto\n";
    std::cout << "13. Generar reporte final\n";
    std::cout << "14. Guardar datos actualizados en CSV\n";
    std::cout << "15. Ejecutar torneo completo automaticamente\n";
    std::cout << "16. Cambiar modo de impresion detallada ";
    std::cout << "(" << (mostrarDetallePartidos ? "ACTIVADO" : "DESACTIVADO") << ")\n";
    std::cout << "0.  Salir\n";
    std::cout << "Seleccione una opcion: ";
}

int main() {
    Torneo torneo;

    const std::string rutaCSV = "C:/Users/Admin/Downloads/selecciones_clasificadas_mundial.csv";

    bool equiposCargados = false;
    bool jugadoresGenerados = false;
    bool bombosCreados = false;
    bool gruposConformados = false;
    bool calendarioGenerado = false;
    bool gruposSimulados = false;
    bool r16Generado = false;
    bool r16Simulado = false;
    bool r8Simulado = false;
    bool qfSimulado = false;
    bool sfSimulado = false;
    bool finalSimulada = false;

    bool mostrarDetallePartidos = false;

    int opcion = -1;

    do {
        mostrarMenu(mostrarDetallePartidos);

        if (!(std::cin >> opcion)) {
            limpiarEntrada();
            std::cout << "Entrada invalida. Intente nuevamente.\n";
            continue;
        }

        try {
            switch (opcion) {
            case 1:
                torneo.reiniciarIteraciones();
                torneo.cargarEquiposCSV(rutaCSV);
                torneo.medirEstadoSistema("Carga de equipos CSV");

                equiposCargados = true;
                jugadoresGenerados = false;
                bombosCreados = false;
                gruposConformados = false;
                calendarioGenerado = false;
                gruposSimulados = false;
                r16Generado = false;
                r16Simulado = false;
                r8Simulado = false;
                qfSimulado = false;
                sfSimulado = false;
                finalSimulada = false;

                std::cout << "Equipos cargados correctamente.\n";
                break;

            case 2:
                if (!equiposCargados) {
                    std::cout << "Primero debe cargar los equipos desde el CSV.\n";
                    break;
                }

                torneo.reiniciarIteraciones();
                torneo.generarJugadoresIniciales();
                torneo.medirEstadoSistema("Generacion de jugadores");

                jugadoresGenerados = true;
                std::cout << "Jugadores generados correctamente.\n";
                break;

            case 3:
                if (!equiposCargados) {
                    std::cout << "Primero debe cargar los equipos desde el CSV.\n";
                    break;
                }

                torneo.reiniciarIteraciones();
                torneo.crearBombos();
                torneo.medirEstadoSistema("Creacion de bombos");
                torneo.imprimirBombos();

                bombosCreados = true;
                gruposConformados = false;
                calendarioGenerado = false;
                gruposSimulados = false;
                r16Generado = false;
                r16Simulado = false;
                r8Simulado = false;
                qfSimulado = false;
                sfSimulado = false;
                finalSimulada = false;
                break;

            case 4:
                if (!bombosCreados) {
                    std::cout << "Primero debe crear los bombos.\n";
                    break;
                }

                torneo.reiniciarIteraciones();
                torneo.conformarGrupos();
                torneo.medirEstadoSistema("Conformacion de grupos");
                torneo.imprimirGrupos();

                gruposConformados = true;
                calendarioGenerado = false;
                gruposSimulados = false;
                r16Generado = false;
                r16Simulado = false;
                r8Simulado = false;
                qfSimulado = false;
                sfSimulado = false;
                finalSimulada = false;
                break;

            case 5:
                if (!gruposConformados) {
                    std::cout << "Primero debe conformar los grupos.\n";
                    break;
                }

                torneo.reiniciarIteraciones();
                torneo.generarCalendarioGrupos();
                torneo.medirEstadoSistema("Generacion del calendario de grupos");
                torneo.imprimirCalendarioGrupos();

                calendarioGenerado = true;
                gruposSimulados = false;
                r16Generado = false;
                r16Simulado = false;
                r8Simulado = false;
                qfSimulado = false;
                sfSimulado = false;
                finalSimulada = false;
                break;

            case 6:
                if (!calendarioGenerado) {
                    std::cout << "Primero debe generar el calendario de grupos.\n";
                    break;
                }

                torneo.reiniciarIteraciones();
                torneo.simularFaseGrupos();
                torneo.medirEstadoSistema("Simulacion de fase de grupos");
                torneo.imprimirTablasGrupos();

                if (mostrarDetallePartidos) {
                    torneo.imprimirDetalleFaseGrupos();
                }

                gruposSimulados = true;
                r16Generado = false;
                r16Simulado = false;
                r8Simulado = false;
                qfSimulado = false;
                sfSimulado = false;
                finalSimulada = false;
                break;

            case 7:
                if (!gruposSimulados) {
                    std::cout << "Primero debe simular la fase de grupos.\n";
                    break;
                }

                torneo.reiniciarIteraciones();
                torneo.generarR16();
                torneo.medirEstadoSistema("Transicion a R16");
                torneo.imprimirClasificadosR16();
                torneo.imprimirR16();

                r16Generado = true;
                r16Simulado = false;
                r8Simulado = false;
                qfSimulado = false;
                sfSimulado = false;
                finalSimulada = false;
                break;

            case 8:
                if (!r16Generado) {
                    std::cout << "Primero debe generar R16.\n";
                    break;
                }

                torneo.reiniciarIteraciones();
                torneo.simularR16();
                torneo.medirEstadoSistema("Simulacion de R16");

                if (mostrarDetallePartidos) {
                    torneo.imprimirDetalleR16();
                }

                r16Simulado = true;
                r8Simulado = false;
                qfSimulado = false;
                sfSimulado = false;
                finalSimulada = false;
                break;

            case 9:
                if (!r16Simulado) {
                    std::cout << "Primero debe simular R16.\n";
                    break;
                }

                torneo.reiniciarIteraciones();
                torneo.generarR8();
                torneo.medirEstadoSistema("Transicion a R8");
                torneo.imprimirR8();

                torneo.reiniciarIteraciones();
                torneo.simularR8();
                torneo.medirEstadoSistema("Simulacion de R8");

                if (mostrarDetallePartidos) {
                    torneo.imprimirDetalleR8();
                }

                r8Simulado = true;
                qfSimulado = false;
                sfSimulado = false;
                finalSimulada = false;
                break;

            case 10:
                if (!r8Simulado) {
                    std::cout << "Primero debe generar y simular R8.\n";
                    break;
                }

                torneo.reiniciarIteraciones();
                torneo.generarQF();
                torneo.medirEstadoSistema("Transicion a QF");
                torneo.imprimirQF();

                torneo.reiniciarIteraciones();
                torneo.simularQF();
                torneo.medirEstadoSistema("Simulacion de QF");

                if (mostrarDetallePartidos) {
                    torneo.imprimirDetalleQF();
                }

                qfSimulado = true;
                sfSimulado = false;
                finalSimulada = false;
                break;

            case 11:
                if (!qfSimulado) {
                    std::cout << "Primero debe generar y simular QF.\n";
                    break;
                }

                torneo.reiniciarIteraciones();
                torneo.generarSF();
                torneo.medirEstadoSistema("Transicion a SF");
                torneo.imprimirSF();

                torneo.reiniciarIteraciones();
                torneo.simularSF();
                torneo.medirEstadoSistema("Simulacion de SF");

                if (mostrarDetallePartidos) {
                    torneo.imprimirDetalleSF();
                }

                sfSimulado = true;
                finalSimulada = false;
                break;

            case 12:
                if (!sfSimulado) {
                    std::cout << "Primero debe generar y simular SF.\n";
                    break;
                }

                torneo.reiniciarIteraciones();
                torneo.generarFinalYTercerPuesto();
                torneo.medirEstadoSistema("Generacion de final y tercer puesto");
                torneo.imprimirFinalYTercerPuesto();

                torneo.reiniciarIteraciones();
                torneo.simularFinalYTercerPuesto();
                torneo.medirEstadoSistema("Simulacion de final y tercer puesto");

                if (mostrarDetallePartidos) {
                    torneo.imprimirDetalleFinalYTercerPuesto();
                }

                finalSimulada = true;
                break;

            case 13:
                if (!finalSimulada) {
                    std::cout << "Primero debe simular la final y el tercer puesto.\n";
                    break;
                }

                torneo.generarReporteFinal();
                break;

            case 14:
                if (!finalSimulada) {
                    std::cout << "Primero debe simular el torneo completo antes de guardar los datos.\n";
                    break;
                }

                torneo.reiniciarIteraciones();
                torneo.guardarDatosActualizados();
                torneo.medirEstadoSistema("Persistencia de datos actualizados");
                break;

            case 15:
                torneo.reiniciarIteraciones();
                torneo.cargarEquiposCSV(rutaCSV);
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

                if (mostrarDetallePartidos) {
                    torneo.imprimirDetalleFaseGrupos();
                }

                torneo.reiniciarIteraciones();
                torneo.generarR16();
                torneo.medirEstadoSistema("Transicion a R16");
                torneo.imprimirClasificadosR16();
                torneo.imprimirR16();

                torneo.reiniciarIteraciones();
                torneo.simularR16();
                torneo.medirEstadoSistema("Simulacion de R16");

                if (mostrarDetallePartidos) {
                    torneo.imprimirDetalleR16();
                }

                torneo.reiniciarIteraciones();
                torneo.generarR8();
                torneo.medirEstadoSistema("Transicion a R8");
                torneo.imprimirR8();

                torneo.reiniciarIteraciones();
                torneo.simularR8();
                torneo.medirEstadoSistema("Simulacion de R8");

                if (mostrarDetallePartidos) {
                    torneo.imprimirDetalleR8();
                }

                torneo.reiniciarIteraciones();
                torneo.generarQF();
                torneo.medirEstadoSistema("Transicion a QF");
                torneo.imprimirQF();

                torneo.reiniciarIteraciones();
                torneo.simularQF();
                torneo.medirEstadoSistema("Simulacion de QF");

                if (mostrarDetallePartidos) {
                    torneo.imprimirDetalleQF();
                }

                torneo.reiniciarIteraciones();
                torneo.generarSF();
                torneo.medirEstadoSistema("Transicion a SF");
                torneo.imprimirSF();

                torneo.reiniciarIteraciones();
                torneo.simularSF();
                torneo.medirEstadoSistema("Simulacion de SF");

                if (mostrarDetallePartidos) {
                    torneo.imprimirDetalleSF();
                }

                torneo.reiniciarIteraciones();
                torneo.generarFinalYTercerPuesto();
                torneo.medirEstadoSistema("Generacion de final y tercer puesto");
                torneo.imprimirFinalYTercerPuesto();

                torneo.reiniciarIteraciones();
                torneo.simularFinalYTercerPuesto();
                torneo.medirEstadoSistema("Simulacion de final y tercer puesto");

                if (mostrarDetallePartidos) {
                    torneo.imprimirDetalleFinalYTercerPuesto();
                }

                torneo.generarReporteFinal();

                torneo.reiniciarIteraciones();
                torneo.guardarDatosActualizados();
                torneo.medirEstadoSistema("Persistencia de datos actualizados");

                equiposCargados = true;
                jugadoresGenerados = true;
                bombosCreados = true;
                gruposConformados = true;
                calendarioGenerado = true;
                gruposSimulados = true;
                r16Generado = true;
                r16Simulado = true;
                r8Simulado = true;
                qfSimulado = true;
                sfSimulado = true;
                finalSimulada = true;
                break;

            case 16:
                mostrarDetallePartidos = !mostrarDetallePartidos;
                std::cout << "Modo de impresion detallada: "
                          << (mostrarDetallePartidos ? "ACTIVADO" : "DESACTIVADO")
                          << '\n';
                break;

            case 0:
                std::cout << "Saliendo del programa.\n";
                break;

            default:
                std::cout << "Opcion no valida.\n";
                break;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << '\n';
        }

    } while (opcion != 0);

    return 0;
}
