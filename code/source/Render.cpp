#include <Render.hpp>

namespace example 
{
	Render::Render(View* scene, Camera* camera)
		:
		height(scene->height),
		width(scene->width),
		color_buffer(width, height),
		rasterizer(color_buffer)
	{
		this->scene = scene;
		this->camera = camera;
	}


	void Render::render_scene()
	{
		// Se borra el framebúffer y se dibujan los triángulos:
		rasterizer.clear();

		for (size_t i = 0; i < scene->number_of_models; ++i)
		{
			for (size_t j = 0; j < scene->models[i]->meshes.size(); ++j)
			{
				// Se aplica la matriz de camara

				camera->update();
				Matrix44 camera_coordinates = inverse(camera->camera_transform) * scene->models[i]->meshes[j]->mesh_transform;				

				Matrix44 projection = perspective(20, 1, 15, float(scene->width) / scene->height);

				// Creación de la matriz de transformación unificada:

				Matrix44 transformation = projection * camera_coordinates;

				// Se transforman todos los vértices usando la matriz de transformación resultante:

				for (size_t index = 0, number_of_vertices = scene->models[i]->meshes[j]->original_vertices.size(); index < number_of_vertices; index++)
				{
					// Se multiplican todos los vértices originales con la matriz de transformación y
					// se guarda el resultado en otro vertex buffer:

					Point4f& vertex = scene->models[i]->meshes[j]->transformed_vertices[index] = transformation * scene->models[i]->meshes[j]->original_vertices[index];

					// La matriz de proyección en perspectiva hace que el último componente del vector
					// transformado no tenga valor 1.0, por lo que hay que normalizarlo dividiendo:

					float divisor = 1.f / vertex.w;

					vertex.x *= divisor;
					vertex.y *= divisor;
					vertex.z *= divisor;
					vertex.w = 1.f;
				}
				// Se convierten las coordenadas transformadas y proyectadas a coordenadas
				// de recorte (-1 a +1) en coordenadas de pantalla con el origen centrado.
				// La coordenada Z se escala a un valor suficientemente grande dentro del
				// rango de int (que es lo que espera fill_convex_polygon_z_buffer).

				Matrix44 identity(1);
				Matrix44 scaling = scale(identity, float(scene->width / 2), float(scene->height / 2), 100000000.f);
				Matrix44 translation = translate(identity, Vector3f{ float(scene->width / 2), float(scene->height / 2), 0.f });
				transformation = translation * scaling;

				for (size_t index = 0, number_of_vertices = scene->models[i]->meshes[j]->transformed_vertices.size(); index < number_of_vertices; index++)
				{
					scene->models[i]->meshes[j]->display_vertices[index] = Point4i(transformation * scene->models[i]->meshes[j]->transformed_vertices[index]);
				}
				
				for (int* indices = scene->models[i]->meshes[j]->original_indices.data(), *end = indices + scene->models[i]->meshes[j]->original_indices.size(); indices < end; indices += 3)
				{
					if (is_in_viewport(scene->models[i]->meshes[j]->display_vertices.data(), indices)) //frustrum culling
					{
						if (is_frontface(scene->models[i]->meshes[j]->transformed_vertices.data(), indices))
						{
							// Se calcula el color del triangulo iluminado a partir del primer vertice del mismo

							Rgb888 triangle_color = calculate_color(scene->models[i]->meshes[j]->transformed_vertices.data(), scene->models[i]->meshes[j]->original_colors.data(), scene->light, indices);

							// Se establece el color del polígono

							rasterizer.set_color(triangle_color);

							// Se rellena el polígono:

							Point4i clipped_vertices[10];
							int clipped_index[10] = { 0,1,2,3,4,5,6,7,8,9 };

							int new_vertices_count = clip_polygon(scene->models[i]->meshes[j]->display_vertices.data(), indices, indices + 3, clipped_vertices);

							if (new_vertices_count > 0)
							{
								rasterizer.fill_convex_polygon_z_buffer(clipped_vertices, clipped_index, clipped_index + new_vertices_count);
							}
							else
							{
								rasterizer.fill_convex_polygon_z_buffer(scene->models[i]->meshes[j]->display_vertices.data(), indices, indices + 3);
							}
						}
					}					
				}				
			}
		}
		// Se copia el framebúffer oculto en el framebúffer de la ventana:

		color_buffer.blit_to_window();
	}

	bool Render::is_frontface(const Point4f* const projected_vertices, const int* const indices)
	{
		const Point4f& v0 = projected_vertices[indices[0]];
		const Point4f& v1 = projected_vertices[indices[1]];
		const Point4f& v2 = projected_vertices[indices[2]];

		// Se asumen coordenadas proyectadas y polígonos definidos en sentido horario.
		// Se comprueba a qué lado de la línea que pasa por v0 y v1 queda el punto v2:

		return ((v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]) < 0.f);
	}

	bool Render::is_in_viewport(const Point4i* const display_vertices, const int* const indices)
	{
		const Point4f& v0 = display_vertices[indices[0]];
		const Point4f& v1 = display_vertices[indices[1]];
		const Point4f& v2 = display_vertices[indices[2]];

		if (
			v0.x > 0 && v0.x < width && v0.y > 0 && v0.y < height ||
			v1.x > 0 && v1.x < width && v1.y > 0 && v1.y < height ||
			v2.x > 0 && v2.x < width && v2.y > 0 && v2.y < height
			)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//Recorte
	int Render::clip_polygon(Point4i* input_vertices, int* first, int* last, Point4i* clipped_vertices)
	{
		bool needs_clipping = false;

		for (const int* index_iterator = first; index_iterator < last; ++index_iterator)
		{
			if (input_vertices[*index_iterator].x < 0 || input_vertices[*index_iterator].x > width || input_vertices[*index_iterator].y < 0 || input_vertices[*index_iterator].y > height)
			{
				needs_clipping = true;
				break;
			}
		}

		if (needs_clipping)
		{
			int new_vertex_count = 0;
			int poly_count = 3;

			Point4i aux_clipped_vertices[10];

			new_vertex_count = clip_polygon_side(input_vertices, aux_clipped_vertices, Point4i{ 0,0,0,0 }, Point4i{ width, 0, 0, 0 }, new_vertex_count,poly_count); //lado superior
			if (new_vertex_count > 0)
			{
				poly_count = new_vertex_count;

				for (int i = 0; i < poly_count; ++i)
				{
					input_vertices[i] = aux_clipped_vertices[i];
				}
			}



			new_vertex_count = clip_polygon_side(input_vertices, aux_clipped_vertices, Point4i{width,0 ,0 ,0 }, Point4i{width,height,0,0}, new_vertex_count, poly_count); //lado derecho
			if (new_vertex_count > 0)
			{
				poly_count = new_vertex_count;
				for (int i = 0; i < poly_count; ++i)
				{
					input_vertices[i] = aux_clipped_vertices[i];
				}
			}



			new_vertex_count = clip_polygon_side(input_vertices, aux_clipped_vertices, Point4i{ 0, height,0,0}, Point4i{ width,height,0,0}, new_vertex_count, poly_count); //lado inferior
			if (new_vertex_count > 0)
			{
				poly_count = new_vertex_count;

				for (int i = 0; i < poly_count; ++i)
				{
					input_vertices[i] = aux_clipped_vertices[i];
				}
			}

			new_vertex_count = clip_polygon_side(input_vertices, aux_clipped_vertices, Point4i{ 0,0,0,0 }, Point4i{ 0,height,0,0 }, new_vertex_count, poly_count); //lado izquierdo

			return new_vertex_count;
		}

		return 0;
	}

	int Render::clip_polygon_side(Point4i* input_vertices, Point4i* clipped_vertices, Point4i intersection_line_origin, Point4i intersection_line_end, int vertex_count, int poly_count)
	{
		for (int i = 0; i < poly_count; ++i)
		{
			if ((i + 1) < poly_count) // si no estamos en el ultimo vertice 
			{
				Point4i intersection_point = intersection(input_vertices[i], input_vertices[i + 1], intersection_line_origin, intersection_line_end);

				//Si el punto de interseccion esta en la recta con la que queremos comprobar que intersecta
				if (intersection_point.x >= intersection_line_origin.x && intersection_point.x <= intersection_line_end.x && intersection_point.y >= intersection_line_origin.x && intersection_point.y <= intersection_line_end.y)
				{

					//case 1: ambos puntos estan dentro
					if (input_vertices[i].x >= 0 && input_vertices[i].x <= width &&
						input_vertices[i].y >= 0 && input_vertices[i].y <= height &&
						input_vertices[i + 1].x >= 0 && input_vertices[i + 1].x <= width &&
						input_vertices[i + 1].y >= 0 && input_vertices[i + 1].y <= height)
					{
						//solo se guarda el 2 vertice
						clipped_vertices[vertex_count] = input_vertices[i + 1];
						++vertex_count;
					}

					//case 2: cuando solo el primer vertice esta fuera
					else if
						 (
							input_vertices[i].x < 0 || input_vertices[i].x > width ||
							input_vertices[i].y < 0 || input_vertices[i].y > height &&
							input_vertices[i + 1].x >= 0 && input_vertices[i + 1].x <= width &&
							input_vertices[i + 1].y >= 0 && input_vertices[i + 1].y <= height
						 )
					{
						//se guarda el punto de interseccion y el 2 vertice
						clipped_vertices[vertex_count] = intersection_point;
						++vertex_count;
						clipped_vertices[vertex_count] = input_vertices[i + 1];
						++vertex_count;
					}

					//case 3: cuando solo el 2 vertice esta fuera
					else if
						 (
							input_vertices[i].x >= 0 && input_vertices[i].x <= width &&
							input_vertices[i].y >= 0 && input_vertices[i].y <= height &&
							input_vertices[i + 1].x < 0 || input_vertices[i + 1].x > width ||
							input_vertices[i + 1].y < 0 || input_vertices[i + 1].y > height
						 )
					{
						//se guarda el punto de interseccion
						clipped_vertices[vertex_count] = intersection_point;
						++vertex_count;
					}

					//case4 ambos puntos estan fuera
					else
					{
						//no se guarda ningun punto
					}
				}
			}

			else if ((i + 1) == poly_count) //si estamos en el ultimo vertice
			{
				Point4i intersection_point = intersection(input_vertices[i], input_vertices[0], intersection_line_origin, intersection_line_end);

				if (intersection_point.x >= intersection_line_origin.x && intersection_point.x <= intersection_line_end.x && intersection_point.y >= intersection_line_origin.x && intersection_point.y <= intersection_line_end.y)
				{
					//case 1: ambos puntos estan dentro
					if (input_vertices[i].x >= 0 && input_vertices[i].x <= width &&
						input_vertices[i].y >= 0 && input_vertices[i].y <= height &&
						input_vertices[0].x >= 0 && input_vertices[0].x <= width &&
						input_vertices[0].y >= 0 && input_vertices[0].y <= height)
					{
						//solo se guarda el 2 vertice
						clipped_vertices[vertex_count] = input_vertices[0];
						++vertex_count;
					}

					//case 2: cuando solo el primer vertice esta fuera
					else if (input_vertices[i].x < 0 || input_vertices[i].x > width ||
						input_vertices[i].y < 0 || input_vertices[i].y > height &&
						input_vertices[0].x >= 0 && input_vertices[0].x <= width &&
						input_vertices[0].y >= 0 && input_vertices[0].y <= height)
					{
						//se guarda el punto de interseccion y el 2 vertice
						clipped_vertices[vertex_count] = intersection_point;
						++vertex_count;
						clipped_vertices[vertex_count] = input_vertices[0];
						++vertex_count;
					}

					//case 3: cuando solo el 2 vertice esta fuera
					else if (input_vertices[i].x >= 0 && input_vertices[i].x <= width &&
						input_vertices[i].y >= 0 && input_vertices[i].y <= height &&
						input_vertices[0].x < 0 || input_vertices[0].x > width ||
						input_vertices[0].y < 0 || input_vertices[0].y > height)
					{
						//se guarda el punto de interseccion
						clipped_vertices[vertex_count] = intersection_point;
						++vertex_count;
					}

					//case4 ambos puntos estan fuera
					else
					{
						//no se guarda ningun punto
					}
				}
			}
		}

		return vertex_count;
	}

	Point4i Render::intersection(Point4i A, Point4i B, Point4i C, Point4i D)
	{		

		int num_x = (A.x * B.y - A.y * B.x) * (C.x - D.x) - (A.x - B.x) * (C.x * D.y - C.y * D.x);
		int den_x = (A.x - B.x) * (C.y - D.y) - (A.y - B.y) * (C.x - D.x);

		int num_y = (A.x * B.y - A.y * B.x) * (C.y - D.y) - (A.y - B.y) * (C.x * D.y - C.y * D.x);
		int den_y = (A.x - B.x) * (C.y - D.y) - (A.y - B.y) * (C.x - D.x);

		if (den_x == 0 || den_y == 0)
		{
			return Point4i{ INT_MAX, INT_MAX,0,0 };
		}

		else
		{
			int point_x = num_x / den_x;
			int point_y = num_y / den_y;

			return Point4i{ point_x, point_y, 0 ,0 };
		}
	}

	//Luz
	Point3f Render::calculate_normal(const Point4f& a, const Point4f& b, const Point4f& c)
	{
		Point3f vector1 = b - a;
		Point3f vector2 = c - a;

		Point3f normal = glm::cross(vector1, vector2);

		return normal;
	}

	float Render::calculate_intensity(const Point3f& normal_vector, const Point3f& ligth_vector)
	{
		float intensity = glm::dot(normal_vector, ligth_vector);

		if (intensity > 1)
		{
			intensity = 1;
		}
		else if (intensity < 0)
		{
			intensity = 0;
		}

		return intensity;
	}

	Rgb888 Render::calculate_color(const Point4f* const projected_vertices, const Rgb888* original_color, Point3f ligth_vector, const int* const indices)
	{
		// normal

		Point3f triangle_normal = calculate_normal
		(
			projected_vertices[indices[0]],
			projected_vertices[indices[1]],
			projected_vertices[indices[2]]
		);

		triangle_normal = glm::normalize(triangle_normal);

		// intensidad

		float triangle_intesity = calculate_intensity(triangle_normal, ligth_vector);

		Rgb888 final_color = Rgb888
		(
			0.5 * triangle_intesity,
			0.5 * triangle_intesity,
			0.5 * triangle_intesity
		);

		return final_color;
	}
}
