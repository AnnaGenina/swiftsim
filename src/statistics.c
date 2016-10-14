/*******************************************************************************
 * This file is part of SWIFT.
 * Copyright (c) 2016 Matthieu Schaller (matthieu.schaller@durham.ac.uk)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

/* Config parameters. */
#include "../config.h"

/* Some standard headers. */
#include <string.h>

/* This object's header. */
#include "statistics.h"

/* Local headers. */
#include "cooling.h"
#include "engine.h"
#include "error.h"
#include "hydro.h"
#include "threadpool.h"

/**
 * @brief Information required to compute the statistics
 */
struct index_data {
  const struct space *s;
  int chunk_size;
  int num_threads;
  struct statistics *stats;
};

void stats_collect_part_mapper(void *map_data, int nr_parts, void *extra_data) {

  /* Unpack the data */
  struct index_data *data = (struct index_data *)extra_data;
  const struct space *s = data->s;
  struct part *restrict parts = (struct part *)map_data;
  struct xpart *restrict xparts = s->xparts + (ptrdiff_t)(parts - s->parts);
  const int chunk_size = data->chunk_size;
  const int num_threads = data->num_threads;
  const int ti_current = s->e->ti_current;
  const double timeBase = s->e->timeBase;

  /* Local accumulator */
  struct statistics stats;
  bzero(&stats, sizeof(struct statistics));

  /* Loop over particles */
  for (int k = 0; k < nr_parts; k++) {

    /* Get the particle */
    struct part *restrict p = &parts[k];
    struct xpart *restrict xp = &xparts[k];

    const float dt = (ti_current - (p->ti_begin + p->ti_end) / 2) * timeBase;
    const double x[3] = {p->x[0], p->x[1], p->x[2]};
    const float v[3] = {xp->v_full[0] + p->a_hydro[0] * dt,
                        xp->v_full[1] + p->a_hydro[1] * dt,
                        xp->v_full[2] + p->a_hydro[2] * dt};

    const float m = hydro_get_mass(p);

    /* Collect mass */
    stats.mass += m;

    /* Collect momentum */
    stats.mom[0] += m * v[0];
    stats.mom[1] += m * v[1];
    stats.mom[2] += m * v[2];

    /* Collect angular momentum */
    stats.ang_mom[0] += m * (x[1] * v[2] - x[2] * v[1]);
    stats.ang_mom[1] += m * (x[2] * v[0] - x[0] * v[2]);
    stats.ang_mom[2] += m * (x[0] * v[1] - x[1] * v[0]);

    /* Collect energies. */
    stats.E_kin += 0.5f * m * (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    stats.E_pot += 0.;
    stats.E_int += m * hydro_get_internal_energy(p, dt);
    stats.E_rad += cooling_get_radiated_energy(xp);

    /* Collect entropy */
    stats.entropy += m * hydro_get_entropy(p, dt);
  }

  /* Where do we write back ? */
  const int thread_id =
      ((ptrdiff_t)(parts - s->parts) / chunk_size) % num_threads;

  /* Now write back to memory */
  data->stats[thread_id].E_kin += stats.E_kin;
  data->stats[thread_id].E_int += stats.E_int;
  data->stats[thread_id].E_pot += stats.E_pot;
  data->stats[thread_id].E_rad += stats.E_rad;
  data->stats[thread_id].entropy += stats.entropy;
  data->stats[thread_id].mass += stats.mass;
  data->stats[thread_id].mom[0] += stats.mom[0];
  data->stats[thread_id].mom[1] += stats.mom[1];
  data->stats[thread_id].mom[2] += stats.mom[2];
  data->stats[thread_id].ang_mom[0] += stats.ang_mom[0];
  data->stats[thread_id].ang_mom[1] += stats.ang_mom[1];
  data->stats[thread_id].ang_mom[2] += stats.ang_mom[2];
}

void stats_collect(const struct space *s, struct statistics *stats) {

  const int num_threads = s->e->threadpool.num_threads;
  const int chunk_size = s->nr_parts / num_threads;

  /* Prepare the data */
  struct index_data extra_data;
  extra_data.s = s;
  extra_data.chunk_size = chunk_size;
  extra_data.num_threads = num_threads;
  extra_data.stats = malloc(sizeof(struct statistics) * num_threads);
  if (extra_data.stats == NULL)
    error("Impossible to allocate memory for statistics");
  bzero(&extra_data.stats, sizeof(struct statistics) * num_threads);

  /* Run parallel collection of statistics */
  threadpool_map(&s->e->threadpool, stats_collect_part_mapper, s->parts,
                 s->nr_parts, sizeof(struct part), chunk_size, &extra_data);

  /* Collect data from all the threads in the pool */
  for (int i = 0; i < num_threads; ++i) {

    stats->E_kin += extra_data.stats[i].E_kin;
    stats->E_int += extra_data.stats[i].E_int;
    stats->E_pot += extra_data.stats[i].E_pot;
    stats->E_rad += extra_data.stats[i].E_rad;
    stats->entropy += extra_data.stats[i].entropy;
    stats->mass += extra_data.stats[i].mass;
    stats->mom[0] += extra_data.stats[i].mom[0];
    stats->mom[1] += extra_data.stats[i].mom[1];
    stats->mom[2] += extra_data.stats[i].mom[2];
    stats->ang_mom[0] += extra_data.stats[i].ang_mom[0];
    stats->ang_mom[1] += extra_data.stats[i].ang_mom[1];
    stats->ang_mom[2] += extra_data.stats[i].ang_mom[2];
  }

  /* Free stuff */
  free(extra_data.stats);
}
