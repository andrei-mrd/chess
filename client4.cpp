#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <cstring>
#include <map>

#define PORT 50000
#define BUFFER_SIZE 1024

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define SQUARE_SIZE (WINDOW_WIDTH / 8)

void draw_board(sf::RenderWindow *window, sf::RectangleShape *square) {
    window->clear(sf::Color::White);

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            square->setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            square->setPosition(col * SQUARE_SIZE, row * SQUARE_SIZE);
            if ((row + col) % 2 == 0)
                square->setFillColor(sf::Color::White);
            else
                square->setFillColor(sf::Color(118, 150, 86)); // Verde
            window->draw(*square);
        }
    }
    window->display();
}

void update_board(sf::RenderWindow *window, sf::RectangleShape *square, const char *move, sf::Color color) {
    int col = move[0] - 'a';
    int row = 8 - (move[1] - '0');

    square->setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
    square->setPosition(col * SQUARE_SIZE, row * SQUARE_SIZE);
    square->setFillColor(color);

    window->draw(*square);
    window->display();
}

void get_move_from_mouse(sf::RenderWindow *window, int *row, int *col) {
    sf::Event event;
    while (true) {
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
                return;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                *col = event.mouseButton.x / SQUARE_SIZE;
                *row = event.mouseButton.y / SQUARE_SIZE;
                return;
            }
        }
    }
}

int main() {
    // Crează socketul TCP
    sf::TcpSocket socket;
    sf::Socket::Status status;
    char buffer[BUFFER_SIZE];

    // Conectează la server
    if ((status = socket.connect("127.0.0.1", PORT, sf::seconds(10))) != sf::Socket::Done) {
        std::cerr << "Error connecting to server.\n";
        return EXIT_FAILURE;
    }

    // Primește culoarea (alb sau negru)
    size_t received;
    if (socket.receive(buffer, BUFFER_SIZE, received) != sf::Socket::Done) {
        std::cerr << "Error receiving color.\n";
        return EXIT_FAILURE;
    }

    buffer[received] = '\0';
    int isWhite = (strcmp(buffer, "A") == 0);
    std::cout << "Your color is: " << (isWhite ? "White" : "Black") << "\n";

    // Crează fereastra SFML
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), isWhite ? "Chess Board - White" : "Chess Board - Black", sf::Style::Resize | sf::Style::Close);
    if (!window.isOpen()) {
        std::cerr << "Error creating window.\n";
        return EXIT_FAILURE;
    }

    // Crează un dreptunghi pentru pătrățelele tablei
    sf::RectangleShape square;
    draw_board(&window, &square);

    sf::Texture pion_alb, pion_negru, *tura_alba, *tura_neagra
    , nebun_alb, nebun_negru, cal_alb, cal_negru, regina_alba, regina_neagra;

    if(!pion_alb.loadFromFile("assets/pion_alb.png")) {
        std::cout<<"eroare pion_alb";
    }

    sf::Sprite pionSprite;
    pionSprite.setTexture(pion_alb);

    sf::FloatRect bounds = pionSprite.getLocalBounds();

    float posX = 1 * SQUARE_SIZE + (SQUARE_SIZE - bounds.width) / 2;
    float posY = 1 * SQUARE_SIZE + (SQUARE_SIZE - bounds.height) / 2;

    // Setează poziția sprite-ului
    pionSprite.setPosition(posX, posY);

    window.draw(pionSprite);
    window.display();


    bool myTurn = isWhite;

    while (window.isOpen()) {
        if (myTurn) {
            // Mutarea jucătorului
            int row, col;
            std::cout << "Select your move with the mouse.\n";
            get_move_from_mouse(&window, &row, &col);

            snprintf(buffer, BUFFER_SIZE, "%c%d", 'a' + col, 8 - row);
            if (socket.send(buffer, std::strlen(buffer)) != sf::Socket::Done) {
                std::cerr << "Error sending move.\n";
                break;
            }
            std::cout << "Your move was sent: " << buffer << "\n";
            update_board(&window, &square, buffer, sf::Color::Red);

            myTurn = false;
        } else {
            // Așteaptă mutarea adversarului
            std::memset(buffer, 0, BUFFER_SIZE);
            while (true) {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                        return EXIT_FAILURE;
                    }
                }

                if (socket.receive(buffer, BUFFER_SIZE, received) == sf::Socket::Done) {
                    break;
                }
            }

            buffer[received] = '\0';
            std::cout << "Received opponent's move: " << buffer << "\n";
            update_board(&window, &square, buffer, sf::Color::Blue);

            myTurn = true;
        }
    }

    return 0;
}
