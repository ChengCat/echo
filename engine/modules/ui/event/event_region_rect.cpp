#include "event_region_rect.h"

namespace Echo
{
    UiEventRegionRect::UiEventRegionRect()
    {
    }
    
    UiEventRegionRect::~UiEventRegionRect()
    {
    }
    
    void UiEventRegionRect::bindMethods()
    {
		CLASS_BIND_METHOD(UiEventRegionRect, getWidth, DEF_METHOD("getWidth"));
		CLASS_BIND_METHOD(UiEventRegionRect, setWidth, DEF_METHOD("setWidth"));
		CLASS_BIND_METHOD(UiEventRegionRect, getHeight, DEF_METHOD("getHeight"));
		CLASS_BIND_METHOD(UiEventRegionRect, setHeight, DEF_METHOD("setHeight"));

		CLASS_REGISTER_PROPERTY(UiEventRegionRect, "Width", Variant::Type::Int, "getWidth", "setWidth");
		CLASS_REGISTER_PROPERTY(UiEventRegionRect, "Height", Variant::Type::Int, "getHeight", "setHeight");
    }

	void UiEventRegionRect::setWidth(i32 width)
	{
		if (m_width != width)
		{
			m_width = width;

			updateLocalAABB();
		}
	}

	void UiEventRegionRect::setHeight(i32 height)
	{
		if (m_height != height)
		{
			m_height = height;

			updateLocalAABB();
		}
	}

	bool UiEventRegionRect::isIntersect(const Ray& ray)
	{
		AABB worldBox;
		buildWorldAABB(worldBox);

		return worldBox.isValid() ? ray.hitBox(worldBox) : false;
	}

	bool UiEventRegionRect::getHitPosition(const Ray& ray, Vector3& worldPos)
	{
		AABB worldBox;
		buildWorldAABB(worldBox);
		if (worldBox.isValid())
		{
			float min;
			Ray::HitInfo hitInfo;
			if (ray.hitBox(worldBox, min, hitInfo))
			{
				worldPos = hitInfo.hitPos;
				return true;
			}
		}

		return false;
	}

	bool UiEventRegionRect::notifyClicked(const Ray& ray, const Vector2& screenPos)
	{
		Vector3 worldPos;
		if (getHitPosition(ray, worldPos))
		{
			m_mouseEvent.setScreenPosition(screenPos);
			m_mouseEvent.setWorldPosition(worldPos);
			m_mouseEvent.setLocalPosition(worldPos - getWorldPosition());

			clicked();

			return true;
		}

		return false;
	}

	void UiEventRegionRect::updateLocalAABB()
	{
		Vector3 halfUp = Vector3::UNIT_Y * getHeight() * 0.5f;
		Vector3 halfRight = Vector3::UNIT_X * getWidth() * 0.5f;
		Vector3 center = Vector3::ZERO;

		Vector3 v0 = center - halfRight + halfUp;
		Vector3 v1 = center - halfRight - halfUp;
		Vector3 v2 = center + halfRight - halfUp;
		Vector3 v3 = center + halfRight + halfUp;

		m_localAABB.reset();
		m_localAABB.addPoint(v0);
		m_localAABB.addPoint(v1);
		m_localAABB.addPoint(v2);
		m_localAABB.addPoint(v3);
	}
}
