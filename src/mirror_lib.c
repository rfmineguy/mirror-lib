#include "mirror_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <raymath.h>
#include <limits.h>

ray_ll_t ml_ll_new() {
  ray_ll_t ll = {0};
  ll.header = calloc(1, sizeof(ray_t));
  ll.trailer = calloc(1, sizeof(ray_t));
  ll.header->next = ll.trailer;
  ll.trailer->prev = ll.header;
  return ll;
}

void ml_ll_free(ray_ll_t ll) {
  ray_t* t = ll.header;
  while (t) {
    ray_t* temp = t->next;
    free(t);
    t = temp;
    }
}

void ml_ll_append(ray_ll_t ll, ray_t* ray) {
  ray_t* prev = ll.trailer->prev;
  prev->next = ray;
  ll.trailer->prev = ray;
}

void ml_run(mirror_lib_setup_t* setup, ray_ll_t* rays) {
  boundary_t *boundaries = setup->boundaries;

  ray_t* temp = rays->header->next;
  int ll_depth = 0;
#define MAX_DEPTH 1
  while (temp && temp != rays->trailer && ll_depth < MAX_DEPTH) {
    int closest_boundary = -1;
    int closest_distance = INT_MAX;
    Vector2 closest_point;
    int hit = 0;
    for (int i = 0; i < setup->boundary_count; i++) {
      DrawLineEx(boundaries[i].p1, boundaries[i].p2, 5, BLUE);
      DrawLineEx(boundaries[i].p1, boundaries[i].normal, 2, YELLOW);
      Vector2 point;
      if (ml_ray_boundary_intersection(*temp, boundaries[i], &point)) {
        int dist_sq = (temp->origin.x - point.x) * (temp->origin.x - point.x) + (temp->origin.y - point.y) * (temp->origin.y - point.y);
        if (dist_sq > 100 && dist_sq < closest_distance) {
          closest_distance = dist_sq;
          closest_boundary = i;
          closest_point = point;
          hit = 1;
        }
      }
    }
    if (hit) {
      boundary_t boundary = boundaries[closest_boundary];
      float dx = boundary.p2.x - boundary.p1.x;
      float dy = boundary.p2.y - boundary.p1.y;
      float m = dy / dx;
      float b = boundary.p1.y - m * boundary.p1.x;
      // y = m*x + b
      Vector2 boundary_normal;// = (Vector2) {.x = dy, .y = -dx};
      if (temp->origin.y < m * temp->origin.x + b) {
        boundary_normal = (Vector2) {.x = dy, .y = -dx};
      }
      else {
        boundary_normal = (Vector2) {.x = -dy, .y = dx};
      }

      // Drawing the normal
      // Vector2 boundary_normal_p = Vector2Add(closest_point, boundary_normal);
      // DrawCircleV(closest_point, 5, RED);
      // DrawLineEx(closest_point, boundary_normal_p, 5, GREEN);

      int dist = sqrt((temp->origin.x - closest_point.x) * (temp->origin.x - closest_point.x) + (temp->origin.y - closest_point.y) * (temp->origin.y - closest_point.y));
      printf("%d\n", dist);
      // Calculating the reflection
      //    I - 2 * (N.I) * N
      Vector2 I = temp->direction;
      Vector2 N = boundary_normal;
      Vector2 I_mid_N = ml_reflect(I, N);
      Vector2 reflect = Vector2Add(I_mid_N, closest_point);

      // Generate new ray
      ray_t* r = ml_new_ray(closest_point.x, closest_point.y, reflect, dist);
      ml_ray_update_xy(r, reflect.x, reflect.y);
      ml_ll_append(*rays, r);
      ml_ray_update_xy(temp, closest_point.x, closest_point.y);
      temp = temp->next;
      continue;
    }
    temp = temp->next;
    ll_depth ++;
  }
}

ray_t* ml_new_ray(float ox, float oy, Vector2 direction, float mag) {
  ray_t* r = malloc(sizeof(ray_t));
  r->origin.x = ox;
  r->origin.y = oy;
  r->magnitude = mag;
  r->next = NULL;
  r->direction = direction;
  return r;
}

void ml_ray_update_xy(ray_t* ray, int x, int y) {
  ray->endp.x = x;
  ray->endp.y = y;
  ray->direction = (Vector2) {.x=ray->origin.x - x, .y=ray->origin.y - y};
  float mag = sqrt(ray->direction.x * ray->direction.x + ray->direction.y * ray->direction.y);
  ray->direction.x /= mag;
  ray->direction.y /= mag;
}

boundary_t ml_new_boundary(float x1, float y1, float x2, float y2) {
  boundary_t b = {0};
  b.p1.x = x1;
  b.p1.y = y1;
  b.p2.x = x2;
  b.p2.y = y2;
  float dx = b.p2.x - b.p1.x;
  float dy = b.p2.y - b.p1.y;
  b.normal = (Vector2) {.x=x1 + dy / 4.f, .y=y1 - dx / 4.f};
  return b;
}

int ml_ray_boundary_intersection(ray_t ray, boundary_t b, Vector2* p_out) {
  float x1 = ray.origin.x, y1 = ray.origin.y;
  float x2 = ray.endp.x,   y2 = ray.endp.y;
  float x3 = b.p1.x,       y3 = b.p1.y;
  float x4 = b.p2.x,       y4 = b.p2.y;

  float t_num = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
  float t_den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
  float t = t_num / t_den;

  float u_num = (x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2);
  float u_den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
  float u = u_num / u_den;

  float x = x1 + t * (x2 - x1);
  float y = y1 + t * (y2 - y1);
  if (p_out)
    *p_out = (Vector2) {.x=x,.y=y};
  return t >= 0 && t <= 1 && u >= 0 && u <= 1;
}

Vector2 ml_reflect(Vector2 I, Vector2 N) {
  N = Vector2Normalize(N);
  float IdotN     = Vector2DotProduct(I, N);
  float mid       = (IdotN / 1) * 2;
  Vector2 mid_N   = Vector2Scale(N, mid);
  Vector2 I_mid_N = Vector2Subtract(I, mid_N);
  I_mid_N         = Vector2Normalize(I_mid_N);
  I_mid_N         = Vector2Scale(I_mid_N, -300);
  return I_mid_N;
}
