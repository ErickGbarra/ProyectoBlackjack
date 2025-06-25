#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cstring>
#include <ctime>
#include <fstream>
#include <string>

using namespace std;
using namespace sf;

const int WIDTH = 1024;//Dimensiones de la ventana
const int HEIGHT = 768;

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
//funciones de juego en consola
void inicializarMazo(Carta* &mazo, int &cantCartas);
void mixearMazo(Carta* mazo, int &cantCartas);
void inicializarJugador(Jugador &jugdr, const string &nombre);
void repartirCarta(Jugador &jugador, Carta* mazo, int &indiceMazo);
string determinarGanador(const Jugador &jugador, const Jugador &dealer);
void guardarHistorial(const string &nombreJugador, const string &nombreGanador);
Historial* leerDesdeArchivo(int &totalPartidas);
//funciones de graficos sfml
string capturarNombre(RenderWindow &window, Font &fuente);
void mostrarReglas(RenderWindow &window, Font &fuente);
void mostrarHistorial(RenderWindow &window, Font &fuente);
string obtenerNombreArchivoCarta(const Carta& carta);
void jugarBlackJack(RenderWindow &window, Font &fuente, const string &nombre);
int elegirValorAs(RenderWindow &window, Font &fuente);
void mostrarResultadoFinal(RenderWindow &parentWindow, Font &fuente, const string &ganador, const string &nombreJugador, bool &salirAlMenu);

int main(){
    //nuevo main, antes era la funcion que ahora se llama jugarBlackJack
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Blackjack - Menu Principal");
    window.setFramerateLimit(60);

    // Fondo
    Texture fondoTexture;
    if (!fondoTexture.loadFromFile("recursos/FondoBlackJack.png")) {
        cerr << "No se pudo cargar fondo.png\n";
        return 1;
    }
    Sprite fondoSprite(fondoTexture);
    // Fuente
    Font fuente;
    if (!fuente.loadFromFile("recursos/arial.ttf")) {
        cerr << "No se pudo cargar la fuente\n";
        return 1;
    }
    // Botones (usamos rectángulos y texto)
    //Se declaran primero con sus dimensiones
    RectangleShape btnReglas(Vector2f(300, 60));
    RectangleShape btnHistorial(Vector2f(300, 60));
    RectangleShape btnJugar(Vector2f(300, 60));
    //se escoge su posicion
    btnReglas.setPosition(362, 200);
    btnHistorial.setPosition(362, 300);
    btnJugar.setPosition(362, 400);
    //color
    btnReglas.setFillColor(Color::Blue);
    btnHistorial.setFillColor(Color::Green);
    btnJugar.setFillColor(Color::Red);
    //texto de los botones
    Text txtReglas("Reglas del Juego", fuente, 24);
    Text txtHistorial("Historial", fuente, 24);
    Text txtJugar("Jugar", fuente, 24);
    //posicion del texto
    txtReglas.setPosition(390, 215);
    txtHistorial.setPosition(420, 315);
    txtJugar.setPosition(440, 415);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)//si presionamos la "x" se cierra la ventana
                window.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (btnReglas.getGlobalBounds().contains(mousePos)) {
                    cout << "[INFO] Se seleccionó: Reglas" << endl;
                    mostrarReglas(window, fuente);
                } else if (btnHistorial.getGlobalBounds().contains(mousePos)) {
                    cout << "[INFO] Se seleccionó: Historial" << endl;
                    mostrarHistorial(window, fuente);
                } else if (btnJugar.getGlobalBounds().contains(mousePos)) {
                    cout << "[INFO] Se seleccionó: Jugar" << endl;
                    string nombre = capturarNombre(window, fuente);
                    jugarBlackJack(window, fuente, nombre);
                }
            }
        }
        // Efecto hover siempre activo mientras el mouse se mueve
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

        if (btnReglas.getGlobalBounds().contains(mousePos))
            btnReglas.setFillColor(Color(70, 70, 200)); // azul mas claro
        else
            btnReglas.setFillColor(Color::Blue);

        if (btnHistorial.getGlobalBounds().contains(mousePos))
            btnHistorial.setFillColor(Color(70, 200, 70)); // verde mas claro
        else
            btnHistorial.setFillColor(Color::Green);

        if (btnJugar.getGlobalBounds().contains(mousePos))
            btnJugar.setFillColor(Color(200, 70, 70)); // rojo ms claro
        else
            btnJugar.setFillColor(Color::Red);

        window.clear();
        window.draw(fondoSprite);
        window.draw(btnReglas);
        window.draw(btnHistorial);
        window.draw(btnJugar);
        window.draw(txtReglas);
        window.draw(txtHistorial);
        window.draw(txtJugar);
        window.display();
    }

    return 0;
}

string capturarNombre(RenderWindow &window, Font &fuente) {
    string nombre;
    Text titulo("Ingresa tu nombre y presiona ENTER", fuente, 28);
    titulo.setPosition(200, 100);
    titulo.setFillColor(Color::White);

    Text entrada("", fuente, 26);
    entrada.setPosition(250, 200);
    entrada.setFillColor(Color::Yellow);

    RectangleShape cuadro(Vector2f(500, 50));
    cuadro.setPosition(250, 200);
    cuadro.setFillColor(Color(30, 30, 30, 200));
    cuadro.setOutlineColor(Color::White);
    cuadro.setOutlineThickness(2);

    bool capturando = true;

    while (capturando && window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return "Sin Nombre";
            }

            if (event.type == Event::TextEntered) {
                if (event.text.unicode == '\b') { // Backspace
                    if (!nombre.empty()) {
                        nombre.pop_back();
                    }
                } else if (event.text.unicode == '\r' || event.text.unicode == '\n') {
                    capturando = false; // Enter
                } else if (event.text.unicode < 128 && nombre.size() < 20) {
                    nombre += static_cast<char>(event.text.unicode);
                }
                entrada.setString(nombre);
            }
        }

        window.clear(Color::Black);
        window.draw(titulo);
        window.draw(cuadro);
        window.draw(entrada);
        window.display();
    }

    return nombre;
}

string obtenerNombreArchivoCarta(const Carta& carta) {
    return "recursos/Cartas/" + carta.valor + carta.tipo + ".png";
}


void mostrarReglas(RenderWindow &window, Font &fuente){
    RenderWindow reglasVentana(VideoMode(800, 600), "Reglas del Blackjack");

    Text titulo("Reglas del Blackjack", fuente, 30);
    titulo.setPosition(50, 20);
    titulo.setFillColor(Color::White);

    string textoReglas =
        "1. El objetivo es sumar 21 puntos o acercarse sin pasarse.\n"
        "2. Las figuras (J, Q, K) valen 10 puntos.\n"
        "3. El As puede valer 1 u 11 puntos.\n"
        "4. Se reparten 2 cartas al inicio a cada jugador.\n"
        "5. Puedes pedir cartas (Hit) o quedarte (Stand).\n"
        "6. El dealer debe seguir hasta tener al menos 17 puntos.\n"
        "7. Si te pasas de 21, pierdes automaticamente.\n"
        "8. Si empatan, gana el dealer por defecto.";

    Text cuerpo(textoReglas, fuente, 20);
    cuerpo.setPosition(50, 80);
    cuerpo.setFillColor(Color::White);

    RectangleShape botonVolver(Vector2f(180, 40));
    botonVolver.setPosition(600, 520);
    botonVolver.setFillColor(Color(100, 100, 255));

    Text textoVolver("Volver al menu", fuente, 20);
    textoVolver.setPosition(610, 530);
    textoVolver.setFillColor(Color::White);

    while (reglasVentana.isOpen()) {
        Event event;
        while (reglasVentana.pollEvent(event)) {
            if (event.type == Event::Closed)
                reglasVentana.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (botonVolver.getGlobalBounds().contains(mousePos)) {
                    reglasVentana.close();
                    return;
                }
            }
        }
        

        reglasVentana.clear(Color::Black);
        reglasVentana.draw(titulo);
        reglasVentana.draw(cuerpo);
        reglasVentana.draw(botonVolver);
        reglasVentana.draw(textoVolver);
        reglasVentana.display();
    }
}
void mostrarHistorial(RenderWindow &window, Font &fuente){
    RenderWindow histVentana(VideoMode(800, 600), "Historial de Partidas");
    Text titulo("Historial de Partidas", fuente, 30);
    titulo.setPosition(50, 20);
    titulo.setFillColor(Color::White);
    float offsetY=0;
    int total = 0;
    Historial* registros = leerDesdeArchivo(total);
    string textoHist;
    
    if (registros && total > 0) {
        for (int i = 0; i < total && i < 10; i++) {
            textoHist += "Jugador: " + string(registros[i].jugador) + "\n";
            textoHist += "Ganador: " + string(registros[i].ganador) + "\n";
            textoHist += "Fecha: " + string(registros[i].fecha) + "\n";
            textoHist += "--------------------------\n";
        }
    } else {
        textoHist = "No hay historial disponible.";
    }

    Text cuerpo(textoHist, fuente, 18);
    cuerpo.setPosition(50, 80);
    cuerpo.setFillColor(Color::White);
    // Calcular altura total del texto para limitar el scroll
    float altoTexto = cuerpo.getLocalBounds().height;
    float maxScroll = altoTexto - 460; // 600 ventana - 80 margen arriba - 60 boton
    if (maxScroll < 0) maxScroll = 0;

    RectangleShape botonVolver(Vector2f(180, 40));
    botonVolver.setPosition(600, 520);
    botonVolver.setFillColor(Color(100, 100, 255));

    Text textoVolver("Volver al menu", fuente, 20);
    textoVolver.setPosition(610, 530);
    textoVolver.setFillColor(Color::White);

    while (histVentana.isOpen()) {
        Event event;
        while (histVentana.pollEvent(event)) {
            if (event.type == Event::Closed)
                histVentana.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (botonVolver.getGlobalBounds().contains(mousePos)) {
                    histVentana.close();
                    delete[] registros;
                    return;
                }
            }
            if (event.type == Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0) {
                    offsetY += 20;
                } else if (event.mouseWheelScroll.delta < 0) {
                    offsetY -= 20;
                }

                // Limitar el desplazamiento
                if (offsetY > 0) offsetY = 0;
                if (offsetY < -maxScroll) offsetY = -maxScroll;

                cuerpo.setPosition(50, 80 + offsetY);
            }
        }
        Vector2f mousePos = histVentana.mapPixelToCoords(Mouse::getPosition(histVentana));
        if (botonVolver.getGlobalBounds().contains(mousePos)) {
            botonVolver.setFillColor(Color(120, 120, 255)); // Hover
        } else {
            botonVolver.setFillColor(Color(100, 100, 255)); // Normal
        }

        histVentana.clear(Color::Black);
        histVentana.draw(titulo);
        histVentana.draw(cuerpo);
        histVentana.draw(botonVolver);
        histVentana.draw(textoVolver);
        histVentana.display();
    }
}

void jugarBlackJack(RenderWindow &window, Font &fuente, const string &nombre){
    Carta* mazo;
    int cantCartas;
    cout << "Inicializando mazo..." << endl;
    inicializarMazo(mazo, cantCartas);
    cout << "Mazo inicializado con " << cantCartas << " cartas." << endl;

    cout << "Mezclando mazo..." << endl;
    mixearMazo(mazo, cantCartas);
    cout << "Mazo mezclado exitosamente." << endl;

    string nombreJugador = nombre;
    cout << "Jugador: " << nombreJugador << endl;

    Jugador jugador, dealer;
    inicializarJugador(jugador, nombreJugador);
    inicializarJugador(dealer, "Dealer");
    cout << "Jugadores inicializados. Repartiendo cartas iniciales..." << endl;

    int indiceMazo=0;
    //se repartem 2 cartas a cada uno
    repartirCarta(jugador, mazo, indiceMazo);
    repartirCarta(dealer, mazo, indiceMazo);
    repartirCarta(jugador, mazo, indiceMazo);
    repartirCarta(dealer, mazo, indiceMazo);
    cout << "Cartas iniciales repartidas." << endl;

    //boton "Hit"
    RectangleShape botonHit(Vector2f(100, 40));
    botonHit.setPosition(800, 40);
    botonHit.setFillColor(Color::Blue);

    Text textoHit("Hit", fuente, 20);
    textoHit.setFillColor(Color::White);
    textoHit.setPosition(820, 45);

    //boton "Stand"
    RectangleShape botonStand(Vector2f(100, 40));
    botonStand.setPosition(920, 40);
    botonStand.setFillColor(Color::Red);

    Text textoStand("Stand", fuente, 20);
    textoStand.setFillColor(Color::White);
    textoStand.setPosition(935, 45);
    //texto para jugador
    Text titulo("Tus Cartas", fuente, 28);
    titulo.setPosition(20, 15);
    titulo.setFillColor(Color::White);
    // Texto para dealer
    Text tituloDealer("Cartas del Dealer", fuente, 28);
    tituloDealer.setPosition(20, 450);
    tituloDealer.setFillColor(Color::White);

    //crear arreglos para mostrar cartas del jugador (maximo 12)
    Texture texturasJugador[12];
    Sprite spritesJugador[12];
    int cantidadCartasJugador = jugador.numCartas;
    //ajuste de posicion de cartas del dealer
    const float espacioCartas = 80.f;
    for (int i = 0; i < cantidadCartasJugador; i++) {
        string ruta = obtenerNombreArchivoCarta(jugador.mano[i]);
        if (!texturasJugador[i].loadFromFile(ruta)) {
            cout << "[ERROR] No se pudo cargar: " << ruta << endl;
        }

        spritesJugador[i].setTexture(texturasJugador[i]);
        spritesJugador[i].setScale(0.5f, 0.5f);
        spritesJugador[i].setPosition(200 + i * espacioCartas, 100);
    }
    
    // Crear arreglos para las cartas del dealer
    Texture texturasDealer[12];
    Sprite spritesDealer[12];
    int cantidadCartasDealer = dealer.numCartas;
    const float espaciocartasdealer = 60.f;
    for (int i = 0; i < cantidadCartasDealer; i++) {
        string ruta = obtenerNombreArchivoCarta(dealer.mano[i]);
        if (!texturasDealer[i].loadFromFile(ruta)) {
            cout << "[ERROR] No se pudo cargar: " << ruta << endl;
        }

        spritesDealer[i].setTexture(texturasDealer[i]);
        spritesDealer[i].setScale(0.3f, 0.3f);
        spritesDealer[i].setPosition(250 + i * espaciocartasdealer, 500); // Segunda fila
    }

    // Mostrar ventana del juego
    bool turnoJugadorActivo = true;
    bool dealerTermino = false;
    bool finDelJuego = false;
    Text resultado;
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left && turnoJugadorActivo) {
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (botonHit.getGlobalBounds().contains(mousePos)){
                    // HIT: pedir una carta
                    if (jugador.numCartas < 12 && turnoJugadorActivo){
                        repartirCarta(jugador, mazo, indiceMazo);

                        int i = jugador.numCartas - 1;
                        cantidadCartasJugador = jugador.numCartas;

                        string ruta = obtenerNombreArchivoCarta(jugador.mano[i]);
                        texturasJugador[i].loadFromFile(ruta);
                        spritesJugador[i].setTexture(texturasJugador[i]);
                        spritesJugador[i].setScale(0.5f, 0.5f); // 60% del tamaño original
                        spritesJugador[i].setPosition(200 + i * espacioCartas, 100);

                        // Ajustar por As si se quiere
                        if (jugador.mano[i].valor == "A") {
                            int valorElegido = elegirValorAs(window, fuente);
                            jugador.puntos -= 11; // se resta el valor por defecto
                            jugador.puntos += valorElegido;
                        }

                        if (jugador.puntos >= 21){
                            turnoJugadorActivo = false;
                            bool salirAlMenu = false;
                            cout << "turno del jugador terminado"<< endl;
                            string ganador = determinarGanador(jugador, dealer);
                            guardarHistorial(jugador.nombre, ganador);
                            mostrarResultadoFinal(window, fuente, ganador, jugador.nombre, salirAlMenu);
                        }
                            
                    }
                }// STAND: pasar turno al dealer
                if (botonStand.getGlobalBounds().contains(mousePos)) {
                    turnoJugadorActivo = false;
                    // Simular turno del dealer gráficamente
                    bool dealerTermino = false;
                    Text resultado;
                    while (dealer.puntos < 17 && dealerTermino == false) {
                        repartirCarta(dealer, mazo, indiceMazo);
                        int i = dealer.numCartas - 1;
                        cantidadCartasDealer = dealer.numCartas;

                        string ruta = obtenerNombreArchivoCarta(dealer.mano[i]);
                        texturasDealer[i].loadFromFile(ruta);
                        spritesDealer[i].setTexture(texturasDealer[i]);
                        spritesDealer[i].setScale(0.3f, 0.3f);
                        spritesDealer[i].setPosition(250 + i * espaciocartasdealer, 500);
                         // Ajustar por As
                        if (dealer.puntos > 21) {
                            for (int j = 0; j < dealer.numCartas; j++) {
                                if (dealer.mano[j].valor == "A" && dealer.puntos > 21)
                                    dealer.puntos -= 10;
                            }
                        }
                
                        window.clear(Color(41, 99, 23));
                        window.draw(titulo);
                        window.draw(tituloDealer);
                        for (int j = 0; j < cantidadCartasJugador; j++)
                            window.draw(spritesJugador[j]);

                        for (int j = 0; j < cantidadCartasDealer; j++)
                            window.draw(spritesDealer[j]);

                        window.draw(botonHit);
                        window.draw(textoHit);
                        window.draw(botonStand);
                        window.draw(textoStand);
                        window.display();

                        sleep(seconds(0.8f)); // Esperar 0.8 segundos para mostrar la carta
                    }

                    dealerTermino = true;
                    string ganador = determinarGanador(jugador, dealer);
                    guardarHistorial(jugador.nombre, ganador);

                    finDelJuego = true;
                    bool salirAlMenu = false;
                    mostrarResultadoFinal(window, fuente, ganador, jugador.nombre, salirAlMenu);
                }
            }
        }
                
        //Window.draw da vida a lo declarado antes

        window.clear(Color(41, 99, 23)); // fondo verde

        window.draw(titulo);
        window.draw(tituloDealer);

        // Dibujar cartas del jugador
        for (int i = 0; i < cantidadCartasJugador; ++i)
            window.draw(spritesJugador[i]);
        // Dibujar cartas del dealer
        for (int i = 0; i < cantidadCartasDealer; ++i)
            window.draw(spritesDealer[i]);

        window.draw(botonHit);
        window.draw(textoHit);
        window.draw(botonStand);
        window.draw(textoStand);

        if (finDelJuego) {
            window.draw(resultado);
        }
        window.display();
    }

    int totalPartidas = 0;
    Historial* historial = leerDesdeArchivo(totalPartidas);//guarda el historial en un arreglo dinamico de estructuras 
    cout << "Historial cargado. Total de partidas: " << totalPartidas << endl;

    if (historial == nullptr) {
        cout << "No se pudo abrir el historial";
    }
    delete[] mazo;
    delete[] jugador.mano;
    delete[] dealer.mano;
    delete[] historial;
    
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

// Sección añadida para permitir al jugador decidir el valor de un As
int elegirValorAs(RenderWindow &window, Font &fuente) {
    RectangleShape fondo(Vector2f(400, 200));
    fondo.setPosition(300, 250);
    fondo.setFillColor(Color(20, 20, 20, 230));
    fondo.setOutlineColor(Color::White);
    fondo.setOutlineThickness(3);

    Text mensaje("Has recibido un As.\n¿Que valor deseas asignar?", fuente, 22);
    mensaje.setPosition(320, 270);
    mensaje.setFillColor(Color::White);

    RectangleShape boton11(Vector2f(120, 40));
    boton11.setPosition(330, 340);
    boton11.setFillColor(Color(70, 130, 180));
    Text txt11("Contar como 11", fuente, 16);
    txt11.setPosition(335, 348);

    RectangleShape boton1(Vector2f(120, 40));
    boton1.setPosition(530, 340);
    boton1.setFillColor(Color(180, 100, 100));
    Text txt1("Contar como 1", fuente, 16);
    txt1.setPosition(540, 348);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                if (boton11.getGlobalBounds().contains(mousePos)) {
                    return 11;
                }
                if (boton1.getGlobalBounds().contains(mousePos)) {
                    return 1;
                }
            }
        }

        window.clear(Color(41, 99, 23));
        window.draw(fondo);
        window.draw(mensaje);
        window.draw(boton11);
        window.draw(txt11);
        window.draw(boton1);
        window.draw(txt1);
        window.display();
    }
    return 11; // por defecto
}
//mostrar el resultado final
void mostrarResultadoFinal(RenderWindow &parentWindow, Font &fuente, const string &ganador, const string &nombreJugador, bool &salirAlMenu) {
    RenderWindow ventanaResultado(VideoMode(500, 300), "Resultado de la Partida");

    Text mensaje("Ganador: " + ganador, fuente, 28);
    mensaje.setFillColor((ganador == nombreJugador) ? Color::Green : Color::Red);
    mensaje.setPosition(80, 60);

    RectangleShape btnVolverMenu(Vector2f(180, 40));
    btnVolverMenu.setPosition(60, 200);
    btnVolverMenu.setFillColor(Color(100, 100, 255));
    Text txtMenu("Menu Principal", fuente, 20);
    txtMenu.setPosition(75, 210);
    txtMenu.setFillColor(Color::White);

    RectangleShape btnJugarOtra(Vector2f(180, 40));
    btnJugarOtra.setPosition(260, 200);
    btnJugarOtra.setFillColor(Color(100, 255, 100));
    Text txtJugar("Jugar Otra Vez", fuente, 20);
    txtJugar.setPosition(275, 210);
    txtJugar.setFillColor(Color::White);

    while (ventanaResultado.isOpen()) {
        Event event;
        while (ventanaResultado.pollEvent(event)) {
            if (event.type == Event::Closed) {
                ventanaResultado.close();
            }
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = ventanaResultado.mapPixelToCoords(Mouse::getPosition(ventanaResultado));
                if (btnVolverMenu.getGlobalBounds().contains(mousePos)) {
                    ventanaResultado.close();
                    salirAlMenu = true;
                    // Volver al menu principal se gestiona desde el flujo del programa, no desde visibilidad.
                    parentWindow.close();
                    main(); // simplemente volverá al main loop y mostrará el menu principal
                }
                if (btnJugarOtra.getGlobalBounds().contains(mousePos)) {
                    ventanaResultado.close();
                    jugarBlackJack(parentWindow, fuente, nombreJugador);
                    return;
                }
            }
        }

        ventanaResultado.clear(Color::Black);
        ventanaResultado.draw(mensaje);
        ventanaResultado.draw(btnVolverMenu);
        ventanaResultado.draw(txtMenu);
        ventanaResultado.draw(btnJugarOtra);
        ventanaResultado.draw(txtJugar);
        ventanaResultado.display();
    }
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
