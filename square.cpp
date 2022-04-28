#include <iostream>
#include <SFML/Graphics.hpp>

extern int size;

class Square{
	public:
		int x, y;
		Square* previus;
		bool explored, picked, close, wall, start, end, mazeVisited;
		float Gcost, Hcost, Fcost;	
		sf::RectangleShape square;

		void setPosSize(int row, int col){
			x = col; y = row;
			explored = false, picked = false, close = false, wall = false, start = false, end = false, mazeVisited = false;
			Gcost = 10000000; Fcost = 10000000; Hcost = 1000000;

			square.setPosition(sf::Vector2f((float)row*size, (float)col*size));
			square.setSize(sf::Vector2f((float)size, (float)size));

			// square.setFillColor(sf::Color::White);
		}

		//cambia dati e vero se li ha cambiati
		void setCost(float nGcost, float nHcost, float nFcost){
			Gcost = nGcost; Hcost = nHcost; Fcost = nFcost;
		}

		void pathClicked(){
			picked = false;
			mazeVisited = true;
			wall = false;
			square.setFillColor(sf::Color::White);
		}
		
		void startClicked(){
			picked = true;
			start = true;
			square.setFillColor(sf::Color::Magenta);
		}

		void endClicked(){
			picked = true;
			end = true;
			square.setFillColor(sf::Color::Blue);
		}
		void wallClicked(){
			picked = true;
			wall = true;
			square.setFillColor(sf::Color::Black);
		}
		void draw(sf::RenderWindow &window){
			window.draw(square);
		}


};
