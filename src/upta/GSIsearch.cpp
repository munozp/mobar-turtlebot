/*
 * GSIsearch.cpp
 * Author: GSI-UAH
 * Date:   28/03/2012
 *
 * Description: path search using Basic Theta* algorithm.
 *
*/

#include "GSIsearch.h"

GSIsearch::GSIsearch(Map *dem, bool cost, bool dbg)
{
	map = dem;
	costs = cost;
	debug = dbg;
	exp = 0;
    distancia = -1;
}

GSIsearch::~GSIsearch()
{
	open.clear();
	closed.clear();
	path.clear();
}

void GSIsearch::clear_data()
{
	open.clear();
	closed.clear();
	path.clear();
    distancia = -1;
	exp = 0;
}

bool GSIsearch::search(POINT start, POINT goal)
{
	// Establecer valores del punto inicial
	POINT* pos;
	pos = map->get_point(start.X, start.Y);
	setF(pos, 0 ,map->get_dist(&start, &goal));
	setParent(pos, pos);
	open.insert(pair<float,POINT*>(pos->F, pos));
	vector<POINT*> s;
    if(debug)
    {
        std::cout << "BUSCANDO RUTA ENTRE:" << std::endl;
        printPoint(start);
        printPoint(goal);
    }
	while(!open.empty())
	{
		// Sacar el primer nodo de la lista de abiertos ...
		pos = open.begin()->second;
		open.erase(pos->F);
		// ... e insertarlo en la de cerrados
		closed.insert(pair<unsigned int,POINT*>(hash(*pos),pos));
		exp++;
		if(debug)
		{	
			std::cout<<"----------" << endl;
			std::cout<<"Posicion: "; printPoint(*pos);
			std::cout<<"  \\padre: "; printPoint(*pos->parent);
			std::cout<<"----------" << endl;
		}
		// El nodo actual es el objetivo?
		if(*pos == goal)
		{
			if(debug)
				std::cout << "OBJETIVO ALCANZADO" << endl;
            distancia = 0;
			while(*pos != start) // Recorrer punteros padre hacia atras //BUCLE INFINITO
			{
                distancia += map->get_dist(pos, pos->parent);
				path.push_back(pos);
				pos=pos->parent;
			}
			path.push_back(pos); // Añadir posicion inicial
			return true;
		}
		// Generar sucesores para la posicion actual
		s = map->get_succesors(pos);
		for(unsigned int i=0; i<s.size(); i++)
			// Comprobar si el nodo ya ha sido tratado
			if(closed.find(hash(*s[i])) == closed.end())
			{
				// Si F<0 el sucesor no esta en la lista de abiertos
				if(s[i]->F < 0)
					setF(s[i], FLT_MAX, 0);
				update_vertex(pos, s[i], &goal); // Actualizar el sucesor
			}
		s.clear(); // Limpiar lista de sucesores
		if(debug)
		{
			print_open();
			print_closed();
		}
	}
	// Lista de abiertos vacia, no se ha encontrado solucion
	if(debug)
		std::cout << "NO SE HA ENCONTRADO RUTA" << endl;
	return false;
}

void GSIsearch::update_vertex(POINT* pos, POINT* succ, POINT* goal)
{
    std::multimap<float,POINT*>::iterator it;
    float cost = segment_cost(pos->parent, succ); // Coste y linea de visibilidad
    if(debug)
   	{
   		std::cout << "Linea de vision?: " << (cost>0?"SI":"NO") << endl;
    	printPoint(*pos->parent); std::cout << " > "; printPoint(*succ);
  	}

	// ******** ELIMINAR PARA CONSEGUIR A* ...
	if(cost > 0) // Hay linea de visibilidad => any-angle path
	{
		cost += pos->parent->G;
		if(cost < succ->G) // Mejora el coste?
		{
			// Eliminarlo de la lista de abiertos ...
			bool removed = false;
			it=open.begin();
			while(it != open.end() && !removed)
			{
				if(hash(*(*it).second) == hash(*succ))
				{
					open.erase(it);
					removed = true;
				}
				it++;
			}
			// ... e insertarlo actualizado
			setF(succ, cost, map->get_dist(succ, goal));
			setParent(succ, pos->parent);
			open.insert(pair<float,POINT*>(succ->F,succ));
		}
	}
	else
	// ... HASTA AQUI ************************
	{
		// No hay linea de visibilidad => igual que A*
		cost = pos->G + map->get_dist(pos, succ) * succ->S;
		if(cost < succ->G) // Mejora el coste?
		{
			// Eliminarlo de la lista de abiertos ...
			bool removed = false;
			it=open.begin();
			while(it != open.end() && !removed)
			{
				if(hash(*(*it).second) == hash(*succ))
				{
					open.erase(it);
					removed = true;
				}
				it++;
			}
			// ... e insertarlo actualizado
			setF(succ, cost, map->get_dist(succ, goal));
			setParent(succ, pos);
			open.insert(pair<float,POINT*>(succ->F,succ));
		}
	}
}


float GSIsearch::segment_cost(POINT* pos, POINT* succ)
{
	int x, y;
	// Obtener puntos de corte
    vector<pair<float,float> > points= axis_points(pos, succ);
	// Analizar puntos de corte
	for(unsigned int i=0; i<points.size()-1; i++)
	{
		x = (int)(points[i].first+0.5);
		y = (int)(points[i].second+0.5);
		if(map->is_obstacle(x,y))
        {
            points.clear();
			return -1;
        }
		if(map->get_point(x,y)->S > 2) // Margen de seguridad
        {
            points.clear();
			return -1;
        }
	}
    points.clear();
	return map->get_dist(pos, succ);
}

vector<pair<float,float> > GSIsearch::axis_points(POINT* pos, POINT* succ)
{
	int xs = pos->X;
	int ys = pos->Y;        
	int xg = succ->X;
	int yg = succ->Y;
	float stepx = (xg >= xs)? 1 : -1;
	float stepy = (yg >= ys)? 1 : -1;
	vector<pair<float,float> > points;
	if((yg - ys) == 0) // Movimiento horizontal
	{
		int xa;
		points.push_back(pair<float,float>(xs,ys));
		while(xs != xg)
		{
			xa = xs;
			xs += (int)stepx;
			points.push_back(pair<float,float>(xs,ys));
		}
		return points;
	}
	if((xg - xs) == 0) // Movimiento vertical
	{
		int ya;
		points.push_back(pair<float,float>(xs,ys));
		while(ys != yg)
		{
			ya = ys;
			ys += (int)stepy;
			points.push_back(pair<float,float>(xs,ys));
		}
		return points;
	}
	// Movimiento diagonal
	float p = (float)(yg - ys) / (xg - xs);
	stepx /= 2;
	stepy /= 2;
	float xa = xs;
	float ya = ys;
	float Xx = xs + stepx;  // Eje X coordenada x
	float Yy = ys + stepy;  // Eje Y coordenada y
	float Yx = xs + (Yy - ys)/p; // Eje Y coordenada x
	float Xy = ys + (Xx - xs)*p; // Eje X coordenada y
	// Añadir el punto origen a la lista
	points.push_back(pair<float,float>(xs,ys));
	while(Xx != xg || Yy != yg)
		if((Xx <= Yx && stepx > 0) || (Xx >= Yx && stepx < 0))
		{
			points.push_back(pair<float,float>(Xx,Xy)); // Añadir corte en eje X
			if(Xx == Yx) // El punto es un vertice
			{
				Yy += stepy; // Avanzar eje Y
				Yx = xs + (Yy - ys)/p;
			}
			xa = Xx; // Avanzar eje X
			ya = Xy;
			Xx += stepx;
			Xy = ys + (Xx - xs)*p;
		}
		else
		{
			points.push_back(pair<float,float>(Yx,Yy)); // Añadir corte en eje Y
			xa = Yx; // Avanzar eje Y
			ya = Yy;
			Yy += stepy;
			Yx = xs + (Yy - ys)/p;
		}
	// Añadir punto destino (no incluido en el while)
	points.push_back(pair<float,float>(xg,yg));
	return points;
}


vector<POINT*> GSIsearch::get_path()
{
	if(debug)
	{
		std::cout << "RUTA OBTENIDA:" << endl;
		for(int i=path.size()-1; i>=0; i--)
			printPoint(*path[i]);
	}
	return path;
}

float GSIsearch::get_dist()
{
    if(distancia > 0)
       return distancia;
	else
		return -1;
}

float GSIsearch::get_cost()
{
	if(path.size() > 0)
		return path[0]->G;
	else
		return -1;
}

unsigned int GSIsearch::expanded_nodes()
{
	return exp;
}

void GSIsearch::print_open()
{
	std::cout << endl << "Abiertos: [" << open.size() << "]" << endl;
	std::multimap<float,POINT*>::iterator it;
	for(it=open.begin(); it != open.end(); it++ )
	{
	    printPoint(*(*it).second);
		std::cout << "  \\padre: ";
		printPoint(*(*it).second->parent);
	}
}

void GSIsearch::print_closed()
{
	std::cout << endl << "CERRADOS: [" << closed.size() << "]" << endl;
	std::map<unsigned int, POINT*>::iterator it;
	for(it=closed.begin(); it != closed.end(); it++ )
	{
		printPoint(*(*it).second);
		std::cout << "  \\padre: ";
		printPoint(*(*it).second->parent);
	}
}

