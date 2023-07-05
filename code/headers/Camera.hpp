/*
	Author: Marcos Butron Rua
	Last Update: May/2021
	email: marcos.butron.rua@hotmail.com
*/

#ifndef CAMERA_HEADER
#define CAMERA_HEADER

	#include <math.hpp>
		
	namespace example
	{
	/**
	* @brief Camera Class
	* @details Clase que configura la camara de la escena
	*/
		class Camera
		{
		public:
			Point3f  camera_position; //< posicion
			Point3f  camera_rotation; //< rotacion
			Point3f  camera_scale;	  //< escala
			Matrix44 camera_transform;//< transform
		
		public:
		/**
		* @brief Camera 
		* @details Constructor por defecto
		*/
			Camera();

		/**
		* @brief Camera
		* @details Constructor indicando la posicion, rotacion y escala iniciales
		*/
			Camera(Point3f position, Point3f rotation, Point3f scale);

		/**
		* @brief Update
		* @details Se encarga de actualizar el transform de la camara aplicando las matrices de transformacion
		*/
			void update();		
		};
	}

#endif