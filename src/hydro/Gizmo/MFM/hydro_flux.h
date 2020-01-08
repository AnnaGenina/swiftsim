/*******************************************************************************
 * This file is part of SWIFT.
 * Coypright (c) 2019 Bert Vandenbroucke (bert.vandenbroucke@gmail.com)
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
#ifndef SWIFT_GIZMO_MFM_HYDRO_FLUX_H
#define SWIFT_GIZMO_MFM_HYDRO_FLUX_H

#include "riemann.h"

/**
 * @brief Reset the fluxes for the given particle.
 *
 * @param p Particle.
 */
__attribute__((always_inline)) INLINE static void hydro_part_reset_fluxes(
    struct part* restrict p) {

  p->flux.momentum[0] = 0.0f;
  p->flux.momentum[1] = 0.0f;
  p->flux.momentum[2] = 0.0f;
  p->flux.energy = 0.0f;
}

/**
 * @brief Get the fluxes for the given particle.
 *
 * @param p Particle.
 * @param flux Fluxes for the particle (array of size 5 or more).
 */
__attribute__((always_inline)) INLINE static void hydro_part_get_fluxes(
    const struct part* restrict p, float* flux) {

  flux[1] = p->flux.momentum[0];
  flux[2] = p->flux.momentum[1];
  flux[3] = p->flux.momentum[2];
  flux[4] = p->flux.energy;
}

/**
 * @brief Compute the flux for the Riemann problem with the given left and right
 * state, and interface normal, surface area and velocity.
 *
 * @param WL Left state variables.
 * @param WR Right state variables.
 * @param n_unit Unit vector of the interface.
 * @param vLR Velocity of the interface.
 * @param Anorm Surface area of the interface.
 * @param fluxes Array to store the result in (of size 5 or more).
 */
__attribute__((always_inline)) INLINE static void hydro_compute_flux(
    const float* WL, const float* WR, const float* n_unit, const float* vLR,
    const float Anorm, float* fluxes) {

  riemann_solve_for_middle_state_flux(WL, WR, n_unit, vLR, fluxes);

  fluxes[1] *= Anorm;
  fluxes[2] *= Anorm;
  fluxes[3] *= Anorm;
  fluxes[4] *= Anorm;
}

/**
 * @brief Update the fluxes for the particle with the given contributions,
 * assuming the particle is to the left of the interparticle interface.
 *
 * @param p Particle.
 * @param fluxes Fluxes accross the interface.
 * @param dx Distance between the particles that share the interface.
 */
__attribute__((always_inline)) INLINE static void hydro_part_update_fluxes_left(
    struct part* restrict p, const float* fluxes, const float* dx) {

  p->flux.momentum[0] -= fluxes[1];
  p->flux.momentum[1] -= fluxes[2];
  p->flux.momentum[2] -= fluxes[3];
  p->flux.energy -= fluxes[4];

#ifndef GIZMO_TOTAL_ENERGY
  p->flux.energy += fluxes[1] * p->v[0];
  p->flux.energy += fluxes[2] * p->v[1];
  p->flux.energy += fluxes[3] * p->v[2];
#endif
}

/**
 * @brief Update the fluxes for the particle with the given contributions,
 * assuming the particle is to the right of the interparticle interface.
 *
 * @param p Particle.
 * @param fluxes Fluxes accross the interface.
 * @param dx Distance between the particles that share the interface.
 */
__attribute__((always_inline)) INLINE static void
hydro_part_update_fluxes_right(struct part* restrict p, const float* fluxes,
                               const float* dx) {

  p->flux.momentum[0] += fluxes[1];
  p->flux.momentum[1] += fluxes[2];
  p->flux.momentum[2] += fluxes[3];
  p->flux.energy += fluxes[4];

#ifndef GIZMO_TOTAL_ENERGY
  p->flux.energy -= fluxes[1] * p->v[0];
  p->flux.energy -= fluxes[2] * p->v[1];
  p->flux.energy -= fluxes[3] * p->v[2];
#endif
}

/**
 * @brief Get the drift term for the density based on the mass flux.
 *
 * @param mass_flux Current mass flux for the particle.
 * @param dt Drift time step (in co-moving units).
 * @param volume Volume of the particle.
 * @return 0, since this is Gizmo MFM.
 */
__attribute__((always_inline)) INLINE static float
hydro_gizmo_mfv_density_drift_term(const float mass_flux, const float dt,
                                   const float volume) {
  return 0.0f;
}

/**
 * @brief Add the gravitational contribution to the fluid velocity drift.
 *
 * This method does nothing, since this is Gizmo MFM.
 *
 * @param fluid_v Fluid velocity.
 * @param v (Undrifted) particle velocity.
 * @param v_full (Drifted) particle velocity.
 */
__attribute__((always_inline)) INLINE static void
hydro_gizmo_mfv_extra_velocity_drift(float* fluid_v, const float* v,
                                     const float* v_full) {}

/**
 * @brief Get the term required to update the MFV energy due to the change in
 * gravitational energy.
 *
 * @param dt_kick_corr Time step for the potential energy correction.
 * @param dt_grav Time step for the (optional) kinetic energy correction.
 * @param p Particle.
 * @param momentum Momentum of the particle, explicitly requested so that it is
 * clear from the code that the momentum needs to be updated after the call to
 * this function.
 * @param a_grav Gravitational acceleration.
 * @return 0, since this is Gizmo MFM.
 */
__attribute__((always_inline)) INLINE static float
hydro_gizmo_mfv_gravity_energy_update_term(const float dt_kick_corr,
                                           const float dt_grav,
                                           const struct part* restrict p,
                                           const float* momentum,
                                           const float* a_grav) {

  return 0.0f;
}

/**
 * @brief Get the term required to update the MFV mass due to the mass flux.
 *
 * @param mass_flux Mass flux rate.
 * @param dt Time step (in comoving units).
 * @return 0, since this is Gizmo MFM.
 */
__attribute__((always_inline)) INLINE static float
hydro_gizmo_mfv_mass_update_term(const float mass_flux, const float dt) {
  return 0.0f;
}

/**
 * @brief Get the term required to update the MFV entropy due to the mass flux.
 *
 * @param entropy_flux Entropy flux rate.
 * @param dt Time step (in comoving units).
 * @return Entropy flux update term.
 */
__attribute__((always_inline)) INLINE static float
hydro_gizmo_mfv_entropy_update_term(const float entropy_flux, const float dt) {
  return 0.0f;
}

/**
 * @brief Update the mass of the gpart associated with the given particle after
 * the mass has been updated with the hydrodynamical mass flux.
 *
 * This function does nothing, since this is Gizmo MFM.
 *
 * @param p Particle.
 */
__attribute__((always_inline)) INLINE static void
hydro_gizmo_mfv_update_gpart_mass(struct part* restrict p) {}

#endif /* SWIFT_GIZMO_MFM_HYDRO_FLUX_H */
