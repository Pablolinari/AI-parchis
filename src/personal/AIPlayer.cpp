#include "AIPlayer.h"
#include "../../include/model/Parchis.h"
#include "model/Heuristic.h"
#include <iterator>
#include <utility>

const float masinf = 9999999999.0, menosinf = -9999999999.0;
const float gana = masinf / 10.f, pierde = menosinf / 10.f;
const int num_pieces = 2;
const int PROFUNDIDAD_MINIMAX =
    4; // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA =
    8; // Umbral maximo de profundidad para la poda Alfa_Beta

bool AIPlayer::move() {
  cout << COUT_ORANGE_BOLD << "Realizo un movimiento automatico" << COUT_NOCOLOR
       << endl;

  color c_piece;
  int id_piece;
  int dice;
  think(c_piece, id_piece, dice);

  cout << COUT_ORANGE_BOLD << "Movimiento elegido: " << str(c_piece) << " "
       << id_piece << " " << dice << COUT_NOCOLOR << endl;

  actual->movePiece(c_piece, id_piece, dice);
  return true;
}

float Minimax(const Parchis &actual, int jugador, int profundidad,
              int profundidad_max, color &c_piece, int &id_piece, int &dice,
              Heuristic *heuristic) {

  if (profundidad == profundidad_max ||
      actual.gameOver()) { // Nodo terminal o profundidad límite: llamo a la
                           // función heurística
    // IMPORTANTE: USAMOS EL MÉTODO evaluate AUNQUE HAYAMOS REDEFINIDO LA CLASE
    // HEURISTIC
    return heuristic->evaluate(actual, jugador);
  }
  // Comparo mi jugador con el actual de la partida para saber en qué tipo de
  // nodo estoy
  else if (actual.getCurrentPlayerId() == jugador) { // Nodo MAX
    float valor = menosinf; // Inicialización lo más pequeña posible para ir
                            // calculando el máximo
    // Obtengo los hijos del nodo actual y los recorro
    vector<ParchisSis> rama = actual.getChildrenList();
    for (int i = 0; i < rama.size(); i++) {
      ParchisSis hijo_i = rama[i];
      Parchis nuevo_hijo = *hijo_i;
      if (NodeCounter::isLimitReached()) {
        cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial"
             << endl;
        if (profundidad == 0) {
          c_piece = hijo_i.getMovedColor();
          id_piece = hijo_i.getMovedPieceId();
          dice = hijo_i.getMovedDiceValue();
        }
        return valor;
      }
      // Búsqueda en profundidad (llamada recursiva)

      float new_val =
          Minimax(nuevo_hijo, jugador, profundidad + 1, profundidad_max,
                  c_piece, id_piece, dice, heuristic);
      if (new_val > valor) {
        // Me voy quedando con el máximo
        valor = new_val;
        if (profundidad == 0) {
          // Almaceno el movimiento que me ha llevado al mejor valor (solo en la
          // raíz)
          c_piece = hijo_i.getMovedColor();
          id_piece = hijo_i.getMovedPieceId();
          dice = hijo_i.getMovedDiceValue();
        }
      }
    }
    return valor;
  } else {                // Nodo MIN
    float valor = masinf; // Inicialización lo más grande posible para ir
                          // calculando el mínimo
    // Obtengo los hijos del nodo actual y los recorro
    ParchisBros rama = actual.getChildren();
    for (ParchisBros::Iterator it = rama.begin(); it != rama.end(); ++it) {
      Parchis nuevo_hijo = *it;
      // Búsqueda en profundidad (llamada recursiva)
      float new_val =
          Minimax(nuevo_hijo, jugador, profundidad + 1, profundidad_max,
                  c_piece, id_piece, dice, heuristic);
      // Me voy quedando con el mínimo
      if (new_val < valor) {
        valor = new_val;
      }
    }
    return valor;
  }
}

float PodaAlphaBeta(const Parchis &actual, int jugador, int profundidad,
                    int profundidad_max, color &c_piece, int &id_piece,
                    int &dice, Heuristic *heuristic, float alpha, float beta) {

  if (profundidad == profundidad_max ||
      actual.gameOver()) { // Nodo terminal o profundidad límite: llamo a la
                           // función heurística
    // IMPORTANTE: USAMOS EL MÉTODO evaluate AUNQUE HAYAMOS REDEFINIDO LA CLASE
    // HEURISTIC
    return heuristic->evaluate(actual, jugador);
  }
  // Comparo mi jugador con el actual de la partida para saber en qué tipo de
  // nodo estoy
  else if (actual.getCurrentPlayerId() == jugador) { // Nodo MAX
    float valor = menosinf; // Inicialización lo más pequeña posible para ir
                            // calculando el máximo
    // Obtengo los hijos del nodo actual y los recorro
    vector<ParchisSis> rama = actual.getChildrenList();
    for (int i = 0; i < rama.size(); i++) {
      ParchisSis hijo_i = rama[i];
      Parchis nuevo_hijo = *hijo_i;
      if (NodeCounter::isLimitReached()) {
        cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial"
             << endl;
        if (profundidad == 0) {
          c_piece = hijo_i.getMovedColor();
          id_piece = hijo_i.getMovedPieceId();
          dice = hijo_i.getMovedDiceValue();
        }
        return valor;
      }
      // Búsqueda en profundidad (llamada recursiva)
      float new_val =
          PodaAlphaBeta(nuevo_hijo, jugador, profundidad + 1, profundidad_max,
                        c_piece, id_piece, dice, heuristic, alpha, beta);
      if (new_val > valor) {
        // Me voy quedando con el máximo
        valor = new_val;
        if (profundidad == 0) {
          // Almaceno el movimiento que me ha llevado al mejor valor (solo en la
          // raíz)
          c_piece = hijo_i.getMovedColor();
          id_piece = hijo_i.getMovedPieceId();
          dice = hijo_i.getMovedDiceValue();
        }
      }
      alpha = max(alpha, valor);
      if (alpha >= beta) {
        break;
      }
    }
    return valor;
  } else {                // Nodo MIN
    float valor = masinf; // Inicialización lo más grande posible para ir
                          // calculando el mínimo
    // Obtengo los hijos del nodo actual y los recorro
    ParchisBros rama = actual.getChildren();
    for (ParchisBros::Iterator it = rama.begin(); it != rama.end(); ++it) {
      Parchis nuevo_hijo = *it;
      // Búsqueda en profundidad (llamada recursiva)
      float new_val =
          PodaAlphaBeta(nuevo_hijo, jugador, profundidad + 1, profundidad_max,
                        c_piece, id_piece, dice, heuristic, alpha, beta);
      // Me voy quedando con el mínimo
      if (new_val < valor) {
        valor = new_val;
      }
      beta = min(beta, valor);
      if (alpha >= beta) {
        break;
      }
    }
    return valor;
  }
}

float OrdenacionMovimientos(const Parchis &actual, int jugador, int profundidad,
                            int profundidad_max, color &c_piece, int &id_piece,
                            int &dice, Heuristic *heuristic, Heuristic *ordenar,
                            float alpha, float beta) {

  if (profundidad == profundidad_max || actual.gameOver()) {
    return heuristic->evaluate(actual, jugador);
  } else if (actual.getCurrentPlayerId() == jugador) { // Nodo MAX
    float valor = menosinf;
    vector<ParchisSis> rama = actual.getChildrenList();
    if (profundidad < 2) {
      vector<pair<float, ParchisSis>> ordenados;
      for (int i = 0; i < rama.size(); i++) {
        ParchisSis hijo_i = rama[i];
        Parchis hijonuevo = *hijo_i;
        float heuristica = ordenar->evaluate(hijonuevo, jugador);
        ordenados.emplace_back(make_pair(heuristica, hijo_i));
      }
      sort(ordenados.begin(), ordenados.end(),
           [](const auto &a, const auto &b) { return a.first > b.first; });
      rama.clear();
      for (int i = 0; i < ordenados.size(); i++) {
        rama.emplace_back(ordenados[i].second);
      }
    }
    for (int i = 0; i < rama.size(); i++) {
      ParchisSis hijo_i = rama[i];
      Parchis nuevo_hijo = *hijo_i;
      if (NodeCounter::isLimitReached()) {
        cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial"
             << endl;
        if (profundidad == 0) {
          c_piece = hijo_i.getMovedColor();
          id_piece = hijo_i.getMovedPieceId();
          dice = hijo_i.getMovedDiceValue();
        }
        return valor;
      }
      float new_val =
          PodaAlphaBeta(nuevo_hijo, jugador, profundidad + 1, profundidad_max,
                        c_piece, id_piece, dice, heuristic, alpha, beta);
      if (new_val > valor) {
        valor = new_val;
        if (profundidad == 0) {
          c_piece = hijo_i.getMovedColor();
          id_piece = hijo_i.getMovedPieceId();
          dice = hijo_i.getMovedDiceValue();
        }
      }
      alpha = max(alpha, valor);
      if (alpha >= beta) {
        break;
      }
    }
    return valor;
  } else {                // Nodo MIN
    float valor = masinf; // Inicialización lo más grande posible para ir
                          // calculando el mínimo
                          // Obtengo los hijos del nodo actual y los recorro
    vector<ParchisSis> rama = actual.getChildrenList();
    if (profundidad < 2) {
      vector<pair<float, ParchisSis>> ordenados;
      for (int i = 0; i < rama.size(); i++) {
        ParchisSis hijo_i = rama[i];
        Parchis hijonuevo = *hijo_i;
        float heuristica = ordenar->evaluate(hijonuevo, jugador);
        ordenados.emplace_back(make_pair(heuristica, hijo_i));
      }
      sort(ordenados.begin(), ordenados.end(),
           [](const auto &a, const auto &b) { return a.first < b.first; });
      rama.clear();
      for (int i = 0; i < ordenados.size(); i++) {
        rama.emplace_back(ordenados[i].second);
      }
    }
    for (int i = 0; i < rama.size(); i++) {
      Parchis nuevo_hijo = *rama[i];
      // Búsqueda en profundidad (llamada recursiva)
      float new_val =
          PodaAlphaBeta(nuevo_hijo, jugador, profundidad + 1, profundidad_max,
                        c_piece, id_piece, dice, heuristic, alpha, beta);
      // Me voy quedando con el mínimo
      if (new_val < valor) {
        valor = new_val;
      }
      beta = min(beta, valor);
      if (alpha >= beta) {
        break;
      }
    }
    return valor;
  }
}

void AIPlayer::thinkAleatorio(color &c_piece, int &id_piece, int &dice) const {
  // IMPLEMENTACIÓN INICIAL DEL AGENTE
  // Esta implementación realiza un movimiento aleatorio.
  // Se proporciona como ejemplo, pero se debe cambiar por una que realice un
  // movimiento inteligente
  // como lo que se muestran al final de la función.

  // OBJETIVO: Asignar a las variables c_piece, id_piece, dice (pasadas por
  // referencia) los valores,
  // respectivamente, de:
  // - color de ficha a mover
  // - identificador de la ficha que se va a mover
  // - valor del dado con el que se va a mover la ficha.

  // El id de mi jugador actual.
  int player = actual->getCurrentPlayerId();

  // Vector que almacenará los dados que se pueden usar para el movimiento
  vector<int> current_dices;
  // Vector que almacenará los ids de las fichas que se pueden mover para el
  // dado elegido.
  vector<tuple<color, int>> current_pieces;

  // Se obtiene el vector de dados que se pueden usar para el movimiento
  current_dices = actual->getAvailableNormalDices(player);
  // Elijo un dado de forma aleatoria.
  dice = current_dices[rand() % current_dices.size()];

  // Se obtiene el vector de fichas que se pueden mover para el dado elegido
  current_pieces = actual->getAvailablePieces(player, dice);

  // Si tengo fichas para el dado elegido muevo una al azar.
  if (current_pieces.size() > 0) {
    int random_id = rand() % current_pieces.size();
    id_piece = get<1>(current_pieces[random_id]); // get<i>(tuple<...>) me
                                                  // devuelve el i-ésimo
    c_piece = get<0>(current_pieces[random_id]);  // elemento de la tupla
  } else {
    // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN
    // me permite no mover).
    id_piece = SKIP_TURN;
    c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color
                                         // actual al pasar turno.
  }

  // El siguiente código se proporciona como sugerencia para iniciar la
  // implementación del agente.

  float valor; // Almacena el valor con el que se etiqueta el estado tras el
               // proceso de busqueda.
  float alpha = menosinf,
        beta = masinf; // Cotas iniciales de la poda AlfaBeta
  // Llamada a la función para la poda (los parámetros son solo una
  // sugerencia, se pueden modificar).
  ValoracionTest valoracionTest;

  // ----------------------------------------------------------------- //

  // Si quiero poder manejar varios comportamientos, puedo usar la variable id
  // del agente para usar una u otra. switch (id)
  // {
  // case 0:
  //    // Mi implementación base de la poda con ValoracionTest
  //    valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA,
  //    c_piece, id_piece, dice, alpha, beta, &valoracionTest); break;
  // case 1:
  //    // Mi implementación definitiva con la que gano a todos los ninjas.
  //    valor = Poda_Final2DefinitivaAhoraSi(*actual, jugador, 0,
  //    PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta,
  //    &miValoracion3); break;
  // case 2:
  //    // Las distintas pruebas que he realizado (primera prueba)
  //    valor = Poda_AlfaBeta_Mejorada(*actual, jugador, 0,
  //    PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta,
  //    &miValoracion1); break;
  // case 3:
  //    // Las distintas pruebas que he realizado (segunda prueba)
  //    valor = Poda_AlfaBeta_SegundaMejora(*actual, jugador, 0,
  //    PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta,
  //    &miValoracion1); break;
  //  // ...
  // }
}

void AIPlayer::thinkFichaMasAdelantada(color &c_piece, int &id_piece,
                                       int &dice) const {
  // Elijo el dado haciendo lo mismo que el jugador aleatorio.
  thinkAleatorio(c_piece, id_piece, dice);
  // Tras llamar a esta función, ya tengo en dice el número de dado que quiero
  // u ar. Ahora, en vez de mover una ficha al azar, voy a mover (o a aplicar
  // el dado especial a) la que esté más adelantada (equivalentemente, la más
  // ercana a la meta).
  int player = actual->getCurrentPlayerId();
  vector<tuple<color, int>> current_pieces =
      actual->getAvailablePieces(player, dice);
  int id_ficha_mas_adelantada = -1;
  color col_ficha_mas_adelantada = none;
  int min_distancia_meta = 9999;
  for (int i = 0; i < current_pieces.size(); i++) {
    // distanceToGoal(color, id) devuelve la distancia a la meta de la ficha
    // [id] del color que le indique.
    color col = get<0>(current_pieces[i]);
    int id = get<1>(current_pieces[i]);
    int distancia_meta = actual->distanceToGoal(col, id);
    if (distancia_meta < min_distancia_meta) {
      min_distancia_meta = distancia_meta;
      id_ficha_mas_adelantada = id;
      col_ficha_mas_adelantada = col;
    }
  }
  // Si no he encontrado ninguna ficha, paso turno.
  if (id_ficha_mas_adelantada == -1) {
    id_piece = SKIP_TURN;
    c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color
                                         // actual al pasar turno.
  }
  // En caso contrario, moveré la ficha más adelantada.
  else {
    id_piece = id_ficha_mas_adelantada;
    c_piece = col_ficha_mas_adelantada;
  }
}
void AIPlayer::thinkMejorOpcion(color &c_piece, int &id_piece,
                                int &dice) const {}
void AIPlayer::think(color &c_piece, int &id_piece, int &dice) const {
  float valor;
  Heuristica1 heuristic;
	Heuristica2 heuristic2;
  HeuristicaOrden hordenar;
  switch (id) {
  case 0:
    thinkAleatorio(c_piece, id_piece, dice);
    break;
  case 1:
    thinkFichaMasAdelantada(c_piece, id_piece, dice);
    break;
  case 2:
    valor = PodaAlphaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece,
                          id_piece, dice, &heuristic, menosinf, masinf);
    break;
  case 3:
    valor = OrdenacionMovimientos(*actual, jugador, 0, PROFUNDIDAD_ALFABETA,
                                  c_piece, id_piece, dice, &heuristic,
                                  &hordenar, menosinf, masinf);
    break;
  case 4:
    valor = PodaAlphaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece,
                          id_piece, dice, &heuristic2, menosinf, masinf);
    break;
  case 5:
    valor = OrdenacionMovimientos(*actual, jugador, 0, PROFUNDIDAD_ALFABETA,
                                  c_piece, id_piece, dice, &heuristic2,
                                  &hordenar, menosinf, masinf);
    break;
  }
}
float ValoracionTest::getHeuristic(const Parchis &estado, int jugador) const {
  // Heurística de prueba proporcionada para validar el funcionamiento del
  // algoritmo de búsqueda.

  int ganador = estado.getWinner();
  int oponente = (jugador + 1) % 2;

  // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o
  // el oponente.
  if (ganador == jugador) {
    return gana;
  } else if (ganador == oponente) {
    return pierde;
  } else {
    // Colores que juega mi jugador y colores del oponente
    vector<color> my_colors = estado.getPlayerColors(jugador);
    vector<color> op_colors = estado.getPlayerColors(oponente);

    // Recorro todas las fichas de mi jugador
    int puntuacion_jugador = 0;
    // Recorro colores de mi jugador.
    for (int i = 0; i < my_colors.size(); i++) {
      color c = my_colors[i];
      // Recorro las fichas de ese color.
      for (int j = 0; j < num_pieces; j++) {
        // Valoro positivamente que la ficha esté en casilla segura o meta.
        if (estado.isSafePiece(c, j)) {
          puntuacion_jugador++;
        } else if (estado.getBoard().getPiece(c, j).get_box().type == goal) {
          puntuacion_jugador += 5;
        }
      }
    }

    // Recorro todas las fichas del oponente
    int puntuacion_oponente = 0;
    // Recorro colores del oponente.
    for (int i = 0; i < op_colors.size(); i++) {
      color c = op_colors[i];
      // Recorro las fichas de ese color.
      for (int j = 0; j < num_pieces; j++) {
        if (estado.isSafePiece(c, j)) {
          // Valoro negativamente que la ficha esté en casilla segura o
          // meta.
          puntuacion_oponente++;
        } else if (estado.getBoard().getPiece(c, j).get_box().type == goal) {
          puntuacion_oponente += 5;
        }
      }
    }

    // Devuelvo la puntuación de mi jugador menos la puntuación del
    // oponente.
    return puntuacion_jugador - puntuacion_oponente;
  }
}
float HeuristicaOrden::getHeuristic(const Parchis &estado, int jugador) const {

  int ganador = estado.getWinner();
  int oponente = (jugador + 1) % 2;
  float puntuacion;

  if (ganador == jugador) {
    return gana;
  } else if (ganador == oponente) {
    return pierde;
  } else {
    vector<color> my_colors = estado.getPlayerColors(jugador);
    vector<color> op_colors = estado.getPlayerColors(oponente);

    for (int i = 0; i < my_colors.size(); i++) {
      color c = my_colors[i];
      for (int j = 0; j < num_pieces; j++) {
        puntuacion += 100 - estado.distanceToGoal(c, j);

        if (estado.isEatingMove()) {
          puntuacion += 20;
        }

        puntuacion -= estado.piecesAtHome(c) * 5;
      }
    }
    for (int i = 0; i < op_colors.size(); i++) {
      color c = op_colors[i];
      for (int j = 0; j < num_pieces; j++) {
        puntuacion -= 100 - estado.distanceToGoal(c, j);

        if (estado.isEatingMove()) {
          puntuacion -= 20;
        }
        puntuacion += estado.piecesAtHome(c) * 5;
      }
    }
  }
  return puntuacion;
}
float Heuristica1::getHeuristic(const Parchis &estado, int jugador) const {

  int ganador = estado.getWinner();
  int oponente = (jugador + 1) % 2;
  float puntuacion;

  if (ganador == jugador) {
    return gana;
  } else if (ganador == oponente) {
    return pierde;
  } else {
    vector<color> my_colors = estado.getPlayerColors(jugador);
    vector<color> op_colors = estado.getPlayerColors(oponente);

    for (int i = 0; i < my_colors.size(); i++) {
      color c = my_colors[i];
      for (int j = 0; j < num_pieces; j++) {
        puntuacion += 100 - estado.distanceToGoal(c, j);

        if (estado.isEatingMove()) {
          puntuacion += 20;
        }

        puntuacion -= estado.piecesAtHome(c);
        if (estado.isGoalMove()) {
          puntuacion += 15;
        }
      }
    }
    for (int i = 0; i < op_colors.size(); i++) {
      color c = op_colors[i];
      for (int j = 0; j < num_pieces; j++) {
        puntuacion -= 100 - estado.distanceToGoal(c, j);

        if (estado.isEatingMove()) {
          puntuacion -= 20;
        }

        puntuacion += estado.piecesAtHome(c);
        if (estado.isGoalMove()) {
          puntuacion -= 15;
        }
      }
    }
  }
  return puntuacion;
}
float Heuristica2::getHeuristic(const Parchis &estado, int jugador) const {

  int ganador = estado.getWinner();
  int oponente = (jugador + 1) % 2;
  float puntuacion;

  if (ganador == jugador) {
    return gana;
  } else if (ganador == oponente) {
    return pierde;
  } else {
    vector<color> my_colors = estado.getPlayerColors(jugador);
    vector<color> op_colors = estado.getPlayerColors(oponente);

    for (int i = 0; i < my_colors.size(); i++) {
      color c = my_colors[i];
      for (int j = 0; j < num_pieces; j++) {
        puntuacion +=estado.distanceToGoal(c, j);

        if (estado.isEatingMove()) {
          puntuacion += 30;
        }
        if (estado.isSafePiece(c, j)) {
          puntuacion += 10;
        }
        puntuacion -= estado.piecesAtHome(c) * 5;
        if (estado.isGoalMove()) {
          puntuacion += 15;
        }
      }
    }
    for (int i = 0; i < op_colors.size(); i++) {
      color c = op_colors[i];
      for (int j = 0; j < num_pieces; j++) {
        puntuacion -= 100 - estado.distanceToGoal(c, j);

        if (estado.isEatingMove()) {
          puntuacion -= 30;
        }
        if (estado.isSafePiece(c, j)) {
          puntuacion -= 10;
        }
        puntuacion += estado.piecesAtHome(c) * 5;
        if (estado.isGoalMove()) {
          puntuacion -= 20;
        }
      }
    }
  }
  return puntuacion;
}
