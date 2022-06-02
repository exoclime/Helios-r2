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


#include "brown_dwarf.h"


#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>


#include "../../retrieval/retrieval.h"
#include "../../retrieval/prior.h"
#include "../../additional/exceptions.h"



namespace helios{


BrownDwarfConfig::BrownDwarfConfig (const std::string& folder_path)
{
  const std::string config_file_name = folder_path + "forward_model.config";

  readConfigFile(config_file_name);
}



void BrownDwarfConfig::readConfigFile(const std::string& file_name)
{
  std::fstream file;
  file.open(file_name.c_str(), std::ios::in);

  
  if (file.fail())  
    throw ExceptionFileNotFound(std::string ("BrownDwarfConfig::readConfigFile"), file_name);

  
  std::string line;
  std::string input;


  std::getline(file, line);
  
  file >> nb_grid_points >> line;
  std::cout << "- Atmosphere levels: " << nb_grid_points << "\n";


  std::getline(file, line);

  file >> atmos_top_pressure >> line;
  std::cout << "- Top of atmosphere pressure: " << atmos_top_pressure << "\n";

  std::getline(file, line);

  file >> atmos_bottom_pressure >> line;
  std::cout << "- Bottom of atmosphere pressure: " << atmos_bottom_pressure << "\n";
  
  atmos_boundaries[0] = atmos_top_pressure;
  atmos_boundaries[1] = atmos_bottom_pressure;


  //temperature profile input
  std::getline(file, line);
  std::getline(file, line);
  std::istringstream input_stream(line);

  input_stream >> temperature_profile_model;

  while (input_stream >> input)
    temperature_profile_parameters.push_back(input);

  std::cout << "- Temperature profile: " << temperature_profile_model;
  for (auto & i : temperature_profile_parameters) std::cout << "  " << i;
  std::cout << "\n";


  std::getline(file, line);
  std::getline(file, line);

  file >> input >> line;
  if (input == "Y" || input == "Yes" || input == "1") use_cloud_layer = true;
  std::cout << "- Use Cloud Layer: " << use_cloud_layer << "\n";

  //the radiative transfer input
  std::getline(file, line);
  std::getline(file, line);
  std::cout << line << "\n";

  input_stream.str(line); input_stream.clear();

  input_stream >> radiative_transfer_model;

  while (input_stream >> input)
    radiative_transfer_parameters.push_back(input);

  std::cout << "- Radiative transfer model: " << radiative_transfer_model;
  for (auto & i : radiative_transfer_parameters) std::cout << "  " << i;
  std::cout << "\n";

  std::getline(file, line);


  readChemistryConfig(file);

  readOpacityConfig(file);


  file.close();
}



void BrownDwarfConfig::readChemistryConfig(std::fstream& file)
{
  std::string line;
  std::getline(file, line);  
  

  while (std::getline(file, line) && line.size() != 0)
  { 
    std::istringstream input(line);
    
    std::string chem_model;
    input >> chem_model;
    
    chemistry_model.push_back(chem_model);
    chemistry_parameters.resize(chemistry_parameters.size()+1);

    std::string param;

    while (input >> param)
      chemistry_parameters.back().push_back(param);
  }

}



void BrownDwarfConfig::readOpacityConfig(std::fstream& file)
{
  std::string line;
  std::getline(file, line);
  
  
  while(std::getline(file, line))
  {
    std::istringstream input(line);

    std::string species, folder;

    input >> species >> folder;
    
    if (species.length() > 0 && folder.length() > 0)
    {
      opacity_species_symbol.push_back(species);
      opacity_species_folder.push_back(folder);
    }
    
  }


  std::cout << "- Opacity species:\n";
  for (size_t i=0; i<opacity_species_symbol.size(); ++i)
    std::cout << "   species " << opacity_species_symbol[i] << "\t folder: " << opacity_species_folder[i] << "\n"; 
  
  
  std::cout << "\n";
}



}

