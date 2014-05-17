
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
			m_eEdfStatus = CReadEDF::EDF_FILE_OPEN_ERROR;
			break;
		}
		else
		{
			m_pcFileData = new char[sizeof(CReadEDF::headerFixedLength_S)+1 ];
			{
				// error handling
			}

			int iDebug = sizeof(CReadEDF::headerFixedLength_S);
			int extracted = m_poEdfFile->read( (char *)&m_acHeaderFixedLength, sizeof(CReadEDF::headerFixedLength_S) ).gcount();

			//m_acHeaderFixedLength = (HeaderFixedLength_S *)m_pacFileData;

			m_eEdfStatus = CReadEDF::EDF_SUCCESS;
			break;
		} //...fail()
	} //for()

} // CReadEDF()


/*!
*   \brief Destructor
*
*   \param (none)
*
*   \return (none)
*/

CReadEDF::~CReadEDF( void )
{
	if( m_poEdfFile )
	{
		m_poEdfFile->close();
	}
}

/*!
*   \brief Validate time value.
*
*	\note **The time value to be validated must already be loaded into m_szValue **
*
*   \param (none)
*
*   \return (none)
*/

bool CReadEDF::bValidTimeValue( void )
	{
		bool bRetVal = false;		// be pessimestic
		time_S *pacTime = (time_S *)&m_szValue[0];

		// Fake for loop for common error exit:
		for( bool allDone = false; allDone == false; allDone = true )
		{
			// \todo if leading space(s) are allowed in date then test for them here:	
			if( pacTime->cH_MSD < '0' || pacTime->cH_MSD > '2' ) break;		// check for max 24-hour time
			if( pacTime->cH_LSD < '0' || pacTime->cH_MSD > '9' ) break;		// allow 09:00 and 19:00
			if( pacTime->cDot1 != '.' ) break;								// ensure a non-digit terminator for atoi()

//			if( int iHH = atoi( (char *)pacTime->cH_MSD ) > 23 ) break;		// no atoi error checking since done above
			int iHH = ((pacTime->cH_MSD & 0x0F) * 10 + (pacTime->cH_LSD & 0x0F));
			if( ((pacTime->cH_MSD & 0x0F) * 10 + (pacTime->cH_LSD & 0x0F)) > 23 ) break;

			if( pacTime->cM_MSD < '0' || pacTime->cM_MSD > '5' ) break;
			if( pacTime->cM_LSD < '0' || pacTime->cM_MSD > '9' ) break;
			if( pacTime->cDot2 != '.' ) break;

			if( pacTime->cS_MSD < '0' || pacTime->cS_MSD > '5' ) break;
			if( pacTime->cS_LSD < '0' || pacTime->cS_MSD > '9' ) break;

			bRetVal= true;
			break;
		} //for()

		m_szValue[ sizeof( time_S ) ] = '\0';		// make sure there is a string terminator

		return( bRetVal );
	}

/*!
*   \brief Validate date value.
*
*	\note **The date value to be validated must already be loaded into m_szValue. **
*
*   \param (none)
*
*   \return (none)
*/

bool CReadEDF::bValidDateValue( void )
{
	bool bRetVal = false;		// be pessimestic
	date_S *pacDate = (date_S *)&m_szValue[0];

	// Fake for loop for common error exit:
	for( bool allDone = false; allDone == false; allDone = true )
	{
		// \todo if leading space(s) are allowed in date then test for them here:	
		if( pacDate->cD_MSD < '0' || pacDate->cD_MSD > '3' ) break;		// check for max 31 days
		if( pacDate->cD_LSD < '0' || pacDate->cD_MSD > '9' ) break;		// allow day 9, 19, 29
		if( pacDate->cDot1 != '.' ) break;								// ensure a non-digit terminator for atoi()

//			if( int iDD = atoi( (char *)pacDate->cD_MSD ) > 31 ) break;		// no atoi error checking since done above
		int iDD = ((pacDate->cD_MSD & 0x0F) * 10 + (pacDate->cD_LSD & 0x0F));
		if( ((pacDate->cD_MSD & 0x0F) * 10 + (pacDate->cD_LSD & 0x0F)) > 31 ) break;

		if( pacDate->cM_MSD < '0' || pacDate->cM_MSD > '1' ) break;
		if( pacDate->cM_LSD < '0' || pacDate->cM_MSD > '9' ) break;
		if( pacDate->cDot2 != '.' ) break;

		if( pacDate->cY_MSD < '0' || pacDate->cY_MSD > '9' ) break;
		if( pacDate->cY_LSD < '0' || pacDate->cY_MSD > '9' ) break;
		
		bRetVal= true;
		break;
	} //for()

	m_szValue[ sizeof( time_S ) ] = '\0';		// make sure there is a string terminator
	return( bRetVal );
}

/*!
*   \brief Return the start time string.
*
*   \param peEdfStatus is loaded with status value if not null.
*
*   \return Start time string.
*/

char *CReadEDF::pszGetStartTime( edfStatus_E *peEdfStatus )
{
	m_szValue[0] = '\0';	// make sure there is a string terminator
	memcpy( (char *)&m_szValue, (void *)&m_acHeaderFixedLength.cStartTime,sizeof( time_S ));

	if( bValidTimeValue() )
	{
		if( peEdfStatus != NULL)
		{
			*peEdfStatus = EDF_SUCCESS;
		}
	}
	else
	{
		strcpy_s( m_szValue, "BAD!" );

		if( peEdfStatus != NULL)
		{
			*peEdfStatus = EDF_TIME_ERROR;
		}
	}

	return( &m_szValue[0] );
}

/*!
*   \brief Return the start date string.
*
*   \param peEdfStatus is loaded with status value if not null.
*
*   \return Start date string.
*/

char *CReadEDF::pszGetStartDate( edfStatus_E *peEdfStatus )
{
	m_szValue[0] = '\0';	// make sure there is a string terminator
	memcpy( &m_szValue, &m_acHeaderFixedLength.cStartDate, sizeof( date_S ));
	m_szValue[ sizeof( date_S) ] = '\0';

	if( bValidDateValue() )
	{
		if( peEdfStatus != NULL)
		{
			*peEdfStatus = EDF_SUCCESS;
		}
	}
	else

	{
		strcpy_s( m_szValue, 5, "BAD!" );

		if( peEdfStatus != NULL)
		{
			*peEdfStatus = EDF_DATE_ERROR;
		}
	}

	return( &m_szValue[0] );
}

/*!
*   \brief Return the number of records.
*   \param peEdfStatus is loaded with status value if not null.
*   \return Pointer to string of ASCII data.
*/
	CReadEDF::edfStatus_E CReadEDF::eGetNumberRecords( char *pszNumberRecords )
	{
		edfStatus_E	eRetVal = EDF_FILE_CONTENTS_ERROR;	// be pessimistic

		memcpy( &pszNumberRecords, &m_acHeaderFixedLength.cStartDate, eNumberRecordsSize );
		m_szValue[eNumberRecordsSize] = '\0';

		atoi( m_szValue );
		if( errno == ERANGE )
		{
			strcpy_s( m_szValue, "BAD!" );
			eRetVal = EDF_SUCCESS;
		}

		pszNumberRecords = &m_szValue[0];
		return( eRetVal );
	};

/*!
*   \brief Return the number of records.
*	\note No Hungarian notation here for function overloading.
*   \param peEdfStatus is loaded with status value if not null.
*   \return Integer Value.
*/
	CReadEDF::edfStatus_E CReadEDF::eGetNumberRecords( int *iNumberRecords )
	{
		edfStatus_E	eRetVal = EDF_FILE_CONTENTS_ERROR;	// be pessimistic

		memcpy( &m_szValue, &m_acHeaderFixedLength.cStartDate, eNumberRecordsSize );
		m_szValue[eNumberRecordsSize] = '\0';	// terminate string for atoi()

		m_iValue = atoi( m_szValue );
		if( errno != ERANGE )
		{
			eRetVal = EDF_SUCCESS;
		}

		return( eRetVal );

	};

/*!
*   \brief Return the number of signals
*   \param peEdfStatus is loaded with status value if not null
*   \return pointer to string of ASCII data
*/
	char *CReadEDF::pszGetNumberSignals( edfStatus_E *peEdfStatus )
	{
		return( &m_szValue[0] );
	};

/*!
*   \brief Return the number of signals
*
*   \param peEdfStatus is loaded with status value if not null
*
*   \return integer number of signals
*/

int CReadEDF::iGetNumberSignals( edfStatus_E *peEdfStatus )
{
	int iNumSig = 0;
	memcpy( m_szValue, m_acHeaderFixedLength.cNumberSignals, sizeof( numberSignals_S ) );
	m_szValue[ sizeof(numberSignals_S) ] = '\0' ;	// make sure there is a string terminator

	iNumSig = atoi( m_szValue);
	if( errno == ERANGE )
	{
		strcpy_s( m_szValue, "BAD!" );
		iNumSig = 0;

		if( peEdfStatus != NULL)
		{
			*peEdfStatus = EDF_DATE_ERROR;
		}
	}
		
	return( iNumSig );
}

/*!
*   \brief 
*
*   \param peEdfStatus is loaded with status value if not null
*
*   \return integer number of signals
*/

char *CReadEDF::pszGetSignalLabel( int iSignalNumber )
{
	m_szValue[0] = '\0';
	return( &m_szValue[0] );
}

/*!
*   \brief 
*
*   \param peEdfStatus is loaded with status value if not null
*
*   \return integer number of signals
*/

char *CReadEDF::pszGetNumberRecords()
{
	m_szValue[0] = '\0';
	return( &m_szValue[0] );
}

/*!
*   \brief 
*
*   \param peEdfStatus is loaded with status value if not null
*
*   \return integer number of signals
*/

char *CReadEDF::pszGetDuration( edfStatus_E *eEdfStatus )
{
	m_szValue[0] = '\0';
	return( &m_szValue[0] );
}
