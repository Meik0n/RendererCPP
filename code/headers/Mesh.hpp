/*
	Author: Marcos Butron Rua
	Last Update: May/2021
	email: marcos.butron.rua@hotmail.com
*/

#ifndef MESH_HEADER
#define MESH_HEADER

	#include <Color_Buffer.hpp>
	#include <cstdlib>
	#include <math.hpp>
	#include <vector>
	#include <memory>

	namespace example
	{
		class Model;
		using  std::vector;
		using argb::Rgb888;
		
	/**
	* @brief Mesh Class
	* @details Clase que define y actualiza las mallas de un modelo
	*/
		class Mesh
		{
		private:
			typedef Rgb888                Color;
			typedef Point4f               Vertex;
			typedef vector< Vertex >      Vertex_Buffer;
			typedef vector< int    >      Index_Buffer;
			typedef vector< Color  >      Vertex_Colors;

		private:
			Model* parent; //< puntero al modelo al que pertenece

		public:
			Vertex_Buffer     original_vertices;    //< vertices iniciales
			Index_Buffer      original_indices;     //< indices iniciales
			Vertex_Colors     original_colors;	    //< color inicial
			Vertex_Buffer     transformed_vertices; //< vertices transformados
			vector< Point4i > display_vertices;		//< vertices de pantallas
			Matrix44		  mesh_transform;		//< transform de la malla
			Point3f			  mesh_position;		//< posicion
			Point3f			  mesh_rotation;		//< rotacion
			Point3f			  mesh_scale;			//< escala

		public:
		/**
		* @brief Mesh
		* @details Constructor indicando solo el modelo al que pertenece la malla
		*/
			Mesh(Model* parent);

		/**
		* @brief Mesh
		* @details Constructor indicando el modelo al que pertenece la malla y una posicion, rotacion y escala especificos respecto al modelo padre
		*/
			Mesh(Model* parent, Point3f position, Point3f rotation, Point3f scale);

		/**
		* @brief Update
		* @details Se encarga de actualizar el transform de la malla aplicando las matrices de transformacion respecto al modelo al que pertecene
		*/
			void update();
		};
	}

#endif
