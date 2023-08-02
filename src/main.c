#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mirror_lib.h"

// https://en.wikipedia.org/wiki/Line–line_intersection
int main() {
  InitWindow(600, 600, "Mirror Game");
  SetTargetFPS(60);

  boundary_t boundaries[7];
  boundaries[0] = ml_new_boundary(90,  40, 100, 100);
  boundaries[1] = ml_new_boundary(140,  100, 200, 160);
  boundaries[2] = ml_new_boundary(260, 100, 300, 180);
  boundaries[3] = ml_new_boundary(260, 120, 300, 200);
  boundaries[4] = ml_new_boundary(260, 140, 300, 220);
  boundaries[5] = ml_new_boundary(260, 160, 300, 240);
  boundaries[6] = ml_new_boundary(300, 100, 430, 100);
  Vector2 ray_origin = {0};
  
  mirror_lib_setup_t setup = {0};
  setup.boundaries = malloc(7 * sizeof(boundary_t));
  setup.boundary_count = 7;
  setup.boundaries[0] = ml_new_boundary(90,  40, 100, 100);
  setup.boundaries[1] = ml_new_boundary(140,  100, 200, 160);
  setup.boundaries[2] = ml_new_boundary(260, 100, 300, 180);
  setup.boundaries[3] = ml_new_boundary(260, 120, 300, 200);
  setup.boundaries[4] = ml_new_boundary(260, 140, 300, 220);
  setup.boundaries[5] = ml_new_boundary(260, 160, 300, 240);
  setup.boundaries[6] = ml_new_boundary(300, 100, 430, 100);

  while (!WindowShouldClose()) {
    if (IsKeyDown(KEY_S)) {
      ray_origin.y ++;
    }
    if (IsKeyDown(KEY_W)) {
      ray_origin.y --;
    }
    if (IsKeyDown(KEY_D)) {
      ray_origin.x ++;
    }
    if (IsKeyDown(KEY_A)) {
      ray_origin.x --;
    }

    // Setup initial ray for the simulation
    ray_ll_t rays = ml_ll_new();
    ray_t* r = ml_new_ray(ray_origin.x, ray_origin.y, Vector2Zero(), 100);
    ml_ray_update_xy(r, GetMousePosition().x, GetMousePosition().y);
    ml_ll_append(rays, r);

    // Run the reflection simulation, and modifiy the rays linked list (this will contain all the reflected rays in it)
    ml_run(&setup, &rays);

    // 
    BeginDrawing();
    ClearBackground(GRAY);

    ray_t* temp = rays.header->next;
    while (temp && temp != rays.trailer) {
      DrawLineEx(temp->origin, temp->endp, 2, PINK);
      temp = temp->next;
    }
    EndDrawing();

    ml_ll_free(rays);
  }
}
