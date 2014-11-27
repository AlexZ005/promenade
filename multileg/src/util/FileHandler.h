#pragma once

#include <string>
#include <vector>
class SettingsData;


bool write_file_binary(std::string const & filename, char const * data, size_t const bytes);

bool saveFloatArray(std::vector<float>* p_inData, const std::string& file_path);

bool loadFloatArray(std::vector<float>* p_outData, const std::string& file_path);


void saveFloatArrayPrompt(std::vector<float>* p_inData, int p_fileTypeIdx);

void loadFloatArrayPrompt(std::vector<float>*& p_outData, int p_fileTypeIdx);

bool loadSettings(SettingsData& p_settingsfile);