#pragma once
namespace Arena
{
	namespace PowerUps
	{
		class AdditionalBarrel;
		class Health;
	}
	class PhysicsObject;
	class Enemy;
	class Projectile;
	struct GameWorldContext;

	class IObjectPool
	{
	public:
		virtual ~IObjectPool() {}
		virtual void AddPhysicsObject(PhysicsObject* physObj) = 0;
		virtual void UpdatePhysicsObjects(GameWorldContext& context) = 0;

		virtual Enemy* GetEnemyObject() = 0;
		virtual void DestroyActiveEnemyObject(Enemy *enemy) = 0;
		virtual unsigned int GetActiveEnemyCount() = 0;

		virtual Projectile* GetProjectileObject() = 0;
		virtual void DestroyActiveProjectileObject(Projectile *proj) = 0;

		virtual PowerUps::AdditionalBarrel* GetAdditionalBarrelObject() = 0;
		virtual void DestroyActiveAdditionalBarrelObject(PowerUps::AdditionalBarrel *additionalBarrel) = 0;

		virtual PowerUps::Health* GetHealthObject() = 0;
		virtual void DestroyActiveHealthObject(PowerUps::Health *health) = 0;

		virtual octet::image* GetTexture(char* name) = 0;

	};
}