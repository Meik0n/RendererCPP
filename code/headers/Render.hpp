/*
	Author: Marcos Butron Rua
	Last Update: May/2021
	email: marcos.butron.rua@hotmail.com
*/

#ifndef RENDER_HEADER
#define RENDER_HEADER

	#include <Model.hpp>
	#include <View.hpp>
	#include <Color.hpp>
	#include <Color_Buffer.hpp>
	#include <Rasterizer.hpp>
	#include <Camera.hpp>

	namespace example
	{
		using argb::Color_Buffer;
	/**
	* @brief Render Class
	* @details Esta clase renderiza, recorta e ilumina los triangulos de la escena
	*/
		class Render
		{
		private:
 		    typedef Color_Buffer< Rgb888 > Color_Buffer;
			unsigned width;  //< ancho de la pantalla
			unsigned height; //< alto de la pantalla

			View   * scene;  //< puntero a la escena
			Camera * camera; //< puntero a la camara

			Color_Buffer               color_buffer; //< buffer de pixels
			Rasterizer< Color_Buffer > rasterizer;   //< rasterizer

		public:
		/**
		* @brief Render
		* @details Constructor de la clase
		* @param scene Puntero a la escena que se va a renderizar
		* @param camera Puntero a la camara de la escena
		*/
			Render(View* scene, Camera* camera);

		/**
		* @brief render_scene
		* @details Renderiza la escena
		*/
			void render_scene();

		private:
		/**
		* @brief is_frontface
		* @details Funcion que determina si un triangulo debe ser renderizado o no teniendo en cuenta si mira hacia la camara, retornando true o false
		* @param projected_vertices Puntero que apunta hacia el primer valor del array de vertices proyectados
		* @param indices Indices del array de vertices proyectados
		*/
			bool  is_frontface(const Point4f* const projected_vertices, const int* const indices);

		/**
		* @brief is_in_viewport
		* @details Funcion que determina si un triangulo esta dentro del frustrum de la camara o no
		* @param display_vertices Vertices ya en la pantalla
		* @param indices Indices del array de vertices de pantalla
		*/
			bool is_in_viewport(const Point4i* const display_vertices, const int* const indices);

		/**
		* @brief calculate_color
		* @details Calcula el color final del triangulo que se va a renderizar
		* @param projected_vertices Vertices proyectados
		* @param  original_color Color original del vertice
		* @param  light Vector de luz de la escena
		* @param  indices Indices del array de proyected vertices
		*/
			Rgb888  calculate_color(const Point4f* const projected_vertices, const Rgb888* original_color, Point3f ligth_vector, const int* const indices);

		/**
		* @brief calculate_intensity
		* @details Calcula la intensidad de la luz en cada triangulo
		* @param normal_vector Vector normal del triangulo
		* @param light_vector Vector de luz
		*/
			float calculate_intensity(const Point3f& normal_vector, const Point3f& ligth_vector);

		/**
		* @brief calculate_normal
		* @details Calculos de la intensidad
		* @param a Vertice 1 del triangulo
		* @param b Vertice 2 del triangulo
		* @param c Vertice 3 del triangulo
		*/
			Point3f calculate_normal(const Point4f& a, const Point4f& b, const Point4f& c);

		/**
		* @brief clip_polygon
		* @details Recorta los triangulos que se salgan de la ventana
		* @param input_vertices Vertices del poligono de origen
		* @param first Primer vertice
		* @param last Ultimo vertice
		* @param clipped_vertices Array donde se van a guardar los nuevos vertices resultantes de recortar el poligono
		*/
			int clip_polygon(Point4i* input_vertices, int * first, int * last, Point4i* clipped_vertices);

		/**
		* @brief clip_polygon_side
		* @details Calcula los puntos de interseccion de un lado del poligono con los bordes de la pantalla 
		* @param input_vertices Vertices originales del poligono
		* @param intersection_line_origin Punto de origen del segmento con el que se quiere comprobar si intersecciona el lado del poligono
		* @param intersection_line_end Punto final del segmento con el que se quiere comprobar si intersecciona el lado del poligono
		* @param vertex_count Numero de vertices nuevos generados por la interseccion
		* @param poly_count Numero de vertices del poligono original
		*/
			int clip_polygon_side(Point4i* input_vertices, Point4i* clipped_vertices, Point4i intersection_line_origin, Point4i intersection_line_end, int vertex_count, int poly_count);

		/**
		* @brief intersection
		* @details Calcula el punto de interseccion entre 2 segmentos
		* @param Point4i A 1 vertice del segmento 1
		* @param Point4i B 2 vertice del segmento 1
		* @param Point4i C 1 vertice del segmento 2
		* @param Point4i D 2 vertice del segmento 2
		*/
			Point4i intersection(Point4i A, Point4i B, Point4i C, Point4i D);
		};
	}
#endif
