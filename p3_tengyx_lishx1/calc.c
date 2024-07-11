#include "calc.h"
#include <emmintrin.h>
#include <pmmintrin.h>

/* set inlets velocity there are two type inlets*/
int set_inlets(const t_param params, float* inlets) {
  #pragma omp parallel for
  for(int jj=0; jj <params.ny; jj++){
    if(!params.type)
      inlets[jj]=params.velocity; // homogeneous
    else
      inlets[jj]=params.velocity * 4.0 *((1-((float)jj)/params.ny)*((float)(jj+1))/params.ny); // parabolic
  }
  return EXIT_SUCCESS;
}

/* compute average velocity of whole grid, ignore grids with obstacles. */
float av_velocity(const t_param params, t_speed* cells, int* obstacles)
{
  int    tot_cells = 0;  /* no. of cells used in calculation */
  float  tot_u;          /* accumulated magnitudes of velocity for each cell */

  /* initialise */
  tot_u = 0.f;
    /* loop over all non-blocked cells */
    #pragma omp parallel for reduction(+:tot_u,tot_cells)
    for (int jj = 0; jj < params.ny; jj++)
    { 
      for (int ii = 0; ii < params.nx; ii++)
      {
        const int index = ii + jj*params.nx;
        /* ignore occupied cells */
        if (!obstacles[index])
        {
          /* local density total */
          float local_density = 0.f;
          local_density += cells[index].speeds[0];
          local_density += cells[index].speeds[1];
          local_density += cells[index].speeds[2];
          local_density += cells[index].speeds[3];
          local_density += cells[index].speeds[4];
          local_density += cells[index].speeds[5];
          local_density += cells[index].speeds[6];
          local_density += cells[index].speeds[7];
          local_density += cells[index].speeds[8];

          /* x-component of velocity */
          float u_x = (cells[index].speeds[1]
                        + cells[index].speeds[5]
                        + cells[index].speeds[8]
                        - (cells[index].speeds[3]
                          + cells[index].speeds[6]
                          + cells[index].speeds[7]))
                      / local_density;
          /* compute y velocity component */
          float u_y = (cells[index].speeds[2]
                        + cells[index].speeds[5]
                        + cells[index].speeds[6]
                        - (cells[index].speeds[4]
                          + cells[index].speeds[7]
                          + cells[index].speeds[8]))
                      / local_density;
          /* accumulate the norm of x- and y- velocity components */
          tot_u += sqrtf((u_x * u_x) + (u_y * u_y));
          /* increase counter of inspected cells */
          ++tot_cells;
        }
      }
  }
  return tot_u / (float)tot_cells;
}

/* calculate reynold number */
float calc_reynolds(const t_param params, t_speed* cells, int* obstacles)
{
  return av_velocity(params, cells, obstacles) * (float)(params.ny) / params.viscosity;
}
