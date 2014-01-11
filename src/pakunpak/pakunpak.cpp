/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2014 Xesc & Technology
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Xesc <xeskuu@gmail.com>
* Program URL   : http://xviservicethief.sourceforge.net/
*
*/

#include "pakunpak.h"

/* Support functions */

std::string getFileName(const std::string fileName)
{
	size_t found = fileName.find_last_of("/\\");
	return fileName.substr(found + 1);
}

/* Packer */

Packer::Packer()
{
	files = new std::vector<std::string>;
}

Packer::~Packer()
{
	files->clear();
	delete files ;
}

void Packer::buildPackage(const std::string packageFile)
{
	if (files->empty()) return;

	std::ofstream *package = new std::ofstream(packageFile.c_str(), std::ios::binary);

	if (package->is_open())
	{
		// write package header
		const char *header_id = "XPK\0";
		package->write(header_id, 4); // package header id

		// add files into the package
		for (int n = 0; n < static_cast<int>(files->size()); n++)
		{
			// get the file name
			std::string fileName = getFileName(files->at(n));
			unsigned int fileNameLength = fileName.length();

			// open file
			std::ifstream *file = new std::ifstream(files->at(n).c_str(), std::ios::binary);
			if (file->is_open())
			{
				// get the file size
				file->seekg(0, std::ios::end);
				int fileSize = file->tellg();
				file->seekg(0, std::ios::beg);

				// allocate file in memory
				char *buffer = new char[fileSize];
				file->read(buffer, fileSize);

				// write data into the package
				package->write(reinterpret_cast<char *>(&fileNameLength), sizeof(fileNameLength)); // file name length
				package->write(fileName.c_str(), fileNameLength); // file name
				package->write(reinterpret_cast<char *>(&fileSize), sizeof(fileSize)); // file size
				package->write(buffer, fileSize); // file data

				// close file
				file->close();

				// delete buffer
				delete[] buffer;
			}
			delete file;
		}
	}
	delete package ;
}

void Packer::addFile(const std::string fileName)
{
	files->push_back(fileName);
}

/* Unpacker class */

Unpacker::Unpacker()
{
	filesInDisc = new std::vector<std::string>;
	filesOriginal = new std::vector<std::string>;
}

Unpacker::~Unpacker()
{
	filesInDisc->clear();
	filesOriginal->clear();

	delete filesInDisc;
	delete filesOriginal;
}

void Unpacker::extractPackage(std::string packageFile, std::string destination, bool originalNames)
{
	std::ifstream *package = new std::ifstream(packageFile.c_str(), std::ios_base::in | std::ios::binary);
	if (package->is_open())
	{
		// get the file size
		package->seekg(0, std::ios::end);
		int packageSize = package->tellg();
		package->seekg(0, std::ios::beg);

		// get the package header id
		char *header_id = new char[4];
		package->read(header_id, 4);

		// check if is a valid package id
		if (strcmp(header_id, "XPK") == 0)
		{
			while (package->tellg() < packageSize)
			{
				// get the file name lenght
				unsigned int fileNameLength = 0;
				package->read(reinterpret_cast<char *>(&fileNameLength), sizeof(fileNameLength));

				// get the file name
				char *fileName = new char[fileNameLength + 1];
				package->read(fileName, fileNameLength);
				fileName[fileNameLength] = '\0';

				// add the original file name
				filesOriginal->push_back(std::string(fileName));

				// override the original file name?
				if (!originalNames)
				{
					std::string packageFileName = getFileName(packageFile);
					std::string tmpFileName(fileName);

					tmpFileName = packageFileName + "." + fileName;
					fileNameLength = tmpFileName.length();

					delete[] fileName;
					fileName = new char[fileNameLength + 1];

					strcpy(fileName, tmpFileName.c_str());
					fileName[fileNameLength] = '\0';
				}

				// get file length
				int fileSize;
				package->read(reinterpret_cast<char *>(&fileSize), sizeof(fileSize));

				// read the packed file
				char *buffer = new char[fileSize];
				package->read(buffer , fileSize);

				// get the destination file name
				char *finalFileName = new char[destination.length() + fileNameLength + 1];
				strcpy(finalFileName, destination.c_str());
				strcat(finalFileName, fileName);
				finalFileName[destination.length() + fileNameLength] = '\0';

				// add this to the extracted list
				filesInDisc->push_back(std::string(finalFileName));

				// write the packed file to disc
				std::ofstream *file = new std::ofstream(finalFileName, std::ios::binary);
				file->write(buffer, fileSize);
				file->close();

				// delete buffers
				delete[] finalFileName;
				delete[] fileName;
				delete[] buffer;
				delete file;
			}
		}
		package->close();

		delete header_id;
	}
	delete package;
}

std::string Unpacker::getExtractedFileName(const int index, bool originalName)
{
	if (index >= 0 && index < getExtractedFilesCount())
		return originalName ? filesOriginal->at(index) : filesInDisc->at(index);
	else
		return "";
}

int Unpacker::getExtractedFilesCount()
{
	return filesInDisc->size();
}
