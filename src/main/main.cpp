// Application entry point and FLTK interface.
#include "../../const/matrix.h"
#include <algorithm>
#include <atomic>
#include <iostream>
#include <math.h>
#include <mutex>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../headers/gui.h"
#include "../../headers/base.h"
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <utility>
#include <vector>
#include <X11/Xlib.h> 

using namespace std;

bool start_pressed = false;
int population = 0;
int ge_value = 0;
int rotation_interval = 46;
float initial_mutation = 0.00;
int gen = 0;
struct timespec tim, tim2;
bool mut_var = true;
std::atomic<bool> stop_requested(false);
std::atomic<bool> destination_reached(false);
std::atomic<bool> solution_found(false);

Entity **cockroaches = nullptr;
Entity *thebestofthebest = nullptr;
std::mutex population_mutex;
std::vector<std::pair<int, int>> consolidated_path;
std::size_t replay_path_index = 0;
int replay_frame = 0;

void append_solution_position(std::vector<std::pair<int, int>> *path, int *x, int *y, char move) {
  if (move == 'd') ++*x;
  else if (move == 'c') --*x;
  else if (move == 'b') --*y;
  else if (move == 'a') ++*y;
  path->push_back(std::make_pair(*x, *y));
}

class Entity_Shape: public Fl_Widget {
  void draw(){
    std::lock_guard<std::mutex> lock(population_mutex);
    fl_push_clip(x(),y(),w(),h());
    fl_push_matrix();
    fl_color(70, 130, 180);
    for (const auto& position : consolidated_path) {
      fl_rectf(x() + (position.first * mapWidth) + 5,
               y() + ((mapHeight - 1 - position.second) * mapHeight) + 5,
               mapWidth - 10, mapHeight - 10);
    }
    fl_color(66,27,22);
    for(int i=0; i<population; i++){
      if(!cockroaches[i]->dead)
        fl_rectf(x() + (cockroaches[i]->x * mapWidth), y() + ((mapHeight - 1 - cockroaches[i]->y) * mapHeight), mapWidth, mapHeight);
    }
    fl_color(167,114,17);
    if(!thebestofthebest->dead){
      fl_rectf(x() + (thebestofthebest->x * mapWidth), y() + ((mapHeight - 1 - thebestofthebest->y) * mapHeight), mapWidth, mapHeight);
    }
    fl_pop_matrix();
    fl_pop_clip();
  }
public:
  Entity_Shape(int X,int Y,int W,int H) : Fl_Widget(X,Y,W,H) {}
};

Fl_Double_Window *janela_principal=(Fl_Double_Window *)0;
Fl_Return_Button *start=(Fl_Return_Button *)0;
Fl_Value_Input *ge=(Fl_Value_Input *)0;
Fl_Value_Input *populacao=(Fl_Value_Input *)0;
Fl_Value_Input *mutacao_inicial=(Fl_Value_Input *)0;
Fl_Value_Input *rotation_generations=(Fl_Value_Input *)0;
Fl_PNG_Image *png = (Fl_PNG_Image *)0;
Fl_Box *background = (Fl_Box*)0;
Fl_Box *generation = (Fl_Box*)0;
Fl_Box *mutation = (Fl_Box*)0;
Fl_Chart *fitness = (Fl_Chart*)0;
Fl_Chart *distancia_thebestofthebest = (Fl_Chart*)0;
Entity_Shape *entities_on_matrix = (Entity_Shape*)0;

static void update(void*){
  if (destination_reached.exchange(false)) {
    fl_alert("Destination reached!");
  }
  if (solution_found.load()) {
    std::lock_guard<std::mutex> lock(population_mutex);
    if (!consolidated_path.empty() && ++replay_frame % 10 == 0) {
      const auto& position = consolidated_path[replay_path_index];
      thebestofthebest->x = position.first;
      thebestofthebest->y = position.second;
      thebestofthebest->dead = false;
      replay_path_index = (replay_path_index + 1) % consolidated_path.size();
    }
  }
  if (stop_requested.load() && !solution_found.load()) {
    return;
  }
  entities_on_matrix->redraw();
  janela_principal->redraw();
  Fl::repeat_timeout(frames, update);
}

static void close_window(Fl_Widget*, void*) {
  stop_requested.store(true);
  Fl::remove_timeout(update);
  janela_principal->hide();
}

static void start_listener(Fl_Return_Button*, void*){
  ge->deactivate();
  ge_value = std::max(1, static_cast<int>(floor(ge->value())));
  populacao->deactivate();
  population = std::max(1, static_cast<int>(floor(populacao->value())));
  mutacao_inicial->deactivate();
  initial_mutation = (float)mutacao_inicial->value();
  rotation_generations->deactivate();
  rotation_interval = std::max(1, static_cast<int>(floor(rotation_generations->value())));
  start->deactivate();
  {
    std::lock_guard<std::mutex> lock(population_mutex);
    consolidated_path.clear();
    replay_path_index = 0;
    replay_frame = 0;
    solution_found.store(false);
    cockroaches = static_cast<Entity **>(malloc(population * sizeof(Entity *)));
    for (int i = 0; i < population; ++i) cockroaches[i] = static_cast<Entity *>(malloc(sizeof(Entity)));
    initializePopulation(cockroaches, population);
  }
  entities_on_matrix->show();
  entities_on_matrix->redraw();
  janela_principal->redraw();
  start_pressed = true;
  Fl::add_timeout(0.01,update);
}

/**
 Creates the main application window.
*/
Fl_Double_Window* make_window() {
  { // Main GUI window.
    janela_principal = new Fl_Double_Window(800, 500, "LE_EVOLUTION");
    janela_principal->callback(close_window);
    janela_principal->color((Fl_Color)237);
    janela_principal->labelfont(11);
    { // Maze image.
      background = new Fl_Box(375,20,400,400);
      //background->box(FL_SHADOW_BOX);
      png = new Fl_PNG_Image(MATRIX_IMG);
      background->image(png);
      entities_on_matrix = new Entity_Shape(375,20,400,400);
      entities_on_matrix->hide();
      generation = new Fl_Box(375,440,400,20);
      generation->label("GENERATION: 0");
      generation->box(FL_THIN_UP_BOX);
      generation->color((Fl_Color)238);
      //mutation = new Fl_Box(375,470,400,20);
    } // Fl_Box* image
    {
      fitness = new Fl_Chart(30, 235, 330, 97, "Fitness");
      fitness->type(2/*FL_LINE_CHART*/);
      distancia_thebestofthebest = new Fl_Chart(30,362,330,97,"Best-path progress");
      distancia_thebestofthebest->type(2);
      distancia_thebestofthebest->bounds(-2,10);
    }
    { // Starts the evolutionary cycle.
      start = new Fl_Return_Button(150, 205, 115, 30, "START");
      //start->box(FL_RSHADOW_BOX);
      start->color((Fl_Color)215);
      start->labelfont(11);
      start->callback((Fl_Callback*)start_listener);
    } // Fl_Button* start
    { // Genetic mixing factor.
      ge = new Fl_Value_Input(180, 35, 180, 25, "GE:");
      //ge->box(FL_SHADOW_BOX);
      ge->value(23);
      ge->color((Fl_Color)215);
      ge->labelfont(11);
      ge->textfont(11);
    } // Genetic mixing factor input.
    { // Population size.
      populacao = new Fl_Value_Input(180, 75, 180, 25, "INDIVIDUALS:");
      //populacao->box(FL_SHADOW_BOX);
      populacao->value(800);
      populacao->color((Fl_Color)215);
      populacao->labelfont(11);
      populacao->textfont(11);
    } // Population size input.
    { // Initial mutation rate.
      mutacao_inicial = new Fl_Value_Input(180, 120, 180, 25, "INITIAL MUTATION:");
      mutacao_inicial->value(13);
      //mutacao_inicial->box(FL_SHADOW_BOX);
      mutacao_inicial->color((Fl_Color)215);
      mutacao_inicial->labelfont(11);
      mutacao_inicial->textfont(11);
    } // Initial mutation input.
    { // Stagnant generations before rotating the search orientation.
      rotation_generations = new Fl_Value_Input(180, 160, 180, 25, "ROTATE AFTER:");
      rotation_generations->value(rotation_interval);
      rotation_generations->color((Fl_Color)215);
      rotation_generations->labelfont(11);
      rotation_generations->textfont(11);
    } // Search-rotation interval input.
    janela_principal->show();
    janela_principal->end();
  } // Fl_Double_Window* janela_principal
  return janela_principal;
}

bool all_dead(){
  bool is_all_dead = true;
  for(int i=0; i<population; i++){
    if(!cockroaches[i]->dead) is_all_dead = false; 
  }
  return is_all_dead;
}

void restart_pop(int x, int y){
  for(int i=0; i<population; i++){
    cockroaches[i]->x = x;
    cockroaches[i]->y = y;
    cockroaches[i]->dead = false;
    cockroaches[i]->total_steps = 20;
  }
}

void reset_population_after_rotation() {
  const char directions[] = {'a', 'b', 'c', 'd'};
  for (int i = 0; i < population; ++i) {
    cockroaches[i]->x = initial_x;
    cockroaches[i]->y = initial_y;
    cockroaches[i]->dead = false;
    cockroaches[i]->total_steps = 1;
    for (int step = 0; step < vector_size; ++step) {
      cockroaches[i]->moves[step] = 'n';
    }
    cockroaches[i]->moves[0] = directions[std::rand() % 4];
  }
}


// Worker thread for the evolutionary algorithm.
void *evolve_routine(void*){
  int best_x = initial_x, best_y = initial_y, last_bx = best_x, last_by = best_y;
  int best_steps = 0;
  char *best_moves = static_cast<char *>(malloc(vector_size));
  for (int step = 0; step < vector_size; ++step) {
    best_moves[step] = 'n';
  }
  int direction_x = 1;
  int direction_y = 1;
  int search_rotation = 0;
  int last_rotation_degrees = 0;
  int stagnant_generations = 0;
  float best_distance_since_rotation = sqrt(pow(end_x - initial_x, 2) + pow(end_y - initial_y, 2));
  while (!stop_requested.load()) {
    if(start_pressed){
        std::unique_lock<std::mutex> population_lock(population_mutex);
        gen++;
        const int generation_start_x = best_x;
        const int generation_start_y = best_y;
        for(int j=0; j<vector_size; j++){
          if(all_dead()) break;
          for(int i=0; i<population; i++){
            if(j >= cockroaches[i]->total_steps) cockroaches[i]->dead = true;
            if(!cockroaches[i]->dead && cockroaches[i]->moves[j] != 'n'){
              int next_x = cockroaches[i]->x;
              int next_y = cockroaches[i]->y;
              if(cockroaches[i]->moves[j] == 'd'){        // Right.
                ++next_x;
              }else if(cockroaches[i]->moves[j] == 'c'){  // Left.
                --next_x;
              }else if(cockroaches[i]->moves[j] == 'b'){  // Down.
                --next_y;
              }else if(cockroaches[i]->moves[j] == 'a'){  // Up.
                ++next_y;
              }

              // Ignore invalid moves and keep the individual at its last valid position.
              const bool valid_next_position =
                  next_x >= 0 && next_x < mapWidth && next_y >= 0 && next_y < mapHeight &&
                  map[mapHeight - 1 - next_y][next_x] != 1;
              if (valid_next_position) {
                cockroaches[i]->x = next_x;
                cockroaches[i]->y = next_y;
              }
              if (valid_next_position && map[mapHeight - 1 - next_y][next_x] == 2) {
                consolidated_path.clear();
                int path_x = generation_start_x;
                int path_y = generation_start_y;
                consolidated_path.push_back(std::make_pair(path_x, path_y));
                for (int step = 0; step <= j; ++step) {
                  append_solution_position(&consolidated_path, &path_x, &path_y,
                                           cockroaches[i]->moves[step]);
                }
                replay_path_index = 0;
                replay_frame = 0;
                solution_found.store(true);
                destination_reached.store(true);
                stop_requested.store(true);
              }
              
              // Compare progress in the active search orientation.
              if (direction_x * cockroaches[i]->x >= direction_x * best_x &&
                  direction_y * cockroaches[i]->y >= direction_y * best_y &&
                  (direction_x * cockroaches[i]->x > direction_x * best_x ||
                   direction_y * cockroaches[i]->y > direction_y * best_y) &&
                  !cockroaches[i]->dead) {
                best_x = cockroaches[i]->x;
                best_y = cockroaches[i]->y;
                best_steps = cockroaches[i]->total_steps;
                for(int p=0; p<cockroaches[i]->total_steps; p++){
                  best_moves[p] = cockroaches[i]->moves[p];
                }
                mut_var = false;
              }

              // Euclidean distance remains independent of search orientation.
              if ((sqrt(pow(end_x - cockroaches[i]->x, 2) + pow(end_y - cockroaches[i]->y, 2))) <
                  (sqrt(pow(end_x - thebestofthebest->x, 2) + pow(end_y - thebestofthebest->y, 2))) &&
                  !cockroaches[i]->dead) {
                best_x = cockroaches[i]->x;
                best_y = cockroaches[i]->y;
                best_steps = cockroaches[i]->total_steps;
                for(int p=0; p<cockroaches[i]->total_steps; p++){
                  best_moves[p] = cockroaches[i]->moves[p];
                }
                mut_var = false;
              }
            }
      }
      if (stop_requested.load()) break;
      population_lock.unlock();
      Fl::lock();
      entities_on_matrix->redraw();
      Fl::awake();
      Fl::unlock();
      nanosleep(&tim,&tim2);
      population_lock.lock();
        }
      if (stop_requested.load()) {
        population_lock.unlock();
        break;
      }
      nanosleep(&tim,&tim2);
      bool rotate_search = false;
      const float current_best_distance = sqrt(pow(end_x - best_x, 2) + pow(end_y - best_y, 2));
      if (current_best_distance + 0.001F < best_distance_since_rotation) {
        best_distance_since_rotation = current_best_distance;
        stagnant_generations = 0;
      } else if (++stagnant_generations >= rotation_interval) {
        // Randomly choose a new orthogonal or opposite search orientation.
        const int rotation_offsets[] = {1, 2, 3};  // +90, 180, or -90 degrees.
        const int rotation_degrees[] = {90, 180, -90};
        const int rotation_choice = std::rand() % 3;
        search_rotation = (search_rotation + rotation_offsets[rotation_choice]) % 4;
        last_rotation_degrees = rotation_degrees[rotation_choice];
        switch (search_rotation) {
          case 0: direction_x = 1; direction_y = 1; break;
          case 1: direction_x = -1; direction_y = 1; break;
          case 2: direction_x = -1; direction_y = -1; break;
          default: direction_x = 1; direction_y = -1; break;
        }
        stagnant_generations = 0;
        best_distance_since_rotation = sqrt(pow(end_x - initial_x, 2) + pow(end_y - initial_y, 2));
        rotate_search = true;
      }
      if (rotate_search) {
        best_x = initial_x;
        best_y = initial_y;
        best_steps = 0;
        for (int step = 0; step < vector_size; ++step) {
          best_moves[step] = 'n';
        }
        evaluatePopulation(thebestofthebest, best_x, best_y, best_steps, best_moves);
        reset_population_after_rotation();
      } else {
        evaluatePopulation(thebestofthebest, best_x, best_y, best_steps, best_moves);
        reproduce(cockroaches, thebestofthebest, population, initial_mutation);
        restart_pop(best_x, best_y);
      }
      population_lock.unlock();

      string aux_gen, aux_mutation;
      aux_gen.append("GENERATION: ");
      aux_gen.append(to_string(gen));
      aux_gen.append(" | LAST TURN: ");
      if (last_rotation_degrees > 0) aux_gen.append("+");
      aux_gen.append(to_string(last_rotation_degrees));
      aux_gen.append(" DEG");
      Fl::lock();
      generation->copy_label(aux_gen.c_str());
      printf("Mutation: %.4f\n",initial_mutation);

      if(gen%13 == 0){
        if(true){
          if(initial_mutation > 10000) initial_mutation = mutacao_inicial->value();
          else initial_mutation *= 3;
          mut_var = true;
        }
      }

      fitness->add(-sqrt(pow(end_x - thebestofthebest->x,2) + pow(end_y - thebestofthebest->y,2)));
      distancia_thebestofthebest->add(sqrt(pow(best_x - last_bx,2) + pow(best_y - last_by,2)));
      Fl::awake();
      Fl::unlock();
    }

    mut_var = true;
    last_bx = best_x; last_by = best_y;
  }
  free(best_moves);
  pthread_exit(NULL);
}

static void setInitialTheBest(){
  thebestofthebest = static_cast<Entity *>(malloc(sizeof(Entity)));
  thebestofthebest->dead = true;
  thebestofthebest->moves = static_cast<char *>(malloc(vector_size));
  for (int step = 0; step < vector_size; ++step) {
    thebestofthebest->moves[step] = 'n';
  }
  thebestofthebest->x = initial_x;
  thebestofthebest->y = initial_y;
  thebestofthebest->total_steps = 0;
}

int main(){

  XInitThreads();
  Fl::lock();
  srand(static_cast<unsigned int>(time(NULL)));

  tim.tv_sec  = 0;
  tim.tv_nsec = SEC;

  setInitialTheBest();

  fl_register_images(); 
  make_window();

  //##### Opening threads #####//
  pthread_t evolution;
  pthread_create(&evolution, NULL, evolve_routine, NULL);
  //###########################//

  Fl::run();

  stop_requested.store(true);
  Fl::remove_timeout(update);
  pthread_join(evolution, NULL);

  free(thebestofthebest->moves);
  free(thebestofthebest);
  for(int i=0; i<population; i++){
    free(cockroaches[i]->moves);
    free(cockroaches[i]);
  }
  free(cockroaches);
  
  return EXIT_SUCCESS;
}
