#include <SFML/Graphics.hpp>
#include <iostream>

/*
g++ -Wall -Wextra -Werror game.cpp -o app.exe -lsfml-graphics -lsfml-window -lsfml-system
.\app.exe
*/

void setSize(sf::Sprite& sprite, int width, int height) {
    auto dim = sprite.getLocalBounds();
    sprite.setScale((float) width / dim.width, (float) height / dim.height);
}

bool isWall(int i, int j) {
    bool isCorner = (i < 2) || (j == 0) || (i == 15) || (j == 27 && i != 7 && i != 8);
    bool isTrees1 = (j == 7 && i > 0 && i < 5) || (j == 10 && i > 0 && i < 5) || (j > 7 && j < 10 && i == 5);
    bool isTrees2 = (j == 16 && i > 0 && i < 5) || (j == 19 && i > 0 && i < 5) || (j > 16 && j < 19 && i == 5);
    bool isTrees3 = (j == 24 && i > 9) || (j > 24 && i == 9);
    bool isTrees4 = (j == 10 && i > 10) || (i == 10 && j > 10 && j != 15) || (j > 11 && j < 15 && i == 9) || (i >= 8 && j == 14);
    bool isTrees5 = (j == 25 && i < 6) || (j == 26 && i == 6) || (j == 25 && i > 0 && i < 4);
    bool isTrees6 = (j < 3 && i > 5 && i < 8) || (i > 7 && i < 10 && j > 19 && j < 23) || (i > 5 && i < 8 && j == 21);
    bool arbust = (j > 4 && j < 8 && i == 7);
    bool stump = (j == 12 && i == 5) || (j == 11 && i == 3);
    bool lake = (i > 9 && i < 13 && j > 2 && j < 8); 

    if (isCorner || isTrees1 || isTrees2 || isTrees3 || isTrees4 || isTrees5 || isTrees6 || arbust || stump || lake) {
        return true;
    }

    return false;
}

struct Entity {
    int x {0};
    int y {0};
    int step {0};
    sf::Sprite sprite;

    Entity(int x, int y, int step, sf::Texture& texture): 
        x {x}, y {y}, step {step}, sprite(texture) {
    }

    void draw(sf::RenderWindow& window) {
        this->sprite.setPosition(x * step, y * step);
        setSize(this->sprite, step, step);
        window.draw(this->sprite);
    }
};

struct Board {
    int nc {0};
    int nl {0};
    int step {0};
    sf::Sprite sprite;
    std::vector<std::vector<bool>> walls;

    Board(int nl, int nc, int step, sf::Texture& texture) {
        this->nl = nl;
        this->nc = nc;
        this->step = step;
        this->sprite.setTexture(texture);
        setSize(this->sprite, nc * step, nl * step);

        for (int i = 0; i < nl*2; i++) {
            std::vector<bool> row;
            for (int j = 0; j < nc*2; j++) {
                row.push_back(false);
            }
            this->walls.push_back(row);
        }
        // this->walls.resize(nc, std::vector<bool>(nl, false));
        // this->sprite.setPosition(0, 0);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(this->sprite);

        for (int i = 0; i < nl*2; i++) {
            for (int j = 0; j < nc*2; j++) {
                if (isWall(i, j)) {
                    walls[i][j] = true;
                    sf::RectangleShape wallRect;
                    wallRect.setSize(sf::Vector2f(step/2, step/2));
                    wallRect.setPosition(j * step/2, i * step/2);
                    wallRect.setFillColor(sf::Color::Transparent);
                    wallRect.setOutlineColor(sf::Color::Blue);
                    wallRect.setOutlineThickness(2);
                    // window.draw(wallRect);
                }
            }
        }
    }
};

void moveEntity(sf::Keyboard::Key key, Entity& entity, std::vector<sf::Keyboard::Key> move_keys, const Board& board) {

    int dx = 0;
    int dy = 0;

    if (key == move_keys[0])
        dx--;
    else if (key == move_keys[1])
        dy--;
    else if (key == move_keys[2])
        dx++;
    else if (key == move_keys[3])
        dy++;

    int newX = entity.x + dx;
    int newY = entity.y + dy;

    if (board.walls[newY][newX]) {
        return;
    }

    entity.x = newX;
    entity.y = newY;
}
    

sf::Texture loadTexture(std::string path) {
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        std::cout << "Error loading texture\n";
        exit(1);
    }
    return texture;
}

int main() {

    sf::Texture wolf_tex {loadTexture("lobol.png")};
    sf::Texture rabbit_tex {loadTexture("coelho.png")};
    sf::Texture grass_tex {loadTexture("SecretWoods.png")};

    const int STEP {25};

    Entity wolf(8, 9, STEP, wolf_tex);
    Entity rabbit(12, 7, STEP, rabbit_tex);
    Board board(8, 14, STEP * 2, grass_tex);

    sf::RenderWindow window(sf::VideoMode(board.nc * STEP * 2, board.nl * STEP * 2), "SFML works!");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                moveEntity(event.key.code, wolf, {sf::Keyboard::Left, sf::Keyboard::Up, sf::Keyboard::Right, sf::Keyboard::Down}, board);
                moveEntity(event.key.code, rabbit, {sf::Keyboard::A, sf::Keyboard::W, sf::Keyboard::D, sf::Keyboard::S}, board);
            }
        }

        window.clear();
        board.draw(window);
        wolf.draw(window);
        rabbit.draw(window);
        window.display();
    }

    return 0;
}