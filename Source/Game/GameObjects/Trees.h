#pragma once

#include "IGameObject.h"
#include <memory>
#include <array>
#include <string>

namespace RenderEngine
{
	class Sprite;
}

class Trees : public IGameObject
{
public: 

	enum class ETankType : uint8_t
	{
        Player1Yellow_type1,
        Player1Yellow_type2,
        Player1Yellow_type3,
        Player1Yellow_type4,

        Player2Green_type1,
        Player2Green_type2,
        Player2Green_type3,
        Player2Green_type4,

        EnemyWhite_type1,
        EnemyWhite_type2,
        EnemyWhite_type3,
        EnemyWhite_type4,

        EnemyGreen_type1,
        EnemyGreen_type2,
        EnemyGreen_type3,
        EnemyGreen_type4,

        EnemyRed_type1,
        EnemyRed_type2,
        EnemyRed_type3,
        EnemyRed_type4
	};

    inline static std::string TankTypeToSpriteString[] =
    {
        "player1_yellow_tank_type1_sprite",
        "player1_yellow_tank_type2_sprite",
        "player1_yellow_tank_type3_sprite",
        "player1_yellow_tank_type4_sprite",

        "player2_green_tank_type1_sprite",
        "player2_green_tank_type2_sprite",
        "player2_green_tank_type3_sprite",
        "player2_green_tank_type4_sprite",

        "enemy_white_tank_type1_sprite",
        "enemy_white_tank_type2_sprite",
        "enemy_white_tank_type3_sprite",
        "enemy_white_tank_type4_sprite",

        "enemy_green_tank_type1_sprite",
        "enemy_green_tank_type2_sprite",
        "enemy_green_tank_type3_sprite",
        "enemy_green_tank_type4_sprite",

        "enemy_red_tank_type1_sprite",
        "enemy_red_tank_type2_sprite",
        "enemy_red_tank_type3_sprite",
        "enemy_red_tank_type4_sprite"
    };

	enum class EBlockLocation : uint8_t
	{
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight
	};

	Trees(const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer);
	virtual void render() const override;

private:
	void renderBlock(const EBlockLocation eBlockLocation) const;

	std::shared_ptr<RenderEngine::Sprite> m_pSprite;
	const std::array<glm::vec2, 4> m_blockOffsets;
};
