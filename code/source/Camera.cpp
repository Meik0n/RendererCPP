#include <Camera.hpp>

example::Camera::Camera()
	:
	camera_transform(1),
	camera_position(0,0,0),
	camera_rotation(0,0,0),
	camera_scale(0,0,0)
{
}

example::Camera::Camera(Point3f position, Point3f rotation, Point3f scale)
	:
	camera_transform(1)
{
	camera_position = position;
	camera_rotation = rotation;
	camera_scale = scale;
}

void example::Camera::update()
{
	camera_transform = Matrix44(1);

	camera_transform = translate(camera_transform, camera_position);

	camera_transform = rotate_around_x(camera_transform, camera_rotation.x);
	camera_transform = rotate_around_y(camera_transform, camera_rotation.y);
	camera_transform = rotate_around_z(camera_transform, camera_rotation.z);

	camera_transform = glm::scale(camera_transform, camera_scale);
}
