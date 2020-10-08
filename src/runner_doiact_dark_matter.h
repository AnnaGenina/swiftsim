/*******************************************************************************
 * This file is part of SWIFT.
 * Copyright (c) 2020 Camila Correa (camila.correa@uva.nl)
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

#ifndef SWIFT_RUNNER_DOIACT_DARK_MATTER_H
#define SWIFT_RUNNER_DOIACT_DARK_MATTER_H


#include "../config.h"

struct runner;
struct cell;

void runner_doself_dark_matter_sidm(struct runner *r, struct cell *ci);
void runner_dopair_dark_matter_sidm(struct runner *r, struct cell *ci, struct cell *cj);

void runner_dosub_self_dark_matter_sidm(struct runner *r, struct cell *ci);
void runner_dosub_pair_dark_matter_sidm(struct runner *r, struct cell *ci, struct cell *cj);




#endif
