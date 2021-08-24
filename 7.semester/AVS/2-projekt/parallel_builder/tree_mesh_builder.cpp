/**
 * @file    tree_mesh_builder.cpp
 *
 * @author  Matus Liscinsky <xlisci02@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    15.12.2019
 **/

#include <iostream>
#include <math.h>
#include <limits>

#include "tree_mesh_builder.h"


#define MIN_GRID_SIZE = 10

TreeMeshBuilder::TreeMeshBuilder(unsigned gridEdgeSize)
	: BaseMeshBuilder(gridEdgeSize, "Octree")
{

}

unsigned TreeMeshBuilder::octree_func(const ParametricScalarField &field, unsigned gridSize, const Vec3_t<float> &pos) {
	Vec3_t<float> middle ((pos.x + (float)(gridSize) / 2) * mGridResolution,
	                      (pos.y + (float)(gridSize) / 2) * mGridResolution,
	                      (pos.z + (float)(gridSize) / 2) * mGridResolution);
	int condition = ((evaluateFieldAt(middle, field)) >
	                 (mIsoLevel + ((sqrt(3.0) / 2) * (gridSize * mGridResolution))));
	unsigned num;
	if (condition) {
		return 0;
	}
	else {
		if (gridSize > 1) {
			unsigned totalTriangles = 0;
			for (unsigned i = 0; i < 8; i++)
			{
				Vec3_t<float> cubeOffset( pos.x + sc_vertexNormPos[i].x * float(gridSize / 2),
				                          pos.y + sc_vertexNormPos[i].y * float(gridSize / 2),
				                          pos.z + sc_vertexNormPos[i].z * float(gridSize / 2));

				#pragma omp task shared(totalTriangles)
				{
					num = octree_func(field, gridSize / 2, cubeOffset);
					#pragma omp atomic update
					totalTriangles += num;
				}

			}
			#pragma omp taskwait
			return totalTriangles;
		}
		else {
			unsigned result = buildCube(pos, field);
			return result;
		}
	}
}

unsigned TreeMeshBuilder::marchCubes(const ParametricScalarField &field)
{
	// Suggested approach to tackle this problem is to add new method to
	// this class. This method will call itself to process the children.
	// It is also strongly suggested to first implement Octree as sequential
	// code and only when that works add OpenMP tasks to achieve parallelism.
	Vec3_t<float> cubeOffset(0.0, 0.0, 0.0);
	unsigned trianglesCount;
	#pragma omp parallel
	{
		#pragma omp single
		trianglesCount = octree_func(field, mGridSize, cubeOffset);
	}
	return trianglesCount;
}

float TreeMeshBuilder::evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field)
{
	// NOTE: This method is called from "buildCube(...)"!

	// 1. Store pointer to and number of 3D points in the field
	//    (to avoid "data()" and "size()" call in the loop).
	// Since the foreach loop is used, these variables are no longer needed.
	// const Vec3_t<float> *pPoints = field.getPoints().data();
	// const unsigned count = unsigned(field.getPoints().size());

	float value = std::numeric_limits<float>::max();

	// 2. Find minimum square distance from points "pos" to any point in the
	//    field.
	for (auto point : field.getPoints()) //unsigned i = 0; i < count; ++i)
	{
		float distanceSquared  = (pos.x - point.x) * (pos.x - point.x);
		distanceSquared       += (pos.y - point.y) * (pos.y - point.y);
		distanceSquared       += (pos.z - point.z) * (pos.z - point.z);

		// Comparing squares instead of real distance to avoid unnecessary
		// "sqrt"s in the loop.
		value = std::min(value, distanceSquared);
	}

	// 3. Finally take square root of the minimal square distance to get the real distance
	return sqrt(value);
}

void TreeMeshBuilder::emitTriangle(const BaseMeshBuilder::Triangle_t &triangle)
{
	// NOTE: This method is called from "buildCube(...)"!

	// Store generated triangle into vector (array) of generated triangles.
	// The pointer to data in this array is return by "getTrianglesArray(...)" call
	// after "marchCubes(...)" call ends.
	#pragma omp critical
	{
		mTriangles.push_back(triangle);
	}
}
