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


#ifndef _spectral_band_h
#define _spectral_band_h

#include <vector>
#include <string>

#include "spectral_band_type.h"


namespace helios {

//forward declaration
class SpectralGrid;
class GlobalConfig;



class SpectralBands{
  public:
    BandType bandType() const {return band_type;}
    void init(GlobalConfig* global_config, SpectralGrid* grid,  const std::vector< std::vector<double> >& band_edges, 
              const std::vector<double>& band_centres, const BandType type, const std::string filter_file_name);


    void setLocalIndices();
    void setInstrumentProfileFWHW(std::vector<double>& profile_fwhm);

    void bandIntegrateSpectrum(const std::vector<double>& spectrum, std::vector<double>& band_values);
    void convolveSpectrum(const std::vector<double>& spectrum, std::vector<double>& convolved_spectrum);

    size_t nbBands() {return nb_bands;}

    std::vector<double> wavenumbers;                                      //high-res wavenumbers of this observational range
    std::vector<double> wavelengths;                                      //high-res wavelengths of this observational range
    std::vector<size_t> spectral_indices;                                 //index of the full spectral grid that are part of this observation
    std::vector<size_t> edge_indices;                                     //spectral indices corresponding to the edges of the spectral bins

    std::vector< std::vector<size_t> > band_spectral_indices;             //the spectral indices for each band separately
    std::vector< std::vector<double> > band_wavenumbers;                  //the high-res wavenumbers for each band separately

    std::vector<double> band_centers_wavelength;                          //center wavelengths for each spectral bin
    std::vector< std::vector<double> > band_edges_wavelength;             //wavelengths of the spectral bin edges


    std::vector<double> instrument_profile_sigma;                          //standard deviation of the instruments profile
    std::vector< std::vector<size_t> > convolution_quadrature_intervals;   //pre-determined limits (indices) for the convolution integration

  private:
    GlobalConfig* config;
    SpectralGrid* spectral_grid;
    BandType band_type;
    size_t nb_points_bin = 0;
    size_t nb_bands = 0;                                                   //number of sub-bands/bins 


    std::vector<double> filter_transmission;                               //placeholder for a filter function (not yet implemented)

    std::vector<size_t> global_spectral_indices;                           //indices of the high-res wavelenghts in the global spectral grid  
    std::vector<size_t> global_edge_indices;                               //indices of the bin edges in the global spectral grid

    double bandIntegrateSpectrum(const std::vector<double>& spectrum, const size_t& band);
    double convolveSpectrum(const std::vector<double>& spectrum, const unsigned int index);
    void setConvolutionQuadratureIntervals();
    void setConvolutionQuadratureIntervals(const unsigned int index, const double cutoff_distance);
};




}


#endif
