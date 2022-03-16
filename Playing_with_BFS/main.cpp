#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <vector>
constexpr int window_dimensions = 800;
constexpr int box_dimensions = 8;
constexpr int max = window_dimensions / box_dimensions;
constexpr int vertex = max * max;
using namespace sf;

typedef struct cell {
	RectangleShape box;
	int wall;
	int visited;
	int vertex;
}cell;
cell matrix[max][max];
//int adj_matrix[vertex][vertex] = { 0 }; //haa haa not using this anymore 
std::vector<std::vector<int>> adjm;
std::vector<int> temp;
/*
	//new structure looks like this
	//but this is not adjacency list noo noooo.
	//random access on the first level then sequential access for each reference vectors.
	1->1,2,3
	2->5,6,8
	3->2,5
	4->6,7,1
	5->34
*/

int pred[vertex] = { -1 };
int path[vertex] = { 0 };
int source = 0, des = 0;
sf::Vector2i desIndex = sf::Vector2i(max / 2, max / 2);
sf::Vector2i prevDesIndex;

Clock cl;
Time dt;
float mx, my;

void init(void);
void init(int f);
void mouse_update(void);
void create_adj_mat(void);
void add_edge(int, int);
void bfs(void);
int get_path(void);

int main()
{
	RenderWindow window(VideoMode(window_dimensions, window_dimensions), "HAHAHAHAA..", Style::Close);
	window.setVerticalSyncEnabled(true);
	float t = 0;
	int state = 0;
	init();

	while (window.isOpen())
	{
		//dt = cl.restart();
		//t += dt.asSeconds();

		Event e;
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case Event::Closed:
				window.close();
			case Event::KeyPressed:
				if (Keyboard::isKeyPressed(Keyboard::Enter)) {
					if (state == 0) {
						create_adj_mat();
						state = 1;
					}
				}
				else if (Keyboard::isKeyPressed(Keyboard::R)) {
					init();
					state = 0;
				}
				break;

			default:
				break;
			}
		}

		///////////////////////

		mx = Mouse::getPosition(window).x;
		my = Mouse::getPosition(window).y;

		//states updating
		if (state == 0)
			mouse_update();
		
		
		if (state == 1) {
			bfs();
			state = 2;
		}

		if (state == 2) {
			int length = get_path();
			for (int k = 0; k < length; k++)
				for (int i = 0; i < max; i++)
					for (int j = 0; j < max; j++)
						if (matrix[i][j].vertex == path[k] && path[k] != source && path[k] != des)
							matrix[i][j].box.setFillColor(Color::Yellow);
			state = 0;
		}




		/////////////////////////

		window.clear();

		for (int i = 0; i < max; i++)
			for (int j = 0; j < max; j++)
				window.draw(matrix[i][j].box);

		window.display();
	}
}


void init(void)
{
	int count = 0;	//used for vertex numbering 
	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			matrix[i][j].box.setSize(Vector2f(box_dimensions - 2, box_dimensions - 2));
			matrix[i][j].box.setFillColor(Color(92, 92, 92, 255));
			matrix[i][j].box.setOutlineThickness(2.f);
			matrix[i][j].box.setOutlineColor(Color::Black);
			matrix[i][j].box.setPosition(i * box_dimensions, j * box_dimensions);
			matrix[i][j].wall = 0;
			matrix[i][j].visited = 0;
			matrix[i][j].vertex = count;
			count++;
		}
	}

	for (int i = 0; i < vertex; i++) {
		path[i] = 0;
		pred[i] = -1;
		adjm.clear();
	}

	//setting source
	source = matrix[0][0].vertex;
	matrix[0][0].box.setFillColor(sf::Color::Red);
	//for des 
	des = matrix[max / 2][max / 2].vertex;
	matrix[max / 2][max / 2].box.setFillColor(sf::Color::Green);
}

void init(int f)
{
	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			if (matrix[i][j].vertex != source && matrix[i][j].vertex != des && matrix[i][j].wall == 0) {
				matrix[i][j].box.setFillColor(Color(92, 92, 92, 255));
				matrix[i][j].visited = 0;
			}
		}
	}

	for (int i = 0; i < vertex; i++) {
		path[i] = 0;
		pred[i] = -1;
		adjm.clear();
	}
	
}

void create_adj_mat()
{
	adjm.clear();
	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			temp.clear();
			if (matrix[i][j].wall == 0) {
				if (j + 1 < max && matrix[i][j + 1].wall == 0)
					add_edge(matrix[i][j].vertex, matrix[i][j + 1].vertex);
				if (j - 1 > 0 && matrix[i][j - 1].wall == 0)
					add_edge(matrix[i][j].vertex, matrix[i][j - 1].vertex);
				if (i + 1 < max && matrix[i + 1][j].wall == 0)
					add_edge(matrix[i][j].vertex, matrix[i + 1][j].vertex);
				if (i - 1 > 0 && matrix[i - 1][j].wall == 0)
					add_edge(matrix[i][j].vertex, matrix[i - 1][j].vertex);
			}
			adjm.push_back(temp);
		}
	}
	
	//std::cout << "mapSize :" << " " << adjm[1000].size() << "\n";
	//std::cout << adjm[1590] << "\n";
	int c = 0;
	for (auto i = 0; i < adjm.size(); i++) {
		for (auto j = 0; j < adjm[i].size(); j++)
			c++;
	}
	std::cout << "Size with adjMatrix : " << vertex * vertex << "\n";
	std::cout << "Size with new approach : " << c << "\n";
}

void add_edge(int a, int b)
{
	if (b < 0 || b > vertex - 1)
		return;
	temp.push_back(b);
}

void bfs()
{
	std::list<int> queue;
	bool visited[vertex] = { false };

	visited[source] = true;
	queue.push_back(source);

	while (queue.empty() != 1) {
		int x = queue.front();

		if (x == des) {
			pred[source] = -1;
			return;
		}
		queue.pop_front();

		//new approach implementation //fast af and working
		//accessing the first vector element with random access.
		//it's reference vector are accessed sequentially.
		for (auto k = 0; k != adjm[x].size(); k++) {
			int vertexNum = adjm[x][k];
			if (visited[vertexNum] == false) {
				visited[vertexNum] = true;
				queue.push_back(vertexNum);
				pred[vertexNum] = x;
			}
		}

	}
}


int get_path()
{
	int k = 0, i = des;
	while (pred[i] != -1) {
		path[k++] = i;
		i = pred[i];
	}
	path[k++] = i;
	return k;
}

void mouse_update()
{
	/*
		mouse related updates
		like setting walls or blocks, clearing walls, setting destination and source etc;
		nothing fancy with it...
	*/

	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			int hot = mx > matrix[i][j].box.getPosition().x && mx < matrix[i][j].box.getPosition().x + box_dimensions
				&& my > matrix[i][j].box.getPosition().y && my < matrix[i][j].box.getPosition().y + box_dimensions;
			
			if (hot && Keyboard::isKeyPressed(Keyboard::Key::S)) {
				source = matrix[i][j].vertex;
				matrix[i][j].box.setFillColor(Color::Red);
			}
			if (hot && Keyboard::isKeyPressed(Keyboard::Key::D)) {
				if (matrix[i][j].wall == 1)
					continue;
				des = matrix[i][j].vertex;
				matrix[i][j].box.setFillColor(Color::Green);
				init(1);
			}	

			if (hot && Mouse::isButtonPressed(Mouse::Button::Left)) {
				matrix[i][j].wall = 1;
				matrix[i][j].box.setFillColor(Color::Black);
				if (i + 1 < max && j + 1 < max) {	//to check if it goes out of screen
					matrix[i + 1][j].wall = 1;
					matrix[i + 1][j].box.setFillColor(Color::Black);
					matrix[i][j + 1].wall = 1;
					matrix[i][j + 1].box.setFillColor(Color::Black);
					matrix[i + 1][j + 1].wall = 1;
					matrix[i + 1][j + 1].box.setFillColor(Color::Black);
				}
				
			}

			if (hot && Mouse::isButtonPressed(Mouse::Button::Right)) {
				matrix[i][j].wall = 0;
				matrix[i][j].box.setFillColor(Color(92, 92, 92, 255));
			}
		}
	}
}