/*
	Author: Marcos Butron Rua
	Last Update: May/2021
	email: marcos.butron.rua@hotmail.com
*/

#ifndef MODEL_HEADER
#define MODEL_HEADER

	#include <Mesh.hpp>
	#include <assimp/Importer.hpp>
	#include <assimp/scene.h>
	#include <assimp/postprocess.h>
	#include <Rasterizer.hpp>
	#include <string>
	#include <memory>

	namespace example
	{
		using std::vector;
		using std::string;
		using std::shared_ptr;

	/**
	* @brief Model Class
	* @details Clase que define y actualiza los modelos 3D de la escena
	*/
		class Model
		{
		private:
			typedef vector< int > Index_Buffer;
			string				  model_file_path;	       //< path del modelo

		public:
			vector<std::shared_ptr<Mesh>> meshes;		   //< mallas del modelo
			Matrix44					  model_transform; //< transform del modelo
			Point3f					      model_position;  //< posicion
			Point3f					      model_rotation;  //< rotacion
			Point3f						  model_scale;     //< escala

		public:
		/**
		* @brief Model
		* @details Constructor de la clase
		* @param model_file_path Path del modelo
		* @param position Posicion
		* @param position Rotacion
		* @param position Escala
		*/
			Model(string model_file_path, Point3f position, Point3f rotation, Point3f scale);

		/**
		* @brief update
		* @details Actualiza el transform del modelo aplicando las matrices de transformacion
		*/
			void update();

		private:
		/**
		* @brief rand_clamp
		* @details rellena los datos de color originales de los triangulos con un color aleatorio
		*/
			float rand_clamp() { return float(rand() & 0xff) * 0.0039215f; }
		};
	}

#endif
