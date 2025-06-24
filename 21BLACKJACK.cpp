//#include <SFML/Window.hpp>
//#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <ctime>
#include <fstream>
#include <string>

using namespace std;
//using namespace sf;

struct Carta{
    string valor; //A,2,3,...,10,J,Q,K
    string tipo;  //Picas, Treboles, Corazones, Diamantes

                  //Numeros: 2-10,
    int puntos;   //Figuras: 10,
                  //As:1 o 11 
};

struct Jugador{
    string nombre;
    Carta* mano;
    int numCartas;
    int puntos;
};

struct Historial{
    char jugador[50];
    char ganador[50];
    char fecha[50];
};

void inicializarMazo(Carta* &mazo, int &cantCartas);
void mixearMazo(Carta* mazo, int &cantCartas);
void inicializarJugador(Jugador &jugdr, const string &nombre);
void repartirCarta(Jugador &jugador, Carta* mazo, int &indiceMazo);
void turnoJugador(Jugador &jugador, Carta* mazo, int &indiceMazo);
void turnoDealer(Jugador &dealer, Carta* mazo, int &indiceMazo);
string determinarGanador(const Jugador &jugador, const Jugador &dealer);
void guardarHistorial(const string &nombreJugador, const string &nombreGanador);
Historial* leerDesdeArchivo(int &totalPartidas);

int main(){
    /*RenderWindow window(VideoMode({1024, 768}), "BLACK JACK (21)");
    Font fuente;
    if (!fuente.loadFromFile("Casino.ttf")) {
        cout << "Error al cargar la fuente\n";
    }
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == Event::Closed)
                window.close();
        }
    }
    */

    Carta* mazo;
    int cantCartas;
    cout << "Inicializando mazo..." << endl;
    inicializarMazo(mazo, cantCartas);
    cout << "Mazo inicializado con " << cantCartas << " cartas." << endl;

    cout << "Mezclando mazo..." << endl;
    mixearMazo(mazo, cantCartas);
    cout << "Mazo mezclado exitosamente." << endl;

    string nombreJugador;
    cout << "Ingresa tu nombre: ";
    getline(cin, nombreJugador);
    cout << "Jugador: " << nombreJugador << endl;

    Jugador jugador, dealer;
    inicializarJugador(dealer, "Dealer");
    inicializarJugador(jugador, nombreJugador);
    cout << "Jugadores inicializados. Repartiendo cartas iniciales..." << endl;

    int indiceMazo=0;
    //se repartem 2 cartas a cada uno
    repartirCarta(jugador, mazo, indiceMazo);
    repartirCarta(dealer, mazo, indiceMazo);
    repartirCarta(jugador, mazo, indiceMazo);
    repartirCarta(dealer, mazo, indiceMazo);
    cout << "Cartas iniciales repartidas." << endl;

    turnoJugador(jugador, mazo, indiceMazo);
    cout << "Turno del jugador finalizado." << endl;
    turnoDealer(dealer, mazo, indiceMazo);
    cout << "Turno del dealer finalizado." << endl;

    cout << "Determinando ganador..." << endl;
    string ganador = determinarGanador(jugador, dealer);
    cout << "GANADOR: " << ganador << endl;
    
    cout << "Guardando historial..." << endl;
    guardarHistorial(jugador.nombre, ganador);

    int totalPartidas = 0;
    Historial* historial = leerDesdeArchivo(totalPartidas);//guarda el historial en un arreglo dinamico de estructuras 
    cout << "Historial cargado. Total de partidas: " << totalPartidas << endl;

    if (historial == nullptr) {
        return 1;
    }

    for (int i = 0; i < totalPartidas; i++) {
        cout << "Jugador: " << historial[i].jugador << endl;
        cout << "Ganador: " << historial[i].ganador << endl;
        cout << "Fecha: " << historial[i].fecha << endl;
        cout << "----------------------\n";
    }

    delete[] mazo;
    delete[] jugador.mano;
    delete[] dealer.mano;
    delete[] historial;

    return 0;
}

void inicializarMazo(Carta* &mazo, int &cantCartas){
    string valores[]={"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    string tipos[]={"Picas","Treboles","Corazones", "Diamantes"};
    cantCartas = 52;
    mazo = new Carta[cantCartas];
    int indiceMazo = 0;

    for (int i = 0; i < 4; i++) {  // 4 tipos
        for (int j = 1; j <= 13; j++) {  // 13 valores
            mazo[indiceMazo].valor = valores[j-1];//-1 porque empezo en 1 y no en 0...
            mazo[indiceMazo].tipo = tipos[i];
            if (j==13)
                mazo[indiceMazo].puntos = 11; // As... 11 se podra ajustar a 1 u 11 mas adelante
            else{
                if (j >= 10) 
                    mazo[indiceMazo].puntos = 10; // Figuras... 10
                else 
                    mazo[indiceMazo].puntos = j+1; // Numeros... 2 - 10, +1 porque la posicion 0 empieza en 2
                indiceMazo++;
            }
            
        }
    }
}

void mixearMazo(Carta* mazo, int &cantCartas){
    
    srand(time(NULL));
    for(int i=0; i<cantCartas; i++){
        int j = rand()%cantCartas;
        Carta temp;//auxiliar para hacer swap
        temp = mazo[i];
        mazo[i] = mazo[j];
        mazo[j] = temp;
        //podrua usar swap(mazo[i], mazo[j]) pero no jaja
    }
}

void inicializarJugador(Jugador &jugador, const string &nombre){
    jugador.mano=new Carta[12]; //las 11 cartas con los minimos valores suman 21, con la 12va perdería
    jugador.numCartas = 0;
    jugador.puntos = 0;
    jugador.nombre = nombre;
}

void repartirCarta(Jugador &jugador, Carta* mazo, int &indiceMazo) {
    jugador.mano[jugador.numCartas] = mazo[indiceMazo]; //se le da una acrta al jugador
    jugador.puntos += mazo[indiceMazo].puntos;
    jugador.numCartas++;
    
    cout << jugador.nombre << " recibe: " 
    << mazo[indiceMazo].valor << " de " 
    << mazo[indiceMazo].tipo << " (+" 
    << mazo[indiceMazo].puntos << " puntos)" << endl;
    indiceMazo++;
}

void turnoJugador(Jugador &jugador, Carta* mazo, int &indiceMazo) {
    char resp;
    do {
        cout << "\nTu mano actual (" << jugador.puntos << " puntos): " << endl;
        for (int i = 0; i < jugador.numCartas; i++) {
            cout << "- " << jugador.mano[i].valor << " de " << jugador.mano[i].tipo << endl;
        }

        if (jugador.puntos >= 21) break; //Si de casualidad obtiene 21 con dos cartas ya dadas se acaba el turno

        cout << "\nQuieres otra carta? (h: Hit, s: Stand): ";
        cin >> resp;
        cin.ignore();

        if (resp == 'h' || resp == 'H') {
            repartirCarta(jugador, mazo, indiceMazo);
            // Ajuste por As si se pasa de 21
            if (jugador.puntos > 21) {
                for (int i = 0; i < jugador.numCartas; i++) {
                    if (jugador.mano[i].valor == "A" && jugador.puntos > 21) {
                        char resp2;
                        cout << "Te pasaste de 21..." << endl;
                        cout << "Deseas cambair el valor del As [11] por [1]?: (s: Si, n: No)";
                        cin >> resp2;
                        cin.ignore();
                        if(resp2 == 'S' || resp2 == 's')
                            jugador.puntos -= 10; // As pasa de 11 a 1
                    }
                }
            }
        }

    } while ((resp == 'h' || resp == 'H') && jugador.puntos < 21);

    cout << "\nPuntaje final del jugador: " << jugador.puntos << "\n";
}

void turnoDealer(Jugador &dealer, Carta* mazo, int &indiceMazo) {
    cout << "\nTurno del dealer..." << endl;

    while (dealer.puntos < 17) {
        repartirCarta(dealer, mazo, indiceMazo);
        // Ajuste por As si se pasa de 21
        if (dealer.puntos > 21) {
            for (int i=0; i<dealer.numCartas; i++) {
                if (dealer.mano[i].valor == "A") {
                    dealer.puntos -= 10;
                }
            }
        }
    }
    cout << "\nMano del dealer (" << dealer.puntos << " puntos):\n";
    for (int i = 0; i < dealer.numCartas; i++) {
        cout << "- " << dealer.mano[i].valor << " de " << dealer.mano[i].tipo << endl;
    }

    cout << "\nPuntaje final del dealer: " << dealer.puntos << "\n";
}

string determinarGanador(const Jugador &jugador, const Jugador &dealer){
    string ganador;
    if(jugador.puntos > 21 && dealer.puntos > 21){//Si ambos se pasan de 21 gana dealer por que jugador ya perdio
        ganador = dealer.nombre;
    }else{
        if(jugador.puntos > 21){//si solo se paso el jugador gana dealer
            ganador = dealer.nombre;
        }else{
            if(dealer.puntos > 21){//si solo se paso el dealer gana jugador
                ganador = jugador.nombre;
            }else{
                //como ninguno se paso de 21, gana el que tenga mas puntos
                if(jugador.puntos > dealer.puntos){
                    ganador = jugador.nombre;
                }else{
                    if(dealer.puntos > jugador.puntos){
                        ganador = dealer.nombre;
                    }else{//si ninguno es mayor, son iguales por lo tanto empate y gana dealer
                        ganador = dealer.nombre;
                    }
                }
            }
        }
    }
    return ganador;
}

void guardarHistorial(const string &nombreJugador, const string &nombreGanador) {
    fstream archivo;
    char nombreArchivo[30] = "historial.txt";
    time_t ahora;//Dato curios JAJA time_t es un tipo de dato que representa el tiempo en segundos desde el 1 de enero de 1970 (lo que se conoce como "época Unix")
    ahora = time(0);// tiempo actual del sistema en ese formato
    tm *tiempo; //estructura tm
    tiempo = localtime(&ahora);//localtime() convierte el valor en segundos de aghora(time_t) a una estructura tm que contiene campos como: Año, mes, día, hora, minutos, segundos
    archivo.open(nombreArchivo, ios::out | ios::app);
    if (!archivo)
        cout << "No se pudo abrir el archivo\n";
    else {
        archivo << "Jugador: " << nombreJugador << endl;
        archivo << "Ganador: " << nombreGanador << endl;
        archivo << "Fecha: " 
                << (tiempo->tm_year + 1900) << "-"
                << (tiempo->tm_mon + 1) << "-"
                << tiempo->tm_mday << " "
                << tiempo->tm_hour << ":"
                << tiempo->tm_min << endl;
        archivo << "--------------------------" << endl;
        archivo.close();
        cout << "Historial guardado con exito." << endl;
    }
}

Historial* leerDesdeArchivo(int &totalPartidas) {
    fstream archivo;
    char linea[100];
    int capacidad = 10;
    totalPartidas = 0;

    Historial* registros = new Historial[capacidad];

    archivo.open("historial.txt", ios::in);
    if (!archivo) {
        cout << "No se pudo abrir el archivo\n";
        return nullptr;
    }

    while (archivo.getline(linea, 100)) {
        if (strncmp(linea, "Jugador:", 8) == 0) {
            if (totalPartidas >= capacidad) {
                capacidad *= 2;
                Historial* nuevo = new Historial[capacidad];
                for (int i = 0; i < totalPartidas; i++) {
                    nuevo[i] = registros[i];
                }
                delete[] registros;
                registros = nuevo;
            }

            strcpy(registros[totalPartidas].jugador, linea + 9);
            archivo.getline(linea, 100);
            strcpy(registros[totalPartidas].ganador, linea + 9);
            archivo.getline(linea, 100);
            strcpy(registros[totalPartidas].fecha, linea + 7);
            archivo.getline(linea, 100); // línea separadora ----
            totalPartidas++;
        }
    }
    archivo.close();
    return registros;
}



/*
PARA COMPILAR DESDE LA TERMINAL USANDO EL COMPILADOR COMPATIBLE CON LA VERSION QUE DESCARGUE DE SFML
Version de SFML que usé: SFML-2.6.1
Version del compilador que usé: WinLibs MSVCRT 13.1.0 (64-bit)
C:\\mingw64\\bin\\g++ -c 21BLACKJACK.cpp -DSFML_STATIC -IC:\\SFML-2.6.1\\include\\ -o build/21BLACKJACK.o
C:\\mingw64\\bin\\g++ build/21BLACKJACK.o -LC:\\SFML-2.6.1\\lib\\ -static -static-libstdc++ -lsfml-window-s -lsfml-graphics-s -lsfml-system-s -lopengl32 -lfreetype -lgdi32 -lwinmm
*/