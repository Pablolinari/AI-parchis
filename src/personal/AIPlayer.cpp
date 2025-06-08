#include "AIPlayer.h"
#include "../../include/model/Parchis.h"
#include "model/Attributes.h"
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

float PodaProbabilistica(const Parchis &actual, int jugador, int profundidad,
                         int profundidad_max, color &c_piece, int &id_piece,
                         int &dice, Heuristic *heuristic, float alpha,
                         float beta, float tolerancia) {

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
      if (abs(beta - alpha) < tolerancia) {
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
      if (abs(alpha - beta) < tolerancia) {
        break;
      }
    }
    return valor;
  }
}

void AIPlayer::think(color &c_piece, int &id_piece, int &dice) const {
  float valor;
  ValoracionTest heuristic;
  Heuristica1 heuristic1;
  Heuristica2 heuristic2;
  HeuristicaOrden hordenar;
  switch (id) {
  case 0:
    valor = PodaAlphaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece,
                          id_piece, dice, &heuristic, menosinf, masinf);
    break;
  case 1:
    valor = PodaAlphaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece,
                          id_piece, dice, &heuristic2, menosinf, masinf);
    break;
  case 2:
    valor = PodaAlphaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece,
                          id_piece, dice, &heuristic1, menosinf, masinf);
    break;
  case 3:
    valor = OrdenacionMovimientos(*actual, jugador, 0, PROFUNDIDAD_ALFABETA,
                                  c_piece, id_piece, dice, &heuristic2,
                                  &hordenar, menosinf, masinf);
    break;
  case 4:
    valor =
        PodaProbabilistica(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece,
                           id_piece, dice, &heuristic2, menosinf, masinf, 5);
    break;
  case 5:
    valor = OrdenacionMovimientos(*actual, jugador, 0, PROFUNDIDAD_ALFABETA,
                                  c_piece, id_piece, dice, &heuristic1,
                                  &heuristic2, menosinf, masinf);
    break;
  case 6:
    valor =
        PodaProbabilistica(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece,
                           id_piece, dice, &heuristic1, menosinf, masinf, 5);
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
float Heuristica1::getHeuristic(const Parchis &estado, int jugador) const {
  // Heurística de prueba proporcionada para validar el funcionamiento del
  // algoritmo de búsqueda.

  int ganador = estado.getWinner();
  int oponente = (jugador + 1) % 2;

  int puntuacion_oponente = 0;
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
      for (int j = 0; j < num_pieces; j++) {
        puntuacion_jugador += 100 - estado.distanceToGoal(c, j);
        if (estado.isSafePiece(c, j)) {
          puntuacion_jugador += 5;
        }
        if (estado.getBoard().getPiece(c, j).get_box().type == goal) {
          puntuacion_jugador += 10;
        }
        if (estado.eatenPiece().first == c) {
          puntuacion_oponente += 40;
        }

        puntuacion_oponente += estado.piecesAtHome(c) * 30;
      }
    }

    // Recorro todas las fichas del oponente
    // Recorro colores del oponente.
    for (int i = 0; i < op_colors.size(); i++) {
      color c = op_colors[i];
      // Recorro las fichas de ese color.
      for (int j = 0; j < num_pieces; j++) {
        puntuacion_oponente += 100 - estado.distanceToGoal(c, j);
        if (estado.isSafePiece(c, j)) {
          puntuacion_oponente += 5;
        }
        if (estado.getBoard().getPiece(c, j).get_box().type == goal) {
          puntuacion_oponente += 10;
        }
        if (estado.eatenPiece().first == c) {
          puntuacion_jugador += 60;
        }
        puntuacion_jugador += estado.piecesAtHome(c) * 40;
      }
    }
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
        puntuacion -= estado.piecesAtHome(c) * 20;
      }
    }
    for (int i = 0; i < op_colors.size(); i++) {
      color c = op_colors[i];
      for (int j = 0; j < num_pieces; j++) {
        puntuacion -= 100 - estado.distanceToGoal(c, j);
        puntuacion += estado.piecesAtHome(c) * 20;
      }
    }
  }
  return puntuacion;
}
float Heuristica2::getHeuristic(const Parchis &estado, int jugador) const {

  int ganador = estado.getWinner();
  int oponente = (jugador + 1) % 2;
  float poponente = 0, pjugador = 0;

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
        pjugador += 100 - estado.distanceToGoal(c, j);

        if (estado.eatenPiece().first == c) {
          poponente += 40;
        }
        if (estado.isSafePiece(c, j)) {
          pjugador += 10;
        }

        poponente += estado.piecesAtHome(c) * 40;
        pjugador += estado.piecesAtGoal(c) * 15;
      }
    }
    for (int i = 0; i < op_colors.size(); i++) {
      color c = op_colors[i];
      for (int j = 0; j < num_pieces; j++) {
        poponente += 100 - estado.distanceToGoal(c, j);

        if (estado.eatenPiece().first == c) {
          pjugador += 60;
        }
        pjugador += estado.piecesAtHome(c) * 30;
      }
    }
  }
  return pjugador - poponente;
}
