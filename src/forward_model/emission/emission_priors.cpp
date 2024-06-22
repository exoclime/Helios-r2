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

#include "emission.h"

#include "../../retrieval/priors.h"
#include "../../additional/exceptions.h"


namespace bear{


//set the model priors
void EmissionModel::setPriors(Priors* priors)
{
  const std::string file_name = config->retrieval_folder_path + "priors.config";

  std::vector<std::string> prior_type; 
  std::vector<std::string> prior_description; 
  std::vector<std::vector<double>> prior_parameter;


  readPriorConfigFile(file_name, prior_type, prior_description, prior_parameter);


  //check if we have the correct number of piors
  if (prior_type.size() != nb_total_param())
  {
    std::string error_message = "Found " + std::to_string(prior_type.size()) + " priors in priors.config but expected " + std::to_string(nb_total_param()) + "\n";
    throw InvalidInput(std::string ("EmissionModel::setPriors"), error_message);
  }


  priors->add(prior_type, prior_description, prior_parameter);
}




void EmissionModel::readPriorConfigFile(
  const std::string& file_path, 
  std::vector<std::string>& prior_type, 
  std::vector<std::string>& prior_description, 
  std::vector<std::vector<double>>& prior_parameter)
{
  std::fstream file;
  file.open(file_path.c_str(), std::ios::in);


  if (file.fail())  
    throw FileNotFound(std::string ("EmissionModel::readPriorConfigFile"), file_path);


  std::string line;

  while (std::getline(file, line))
  {
    std::istringstream input(line);

    std::string type, description;
    std::vector<double> parameter;

    input >> type >> description;

    double single_parameter;

    while (input >> single_parameter)
      parameter.push_back(single_parameter);


    prior_type.push_back(type);
    prior_description.push_back(description);
    prior_parameter.push_back(parameter);
  }


  file.close();
}



}

