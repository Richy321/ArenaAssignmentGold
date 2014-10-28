#pragma once
namespace Arena
{
	class PhysicsObject;
	class Enemy;
	class Projectile;

	class IObjectPool
	{
	public:
		virtual ~IObjectPool() {}
		virtual void AddPhysicsObject(PhysicsObject* physObj) = 0;
		virtual void UpdatePhysicsObjects() = 0;

		virtual Enemy* GetEnemyObject() = 0;
		virtual Projectile* GetProjectileObject() = 0;

		virtual void DestroyActiveEnemyObject(Enemy *enemy) = 0;
		virtual void DestroyActiveProjectileObject(Projectile *proj) = 0;
	};
}