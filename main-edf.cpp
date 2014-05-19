// edfplus.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "edfplus.h"

int main(int argc, char* argv[])
{
	int iRetVal = EXIT_FAILURE;			// be pessimistic
	
	CReadEDF *poEDF = NULL;
	CReadEDF::edfStatus_E eEdfStatus = CReadEDF::EDF_VOID;

    // Fake for loop for common error exit:
    for( bool allDone = false; allDone == false; allDone = true )
    {
		CReadEDF::edfStatus_E eEdfStatus;
		
		if( argc != 2 )
		{
			cout << endl << "Please enter an input EDF filename as a command line argument!" << endl
				 << "(example: ""ReadEDF c:\\TestFile1.edf"")";
			break;			// if not executable name + single input file then exit with error status
		}
		
		poEDF = new CReadEDF( argv[1] );

		if( poEDF->m_eEdfStatus != CReadEDF::EDF_SUCCESS )
		{
			if( poEDF->m_eEdfStatus == CReadEDF::EDF_FILE_OPEN_ERROR )
			{
				cout << "File read error - please check the path and spelling: " << endl
					 << "(" << argv[1] << ")" << endl;
			}
			break;		// exit with error status
		}

		cout << "Start Time = " << poEDF->pszGetStartTime() << endl;
		cout << "Start Date = " << poEDF->pszGetStartDate() << endl;
		cout << "Number of signals = " << poEDF->pszGetNumberSignals() << endl;
		cout << "Number of data records = " << poEDF->pszGetNumberRecords() << endl;
		cout << "Duration of a data record = " << poEDF->pszGetDuration() << endl;
		
		int iNumberSignals =0;
		eEdfStatus = poEDF->eGetNumberSignals( &iNumberSignals );

		if( eEdfStatus != CReadEDF::EDF_SUCCESS )
		{
			break;
		}

		for( int i=0; i < iNumberSignals; i++ )
		{
			cout << "Signal " << i+1 << " Label = " << poEDF->pszGetSignalLabel( i ) << endl;
		}

		break;

	} // for()

	if( eEdfStatus == CReadEDF::EDF_SUCCESS )
	{
		iRetVal = EXIT_SUCCESS ;
	}

	char cKey = 0;
	cout << endl << "Press any key followed by the Enter key to continue: ";
	cin >> cKey;

	return( iRetVal );

} // main()

