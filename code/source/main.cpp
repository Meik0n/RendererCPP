
/*
	Author: Marcos Butron Rua
	Last Update: May/2021
	email: marcos.butron.rua@hotmail.com
*/

#include <View.hpp>
#include <SFML/Window.hpp>
#include <Model.hpp>
#include <Render.hpp>

using namespace sf;
using namespace example;

int main ()
{
    // Create the window and the view that will be shown within the window:

    unsigned window_width  = 800;
    unsigned window_height = 600;

	Point3f light{ 0.5f, -0.7f, -0.4f };

    Window window(VideoMode(window_width, window_height), "Mesh Loader", Style::Titlebar | Style::Close);
    View  * view = new View(window_width, window_height,light);
	Camera * camera = new Camera(Point3f(0,0,0),Point3f(0,0,0),Point3f(1,1,1));
	Render render(view, camera);

    window.setVerticalSyncEnabled (true);


#ifdef _DEBUG
	std::shared_ptr<Model> wind_mill = std::make_shared<Model>("../../binaries/assets/WindMill.obj", Point3f(0, -0.5, -10), Point3f(0, 0.5, 0), Point3f(0.4, 0.4, 0.4));
	std::shared_ptr<Model> terrain = std::make_shared<Model>("../../binaries/assets/isle.obj", Point3f(0, 4, -10), Point3f(0, -1, 0), Point3f(0.4, 0.4, 0.4));
	std::shared_ptr<Model> tree_1 = std::make_shared<Model>("../../binaries/assets/TreeCartoon1_OBJ.obj", Point3f(-1.5, 0, -8.6), Point3f(0, -1, 0), Point3f(0.2, 0.2, 0.2));
#endif // DEBUG


#ifdef NDEBUG
	std::shared_ptr<Model> wind_mill = std::make_shared<Model>("assets/WindMill.obj", Point3f(0, 0, -10), Point3f(0, 0.5, 0), Point3f(0.4, 0.4, 0.4));
	std::shared_ptr<Model> terrain = std::make_shared<Model>("assets/isle.obj", Point3f(0, 4, -10), Point3f(0, -1, 0), Point3f(0.4, 0.4, 0.4));
	std::shared_ptr<Model> tree_1 = std::make_shared<Model>("assets/TreeCartoon1_OBJ.obj", Point3f(-1.5, 0, -8.6), Point3f(0, -1, 0), Point3f(0.2, 0.2, 0.2));
#endif  // .EXE


	wind_mill->meshes[1]->mesh_position = Point3f(0, -5, 1.8);
	view->add_model(wind_mill);
	view->add_model(terrain);
	view->add_model(tree_1);

    // Run the main loop:

    bool exit = false;
	float angle = 0;

    do
    {
        Event event;

        while (window.pollEvent (event))
        {
			if (event.type == Event::Closed)
			{
				exit = true;
			}

			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::A)
				{
					camera->camera_position = Point3f(camera->camera_position.x + 0.1, camera->camera_position.y, camera->camera_position.z);
				}

				if (event.key.code == sf::Keyboard::D)
				{
					camera->camera_position = Point3f(camera->camera_position.x - 0.1, camera->camera_position.y, camera->camera_position.z);
				}

				if (event.key.code == sf::Keyboard::W)
				{
					camera->camera_position = Point3f(camera->camera_position.x , camera->camera_position.y + 0.1, camera->camera_position.z);
				}

				if (event.key.code == sf::Keyboard::S)
				{
					camera->camera_position = Point3f(camera->camera_position.x , camera->camera_position.y - 0.1, camera->camera_position.z);
				}
			}
        }

		angle += 0.03;
		wind_mill->meshes[1]->mesh_rotation = Point3f(0, 0, angle);

		for (int i = 0; i < view->number_of_models; ++i)
		{
			view->models[i]->update();
		}

        render.render_scene ();

        window.display ();
    }
    while (!exit);

    return 0;
}
