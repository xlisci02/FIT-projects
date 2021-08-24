/**
 * @file    tree_mesh_builder.h
 *
 * @author  Matus Liscinsky <xlisci02@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    15.12.2019
 **/

#ifndef TREE_MESH_BUILDER_H
#define TREE_MESH_BUILDER_H

#include <vector>
#include "base_mesh_builder.h"

class TreeMeshBuilder : public BaseMeshBuilder
{
public:
	TreeMeshBuilder(unsigned gridEdgeSize);

protected:
	unsigned marchCubes(const ParametricScalarField &field);
	float evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field);
	void emitTriangle(const Triangle_t &triangle);
	const Triangle_t *getTrianglesArray() const { return mTriangles.data(); }
	unsigned octree_func(const ParametricScalarField &field, unsigned gridSize, const Vec3_t<float> &pos);
	std::vector<Triangle_t> mTriangles;
};

#endif // TREE_MESH_BUILDER_H
