
/*!
    \file
    \brief Contains class implementation for data extraction from a EDF Plus file.
*/

#include "edfplus.h"
using namespace std;

/*!
*   \brief Constructor
*
*   \param csInputFile - input file 
*
*   \return - status
*/

Cedfplus::Cedfplus( CString oInputFile )
{
	m_poInputFile = new ifstream( oInputFile );

	m_eEdfStatus = Cedfplus::EDF_FILE_OPEN_ERROR;		// be pessimistic
	
	if( m_poInputFile->fail() )
	{
		return;
	}
	else
	{

/*
ofstream fout; // declares an object of type ofstream
	 // ifstream and ofstream are defined in <fstream>
	fin.open("infile.txt");
	fout.open(“outfile.txt");
	// for any stream object s, s.open(<filename>) opens filename
	// and connects the stream s to it so that chars can flow between
	// them.
	int num1, num2, num3;
	fin >> num1 >> num2 >> num3;
	fout << "The sum is " << num1+num2+num3 << endl;
*/
		m_eEdfStatus = Cedfplus::EDF_SUCCESS;
	}
} // Cedfplus

Cedfplus::~Cedfplus( void )
{
	m_poInputFile->close();
}
