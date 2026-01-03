#include "Ray.h"

namespace badEngine {

	//maybe depricate if literture does it differently
	Hit sweep_dynamic(const float4& dynamicBox, const float2& dynamicDir, const float4& staticBox) noexcept
	{
		//expanded rectangle must also be with the consideration of relative velocity
		const float4 expandedTarget = float4(
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