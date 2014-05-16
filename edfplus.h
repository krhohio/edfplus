#ifndef EDFPLUS_H
#define EDFPLUS_H

#include <fstream>
#include <errno.h>
using namespace std;


/*

http://www.edfplus.info/specs/edf.html

One data file contains one uninterrupted digitized polygraphic recording. A data file consists of
a header record followed by data records. The variable-length header record identifies the patient
and specifies the technical characteristics of the recorded signals. The data records contain
consecutive fixed-duration epochs of the polygraphic recording.

The first 256 bytes of the header record specify the version number of this format, local patient
and recording identification, time information about the recording, the number of data records
and finally the number of signals (ns) in each data record. Then for each signal another 256 bytes
follow in the header record, each specifying the type of signal (e.g. EEG, body temperature, etc.),
amplitude calibration and the number of samples in each data record (from which the sampling frequency
can be derived since the duration of a data record is also known). In this way, the format allows for
different gains and sampling frequencies for each signal.

The header record contains 256 + (ns * 256) bytes.
Figure 1 shows its detailed format.
The information in the ASCII strings must be left-justified and filled out with spaces.
Midnight time is 00:00:00.

The duration of each data record is recommended to be a whole number of seconds and its size (number of bytes)
is recommended not to exceed 61440. Only if a 1s data record exceeds this size limit, the duration is recommended
to be smaller than 1s (e.g. 0.01).

The digital minimum and maximum of each signal should specify the extreme values that can occur in the data records.
These often are the extreme output values of the A/D converter. The physical (usually also physiological) minimum
and maximum of this signal should correspond to these digital extremes and be expressed in the also specified physical
dimension of the signal. These 4 extreme values specify offset and amplification of the signal.

Following the header record, each of the subsequent data records contains 'duration' seconds of 'ns' signals,
with each signal being represented by the specified (in the header) number of samples. In order to reduce
data size and adapt to commonly used software for acquisition, processing and graphical display of
polygraphic signals, each sample value is represented as a 2-byte integer in 2's complement format.

Figure 1 shows the detailed format of each data record.
Gains, electrode montages and filters should remain fixed during the recording. Of course, these may
all be digitally modified during replay of the digitized recording.

Below is the detailed digital format of the header record (upper block, ascii's only)
and of each subsequent data record (lower block, integers only). Note that each one
of the ns signals is characterized separately in the header.

HEADER RECORD (we suggest to also adopt the 12 simple additional EDF+ specs)
8 ascii : version of this data format (0) 
80 ascii : local patient identification (mind item 3 of the additional EDF+ specs)
80 ascii : local recording identification (mind item 4 of the additional EDF+ specs)
8 ascii : startdate of recording (dd.mm.yy) (mind item 2 of the additional EDF+ specs)
8 ascii : starttime of recording (hh.mm.ss) 
8 ascii : number of bytes in header record 
44 ascii : reserved 
8 ascii : number of data records (-1 if unknown, obey item 10 of the additional EDF+ specs) 
8 ascii : duration of a data record, in seconds 
4 ascii : number of signals (ns) in data record 
ns * 16 ascii : ns * label (e.g. EEG Fpz-Cz or Body temp) (mind item 9 of the additional EDF+ specs)
ns * 80 ascii : ns * transducer type (e.g. AgAgCl electrode) 
ns * 8 ascii : ns * physical dimension (e.g. uV or degreeC) 
ns * 8 ascii : ns * physical minimum (e.g. -500 or 34) 
ns * 8 ascii : ns * physical maximum (e.g. 500 or 40) 
ns * 8 ascii : ns * digital minimum (e.g. -2048) 
ns * 8 ascii : ns * digital maximum (e.g. 2047) 
ns * 80 ascii : ns * prefiltering (e.g. HP:0.1Hz LP:75Hz) 
ns * 8 ascii : ns * nr of samples in each data record 
ns * 32 ascii : ns * reserved
*/

class CReadEDF
{
	public:
		enum edfStatus_E
		{
			EDF_SUCCESS,
			EDF_VOID,						///< enum used when status is not yet available or updated
			EDF_FILE_OPEN_ERROR,
			EDF_FILE_CONTENTS_ERROR,
			EDF_TIME_ERROR,
			EDF_DATE_ERROR,
		};

		enum eGetEDFStatus;
		
		struct format_S
        {
            char format[8];
        };

        struct localPatientID_S
        {
            char cLocalPatientID[80];
        };

		struct localRecordingID_S
        {
            char cLocalRecordingID[80];
        };

        struct date_S
        {
            char cD_MSD;						///< ASCII Day MSD
            char cD_LSD;						///< ASCII Day LSD
			char cDot1;							///< ASCII ‘.’
			char cM_MSD;						///< ASCII Month MSD
			char cM_LSD;						///< ASCII Month LSD
			char cDot2;							///< ASCII ‘.’
			char cY_MSD;						///< ASCII Year MSD
        	char cY_LSD;						///< ASCII Year LSD
        };

        struct time_S
        {
            char cH_MSD;						///< ASCII Hour MSD
            char cH_LSD;						///< ASCII Hour LSD
			char cDot1;							///< ASCII ‘.’
			char cM_MSD;						///< ASCII Minutes MSD
			char cM_LSD;						///< ASCII Minutes LSD
			char cDot2;							///< ASCII ‘.’
			char cS_MSD;						///< ASCII Seconds MSD
        	char cS_LSD;						///< ASCII Seconds LSD
		};

		struct headerSize_S						
		{
			char cHeaderSize[8];				///< 8 ascii : number of bytes in header record
		};
		               
		struct reserved44_S
		{
			char cReserved44[44];				///< 44 ascii : reserved 
		};

		struct numberRecords_S
		{
			char cNumberRecords[8];				///< 8 ascii : number of data records (-1 if unknown, obey item 10 of the additional EDF+ specs) 
		};

		struct duration_S
		{
			char cDuration[8];					///< 8 ascii : duration of a data record, in seconds 
		};

		struct numberSignals_S
		{
			char cNumberSignals[4];				///< 4 ascii : number of signals (ns) in data record 
		};

		struct label_S
		{
			char cLabel[16];					///< ns * 16 ascii : ns * label (e.g. EEG Fpz-Cz or Body temp) (mind item 9 of the additional EDF+ specs)
		};

	struct generalHeader_S
	{
		format_S            cFormat;            ///< 8 ascii : version of this data format (0) 
        localPatientID_S    cLocalPatientID;    ///< 80 ascii : local patient identification (mind item 3 of the additional EDF+ specs)
		localRecordingID_S  cLocalRecordingID;	///< 80 ascii : local recording identification (mind item 4 of the additional EDF+ specs)
		date_S				cStartDate;         ///< 8 ascii : startdate of recording (dd.mm.yy) (mind item 2 of the additional EDF+ specs)
		time_S				cStartTime;         ///< 8 ascii : starttime of recording (hh.mm.ss) 
		char				cHeaderSize[8];		///< 8 ascii : number of bytes in header record 
		char				cReserved[44];		///< 44 ascii : reserved 
		char				cNumberRecords[8];	///< 8 ascii : number of data records (-1 if unknown, obey item 10 of the additional EDF+ specs) 
		char				cDuration[8];		///< 8 ascii : duration of a data record, in seconds 
		char				cNumberSignals[4];	///< 4 ascii : number of signals (ns) in data record 
		char				cLabel[16];			///< ns * 16 ascii : ns * label (e.g. EEG Fpz-Cz or Body temp) (mind item 9 of the additional EDF+ specs)

/*
ns * 80 ascii : ns * transducer type (e.g. AgAgCl electrode) 
ns * 8 ascii : ns * physical dimension (e.g. uV or degreeC) 
ns * 8 ascii : ns * physical minimum (e.g. -500 or 34) 
ns * 8 ascii : ns * physical maximum (e.g. 500 or 40) 
ns * 8 ascii : ns * digital minimum (e.g. -2048) 
ns * 8 ascii : ns * digital maximum (e.g. 2047) 
ns * 80 ascii : ns * prefiltering (e.g. HP:0.1Hz LP:75Hz) 
ns * 8 ascii : ns * nr of samples in each data record 
ns * 32 ascii : ns * reserved
*/	
	};
/*
DATA RECORD 
nr of samples[1] * integer : first signal in the data record 
nr of samples[2] * integer : second signal 
.. 
.. 
nr of samples[ns] * integer : last signal 
8 ascii : number of bytes in header record 
44 ascii : reserved 
8 ascii : number of data records (-1 if unknown, obey item 10 of the additional EDF+ specs) 
8 ascii : duration of a data record, in seconds 
4 ascii : number of signals (ns) in data record 
ns * 16 ascii : ns * label (e.g. EEG Fpz-Cz or Body temp) (mind item 9 of the additional EDF+ specs)
ns * 80 ascii : ns * transducer type (e.g. AgAgCl electrode) 
ns * 8 ascii : ns * physical dimension (e.g. uV or degreeC) 
ns * 8 ascii : ns * physical minimum (e.g. -500 or 34) 
ns * 8 ascii : ns * physical maximum (e.g. 500 or 40) 
ns * 8 ascii : ns * digital minimum (e.g. -2048) 
ns * 8 ascii : ns * digital maximum (e.g. 2047) 
ns * 80 ascii : ns * prefiltering (e.g. HP:0.1Hz LP:75Hz) 
ns * 8 ascii : ns * nr of samples in each data record 
ns * 32 ascii : ns * reserved
*/

	CReadEDF( char *csInputFile );
	~CReadEDF( void );

	edfStatus_E m_eEdfStatus;
	char *m_pcFileData;

	bool bValidTimeValue( void );
	bool bValidDateValue( void );

	char *pszGetStartTime( edfStatus_E *peEdfStatus = NULL );
	char *pszGetStartDate( edfStatus_E *peEdfStatus = NULL );

	char *pszGetNumberSignals()
	{
		return( &m_szValue[0] );
	};

	int iGetNumberSignals( edfStatus_E *peEdfStatus = NULL );

	char *pszGetSignalLabel( int iSignalNumber );
	char *pszGetNumberRecords();
	char *pszGetDuration( edfStatus_E *eEdfStatus = NULL );

private:	
	ifstream *m_poEdfFile;
	generalHeader_S m_acGeneralHeader;

	char m_szValue[ sizeof( generalHeader_S)+1 ];	///< worst case length for return values

}; //class CReadEDF

#endif // EDFPLUS_H