/*!
 @file DAQFile_test00.cc
 @author Matteo Brini
 @brief Esempio di utilizzo della classe DAQFile
 @version 0.1
 @date 2024-10-16

 @example DAQFile_test00.cc

    The basic usage of class @ref DAQFile is shown in this example. The file is opened and initialized with the constructor @ref DAQFile::DAQFile(const std::string &).
 
 @copyright Copyright (c) 2024
 
 */

#include <iostream>
#include "DAQFile.hh"

int main()
{
    DAQFile file("../data/testDRS.dat");
}