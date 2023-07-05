/*
	Author: Marcos Butron Rua
	Last Update: May/2021
	email: marcos.butron.rua@hotmail.com
*/

#ifndef VIEW_HEADER
#define VIEW_HEADER

    #include <Color_Buffer.hpp>
    #include <cstdlib>
    #include <math.hpp>
    #include <Rasterizer.hpp>
    #include <vector>
	#include <Model.hpp>

    namespace example
    {
        using  std::vector;
        using argb::Rgb888;
        using argb::Color_Buffer;
	/**
	* @brief View Class
	* @details Define la escena donde estaran los modelos
	*/
        class View
        {			
		public:
			int							   number_of_models; //< numero total de modelos
			vector<std::shared_ptr<Model>> models;			 //< vector que guarda punteros a los modelos
			unsigned					   width;			 //< ancho de la pantalla
			unsigned					   height;			 //< alto de la pantalla
			Point3f						   light;			 //< vector de luz
            
        public:
		/**
		* @brief View 
		* @details Constructor de View indicando ancho y alto de la pantalla y el vector de luz
		*/
            View(unsigned width, unsigned height, Point3f light_vector);

		/**
		* @brief add_model
		* @details añade un modelo al vector de modelos
		* @param model_name el modelo que se quiere añadir
		*/
			void add_model(std::shared_ptr<Model> model_name);
        };
    }

#endif
