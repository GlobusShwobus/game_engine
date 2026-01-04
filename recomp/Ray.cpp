#include "Ray.h"

namespace badEngine {

	//maybe depricate if literture does it differently
	Hit sweep_dynamic(const AABB& dynamicBox, const float2& dynamicDir, const AABB& staticBox) noexcept
	{
		//expanded rectangle must also be with the consideration of relative velocity
		const AABB expandedTarget = AABB(
			staticBox.x - (dynamicBox.w / 2),
			staticBox.y - (dynamicBox.h / 2),
			staticBox.w + dynamicBox.w,
			staticBox.h + dynamicBox.h
		);
		Ray ray = Ray(
			dynamicBox.get_center_point(),
			dynamicDir
		);
		Hit hit;
		sweep(ray, expandedTarget, hit);
		return {};
	}
}