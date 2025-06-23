#include <SFML/Window.hpp>

int main(){
    sf::Window window(sf::VideoMode({800, 600}), "My window");

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
    return 0;
}

/*
C:\\mingw64\\bin\\g++ -c 21BLACKJACK.cpp -IC:\\SFML-2.6.1\\include -o build/21BLACKJACK.o
C:\\SFML-3.0.0\\include\\
C:\\mingw64\\bin\\g++ build/21BLACKJACK.o -LC:\\SFML-3.0.0\\lib -static -static-libstdc++ -lsfml-graphics-s -lsfml-system-s -lopengl32 -lfreetype -lgdi32 -lwinmm

*/