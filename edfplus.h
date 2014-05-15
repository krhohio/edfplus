#ifndef EDFPLUS_H
#define EDFPLUS_H

// #if MVCPP
#include <atlstr.h>
// #else
#include <cstring>
// #endif

#include <fstream>
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

class Cedfplus
{
	public:
		enum edfStatus_E
		{
			EDF_SUCCESS,
			EDF_FILE_OPEN_ERROR,
			EDF_FILE_CONTENTS_ERROR,
		};
		
		struct format_S
        {
            char	format[8];		///<
        };

        struct localPatientID_S
        {
                char        localPatientID[80];
        };

		struct localRecordingID_S
        {
                char        localRecordingID[8];
        };

        struct startDate_S
        {
            char        dd[2];                ///< ASCII dd
			char        dot1[1];                ///< ASCII ‘.’
			char        mm[2];                ///< ASCII mm
			char        dot2[1];                ///< ASCII ‘.’
			char        yy[2];                ///< ASCII yy
        };

        struct startTime_S
        {
            char        hh[2];                ///< ASCII hh
			char        dot1[1];                ///< ASCII ‘.’
			char        mm[2];                ///< ASCII mm
			char        dot2[1];                ///< ASCII ‘.’
			char        ss[2];                ///< ASCII ss
        };


	struct generalHeader_S
	{
		format_S            format;                 ///< 8 ascii : version of this data format (0) 
        localPatientID_S    localPatientID;         ///< 80 ascii : local patient identification (mind item 3 of the additional EDF+ specs)
		localRecordingID_S  localRecordingID;		///< 80 ascii : local recording identification (mind item 4 of the additional EDF+ specs)
		startDate_S			startDate;              ///< 8 ascii : startdate of recording (dd.mm.yy) (mind item 2 of the additional EDF+ specs)
		startTime_S			startTime;              ///< 8 ascii : starttime of recording (hh.mm.ss) 
		headerSize_S		headerSize;				///< 8 ascii : number of bytes in header record 
		reserved44_S		reserved44;				///< 44 ascii : reserved 
		numberRecords_S		numberRecords;			///< 8 ascii : number of data records (-1 if unknown, obey item 10 of the additional EDF+ specs) 
		duration_S			duration;				///< 8 ascii : duration of a data record, in seconds 
		numberSignals_S		numberSignals;			///< 4 ascii : number of signals (ns) in data record 
		label_S				label;					///< ns * 16 ascii : ns * label (e.g. EEG Fpz-Cz or Body temp) (mind item 9 of the additional EDF+ specs)

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

	Cedfplus( CString csInputFile );
	~Cedfplus( void );

	edfStatus_E m_eEdfStatus;
	char *m_pcFileData;

private:	
	ifstream *m_poInputFile;
	generalHeader_S *m_pcGeneralHeader;
	
}; //class Cedfplus

#endif // EDFPLUS_H