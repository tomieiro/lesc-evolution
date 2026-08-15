#include "../../headers/base.h"

#include <cstdlib>

namespace {

char directionFromIndex(int direction_index) {
  switch (direction_index) {
    case 0: return 'a';
    case 1: return 'b';
    case 2: return 'c';
    case 3: return 'd';
    default: return 'n';
  }
}

int directionIndex(char direction) {
  switch (direction) {
    case 'a': return 0;
    case 'b': return 1;
    case 'c': return 2;
    case 'd': return 3;
    default: return 0;
  }
}

}  // namespace

void initializePopulation(Entity **entities, int population) {
  for (int i = 0; i < population; ++i) {
    entities[i]->dead = false;
    entities[i]->x = initial_x;
    entities[i]->y = initial_y;
    entities[i]->total_steps = 0;
    entities[i]->moves = static_cast<char *>(std::malloc(vector_size));

    for (int step = 0; step < vector_size; ++step) {
      entities[i]->moves[step] = 'n';
    }
    entities[i]->moves[entities[i]->total_steps++] = directionFromIndex(std::rand() % 4);
  }
}

void reproduce(Entity **entities, const Entity *best_entity, int population, float mutation) {
  for (int i = 0; i < population; ++i) {
    Entity *entity = entities[i];
    if (entity->total_steps >= vector_size) {
      continue;
    }

    // Clone the best path and apply bounded directional mutations.
    const int shared_steps = entity->total_steps < best_entity->total_steps
                                 ? entity->total_steps
                                 : best_entity->total_steps;
    for (int step = 0; step < shared_steps; ++step) {
      int direction = directionIndex(best_entity->moves[step]);
      if (step < 5 || step >= static_cast<int>(entity->total_steps * 0.789F)) {
        const int delta = (std::rand() % 2 == 0 ? -1 : 1) *
                          static_cast<int>((std::rand() % 4) * mutation * 13.0F);
        direction = (direction + delta) % 4;
        if (direction < 0) {
          direction += 4;
        }
      }
      entity->moves[step] = directionFromIndex(direction);
    }
    entity->moves[entity->total_steps++] = directionFromIndex(std::rand() % 4);
  }
}

void evaluatePopulation(Entity *best_entity, int best_x, int best_y, int best_steps, const char *best_moves) {
  best_entity->dead = false;
  best_entity->x = best_x;
  best_entity->y = best_y;
  best_entity->total_steps = best_steps;
  for (int i = 0; i < vector_size; ++i) {
    best_entity->moves[i] = best_moves[i];
  }
}
