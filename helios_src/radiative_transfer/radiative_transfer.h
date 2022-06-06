/*
* This file is part of the Helios-r2 code (https://github.com/exoclime/Helios-r2).
* Copyright (C) 2020 Daniel Kitzmann
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


#ifndef _radiative_transfer_h
#define _radiative_transfer_h

#include <vector>
#include "../forward_model/atmosphere/atmosphere.h"


namespace helios {

//forward declaration
class SpectralGrid;
class Atmosphere;


class RadiativeTransfer{
  public:
    virtual ~RadiativeTransfer() {}
    virtual void calcSpectrum(const Atmosphere& atmosphere,
                              const std::vector< std::vector<double> >& absorption_coeff, 
                              const std::vector< std::vector<double> >& scattering_coeff,
                              const std::vector< std::vector<double> >& cloud_optical_depth,
                              const std::vector< std::vector<double> >& cloud_single_scattering,
                              const std::vector< std::vector<double> >& cloud_asym_param,
                              const double spectrum_scaling,
                              std::vector<double>& spectrum) = 0;
    virtual void calcSpectrumGPU(const Atmosphere& atmosphere,
                                 double* absorption_coeff_dev,
                                 double* scattering_coeff_dev,
                                 double* cloud_optical_depth,
                                 double* cloud_single_scattering,
                                 double* cloud_asym_param,
                                 const double spectrum_scaling,
                                 double* model_spectrum_dev) = 0;
};


}
#endif

