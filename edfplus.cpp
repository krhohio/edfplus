﻿/*!
	File name: $HeadURL:
    \file
    \brief Contains class implementation for data extraction from a EDF Plus file.
	(see the Doxygen |Related Pages| tab "Todo List" for future "EDF Plus" additions)
*/

#include <iostream>	// for cout
#include "edfplus.h"
using namespace std;

/*!
*   \brief Constructor
*   \param csInputFile - input file 
*   \return - status
*/

CReadEDF::CReadEDF( char *pszInputFile, edfStatus_E *peEdfStatus )
{
	m_eStaticStatus = EDF_VOID;		// signify undetermined status
	m_eDynamicStatus = EDF_VOID;		// signify undetermined status

	// Fake for loop for common error exit:
	for( bool allDone = false; allDone == false; allDone = true )
	{
		m_poEdfFile = new ifstream( pszInputFile );

		if( m_poEdfFile->fail() )
		{
			m_eDynamicStatus = EDF_FILE_OPEN_ERROR;
			break;
		}

		m_pcFileData = new char[sizeof(CReadEDF::headerFixedLength_S)+1 ];
		{
			// error handling
		}

		int iDebug = sizeof(CReadEDF::headerFixedLength_S);
		int extracted = m_poEdfFile->read( (char *)&m_acHeaderFixedLength, sizeof(CReadEDF::headerFixedLength_S) ).gcount();

		if( m_poEdfFile->fail() )
		{
			break;
		}

		m_eDynamicStatus = eGetNumberSignals( &m_iNumberSignals );

		// Exit with error if unsuccessful getting the number of signals:
		if(m_eDynamicStatus != EDF_SUCCESS )
		{
			break;
		}

		int iSize = 0;
		
		// Be pessimistic about reading all the variable lenght header fields:
		m_eDynamicStatus = CReadEDF::EDF_FILE_CONTENTS_ERROR;

		// 1st Variable Length Header Field:
		iSize = (m_iNumberSignals * eSignalLabelSize);
		m_pacSignalLabels = new char[iSize];
		m_poEdfFile->read( m_pacSignalLabels, iSize );
		if( m_poEdfFile->fail() ) break;

		// 2nd Variable Length Header Field:
		iSize = (m_iNumberSignals * eTransducerTypeSize);
		m_pacTransducerTypes = new char[iSize];
		m_poEdfFile->read( m_pacTransducerTypes, iSize );
		if( m_poEdfFile->fail() ) break;

		// 3rd Variable Length Header Field:
		iSize = (m_iNumberSignals * ePhysicalDimensionSize);
		m_pacPhysicalDimensions = new char[iSize];
		m_poEdfFile->read( m_pacPhysicalDimensions, iSize );
		if( m_poEdfFile->fail() ) break;

		// 4th Variable Length Header Field:
		iSize = (m_iNumberSignals * ePhysicalMinimumSize);
		m_pacPhysicalMinimums = new char[iSize];
		m_poEdfFile->read( m_pacPhysicalMinimums, iSize );
		if( m_poEdfFile->fail() ) break;

		// 5th Variable Length Header Field:
		iSize = (m_iNumberSignals * ePhysicalMaximumSize);
		m_pacPhysicalMaximums = new char[iSize];
		m_poEdfFile->read( m_pacPhysicalMaximums, iSize );
		if( m_poEdfFile->fail() ) break;

		// 6th Variable Length Header Field:
		iSize = (m_iNumberSignals * eDigitalMinimumSize);
		m_pacDigitalMinimums = new char[iSize];
		m_poEdfFile->read( m_pacDigitalMinimums, iSize );
		if( m_poEdfFile->fail() ) break;

		// 7th Variable Length Header Field:
		iSize = (m_iNumberSignals * eDigitalMaximumSize);
		m_pacDigitalMaximums = new char[iSize];
		m_poEdfFile->read( m_pacDigitalMaximums, iSize );
		if( m_poEdfFile->fail() ) break;

		// 8th Variable Length Header Field:
		iSize = (m_iNumberSignals * ePrefilteringSize);
		m_pacPrefilterings = new char[iSize];
		m_poEdfFile->read( m_pacPrefilterings, iSize );
		if( m_poEdfFile->fail() ) break;

		// 9th Variable Length Header Field:
		iSize = (m_iNumberSignals * eNumberSamplesSize);
		m_pacNumberSamples = new char[iSize];
		m_poEdfFile->read( m_pacNumberSamples, iSize );
		if( m_poEdfFile->fail() ) break;
		
		m_eDynamicStatus = EDF_SUCCESS;			// We are successfull when arriving here
		break;

	} //for()

	m_eStaticStatus = m_eDynamicStatus;		// Set the static (constructor) status

	if( peEdfStatus )
	{
		*peEdfStatus = m_eStaticStatus;
	}

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
*	\note **The time value to be validated must already be loaded into m_szValue **
*   \param (none)
*   \return (none)
*/

bool CReadEDF::bValidTimeValue( void )
	{
		bool bRetVal = false;		// be pessimestic
		time_S *pacTime = (time_S *)&m_szValue[0];

		// Fake for loop for common error exit:
		for( bool allDone = false; allDone == false; allDone = true )
		{
			/// \todo if leading space(s) are allowed in time then test for them here:	
			if( pacTime->cH_MSD < '0' || pacTime->cH_MSD > '2' ) break;		// check for max 24-hour time
			if( pacTime->cH_LSD < '0' || pacTime->cH_MSD > '9' ) break;		// allow 09:00 and 19:00
			if( pacTime->cDot1 != '.' ) break;								// ensure a non-digit terminator for atoi()

			// Test for valid hours:
			int iHH = ((pacTime->cH_MSD & 0x0F) * 10 + (pacTime->cH_LSD & 0x0F));
			if( iHH > 23 ) break;

			// Test for valid minutes:
			if( pacTime->cM_MSD < '0' || pacTime->cM_MSD > '5' ) break;
			if( pacTime->cM_LSD < '0' || pacTime->cM_MSD > '9' ) break;
			if( pacTime->cDot2 != '.' ) break;

			// Test for valid seconds:
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
*	\note **The date value to be validated must already be loaded into m_szValue. **
*   \param (none)
*   \return (none)
*/

bool CReadEDF::bValidDateValue( void )
{
	bool bRetVal = false;		// be pessimestic
	date_S *pacDate = (date_S *)&m_szValue[0];

	// Fake for loop for common error exit:
	for( bool allDone = false; allDone == false; allDone = true )
	{
		/// \todo if leading space(s) are allowed in date then test for them here:	
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
*   \param peEdfStatus is loaded with status value if not null.
*   \return Start time string.
*/

char *CReadEDF::pszGetStartTime( edfStatus_E *peEdfStatus )
{
	m_szValue[0] = '\0';	// make sure there is a string terminator
	memcpy( (char *)&m_szValue, (void *)&m_acHeaderFixedLength.acStartTime,sizeof( time_S ));

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
*   \param peEdfStatus is loaded with status value if not null.
*   \return Start date string.
*/

char *CReadEDF::pszGetStartDate( edfStatus_E *peEdfStatus )
{
	m_szValue[0] = '\0';	// make sure there is a string terminator
	memcpy( &m_szValue, &m_acHeaderFixedLength.acStartDate, sizeof( date_S ));
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
*   \brief Return the number of signals
*   \param peEdfStatus is loaded with status value if not null
*   \return status of operation
*/

CReadEDF::edfStatus_E CReadEDF::eGetNumberSignals( int* piNumberSignals, char* pszNumberSignals )
{
	m_eDynamicStatus =	EDF_SUCCESS;	// be optimistic

	memcpy( m_szValue, m_acHeaderFixedLength.acNumberSignals, eNumberSignalsSize );
	m_szValue[ eNumberSignalsSize ] = '\0' ;	// make sure there is a string terminator

	m_iNumberSignals = atoi( m_szValue);
	if( errno == ERANGE )
	{
		m_iNumberSignals = 0;
		strcpy_s( m_szValue, "BAD!" );
		m_eDynamicStatus =	EDF_FILE_CONTENTS_ERROR;
	}
		
	if( piNumberSignals )
	{
		*piNumberSignals = m_iNumberSignals;
	}

	if( pszNumberSignals )
	{
		pszNumberSignals = &m_szValue[0];
	}

	return( m_eDynamicStatus );
}

/*!
*   \brief Return the number of signals
*   \param peEdfStatus is loaded with status value if not null
*   \return pointer to string of ASCII data
*/
	char *CReadEDF::pszGetNumberSignals( edfStatus_E *peEdfStatus )
	{
		m_eDynamicStatus = eGetNumberSignals( NULL, &m_szValue[0] );

		if( peEdfStatus != NULL)
		{
			peEdfStatus = &m_eDynamicStatus;
		}
		
		return( &m_szValue[0] );
	};

/*!
*   \brief Get the number of data records.
*   \param peEdfStatus is loaded with status value if not null.
*   \return Status of operation.
*/

CReadEDF::edfStatus_E CReadEDF::eGetNumberRecords( int* piNumberRecords, char* pszNumberRecords )
{
	m_eDynamicStatus =	EDF_SUCCESS;	// be optimistic

	memcpy( m_szValue, m_acHeaderFixedLength.acNumberRecords, sizeof( numberRecords_S ) );
	m_szValue[ sizeof(numberRecords_S) ] = '\0' ;	// make sure there is a string terminator

	m_iNumberRecords = atoi( m_szValue);
	if( errno == ERANGE )
	{
		m_iNumberRecords = 0;
		strcpy_s( m_szValue, "BAD!" );
		m_eDynamicStatus =	EDF_FILE_CONTENTS_ERROR;
	}
		
	if( piNumberRecords )
	{
		piNumberRecords = &m_iNumberRecords;
	}

	if( pszNumberRecords )
	{
		pszNumberRecords = &m_szValue[0];
	}

	return( m_eDynamicStatus );
}

/*!
*   \brief Return the number of data records.
*   \param peEdfStatus is loaded with status value if not null.
*   \return Pointer to string of ASCII data.
*/
char *CReadEDF::pszGetNumberRecords( edfStatus_E *peEdfStatus )
	{
		m_eDynamicStatus = eGetNumberRecords( NULL, &m_szValue[0] );

		if( peEdfStatus != NULL)
		{
			peEdfStatus = &m_eDynamicStatus;
		}
		
		return( &m_szValue[0] );
	};

/*!
*   \brief Get the duration of a data record.
*   \param piDuration is loaded with the integer duration if not a null pointer.
*   \param pszDuration is loaded with the string duration if not a null pointer.
*   \return status of operation.
*/

CReadEDF::edfStatus_E CReadEDF::eGetDuration( int* piDuration, char* pszDuration )
{
	m_eDynamicStatus =	EDF_SUCCESS;	// be optimistic

	memcpy( m_szValue, m_acHeaderFixedLength.acDuration, sizeof( duration_S ) );
	m_szValue[ sizeof(duration_S) ] = '\0' ;	// make sure there is a string terminator

	m_iDuration = atoi( m_szValue );
	if( errno == ERANGE )
	{
		m_iDuration = 0;
		strcpy_s( m_szValue, "BAD!" );
		m_eDynamicStatus =	EDF_FILE_CONTENTS_ERROR;
	}
		
	if( piDuration )
	{
		piDuration = &m_iDuration;
	}

	if( pszDuration )
	{
		pszDuration = &m_szValue[0];
	}

	return( m_eDynamicStatus );
}

/*!
*   \brief Return the duration of a data record.
*   \param peEdfStatus is loaded with status value if not null.
*   \return pointer to string of ASCII data.
*/

char *CReadEDF::pszGetDuration( edfStatus_E *peEdfStatus )
	{
		m_eDynamicStatus = eGetDuration( NULL, &m_szValue[0] );

		if( peEdfStatus != NULL)
		{
			peEdfStatus = &m_eDynamicStatus;
		}
		
		return( &m_szValue[0] );
	};

/*!
*   \brief Get a signal label.
*   \param peEdfStatus is loaded with status value if not null.
*   \return Signal label.
*/

char *CReadEDF::pszGetSignalLabel( int iSignalNumber, edfStatus_E *peEdfStatus )
{
	m_szValue[0] = '\0';	// make sure there is a string terminator
	int iNumberSignals = 0;
	
	m_eDynamicStatus = EDF_VOID;	// be pessimistic 

	// Fake for loop for common error exit:
	for( bool allDone = false; allDone == false; allDone = true )
	{
		// First check to make sure that the EDF data is ready to be accessed:
		if( !bReadyStatus() )
		{
			break;
		}

		if( iSignalNumber > m_iNumberSignals )
		{
			m_eDynamicStatus = EDF_INVALID_SIGNAL_REQUESTED;
			break;
		}

		signalLabel_S *pacSignalLabels = (signalLabel_S *)m_pacSignalLabels;

		memcpy( &m_szValue, &pacSignalLabels[iSignalNumber], eSignalLabelSize );
		m_szValue[ eSignalLabelSize ] = '\0';

		m_eDynamicStatus = EDF_SUCCESS;	
		break;

	} // for()

	if( m_eDynamicStatus != EDF_SUCCESS )
	{
		strcpy_s( m_szValue, 5, "BAD!" );

		if( peEdfStatus != NULL)
		{
			*peEdfStatus = m_eDynamicStatus;
		}
	}

	return( &m_szValue[0] );
}

/*!
*   \brief Get number of signal samples
*   \param iSignalNumber must contain the desired signal number.
*   \param peEdfStatus is loaded with status value if not null.
*   \return Number of signal samples.
*/

int CReadEDF::iGetNumberSamples( int iSignalNumber, edfStatus_E *peEdfStatus )
{
	m_eDynamicStatus =	EDF_SUCCESS;	// be optimistic
	int iNumberSamples = 0;

	numberSamples_S *pacNumberSamples = (numberSamples_S *)m_pacNumberSamples;
	memcpy( &m_szValue, &pacNumberSamples[iSignalNumber], eNumberSamplesSize );

	iNumberSamples = atoi( m_szValue );
	if( errno == ERANGE )
	{
		m_eDynamicStatus = EDF_FILE_CONTENTS_ERROR;
	}

	if( peEdfStatus != NULL )
	{
		*peEdfStatus = m_eDynamicStatus;
	}

	return( iNumberSamples );
}


/*!
*   \brief Get a sample from a signal
*   \param peEdfStatus is loaded with status value if not null.
*   \return Status of operation.
*/

CReadEDF::edfStatus_E CReadEDF::eGetSample( short int iSignalNumber, int iSampleNumber, int *piSampleValue )
{
	short int iSampleValue = 0;	
	
	m_eDynamicStatus = EDF_VOID;

	// Fake for loop for common error exit:
	for( bool allDone = false; allDone == false; allDone = true )
	{
		if( !bReadyStatus( &m_eDynamicStatus ) )
		{
			break;
		}

		if( iSignalNumber > m_iNumberSignals )
		{
			m_eDynamicStatus = EDF_INVALID_SIGNAL_REQUESTED;
			break;
		}

		int iOffsetPastHeaders = sizeof(headerFixedLength_S) + (sizeof(headerVariableLength_S) * m_iNumberSignals);
		int iOffsetToSignal = iOffsetPastHeaders;

		for( int iThisSignal = 0; iThisSignal < m_iNumberSignals; iThisSignal++ )
		{
			iOffsetToSignal +=  iGetNumberSamples( iThisSignal, &m_eDynamicStatus );
			if( m_eDynamicStatus != EDF_SUCCESS )
			{
				break;
			}
		}

		if( m_eDynamicStatus != EDF_SUCCESS )
		{
			break;
		}

		int iOffsetToSample = iOffsetToSignal + (iSignalNumber * eSampleSize);
	
		m_poEdfFile->seekg( iOffsetToSample );
		//cout << "File currently at byte: " << m_poEdfFile->tellg() << endl;

		if( m_poEdfFile->fail() )
		{
			m_eDynamicStatus = CReadEDF::EDF_FILE_CONTENTS_ERROR;
			break;
		}

		m_poEdfFile->read( reinterpret_cast<char *>(&iSampleValue), eSampleSize );

		if( piSampleValue != NULL )
		{
			*piSampleValue = iSampleValue;
		}

	} //for()

	return( m_eDynamicStatus );
}
