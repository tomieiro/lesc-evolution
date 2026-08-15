#include "../const/macros.h"

void initializePopulation(Entity **entities, int population);
void reproduce(Entity **entities, const Entity *best_entity, int population, float mutation);
void evaluatePopulation(Entity *best_entity, int best_x, int best_y, int best_steps, const char *best_moves);
