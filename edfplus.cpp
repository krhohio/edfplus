
/*!
    \file
    \brief Contains class implementation for data extraction from a EDF Plus file.
*/

#include <iostream>	// for cout
#include "edfplus.h"
using namespace std;

/*!
*   \brief Constructor
*
*   \param csInputFile - input file 
*
*   \return - status
*/

CReadEDF::CReadEDF( char *pszInputFile )
{
	m_eEdfStatus = CReadEDF::EDF_FILE_OPEN_ERROR;		// be pessimistic

	m_poEdfFile = new ifstream( pszInputFile );

	if( m_poEdfFile->fail() )
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
		//if( m_poInputFile->open();

		m_pcFileData = new char[sizeof(CReadEDF::generalHeader_S)+1 ];
		{
			// error handling
		}

		int iDebug = sizeof(CReadEDF::generalHeader_S);
		int extracted = m_poEdfFile->read( (char *)&m_acGeneralHeader, sizeof(CReadEDF::generalHeader_S) ).gcount();

		//m_acGeneralHeader = (generalHeader_S *)m_pacFileData;

		m_eEdfStatus = CReadEDF::EDF_SUCCESS;
	}
} // CReadEDF

CReadEDF::~CReadEDF( void )
{
	m_poEdfFile->close();
}
