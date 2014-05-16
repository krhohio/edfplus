
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
	// Fake for loop for common error exit:
	for( bool allDone = false; allDone == false; allDone = true )
	{
		m_poEdfFile = new ifstream( pszInputFile );

		if( m_poEdfFile->fail() )
		{
			m_eEdfStatus = CReadEDF::EDF_FILE_OPEN_ERROR;		// be pessimistic
			break;
		}
		else
		{
			m_pcFileData = new char[sizeof(CReadEDF::generalHeader_S)+1 ];
			{
				// error handling
			}

			int iDebug = sizeof(CReadEDF::generalHeader_S);
			int extracted = m_poEdfFile->read( (char *)&m_acGeneralHeader, sizeof(CReadEDF::generalHeader_S) ).gcount();

			//m_acGeneralHeader = (generalHeader_S *)m_pacFileData;

			m_eEdfStatus = CReadEDF::EDF_SUCCESS;
			break;
		} //...fail()
	} //for()

} // CReadEDF

CReadEDF::~CReadEDF( void )
{
	m_poEdfFile->close();
}
