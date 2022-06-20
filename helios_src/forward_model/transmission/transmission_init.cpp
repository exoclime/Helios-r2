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


#include "transmission.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "../../additional/exceptions.h"
#include "../../chemistry/select_chemistry.h"
#include "../../temperature/select_temperature_profile.h"
#include "../../cloud_model/select_cloud_model.h"
#include "../../CUDA_kernels/data_management_kernels.h"


namespace helios{


//initialises the varous modules of the forward model
void TransmissionModel::initModules(const TransmissionModelConfig& model_config)
{
  chemistry.assign(model_config.chemistry_model.size(), nullptr);

  for (size_t i=0; i<model_config.chemistry_model.size(); ++i)
    chemistry[i] = selectChemistryModule(
      model_config.chemistry_model[i], 
      model_config.chemistry_parameters[i], 
      config, 
      model_config.atmos_boundaries);

  nb_total_chemistry_param = 0;

  for (auto & i : chemistry)
    nb_total_chemistry_param += i->nbParameters(); 


  temperature_profile = selectTemperatureProfile(
    model_config.temperature_profile_model, 
    model_config.temperature_profile_parameters, 
    model_config.atmos_boundaries);

  nb_temperature_param = temperature_profile->nbParameters();


  cloud_model = selectCloudModel(model_config.cloud_model, model_config.cloud_model_parameters);

  if (cloud_model != nullptr) nb_cloud_param = cloud_model->nbParameters();
}



}