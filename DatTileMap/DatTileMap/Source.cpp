#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <noise/noise.h>
#include <iostream>
using namespace noise;
using namespace std;

class datNoise
{
public:
	void mapper(int* level,const int r, const int c,double z,int seed)
	{

		module::Perlin myModule;
		const int row = r;
		const int col = c;
		double value;
		int sub_value = 0;
		//srand(time(NULL));
		//int seed = (rand() % 100);
		//cout << seed;
		//myModule.SetSeed(seed);
		myModule.SetSeed(seed);
		for (int i = 0; i < r; i++)
		{
			for (int j = 0; j < c; j++)
			{
				value = myModule.GetValue(.09*i, .09*j, z);
				if (value >= -1.0 && value <= -0.67)
				{
					sub_value = 0;
				}
				else if (value > -0.67 && value <= -0.35)
				{
					sub_value = 1;
				}
				else if (value > -0.35 && value <= -0.10)
				{
					sub_value = 2;
				}
				else if (value > -0.10 && value <= 0.20)
				{
					sub_value = 3;
				}
				else if (value > 0.2 && value <= 0.7)
				{
					sub_value = 4;
				}
				else if (value > 0.7 && value <= 1.0)
				{
					sub_value = 5;
				}
				level[i*c + j] = sub_value;
			}

		}
	}
};

class TileMap : public sf::Drawable, public sf::Transformable
{
public:

	bool load(const std::string& tileset, sf::Vector2u tileSize, int* tiles, unsigned int width, unsigned int height)
	{
		// load the tileset texture
		if (!m_tileset.loadFromFile(tileset))
			return false;

		// resize the vertex array to fit the level size
		m_vertices.setPrimitiveType(sf::Quads);
		m_vertices.resize(width * height * 4);

		// populate the vertex array, with one quad per tile
		for (unsigned int i = 0; i < width; ++i)
			for (unsigned int j = 0; j < height; ++j)
			{
				// get the current tile number
				int tileNumber = tiles[i + j * width];

				// find its position in the tileset texture
				int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
				int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

				// get a pointer to the current tile's quad
				sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

				// define its 4 corners
				quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
				quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
				quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
				quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

				// define its 4 texture coordinates
				quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
				quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
				quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
				quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
			}

		return true;
	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();

		// apply the tileset texture
		states.texture = &m_tileset;

		// draw the vertex array
		target.draw(m_vertices, states);
	}

	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
};


int main()
{
	// create the window
	
	double z = 0.05;
	const int r=54, c=72;
	const int tile_size = 16;
	sf::RenderWindow window(sf::VideoMode(tile_size*c+200, tile_size*r), "DatBeautifulPerlinMap");
	int level[r*c];

	srand(time(NULL));
	int seed = (rand() % 100);

	datNoise noisy;
	noisy.mapper(level,r,c,z,seed);
	string s_txt;
	s_txt = "SEED : ";
	s_txt += to_string(seed);
	sf::Font font;
	sf::Text text;
	sf::Text seedtext;
	sf::Text titletext;

	titletext.setFont(font);
	titletext.setString("DAT\nNOISE!");
	titletext.setPosition(tile_size*c + 48, ((tile_size*r) / 2) - 250);
	titletext.setCharacterSize(18);
	titletext.setFillColor(sf::Color::White);

	seedtext.setFont(font);
	seedtext.setString(s_txt);
	seedtext.setPosition(tile_size*c + 6, ((tile_size*r) / 2) - 72);
	seedtext.setCharacterSize(10);
	seedtext.setFillColor(sf::Color::Green);
	// select the font
	if (!font.loadFromFile("Pixeled.ttf"))
	{
		// error...
		printf("Error");
	}
	text.setFont(font);
	text.setString("UP - INCREASE TIME\n\nDOWN - DECREASE TIME\n\nSPACE - CHANGE SEED\n\n\n\nQ - REGULAR \n\nW - HELL\n\nE - LAGOON");
	text.setPosition(tile_size*c+6, ((tile_size*r)/2)-20);
	text.setCharacterSize(10);
	text.setFillColor(sf::Color::Red);

	string tileset = "Tileset16b.png";

	// define the level with an array of tile indices
	/*const int level[] =
	{
		0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0,
		1, 1, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,
		0, 1, 0, 0, 2, 0, 3, 3, 3, 0, 1, 1, 1, 0, 0, 0,
		0, 1, 1, 0, 3, 3, 3, 0, 0, 0, 1, 1, 1, 2, 0, 0,
		0, 0, 1, 0, 3, 0, 2, 2, 0, 0, 1, 1, 1, 1, 2, 0,
		2, 0, 1, 0, 3, 0, 2, 2, 2, 0, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 0, 3, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1,
	};*/

	// create the tilemap from the level definition
	TileMap map;
	if (!map.load(tileset, sf::Vector2u(tile_size, tile_size), level, c, r))
		return -1;

	// run the main loop
	while (window.isOpen())
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Up)
			{
				z = z + 0.05;
				cout << z;
				noisy.mapper(level, r, c,z,seed);
				if (!map.load(tileset, sf::Vector2u(tile_size, tile_size), level, c, r))
					return -1;
			}
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Down)
			{
				z = z - 0.05;
				cout << z;
				noisy.mapper(level, r, c, z,seed);
				if (!map.load(tileset, sf::Vector2u(tile_size, tile_size), level, c, r))
					return -1;
			}
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space)
			{
				seed = (rand() % 100);
				cout << "seed " << seed << endl;
				s_txt = "SEED : ";
				s_txt += to_string(seed);
				seedtext.setString(s_txt);
				noisy.mapper(level, r, c, z, seed);
				if (!map.load(tileset, sf::Vector2u(tile_size, tile_size), level, c, r))
					return -1;
			}
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Q)
			{
				
				tileset = "Tileset16b.png";
				if (!map.load(tileset, sf::Vector2u(tile_size, tile_size), level, c, r))
					return -1;
			}
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::W)
			{

				tileset = "Tileset16.png";
				if (!map.load(tileset, sf::Vector2u(tile_size, tile_size), level, c, r))
					return -1;
			}
			if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::E)
			{

				tileset = "Tileset16c.png";
				if (!map.load(tileset, sf::Vector2u(tile_size, tile_size), level, c, r))
					return -1;
			}


		}
		
		// draw the map
		window.clear();
		window.draw(map);
		window.draw(text);
		window.draw(seedtext);
		window.draw(titletext);
		window.display();

	}

	return 0;
}