/*
 * Map.cpp
 * Author: GSI-UAH
 * Date:   25/03/2012
 *
 * Description: map information and related functions.
 * Access to point: map[y-row][x-col]
 *
*/

#include "Map.h"

int Map::MAX_C = 8; // Valores mayores son obstaculos
char Map::OBS = 'x';
POINT Map::void_p(-1,-1,MAX_C+1);

Map::Map()
{
	cols = 0;
	rows = 0;
}

Map::~Map()
{
}

bool Map::read_map(const char* filename)
{
	// Archivo con el mapa
	fstream file;
	file.open(filename, std::ios::in);
	if(!file.is_open())
	{
		std::cout << "No se ha podido abrir el archivo de entrada " << filename << endl;
		return false;
	}
	// Leer el archivo de entrada linea a linea
	char lin[32];
	POINT newpoint;
	vector<POINT> newrow;
	map.push_back(newrow); // Primera fila
	file.getline(lin, sizeof(lin), ' '); // Lectura adelantada
	while(!file.eof())
	{
		if(lin[0] == '\n') // Fin de fila
		{
			rows++;
			cols = 0;
			map.push_back(newrow);
		}
		else if(strcmp(lin, (char*)"")) // Siguiente columna
		{
			newpoint.X = cols;
			newpoint.Y = rows;
			if(!isdigit(lin[0])) // Si no es numero es obstaculo
				newpoint.C = MAX_C+1;
			else
				newpoint.C = atoi(lin); // C es el valor dado en el archivo
			cols++;
			map[rows].push_back(newpoint); // Añadir nuevo punto
		}
		file.getline(lin, sizeof(lin), ' ');
	}
	// Establecer numero de filas y columnas
	rows = map.size();
	cols = map[0].size();
	file.close();
	for(int i=0; i<rows/2; i++)
	{
		vector<POINT> newrow = map[i];
		map[i] = map[rows-i-1];
		map[rows-i-1] = newrow;
		
	}
	for(int i=0; i<rows; i++)
		for(int j=0; j<cols; j++)
			map[i][j].Y = i;
	return true;
}

void Map::reset_data()
{
	for(int i=0; i<cols; i++)
		for(int j=0; j<rows; j++)
		{	
			map[j][i].G = 0;
			map[j][i].H = 0;
			map[j][i].F = -1;
		}
}

bool Map::safety_margin(int desp, float segf)
{
	if(desp < 1 || segf < 0.1)
		return false;
	// Establecer margenes de seguridad
	int dist;
	float auxf;
	for(int col = 0; col < cols; col++)
	for(int row = 0; row < rows; row++)
		if(map[row][col].C > MAX_C)
		for(int i = col-desp; i < col+desp+1; i++)
			for(int j = row-desp; j < row+desp+1; j++)
				if(valid_pos(i,j) && map[j][i].C <= MAX_C)
				{
					dist = abs(j-row) + abs(i-col);
					if(dist == 1 || (dist == 2 && abs(j-row) == 1) )
						map[j][i].S = 3;
					else
						if(abs(j-row) == abs(i-col)) dist /= 2;
						auxf = pow(desp - dist + 2, 2);
						if(map[j][i].S < auxf) // Solo se guarda el margen de seguridad mas alto
							map[j][i].S = auxf;
				}
	return true;
}

void Map::deactivate_safety_margin()
{
	// Desactivar margenes de seguridad
	for(int col = 0; col < cols; col++)
	    for(int row = 0; row < rows; row++)
		    map[row][col].S = 0;
}

bool Map::write_map(const char* filename, int ncols, int nrows, int maxc)
{
	if(ncols <=0 || nrows <=0)
		return false;
	int m[ncols*maxc][nrows*maxc];
	char lin[32];
	char room[16];
	fstream file;
	// Se leen tantos patrones de entrada como ncols*nrows
	for(int i=0; i<ncols; i++)
		for(int j=0; j<nrows; j++)
		{
			cols = 0;
			rows = 0;
			// Abrir patron de entrada (elegido aleatoriamente entre r0-r6)
			sprintf(room,"test/r%d",rand()%7);
			file.open(room, std::ios::in);
			if(!file.is_open())
			{
				std::cerr << "No se ha podido abrir el archivo de entrada " << room << endl;
				return false;
			}
			// Leer patron de entrada
			file.getline(lin, sizeof(lin), ' '); // Lectura adelantada
			while(!file.eof())
			{
				if(lin[0] == '\n') // Siguiente fila
				{
					rows++;
					cols = 0;
				}
				else if(strcmp(lin, (char*)"")) // Siguiente punto
				{
					m[cols+(i*maxc)][rows+(j*maxc)] = atoi(lin); // Establecer coste del punto
					cols++;
				}
				file.getline(lin, sizeof(lin), ' ');
			}
			file.close();
		}
	// Se han leido todos los patrones
	// Escribir el nuevo mapa en el archivo pedido
	file.open(filename, std::ios::out);
	if(!file.is_open())
	{
		std::cerr << "No se ha podido abrir el archivo de salida " << filename << endl;
		return false;
	}

	for(int i = 0; i < ncols*maxc; i++)
	{
		for(int j = 0; j < nrows*maxc; j++)
		{
			sprintf(lin, " %d", m[i][j]);
			file.write(lin, strlen(lin));
		}
		file.write((char*)" \n", 2); // Siguiente fila
	}
	file.close();
	return true;
}

bool Map::write_path(const char* filename, vector<POINT*> path, float res, const char* dat)
{
	// Archivo con la solucion
	fstream file;
	file.open(filename, std::ios::out);
	if(!file.is_open())
	{
		std::cout << "No se ha podido abrir el archivo de salida " << filename << endl;
		return false;
	}
	char lin[32];
	// ***** ESCRIBIR LA CABECERA DEL ARCHIVO *****
	file.write(dat, strlen(dat));
	sprintf(lin, "# X   Y   Z; Pasos: %d \n", (int)path.size());
	file.write(lin, strlen(lin));
	// Escribir la ruta
	for(int i=path.size()-1; i>=0; i--)
	{
		// ***** ESCRIBIR SIGUIENTE PUNTO *****
		sprintf(lin, "%0.3f %0.3f   0 \n", path[i]->X*res, path[i]->Y*res);
		file.write(lin, strlen(lin));
	}
	file.close();
	return true;
}
			
POINT* Map::get_point(int x, int y)
{
	if(!valid_pos(x,y)) // Si la posicion no es valida devolver punto vacio
		return &void_p;
	return &map[y][x];
}

vector<POINT*> Map::get_succesors(const POINT* p)
{
	vector<POINT*> succ;
	if(!valid_pos(p->X, p->Y)) // Si el punto no es valido se devuelve vacio
		return succ;
	for(int j = p->Y-1; j < p->Y+2; j++)
		for(int i = p->X-1; i < p->X+2; i++)
			if(valid_pos(i,j) && !(i == p->X && j == p->Y)) // No incluir el punto p
			if(map[j][i].C <= MAX_C) // Si es un obstaculo tampoco se incluye
				succ.push_back(&map[j][i]);
	return succ;
}

float Map::get_dist(const POINT* p, const POINT* t)
{
	return get_dist(p->X, p->Y, t->X, t->Y);
}

float Map::get_dist(int x0, int y0, int x1, int y1)
{
	return sqrt( pow(x1-x0, 2) + pow(y1-y0, 2) );
}

bool Map::valid_pos(const POINT* p)
{
	return valid_pos(p->X, p->Y);
}

bool Map::valid_pos(int x, int y)
{
	return (x>=0 && x<cols && y>=0 && y<rows);
}

bool Map::is_obstacle(const POINT* p)
{
	return is_obstacle(p->X, p->Y);
}

bool Map::is_obstacle(int x, int y)
{
	if(valid_pos(x,y))
		return map[y][x].C > MAX_C;
	else
		return true;
}

int Map::get_ncols()
{
	return cols;
}

int Map::get_nrows()
{
	return rows;
}

void Map::print_map(bool withc)
{
	std::cerr << "Map [ " << cols << " x " << rows << " ]" << endl;
	for(int j = 0; j < rows; j++)
	{
		std::cerr << "<"<<map[j][0].Y<<"> "; 
		if(map[j][0].Y < 10)
			std::cerr << " "; 
		for(int i = 0; i < cols; i++)
			if(map[j][i].C > MAX_C) // Si es un obstaculo mostrar OBS
				std::cerr << " " << OBS << " ";
			else
            {
				if(map[j][i].C < 0)
					std::cerr << " ·"; // El punto pertenece a la ruta
			    else
					if(withc) // Si se usan costes mostrar el coste de la celda
						std::cerr << " " << map[j][i].C;
				if(map[j][i].S > 1)
						std::cerr << " +";
					else
						std::cerr << " ";
            }
		std::cerr << endl;
	}
}
