#pragma once
/*
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "json.hpp"

#include "State.h"

namespace badEngine {

	class Player;
	class Enemy;

	typedef HKey::ENTITY_TYPE EntityType;
	typedef std::shared_ptr<SDL_Texture> pTexture;
	typedef std::shared_ptr<const std::vector<TSA::Reel>> pAnimations;


	class EntityFactory {

		std::map<EntityType, pTexture>          textures;
		std::map<EntityType, pAnimations>       animations;
		std::map<EntityType, TSA::TTransfer>    ttransfers;
		std::map<EntityType, CCP::HitBox>       hitboxes;

		bool isInit = false;

		bool componentValidationPhase(const nlohmann::json* const entityConfig, SDL_Renderer* renderer);
		void componentConstructionPhase(const nlohmann::json& data, SDL_Renderer* renderer, const EntityType key, const std::string& keyStr);


		bool	  constructComponent_Texture(const nlohmann::json& data, SDL_Renderer* renderer, const EntityType key, const std::string& keyStr);
		void	  constructComponent_TTransfer(const nlohmann::json& data, const EntityType key);
		SDL_FRect constructComponent_TTransfer(const nlohmann::json& data, const char* field);
		void	  constructComponent_Animations(const nlohmann::json& data, const EntityType key, const std::string& keyStr);
		void	  constructComponent_Hitbox(const nlohmann::json& data, const EntityType key, const std::string& keyStr);

	public:

		EntityFactory(const nlohmann::json* const entityConfig, SDL_Renderer* renderer);

		bool isInitalized()const;
		void wipeMemory();
		Player* createPlayer(EntityType key)const;		     	//caller is the owner, can return nullptr
		Enemy* createEnemy(EntityType key)const;			    //caller is the owner, can return nullptr

		pTexture             getTexture(EntityType key)const;
		TSA::TTransfer       getTTransfer(EntityType key)const;
		pAnimations          getAnimation(EntityType key)const;
		CCP::HitBox          getHitbox(EntityType key)const;

	public:
		EntityFactory(const EntityFactory&) = delete;
		EntityFactory(EntityFactory&&)noexcept = delete;
		EntityFactory& operator=(const EntityFactory&) = delete;
		EntityFactory& operator=(EntityFactory&&)noexcept = delete;
	};

	class Player {

	public:

		TSA::Sprite sprite;
		CCP::HitBox hitbox;
		SMS::State state;

		Player(const TSA::Sprite& sprite, const CCP::HitBox& hitbox);

		bool WASD_PlayerVelocity(const float moveSpeed);
	};

	class Enemy {
	public:

		TSA::Sprite sprite;
		CCP::HitBox hitbox;

		Enemy(const TSA::Sprite& sprite, const CCP::HitBox& hitbox);
	};

	void setCoordinates(CCP::HitBox& hitbox);
	SMS::Facing facingDirection(const SDL_FPoint& velocity);
	bool isMoving(const SDL_FPoint& velocity);
	TSA::AnimationID animationIDTable(const SMS::State& state);
}
*/