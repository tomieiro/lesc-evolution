#ifndef macros_h
#define macros_h

#define MATRIX_IMG "img/map.png"

#define mapWidth 20
#define mapHeight 20
#define screenWidth 640
#define screenHeight 480
#define frames 0.008
#define vector_size 30
#define end_x 18
#define end_y 18
#define SEC 1000000L

#define initial_x 1
#define initial_y 1

typedef struct ent {
  bool dead;
  int x;
  int y;
  int total_steps;
  char *moves;
} Entity;

#endif
