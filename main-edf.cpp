/*!
	File name: $HeadURL:
	\file
    \brief Defines the entry point for the console application (using edfplus.*)

	\note Enter command line argument for the EDF input file in Microsoft Visual Studio
	      by adding it to <Project | Properties | Configuration Properties | Debug>
		  in the <Command Arguments> edit box.

		  EDF input files previously used:
		  "C:\2014-05-00 MindWare Programming Exercise\TestFile1.edf" (and TestFile2.edf)
*/

/*!
	\mainpage This C++ class was written for a "Programming Exercise" as part of a job application:
	On Tue, May 13, 2014 at 11:49 AM, Eric wrote:
	Hi Kent,
	Thanks again for taking the time to meet with us last week. For the next step we have put together a
	short programming exercise for you to work on that will allow us to see your coding style and practices.
	You will find the details of the exercise in the attached PDF along with 2 test files which will help
	you test your solution. The file specification that you can reference when working on the exercise can
	be found here. http://www.edfplus.info/specs/edf.html If at any point you have questions feel free to ask.
	Please send back the completed exercise no later than one week from today (Tuesday May 20th) but it is
	perfectly acceptable to send it in before then if you finish early. Thanks and good luck!
*/

#include <iostream>
#include "edfplus.h"

int main(int argc, char* argv[])
{
	int iRetVal = EXIT_FAILURE;			// be pessimistic
	
	CReadEDF *poEDF = NULL;
	CReadEDF::edfStatus_E eEdfStatus = CReadEDF::EDF_VOID;

	cout << endl << "ReadEDF program by Kent H, 2014-05-20 (Version 1RC1)" << endl << endl;

    // Fake for loop for common error exit:
    for( bool allDone = false; allDone == false; allDone = true )
    {
		if( argc == 2 )
		{
			cout << "Filename entered was: " << argv[1] << endl;
		}
		else
		{
			cout << endl << "Please enter an input EDF filename as a command line argument!" << endl
				 << "(example: ""ReadEDF c:\\TestFile1.edf"")";
			break;			// if not executable name + single input file then exit with error status
		}
		
		poEDF = new CReadEDF( argv[1] );

		if( !poEDF->bReadyStatus( &eEdfStatus ) )
		{
			if( eEdfStatus == CReadEDF::EDF_FILE_OPEN_ERROR )
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
		cout << "Duration of a data record = " << poEDF->pszGetDuration() << " seconds" << endl;
		
		int iNumberSignals =0;
		eEdfStatus = poEDF->eGetNumberSignals( &iNumberSignals );

		if( eEdfStatus != CReadEDF::EDF_SUCCESS )
		{
			break;
		}

		// Display the Signal Label and first sample:
		for( int i=0; i < iNumberSignals; i++ )
		{
			cout << "Signal " << i+1 << " Label = " << poEDF->pszGetSignalLabel( i ) << endl;

			int iSampleValue = 0;
			eEdfStatus = poEDF->eGetSample( i, 1, &iSampleValue );

			cout << "First Sample = " << iSampleValue << endl;
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

