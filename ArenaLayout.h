#pragma once
#include "Floor.h"
#include "Wall.h"
#include "Column.h"

namespace Arena
{
	/// Manages all static arena objects and randomised locations within the arena
	class ArenaLayout
	{
	private:
		///Generate a wall section with given inputs. Automatically adds to the world.
		void GenerateWall(octet::vec3 size, octet::mat4t worldTransform, GameWorldContext &context)
		{
			Wall *wall = new Wall(worldTransform, size, context);
			wall->addPhysicsObjectToWorld(context);
			context.objectPool.AddPhysicsObject(wall);
		}

		///Generate a column section with given inputs. Automatically adds to the world.
		void GenerateColumn(float radius, float halfExtents, octet::mat4t worldTransform, GameWorldContext &context)
		{
			Column *col = new Column(worldTransform, radius, halfExtents, context);
			col->addPhysicsObjectToWorld(context);
			context.objectPool.AddPhysicsObject(col);
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
			delete floor;
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
				
			float spawnOffset = 10.0f;

			spawnPoints.push_back(octet::vec3(0.0f, 0.0f, 0.0f)); //middle
			spawnPoints.push_back(octet::vec3(-width + wallThickness + spawnOffset, 0.0f, height - wallThickness - spawnOffset)); //top left
			spawnPoints.push_back(octet::vec3(width - wallThickness - spawnOffset, 0.0f, height - wallThickness - spawnOffset)); // top right
			spawnPoints.push_back(octet::vec3(-width + wallThickness + spawnOffset, 0.0f, -height + wallThickness + spawnOffset)); //bottom left
			spawnPoints.push_back(octet::vec3(width - wallThickness - spawnOffset, 0.0f, -height + wallThickness + spawnOffset)); // bottom right
		}
		
		const float GetWidth(){ return width; }
		const float GetHeight(){ return height; }

		const octet::vec3 GetRandomSpawnPoint(unsigned int &spawnIndex)
		{
			spawnIndex = rnd.get(0, spawnPoints.size() - 1);
			return spawnPoints[spawnIndex];
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

