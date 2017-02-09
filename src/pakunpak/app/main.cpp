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

#include "../pakunpak.h"

void displayHelp()
{
	std::cout << "How to use Pakunpaker:" << std::endl
		  << " -i: Build package [package name] [files to add]" << std::endl
		  << " -o: Extract files from package [package name] [destination path]" << std::endl
		  << " -oo: Extract files form package and override the original file name" << std::endl
		  << std::endl
		  << "NOTE: Pakunpaker DON'T create new directories!" << std::endl
		  << std::endl
		  << " Example: <app> -i demo_in/package.demo demo_in/file01.txt demo_in/file2.txt" << std::endl
		  << " Example: <app> -o demo_in/package.demo demo_out/" << std::endl;
}

int main(int argc, char ** argv)
{
	std::cout << "------------------------------------------------" << std::endl
			  << " Pakunpaker v1.2 by Xesc & Technology 2008-2017 " << std::endl
			  << "------------------------------------------------" << std::endl
			  << std::endl;

	if (argc > 2)
	{
		// create a new package
		if (strcmp(argv[1], "-i") == 0)
		{
			Packer *packer = new Packer;

			for (int param = 3; param < argc; param++)
			{
				packer->addFile(std::string(argv[param]));
				std::cout << "added: " << argv[param] << std::endl;
			}

			packer->buildPackage(argv[2]);
			std::cout << "Package build: " << argv[2] << std::endl;

			delete packer;
		}
		// extract files from package
		else if ((strcmp(argv[1], "-o") == 0 || strcmp(argv[1], "-oo") == 0) && argc == 4)
		{
			Unpacker *unpacker = new Unpacker;

			unpacker->extractPackage(std::string(argv[2]), std::string(argv[3]), strcmp(argv[1], "-o") == 0);

			for (int n = 0; n < unpacker->getExtractedFilesCount(); n++)
				std::cout << "Unpacked file " << n + 1 << " to: " << unpacker->getExtractedFileName(n) << std::endl;

			delete unpacker;
		}
		else // unknonw parameters
			displayHelp();
	}
	else // invalid parameters
		displayHelp();

	return 0;
}
