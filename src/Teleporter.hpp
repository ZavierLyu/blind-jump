//========================================================================//
// Copyright (C) 2016 Evan Bowman                                         //
// Liscensed under GPL 3, see: <http://www.gnu.org/licenses/>.            //
//========================================================================//

#pragma once

#include "Drawable.hpp"
#include "effectsController.hpp"
#include "GfxContext.hpp"

namespace detail {
    template <typename DrawPolicy>
    class Teleporter : private DrawPolicy, public framework::Object {
    public:
        void draw(GfxContext & gfxContext, const sf::View & view) {
            DrawPolicy::draw(*this, gfxContext, view);
        }
        static const int drawOffset = 0;
        Teleporter(float _xInit, float _yInit, const sf::Texture & mainTxtr,
                   const sf::Texture & glowTxtr)
            : Object(_xInit, _yInit), smokeTimer(0) {
                for (int i = 0; i < 2; i++) {
                    TeleporterSprites[i].setTexture(mainTxtr);
                }
                glowSprite.setTexture(glowTxtr);
                glowSprite.setOrigin(200, 200);
                TeleporterSprites[0].setTextureRect(sf::IntRect(390, 224, 34, 29));
                TeleporterSprites[1].setTextureRect(sf::IntRect(174, 195, 34, 29));
                glowSprite.setPosition(_xInit + 16, _yInit + 13);
                TeleporterSprites[0].setPosition(_xInit - 3, _yInit);
                TeleporterSprites[1].setPosition(_xInit - 3, _yInit + 4);
            }
        const sf::Sprite & getShadow() const {
            return TeleporterSprites[1];
        }
        const sf::Sprite & getSprite() const {
            return TeleporterSprites[0];
        }
        bool smokeReady();
        template <typename Game>
        void update(const sf::Time &, Game *) {
        
        }
        const sf::Sprite & getGlow() const { return glowSprite; }

    private:
        mutable sf::Sprite glowSprite;
        mutable sf::Sprite TeleporterSprites[2];
        int32_t smokeTimer;
    };
}
