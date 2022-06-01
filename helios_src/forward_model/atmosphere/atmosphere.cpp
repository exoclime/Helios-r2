/*
* This file is part of the Helios-r2 code (https://github.com/exoclime/Helios-r2).
* Copyright (C) 2022 Daniel Kitzmann
*
* Helios-r2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Helios-r2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You find a copy of the GNU General Public License in the main
* Helios-r2 directory under <LICENSE>. If not, see
* <http://www.gnu.org/licenses/>.
*/


#include "atmosphere.h"


#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <omp.h>
#include <iomanip>

#include "../../chemistry/chem_species.h"
#include "../../additional/physical_const.h"


namespace helios{



Atmosphere::Atmosphere(const size_t nb_grid_points_, const double atmos_boundaries [2]) : nb_grid_points(nb_grid_points_)
{
  createPressureGrid(atmos_boundaries);

  //initialise temperatures, altitudes, and number_densities to 0
  temperature.assign(nb_grid_points, 0.0);
  altitude.assign(nb_grid_points, 0.0);
  number_densities.assign(nb_grid_points, std::vector<double>(constants::species_data.size(), 0.0));
}




bool Atmosphere::calcAtmosphereStructure(const double surface_gravity,
                                         Temperature* temperature_profile, const std::vector<double>& temp_parameters,
                                         std::vector<Chemistry*>& chemistry, const std::vector<double>& chem_parameters_all)
{ 
  bool neglect_model = false;

  //temperature profile
  bool neglect_temperature = temperature_profile->calcProfile(temp_parameters, pressure, temperature);

  if (neglect_temperature) neglect_model = true;


  //chemical composition
  std::vector<double> mean_molecular_weights(nb_grid_points, 0.0);
  number_densities.assign(nb_grid_points, std::vector<double>(constants::species_data.size(), 0.0));
  size_t nb_chem_param = 0;

  for (auto & i : chemistry)
  {
    std::vector<double> chem_parameters(chem_parameters_all.begin() + nb_chem_param, 
                                        chem_parameters_all.begin() + nb_chem_param + i->nbParameters());
    nb_chem_param += i->nbParameters();
    
    bool neglect = i->calcChemicalComposition(chem_parameters, temperature, pressure, number_densities, mean_molecular_weights);
    
    if (neglect) neglect_model = true;
  }

  calcAltitude(surface_gravity, mean_molecular_weights);


  return neglect_model;
}



//determine the vertical grid via hydrostatic equilibrium
void Atmosphere::calcAltitude(const double surface_gravity, const std::vector<double>& mean_molecular_weights)
{
  altitude.assign(nb_grid_points, 0.0);
  std::vector<double> mass_density(nb_grid_points, 0.0);

  for (size_t i=0; i<nb_grid_points; ++i)
    mass_density[i] = mean_molecular_weights[i] * pressure[i]*1e6 / (constants::gas_constant  * temperature[i]);

  for (size_t i=1; i<nb_grid_points; i++)
  {
    double delta_z = (1.0/(mass_density[i]*surface_gravity) + 1.0/(mass_density[i-1]*surface_gravity)) * 0.5 * (pressure[i-1]*1e6 - pressure[i]*1e6);

    altitude[i] = altitude[i-1] + delta_z;
  }

}



//Creates a pressure grid between the two boundary points
//nb_grid_point pressures vales are equidistantly spread in the log(p) space
void Atmosphere::createPressureGrid(const double atmos_boundaries [2])
{
  pressure.assign(nb_grid_points, 0.0);

  const double min_pressure = atmos_boundaries[1];
  const double max_pressure = atmos_boundaries[0];

  
  pressure.front() = std::log10(max_pressure);

  const double log_step = (std::log10(max_pressure) - std::log10(min_pressure)) / (nb_grid_points - 1.0);


  for (size_t i=1; i<nb_grid_points-1; ++i)
    pressure[i] = pressure[i-1] - log_step;


  for (size_t i=0; i<nb_grid_points-1; ++i)
    pressure[i] = pow(10.0, pressure[i]);

  pressure.back() = min_pressure;
}



}

