/*
* This file is part of the BeAR code (https://github.com/newstrangeworlds/BeAR).
* Copyright (C) 2024 Daniel Kitzmann
*
* BeAR is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* BeAR is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You find a copy of the GNU General Public License in the main
* BeAR directory under <LICENSE>. If not, see
* <http://www.gnu.org/licenses/>.
*/


#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "transmission.h"

#include "../../additional/exceptions.h"


namespace bear{


TransmissionModelConfig::TransmissionModelConfig (const std::string& folder_path)
{
  const std::string config_file_name = folder_path + "forward_model.config";

  readConfigFile(config_file_name);
}



void TransmissionModelConfig::readConfigFile(const std::string& file_name)
{
  std::fstream file;
  file.open(file_name.c_str(), std::ios::in);

  if (file.fail())  
    throw FileNotFound(std::string ("TransmissionModelConfig::readConfigFile"), file_name);

  std::cout << "Parameters read from " << file_name << " :\n";

  std::string line;
  std::string input;

  std::getline(file, line);
  
  file >> nb_grid_points >> line;
  std::cout << "- Atmosphere levels: " << nb_grid_points << "\n";


  std::getline(file, line);

  file >> atmos_bottom_pressure >> line;
  std::cout << "- Bottom of atmosphere pressure: " << atmos_bottom_pressure << "\n";

  std::getline(file, line);

  file >> atmos_top_pressure >> line;
  std::cout << "- Top of atmosphere pressure: " << atmos_top_pressure << "\n";

  atmos_boundaries[0] = atmos_bottom_pressure;
  atmos_boundaries[1] = atmos_top_pressure;
  

  std::getline(file, line);
  std::string fit_mode = "";
  file >> fit_mode;

  if (fit_mode == "mmw")
  {
    fit_mean_molecular_weight = true;
    std::cout << "- Fit for mean molecular weight: yes\n";
  }
  
  if (fit_mode == "sh")
  {
    fit_scale_height = true;
    std::cout << "- Fit for scale height: yes\n";
  }

  if (fit_mode != "mmw" && fit_mode != "sh" && fit_mode != "no" && fit_mode != "No")
  {
    std::string error_message = 
            "Parameter " + fit_mode + " for fitting mean molecular weight or scale height unknown\n";
    throw InvalidInput(std::string ("forward_model.config"), error_message);
  }
  std::getline(file, line);
  std::getline(file, line);
  
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

  //cloud model input
  std::getline(file, line);

  readCloudConfig(file);

  if (cloud_model.front() != "none" && cloud_model.front() != "None") use_cloud_model = true;


  //optional modules input
  readModuleConfig(file);

  if (modules.front() != "none" && modules.front() != "None") use_optional_modules = true;


  readChemistryConfig(file);

  readOpacityConfig(file);


  file.close();
}



void TransmissionModelConfig::readChemistryConfig(std::fstream& file)
{
  std::string line;
  std::getline(file, line);

  while (std::getline(file, line) && line.size() != 0)
  {
    std::istringstream input(line);
    
    std::string chem_model;
    input >> chem_model;

    std::cout << "- Chemistry model: " << chem_model << "\n";
    
    chemistry_model.push_back(chem_model);
    chemistry_parameters.resize(chemistry_parameters.size()+1);

    std::string param;

    while (input >> param)
      chemistry_parameters.back().push_back(param);
  }

}



void TransmissionModelConfig::readCloudConfig(std::fstream& file)
{
  std::string line;
  std::getline(file, line);

  while (std::getline(file, line) && line.size() != 0)
  { 
    std::istringstream input(line);
    
    std::string model;
    input >> model;

    std::cout << "- Cloud model: " << model << "\n";
    
    cloud_model.push_back(model);
    cloud_model_parameters.resize(cloud_model_parameters.size()+1);

    std::string param;

    while (input >> param)
      cloud_model_parameters.back().push_back(param);
  }

}


void TransmissionModelConfig::readModuleConfig(std::fstream& file)
{
  std::string line;
  std::getline(file, line);

  while (std::getline(file, line) && line.size() != 0)
  { 
    std::istringstream input(line);
    
    std::string model;
    input >> model;

    std::cout << "- Optional modules: " << model << "\n";
    
    modules.push_back(model);
    modules_parameters.resize(modules_parameters.size()+1);

    std::string param;

    while (input >> param)
      modules_parameters.back().push_back(param);
  }

}



void TransmissionModelConfig::readOpacityConfig(std::fstream& file)
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

