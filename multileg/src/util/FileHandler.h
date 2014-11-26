#pragma once
#include <fstream>
#include <string>
#include "ToString.h"
#include "SettingsData.h"

bool write_file_binary (std::string const & filename, 
						char const * data, size_t const bytes)
{
	std::ofstream b_stream(filename.c_str(), 
		std::fstream::out | std::fstream::binary);
	if (b_stream)
	{
		b_stream.write(data, bytes);
		return (b_stream.good());
	}
	return false;
}

bool saveFloatArray(std::vector<float>* p_inData,
	const std::string& file_path)
{
	std::ofstream os;
	os.open(file_path, std::ios::binary | std::ios::out);
	if (!os.good() || !os.is_open())
		return false;
	//os<<length; // write size
	size_t length = p_inData->size();
	os.write(reinterpret_cast<char*>(&(*p_inData)[0]), 
		std::streamsize(length*sizeof(float))); // write data
	os.close();
	return true;
}

bool loadFloatArray(std::vector<float>* p_outData,
	const std::string& file_path)
{
	size_t bytelength = 0;
	std::ifstream is;
	is.open(file_path.c_str(), std::ios::binary | std::ios::in | ios::ate); // ate, place at end to read size
	if (!is.good() || !is.is_open())
		return false;
	//is>>length; // read size
	bytelength = is.tellg(); // byteLen
	if (bytelength>0)
	{
		int members = bytelength / sizeof(float);
		p_outData->resize(members);
		float* tArr = new float[members]; // temp buffer array
		is.seekg(0, ios::beg); // place at start
		is.read(reinterpret_cast<char*>(tArr), bytelength); // read data to buffer
		for (int i = 0; i < members; i++) // copy to vector
		{
			(*p_outData)[i] = tArr[i];
		}
		delete[] tArr;
	}
	is.close();
	return true;
}



void saveFloatArrayPrompt(std::vector<float>* p_inData, int p_fileTypeIdx)
{
	string path = "../output/sav/biptest";
#ifndef _DEBUG
	OPENFILENAME ofn;
	char szFile[255];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Biped Gait\0*.bgait\0Quadruped Gait\0*.qgait\0";
	ofn.nFilterIndex = p_fileTypeIdx;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	bool hasFileName = GetSaveFileName(&ofn);
	if (hasFileName)
	{
		path = ofn.lpstrFile;
		saveFloatArray(p_inData, path);
	}
#else
	saveFloatArray(p_inData, path);
#endif
	//MessageBox(NULL, ofn.lpstrFile, "File Name", MB_OK);
}
void loadFloatArrayPrompt(std::vector<float>*& p_outData, int p_fileTypeIdx)
{
	string path = "../output/sav/biptest";
#ifndef _DEBUG
	OPENFILENAME ofn;
	char szFile[255];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Biped Gait\0*.bgait\0Quadruped Gait\0*.qgait\0";
	ofn.nFilterIndex = p_fileTypeIdx;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	bool hasFileName = GetOpenFileName(&ofn);
	if (hasFileName)
	{
		if (p_outData == NULL)
			p_outData = new std::vector<float>();
		path = ofn.lpstrFile;
		loadFloatArray(p_outData, path);
	}
#else
	if (p_outData == NULL)
		p_outData = new std::vector<float>();
	loadFloatArray(p_outData, path);
#endif
	//MessageBox(NULL, ofn.lpstrFile, "File Name", MB_OK);
}

bool loadSettings(SettingsData& p_settingsfile)
{
	string path = "../settings.txt";
	std::ifstream is;
	is.open(path.c_str(), std::ios::in);
	if (!is.good() || !is.is_open())
		return false;
	//is>>length; // read size
	std::string tmpStr;
	int tmpInt = 0;
	float tmpFlt = 0.0f;
	auto readDiscardHeader = [](std::ifstream* p_is)->void // explicit return type "->void"
	{
		std::string stmp="x";
		do { std::getline(*p_is, stmp); } while (stmp == "");
	};
	// Fullscreen
	std::getline(is, tmpStr); // throwaway title
	is >> tmpInt;
	p_settingsfile.m_fullscreen = tmpInt == 0 ? false : true;
	// window width
	readDiscardHeader(&is);
	is >> tmpInt;
	p_settingsfile.m_wwidth = tmpInt;
	// window height
	readDiscardHeader(&is);
	is >> tmpInt;
	p_settingsfile.m_wheight = tmpInt;
	// mode
	readDiscardHeader(&is);
	std::getline(is, tmpStr);
	p_settingsfile.m_mode = tmpStr;
	// pod
	readDiscardHeader(&is);
	std::getline(is, tmpStr);
	p_settingsfile.m_pod = tmpStr;
	// exec mode
	readDiscardHeader(&is);
	std::getline(is, tmpStr);
	p_settingsfile.m_execMode = tmpStr;
	// charcount serial
	readDiscardHeader(&is);
	is >> tmpInt;
	p_settingsfile.m_charcount_serial = tmpInt;
	// parallel invocs
	readDiscardHeader(&is);
	is >> tmpInt;
	p_settingsfile.m_parallel_invocs = tmpInt;
	// char offset X
	readDiscardHeader(&is);
	is >> tmpFlt;
	p_settingsfile.m_charOffsetX = tmpFlt;
	// start paused
	readDiscardHeader(&is);
	is >> tmpInt;
	p_settingsfile.m_startPaused = tmpInt == 0 ? false : true;
	// op steaps
	readDiscardHeader(&is);
	is >> tmpInt;
	p_settingsfile.m_optimizationSteps = tmpInt;
	// optimization weights
	readDiscardHeader(&is);
	std::getline(is, tmpStr); // extra header here
	is >> tmpFlt; p_settingsfile.m_optW_fd = tmpFlt;
	readDiscardHeader(&is);
	is >> tmpFlt; p_settingsfile.m_optW_fv = tmpFlt;
	readDiscardHeader(&is);
	is >> tmpFlt; p_settingsfile.m_optW_fh = tmpFlt;
	readDiscardHeader(&is);
	is >> tmpFlt; p_settingsfile.m_optW_fr = tmpFlt;
	readDiscardHeader(&is);
	is >> tmpFlt; p_settingsfile.m_optW_fp = tmpFlt;

	is.close();
	return true;
}