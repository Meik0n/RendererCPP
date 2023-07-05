#include <Model.hpp>

example::Model::Model(string model_file_path, Point3f position, Point3f rotation, Point3f scale)
	:
	model_transform(1)
{
	{
		model_position = position;
		model_rotation = rotation;
		model_scale = scale;

		this->model_file_path = model_file_path;
		Assimp::Importer importer;

		auto model = importer.ReadFile
		(
			model_file_path,
			aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType
		);


		// Si scene es un puntero nulo significa que el archivo no se pudo cargar con éxito:

		if (model && model->mNumMeshes > 0)
		{
			for (int i = 0; i < model->mNumMeshes; ++i)
			{
				shared_ptr< Mesh > mesh = std::make_shared< Mesh >(this);
				auto assimp_mesh = model->mMeshes[i];

				size_t number_of_vertices = assimp_mesh->mNumVertices;

				// Se copian los datos de coordenadas de vértices:

				mesh->original_vertices.resize(number_of_vertices);

				for (size_t index = 0; index < number_of_vertices; index++)
				{
					auto& vertex = assimp_mesh->mVertices[index];

					mesh->original_vertices[index] = Point4f(vertex.x, -vertex.y, vertex.z, 1.f);
				}

				mesh->transformed_vertices.resize(number_of_vertices);
				mesh->display_vertices.resize(number_of_vertices);

				// Se inicializan los datos de color de los vértices con colores aleatorios:

				mesh->original_colors.resize(number_of_vertices);

				for (size_t index = 0; index < number_of_vertices; index++)
				{
					mesh->original_colors[index].set(rand_clamp(), rand_clamp(), rand_clamp());
				}

				// Se generan los índices de los triángulos:

				size_t number_of_triangles = assimp_mesh->mNumFaces;

				mesh->original_indices.resize(number_of_triangles * 3);

				Index_Buffer::iterator indices_iterator = mesh->original_indices.begin();

				for (size_t index = 0; index < number_of_triangles; index++)
				{
					auto& face = assimp_mesh->mFaces[index];

					assert(face.mNumIndices == 3);              // Una face puede llegar a tener de 1 a 4 índices,
																// pero nos interesa que solo haya triángulos
					auto indices = face.mIndices;

					*indices_iterator++ = int(indices[0]);
					*indices_iterator++ = int(indices[1]);
					*indices_iterator++ = int(indices[2]);
				}

				meshes.push_back(mesh);
			}
		}
	}
}

void example::Model::update()
{
	model_transform = Matrix44(1);

	model_transform = translate(model_transform, model_position);

	model_transform = rotate_around_x(model_transform, model_rotation.x);
	model_transform = rotate_around_y(model_transform, model_rotation.y);
	model_transform = rotate_around_z(model_transform, model_rotation.z);

	model_transform = glm::scale(model_transform, model_scale);

	for (int i = 0; i < meshes.size(); ++i)
	{
		meshes[i]->update();
	}
}
