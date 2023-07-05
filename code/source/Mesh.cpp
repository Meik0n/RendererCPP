#include <Mesh.hpp>
#include <Model.hpp>

example::Mesh::Mesh(Model* parent)
	:
	mesh_transform(1),
	mesh_position(0, 0, 0),
	mesh_rotation(0, 0, 0),
	mesh_scale(1, 1, 1)
{
	this->parent = parent;
}

example::Mesh::Mesh(Model* parent, Point3f position, Point3f rotation, Point3f scale)
	:
	mesh_transform(1)
{
	this->parent = parent;
	mesh_position = position;
	mesh_rotation = rotation;
	mesh_scale = scale;
}

void example::Mesh::update()
{
	mesh_transform = Matrix44(1);

	mesh_transform = translate(mesh_transform, mesh_position);

	mesh_transform = rotate_around_x(mesh_transform, mesh_rotation.x);
	mesh_transform = rotate_around_y(mesh_transform, mesh_rotation.y);
	mesh_transform = rotate_around_z(mesh_transform, mesh_rotation.z);

	mesh_transform = glm::scale(mesh_transform, mesh_scale);

	mesh_transform = parent->model_transform * mesh_transform;
}
