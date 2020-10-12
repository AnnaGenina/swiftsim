/*******************************************************************************
 * This file is part of SWIFT.
 * Coypright (c) 2020 Camila Correa (camila.correa@uva.nl)
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

/* This object's header. */
#include "dark_matter_properties.h"

/* Standard headers */
#include <float.h>
#include <math.h>

/* Local headers. */
#include "common_io.h"
#include "dimension.h"
#include "parser.h"
#include "units.h"
#include "error.h"
#include "restart.h"


#define sidm_props_default_h_sidm FLT_MAX

/**
 * @brief Initialize the global properties of the self-interacting dark matter scheme.
 *
 * @param fp The #feedback_props.
 * @param phys_const The physical constants in the internal unit system.
 * @param us The internal unit system.
 * @param params The parsed parameters.
 * @param hydro_props The already read-in properties of the hydro scheme.
 * @param cosmo The cosmological model.
 */
void sidm_props_init(struct sidm_props* sidm_props,
                     const struct phys_const* phys_const,
                     const struct unit_system* us,
                     struct swift_params* params,
                     const struct cosmology* cosmo) {
    
    /* Scattering cross section in physical units */
    sidm_props->sigma_cgs = parser_get_param_double(params, "SIDM:sigma_cm2_g");
    
    sidm_props->sigma = sidm_props->sigma_cgs / units_cgs_conversion_factor(us, UNIT_CONV_MASS);
    
    sidm_props->sigma *= units_cgs_conversion_factor(us, UNIT_CONV_LENGTH) * units_cgs_conversion_factor(us, UNIT_CONV_LENGTH);
    
    sidm_props->h_search_radius = parser_get_opt_param_float(params, "SIDM:h_sidm", sidm_props_default_h_sidm);

}

/**
 * @brief Write a black_holes_props struct to the given FILE as a stream of
 * bytes.
 *
 * @param props the black hole properties struct
 * @param stream the file stream
 */
void dark_matter_props_struct_dump(const struct sidm_props *sidm_props, FILE *stream) {
    restart_write_blocks((void *)sidm_props, sizeof(struct sidm_props), 1,
                         stream, "sidmprops", "sidm props");
}

/**
 * @brief Restore a black_holes_props struct from the given FILE as a stream of
 * bytes.
 *
 * @param props the black hole properties struct
 * @param stream the file stream
 */
void dark_matter_props_struct_restore(const struct sidm_props *sidm_props, FILE *stream) {
    restart_read_blocks((void *)sidm_props, sizeof(struct sidm_props), 1,
                        stream, NULL, "sidm props");
}

