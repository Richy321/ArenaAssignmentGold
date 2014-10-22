#pragma once
namespace Arena
{
	class PhysicsObject;

	class IObjectPool
	{
	public:
		virtual ~IObjectPool() {}
		virtual void AddPhysicsObject(PhysicsObject* physObj) = 0;
		virtual void UpdatePhysicsObjects() = 0;
	};
}