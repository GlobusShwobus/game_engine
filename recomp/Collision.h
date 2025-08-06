#pragma once

/*

Need entities as well

do collisions here

collisions between single components

collisions between lists

resolutions

quadtree optimization

*/

/*
		void resolveAABB_overlapSecond(SDL_FRect& rect, const SDL_Rect& mutator)
		{
			if (mutator.w < mutator.h)
				rect.x += mutator.w;
			else
				rect.y += mutator.h;
		}
		void resolveAABB_overlapFirst(SDL_FRect& rect, const SDL_Rect& mutator)
		{
			if (mutator.w < mutator.h)
				rect.x -= mutator.w;
			else
				rect.y -= mutator.h;
		}
		void resolveOutOfBounds(const SDL_FRect& bounds, SDL_FRect& rect)
		{
			if (rect.x < bounds.x)
				rect.x = bounds.x;
			if (rect.y < bounds.y)
				rect.y = bounds.y;
			if (rect.x + rect.w > bounds.w)
				rect.x = bounds.w - rect.w;
			if (rect.y + rect.h > bounds.h)
				rect.y = bounds.h - rect.h;
		}
*/

/*
RECTANGLE (or any entity) PROPER COLLISION TEMPLATE, 2 STAGED

        SDL_FPoint cp1{ 0,0 };
        SDL_FPoint cn1{ 0,0 };
        float hitTime1 = 0;

        struct Collider {
            Transform* rect1 = nullptr;
            Transform* rect2 = nullptr;
            SDL_FPoint contactNormal{ 0,0 };
            //float hitTime = 0;
        };
        std::vector<Collider> colliders;

        for (int i = 0; i < formers.size(); i++) {
            for (int j = i + 1; j < formers.size(); j++) {
                SDL_FPoint cp1{ 0,0 };
                SDL_FPoint cn1{ 0,0 };
                float hitTime1 = 0;

                if (formers[i].dynamicSweptAABB(formers[j], cp1, cn1, hitTime1)) {
                    colliders.push_back({&formers[i], &formers[j], cn1});
                }
            }
        }
        for (auto& move : formers) {
            move.noResolutionMove();
        }

        for (auto& col : colliders) {
            if (col.rect1->staticAABBOverlap(*col.rect2)) {
                SDL_FPoint normal = col.contactNormal;
                col.rect1->reflectVelocity(normal);
                Transform::flipNormalized(normal);
                col.rect2->reflectVelocity(normal);

            }
        }

        for (auto& each : formers) {
            if (!worldBB.containsRect(each.rect)) {
                Transform::clampInOf(worldBB.rect, each.rect);
                each.velocity.x *= -1;
                each.velocity.y *= -1;
            }
        }
*/