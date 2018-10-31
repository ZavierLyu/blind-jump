#include "tileController.hpp"
#include "ResourcePath.hpp"
#include "drawPixels.hpp"
#include "initMapVectors.hpp"
#include "mappingFunctions.hpp"
#include "resourceHandler.hpp"
#include "turret.hpp"
#include <cstring>
#include <random>

// This code could be much cleaner, but it works...
// The class is called tile controller for historical reasons, it used to handle
// actual map tiles
// For performance reasons, I'm grouping all the tiles into a single texture

std::vector<Coordinate> * tileController::getEmptyLocations() {
    return &emptyMapLocations;
}

float tileController::getPosX() const { return posX; }

float tileController::getPosY() const { return posY; }

void createMapImage(const sf::Image & tileImage, Tile mapArray[61][61],
                    sf::Texture tx[2], const sf::Image & grassSet,
                    const sf::Image & grassSetEdge) {
    Tile mapTemp[61][61];
    uint8_t bitMask[61][61], gratePositions[61][61];
    std::memset(mapTemp, 0, sizeof(mapTemp[0][0]) * std::pow(61, 2));
    std::memset(bitMask, 0, sizeof(bitMask[0][0]) * std::pow(61, 2));
    std::memset(gratePositions, 0,
                sizeof(gratePositions[0][0]) * std::pow(61, 2));
    for (int i = 0; i < 61; i++) {
        for (int j = 0; j < 61; j++) {
            if (mapArray[i][j] == Tile::Plate && !rng::random<11>()) {
                gratePositions[i][j] = 1;
            }
        }
    }
    // Now smooth the grate positions
    int count;
    // Run 2 repetitions of smoothing
    for (int rep = 2; rep > 0; rep--)
        for (int i = 1; i < 60; i++) {
            for (int j = 1; j < 60; j++) {
                count = gratePositions[i - 1][j] + gratePositions[i + 1][j] +
                        gratePositions[i][j - 1] + gratePositions[i][j + 1];
                if (count && !rng::random<3>()) {
                    gratePositions[i][j] = 1;
                }
            }
        }
    // Now if the map array contains a grass tile, set the temporary map value
    // to 1
    for (int i = 0; i < 61; i++) {
        for (int j = 0; j < 61; j++) {
            if (mapArray[i][j] == Tile::Grass ||
                mapArray[i][j] == Tile::GrassFlowers ||
                mapArray[i][j] == Tile::GrassUpperEdge ||
                mapArray[i][j] == Tile::GrassLowerEdge ||
                mapArray[i][j] == Tile::_UNUSED1_) {
                mapTemp[i][j] = Tile::Wall;
            }
        }
    }
    // Now loop through each index of the temporary map and set the value of the
    // bit mask according to nearby element values
    for (int i = 0; i < 61; i++) {
        for (int j = 0; j < 61; j++) {
            if (mapTemp[i][j] == Tile::Wall) {
                bitMask[i][j] += 1 * static_cast<int>(mapTemp[i][j - 1]);
                if (mapArray[i + 1][j] != Tile::GrassLowerEdge &&
                    mapArray[i + 1][j] != Tile::GrassUpperEdge) {
                    bitMask[i][j] += 2 * static_cast<int>(mapTemp[i + 1][j]);
                }
                bitMask[i][j] += 4 * static_cast<int>(mapTemp[i][j + 1]);
                if (mapArray[i - 1][j] != Tile::GrassUpperEdge &&
                    mapArray[i - 1][j] != Tile::GrassLowerEdge)
                    bitMask[i][j] += 8 * static_cast<int>(mapTemp[i - 1][j]);
            }
        }
    }
    // At this point the bitmap hash values are ready for the map image
    // generation code to interpret them

    sf::Image tileMap, tileMapEdge;
    // Create an image of the size 61 * tileWidth (32), by 61 * tile height (26)
    tileMap.create(1952, 1586, sf::Color::Transparent);
    tileMapEdge.create(1952, 1586, sf::Color::Transparent);
    // Loop through all indices of the map array and copy the corresponding
    // pixels from the tileset to the image
    for (int i = 10; i < 50; i++) {
        for (int j = 10; j < 50; j++) {
            int select = rng::random<3>();
            switch (mapArray[i][j]) {
            case Tile::Plate:
                if (gratePositions[i][j] != 1) {
                    drawPixels(tileMap, tileImage, i, j, 0, 0);
                } else {
                    drawPixels(tileMap, tileImage, i, j, 256, 0);
                }
                break;

            case Tile::Sand:
                drawPixels(tileMap, tileImage, i, j, 32, 0);
                break;

            case Tile::SandAndGrass:
                drawPixels(tileMap, tileImage, i, j, 64, 0);
                break;

            case Tile::PlateLowerEdge:
                if (select == 2) {
                    drawPixels(tileMapEdge, tileImage, i, j, 96, 0);
                } else if (select == 1) {
                    drawPixels(tileMapEdge, tileImage, i, j, 288, 0);
                } else {
                    drawPixels(tileMapEdge, tileImage, i, j, 320, 0);
                }
                break;

            case Tile::PlateUpperEdge:
                drawPixels(tileMap, tileImage, i, j, 128, 0);
                break;

            case Tile::Grass:
                drawPixels(tileMap, tileImage, i, j, 0, 0);
                if (select != 2) {
                    drawPixels(tileMap, grassSetEdge, i, j, bitMask[i][j] * 32,
                               0);
                } else {
                    drawPixels(tileMap, grassSet, i, j, bitMask[i][j] * 32, 0);
                }
                break;

            case Tile::GrassFlowers:
                drawPixels(tileMap, tileImage, i, j, 32, 0);
                if (select != 2) {
                    drawPixels(tileMap, grassSetEdge, i, j, bitMask[i][j] * 32,
                               0);
                } else {
                    drawPixels(tileMap, grassSet, i, j, bitMask[i][j] * 32, 0);
                }
                break;

            case Tile::GrassLowerEdge:
                if (select != 2) {
                    drawPixels(tileMapEdge, tileImage, i, j, 192, 0);
                } else {
                    drawPixels(tileMapEdge, tileImage, i, j, 160, 0);
                }
                break;

            case Tile::GrassUpperEdge:
                drawPixels(tileMap, tileImage, i, j, 224, 0);
                break;

            case Tile::Grate:
                drawPixels(tileMap, tileImage, i, j, 256, 0);
                break;

            default:
                break;
            }
        }
    }
    tx[0].loadFromImage(tileMap);
    tx[1].loadFromImage(tileMapEdge);
}

tileController::tileController() : posX(-72), posY(-476) {
    transitionLvSpr.setTexture(
        getgResHandlerPtr()->getTexture(ResHandler::Texture::introLevel));
    shadow.setFillColor(sf::Color(188, 188, 198, 255));
}

void tileController::update() {
    for (auto & element : walls) {
        element.setPosition(element.getXinit() + posX,
                            element.getYinit() + posY);
    }
    transitionLvSpr.setPosition(posX, posY);
    mapSprite1.setPosition(posX, posY);
    mapSprite2.setPosition(posX, posY);
}

void tileController::draw(sf::RenderTexture & window,
                          std::vector<sf::Sprite> * glowSprites, int level,
                          const sf::View & worldView,
                          const sf::View & cameraView) {
    // Clear out the RenderTexture
    rt.setView(cameraView);
    rt.clear(sf::Color::Transparent);
    // Draw the map sprite to the texture
    if (level != 0) {
        rt.draw(mapSprite1);
    } else {
        rt.draw(transitionLvSpr);
    }
    // Draw a shadow over everything
    rt.setView(worldView);
    rt.draw(shadow, sf::BlendMultiply);
    rt.setView(cameraView);
    // Draw glow sprites
    for (auto & element : *glowSprites) {
        rt.draw(element, sf::BlendMode(sf::BlendMode(
                             sf::BlendMode::SrcAlpha, sf::BlendMode::One,
                             sf::BlendMode::Add, sf::BlendMode::DstAlpha,
                             sf::BlendMode::Zero, sf::BlendMode::Add)));
    }
    rt.display();
    re.setView(cameraView);
    re.clear(sf::Color::Transparent);
    if (level != 0) {
        re.draw(mapSprite2);
    }
    re.setView(worldView);
    re.draw(shadow, sf::BlendMultiply);
    re.display();
    // Draw the whole thing to the window
    window.draw(sf::Sprite(rt.getTexture()));
    window.draw(sf::Sprite(re.getTexture()));
}

// Set the center position according to the window width and height
void tileController::setPosition(float X, float Y) {
    posX += X;
    posY += Y;
}

// Empty all of the containers to prepare for pushing back a new map set
void tileController::clear() {
    walls.clear();
    emptyMapLocations.clear();
}

void tileController::rebuild(Tileset set) {
    switch (set) {
    case Tileset::intro:
        posX = -72;
        posY = -476;
        shadow.setFillColor(sf::Color(188, 188, 198, 255));
        break;

    case Tileset::regular:
        shadow.setFillColor(sf::Color(188, 188, 198, 255));
        createMapImage(
            getgResHandlerPtr()->getImage(ResHandler::Image::soilTileset),
            mapArray, mapTexture,
            getgResHandlerPtr()->getImage(ResHandler::Image::grassSet1),
            getgResHandlerPtr()->getImage(ResHandler::Image::grassSet2));
        initMapVectors(this);
        mapSprite1.setTexture(mapTexture[0]);
        mapSprite2.setTexture(mapTexture[1]);
        break;
    }
}

void tileController::setWindowSize(float w, float h) {
    rt.create(w, h);
    re.create(w, h);
    sf::Vector2f v;
    v.x = w;
    v.y = h;
    shadow.setSize(v);
}

Coordinate tileController::getTeleporterLoc() { return teleporterLocation; }

void tileController::reset() {}
