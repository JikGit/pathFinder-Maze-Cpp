#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include "square.cpp"

float HEIGHT = 700, WIDTH = 700;
int size = 10;
bool maze = true;
bool squarePath = true;

void display(sf::RenderWindow&, std::vector < std::vector < Square > > &);
bool collide(sf::Vector2i,  sf::RectangleShape);
void mousePressed(std::vector < std::vector < Square > > &, sf::Vector2i, int &, Square*&, Square*&);
float distanza(float, float , float, float);
std::vector < Square* > checkVicini(std::vector < std::vector < Square > > &, Square*);
int min(std::vector < Square *>);
void createBoard(std::vector < std::vector < Square > > &);
int pathfinding(Square*&, std::vector < Square* >, std::vector < std::vector < Square > > &, Square* );
std::vector < Square *> mazeCheckVicini(std::vector < std::vector < Square > > &board, Square *piece);
void recurisveCreateMaze(std::vector < std::vector < Square > > &, Square*);

int main(){
	srand(time(NULL));
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "pathfinder", sf::Style::Default);
	int minFCost;
	int startPathFinder = 0, state = 0;

	std::vector < Square* > Open;
	Square *end, *piece;

	//creation of the board
	std::vector < std::vector < Square > > board(HEIGHT/size, std::vector < Square > (WIDTH/size, Square()));
	createBoard(board);	
	piece = &board[rand() % int (HEIGHT/size)][rand() % int(WIDTH/size)];

	//creation of the maze 
	if (maze)
		recurisveCreateMaze(board, piece);

	while (window.isOpen()){
		sf::Event event;
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			//creo una variabile contenente il valore della posizione del mouse
			sf::Vector2i mousePos = sf::Mouse::getPosition(window);
			//vedo se il mouse va contro qualcosa e gli setto lo start, end o wall a seconda di state
			mousePressed(board, mousePos, state, piece, end);
		}

		//se premo space parte il pathfinding
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && startPathFinder == 0){
			startPathFinder=1;
			piece->previus = piece;
			piece->Gcost = 0;
			Open.clear();
			Open.push_back(piece);
		}

		//se premo esc cambia il maze
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)){
			startPathFinder = 0;
			state = 0;
			//ricreo la board e il labirinto tutti diversi da prima
			createBoard(board);
			recurisveCreateMaze(board, &board[rand() % int (HEIGHT/size)][rand() % int(WIDTH/size)]);
		}

		//algorithm part
		if (startPathFinder == 1){
			int risposta = pathfinding(piece, Open, board, end);
			if (risposta == 1){
				piece = end;
				while (!piece->previus->start){
					piece = piece->previus;
					piece->square.setFillColor(sf::Color::Cyan);
				}
			}
			startPathFinder = 2;
		}
		
		//update lo schermo
		display(window, board);
	}
}

void display(sf::RenderWindow& window, std::vector < std::vector < Square > > &board){
	window.clear();
	//display board e vedo se premo mouse
	for (int col = 0; col < (int)HEIGHT/size; col++)
		for (int row = 0; row < (int)WIDTH/size; row++)
			board[col][row].draw(window);
	window.display();
}

//return true se la posizione del mouse e uno shape collide
bool collide(sf::Vector2i mousePos, sf::RectangleShape square){
	if (mousePos.x >= square.getPosition().x && mousePos.x <= square.getPosition().x + square.getSize().x && mousePos.y >= square.getPosition().y  && mousePos.y <= square.getPosition().y + square.getSize().y)
		return true;
	return false;
}

//cambia lo stato se premo il mouse
void mousePressed(std::vector < std::vector < Square > > &board, sf::Vector2i mousePos, int &state, Square *&piece, Square *&end){
	for (int col = 0; col < (int)HEIGHT/size; col++){
		for (int row = 0; row < (int)WIDTH/size; row++){
			if (!board[col][row].picked && collide(mousePos, board[col][row].square)){
				if (state == 0){
					piece = &board[col][row];
					piece -> startClicked();
					state++;
				}else if (state == 1){
					end = &board[col][row];
					end->endClicked();
					state++;
				}else{
					board[col][row].wallClicked();
				}
			}
		}
	}
}

//return a 2d vector of (col, row) vicini
std::vector < Square *> checkVicini(std::vector < std::vector < Square > > &board, Square *piece){
	std::vector < Square*> vicini;
	int row = piece->y, col = piece->x;

	//cicla per trovare i vicini
	for (int y = col - 1; y < col + 2; y++){
		if (y < 0 || y >= WIDTH/size) continue;
		for (int x = row - 1; x < row + 2; x++)
		{
			if (!squarePath){
				if (x < 0 || x >= HEIGHT/size || (y == col && row == x) || (y == col - 1) && (x == row - 1 || x == row + 1) || (y == col + 1) && (x == row + 1 || x == row - 1 )) continue;
			}else{
				if (x < 0 || x >= HEIGHT/size || (y == col && row == x)) continue;
			}

			if (board[y][x].wall || board[y][x].explored)
				continue;
			vicini.push_back(&board[y][x]);
			/* board[y][x].square.setFillColor(sf::Color::Black); */
		}
	}
	return vicini;
}

//return the index of the minimun Fcost in the given list
int min(std::vector < Square* > Costs){
	int min = 0;
	for (int index = 0; index < Costs.size(); index++){
		if (Costs[index]->Fcost == Costs[min]->Fcost){
			if (Costs[index]->Hcost > Costs[min]->Hcost)
				min = index;
		}else if (Costs[index]->Fcost < Costs[min]->Fcost)
			min = index;	
	}
	return min;
}

//return distanza tra 2 punti 
float distanza(float x1, float y1, float x2, float y2){
	return std::round(std::sqrt(std::pow(y1-y2, 2) + std::pow(x1-x2, 2))*100); 
}


//crea la 2d vector con tutti gli Square
void createBoard(std::vector < std::vector < Square > > &board){
	for (int col = 0; col < (int)HEIGHT/size; col++)
		for (int row = 0; row < (int)WIDTH/size; row++){
			board[col][row].setPosSize(row, col);
			if (maze)
				board[col][row].wallClicked();
		}
	
}

//inizia il pathFinding
int pathfinding(Square*& piece, std::vector < Square* > Open, std::vector < std::vector < Square > > &board, Square* end){
	int index;
	float Gcost, Fcost, Hcost;
	std::vector < Square* > listvicini;
	
	while (true){
		if (Open.size() == 0){
			std::cout << "Nessuna soluzione rilevata!!!" << std::endl;
			return 0;
		}
		else if (piece->end)
			return 1;

		//set the piece and remove it from the Open and see if it's finished
		index = min(Open);
		piece = Open[index];
		Open.erase(Open.begin() + index);
		piece->explored = true;
		/* piece->square.setFillColor(sf::Color::Red); */

		listvicini = checkVicini(board, piece);
		for (int i = 0; i < listvicini.size(); i++){
			//calcolo la sua Hcost per vedere se e' minore della sua vecchia Gcost
			Gcost = piece->Gcost + distanza(listvicini[i]->x, listvicini[i]->y, piece->x, piece->y);
			//vedo il vicino solo se il suo Gcost e' minore del suo vecchio Gcost o se non e' mai stato visto
			if (Gcost < listvicini[i]->Gcost || !listvicini[i]->close){
				Hcost = distanza(end->x, end->y, listvicini[i]->x, listvicini[i]->y); 
				listvicini[i]->setCost(Gcost, Hcost, Gcost/1.5 + Hcost);
				//set the previus for the vicino
				listvicini[i]->previus = piece;

				//se non era stato mai visto da nessno allora lo metto che e' stato visto e lo aggiungo alla lista
				if (!listvicini[i]->close){
					listvicini[i]->close = true;
					Open.push_back(listvicini[i]);
				}
			}
		}	
	}
}

//ritorna tutti i vicini non utilizzati
std::vector < Square *> mazeCheckVicini(std::vector < std::vector < Square > > &board, Square *piece){
	std::vector < Square*> vicini;
	int row = piece->y, col = piece->x;
	int y, x;

	y = col - 2; x = row;
	if (!(x < 0 || y >= HEIGHT / size || x >= WIDTH/size || y < 0 || board[y][x].mazeVisited)){
		vicini.push_back(&board[y][x]);
		/* board[y][x].square.setFillColor(sf::Color::Black); */
	}

	y = col ; x = row - 2;
	if (!(x < 0 || y >= HEIGHT / size || x >= WIDTH/size || y < 0 || board[y][x].mazeVisited)){
		vicini.push_back(&board[y][x]);
		/* board[y][x].square.setFillColor(sf::Color::Black); */
	}

	y = col; x = row + 2;
	if (!(x < 0 || y >= HEIGHT / size || x >= WIDTH/size || y < 0 || board[y][x].mazeVisited)){
		vicini.push_back(&board[y][x]);
		/* board[y][x].square.setFillColor(sf::Color::Black); */
	}

	y = col + 2; x = row;
	if (!(x < 0 || y >= HEIGHT / size || x >= WIDTH/size || y < 0 || board[y][x].mazeVisited)){
		vicini.push_back(&board[y][x]);
		/* board[y][x].square.setFillColor(sf::Color::Black); */
	}

	return vicini;
}

//prendo un punto a caso e vado fino a che la strada non e' chiusa, quando lo e' vado indietro nel percorso fino a trovare un'altra strada
void recurisveCreateMaze(std::vector < std::vector < Square > > &board, Square* piece){

	piece -> pathClicked();
	std::vector < Square* > listvicini;

	while (true){
		listvicini = mazeCheckVicini(board, piece);
		if (listvicini.size() == 0) break;
		int randIndex = rand() % listvicini.size();
		Square* vicino = listvicini[randIndex];
		//uso formula x = (x1  + x2) /2 e y = (y1 + y2) / 2
		board[(piece->x + vicino->x)/2][(piece->y + vicino->y)/2].pathClicked();
		recurisveCreateMaze(board, listvicini[randIndex]);
	}
}

