#pragma once
#include "Floor.h"
#include "Wall.h"
#include "Column.h"

namespace Arena
{
	class ArenaLayout
	{
	private:
		void GenerateWall(octet::vec3 size, octet::mat4t worldTransform, GameWorldContext &context)
		{
			Wall *wall = new Wall(worldTransform, size, context);
			wall->addPhysicsObjectToWorld(context);
			context.objectPool.AddPhysicsObject(wall);
		}

		void GenerateColumn(float radius, float halfExtents, octet::mat4t worldTransform, GameWorldContext &context)
		{
			Column *col = new Column(worldTransform, radius, halfExtents, context);
			col->addPhysicsObjectToWorld(context);
			context.objectPool.AddPhysicsObject(col);
		}

		void GenerateRandomWall()
		{

		}

		void GenerateRandomColumn()
		{

		}

	public:
		Floor* floor = nullptr;
		
		const float wallHeight = 10.0f;
		const float wallThickness = 5.0f;

		octet::dynarray<octet::vec3> spawnPoints;

		octet::random rnd;
		float width;
		float height;

		ArenaLayout(float width, float height, GameWorldContext &context)
		{
			Initialise(width, height, context);
		}

		virtual ~ArenaLayout()
		{
		}

		void Initialise(float width, float height, GameWorldContext &context)
		{
			floor = new Floor(width, height, context);
			this->height = height;
			this->width = width;

			rnd = octet::random(context.timer.GetTime());

			floor->addPhysicsObjectToWorld(context);
			context.objectPool.AddPhysicsObject(floor);

			octet::mat4t worldTransform;
			octet::vec3 size;

			for (int i = 0; i < 4; i++)
			{
				worldTransform.loadIdentity();

				switch (i)
				{
				case 0: //North wall
					size = octet::vec3(width + wallThickness, wallHeight, wallThickness);
					worldTransform.translate(0.0f, floor->thickness*0.5f, height);
					break;
				case 1: //South wall
					size = octet::vec3(width + wallThickness, wallHeight, wallThickness);
					worldTransform.translate(0.0f, floor->thickness*0.5f, -height);
					break;
				case 2: //East wall
					size = octet::vec3(wallThickness, wallHeight, height - wallThickness);
					worldTransform.translate(width, floor->thickness*0.5f, 0.0f);
					break;
				case 3: //West wall
					size = octet::vec3(wallThickness, wallHeight, height - wallThickness);
					worldTransform.translate(-width, floor->thickness*0.5f, 0.0f);
					break;
				}
				GenerateWall(size, worldTransform, context);
			}

				worldTransform.loadIdentity();
				worldTransform.translate(-10.0f, 0.0f, 10.0f);
				GenerateColumn(5.0f, wallHeight, worldTransform, context);

				worldTransform.loadIdentity();
				worldTransform.translate(10.0f, 0.0f, 10.0f);
				GenerateColumn(5.0f, wallHeight, worldTransform, context);

				worldTransform.loadIdentity();
				worldTransform.translate(10.0f, 0.0f, -10.0f);
				GenerateColumn(5.0f, wallHeight, worldTransform, context);

				worldTransform.loadIdentity();
				worldTransform.translate(-10.0f, 0.0f, -10.0f);
				GenerateColumn(5.0f, wallHeight, worldTransform, context);

				spawnPoints.push_back(octet::vec3(0.0f, 0.0f, 0.0f));
		}
		
		const float GetWidth(){ return width; }
		const float GetHeight(){ return height; }

		const octet::vec3 GetRandomSpawnPoint()
		{
			return spawnPoints[rnd.get(0, spawnPoints.size() - 1)];
		}

		octet::vec3 GetRandomSpawnLocation()
		{
			float edgeGap = wallThickness + 0.2f;
			float rndX = rnd.get(-(GetWidth() - edgeGap), GetWidth() - edgeGap);
			float rndY = rnd.get(-(GetHeight() - edgeGap), GetHeight() - edgeGap);
			return octet::vec3(rndX, 0.5f, rndY);
		}
	};
}

