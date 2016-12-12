/*
 * Copyright (c) 2010-2016 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 3532 rue Ste-Famille, #3
 * Montreal, QC, H2X 2L1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * The text above constitutes the entire PortAudio license; however, 
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also 
 * requested that these non-binding requests be included along with the 
 * license above.
 */

#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"

//2012mar17, spi, begin
/*
#include "WavFile.h"
#include "SoundTouch.h"
using namespace soundtouch;
*/
#define BUFF_SIZE	2048
//2012mar17, spi, end

#include <ctime>
#include <iostream>
#include "spiws_WavSet.h"

#include <windows.h>

/* Select sample format. */
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif


//////
//main
//////
//usage: spisplitdisperseplay testbeat2.wav 0.5 10
int main(int argc, char *argv[]);
int main(int argc, char *argv[])
{
	int nShowCmd = false;
	ShellExecuteA(NULL, "open", "begin.bat", "", NULL, nShowCmd);

    PaStreamParameters outputParameters;
    PaStream* stream;
    PaError err;
	WavSet* pWavSet = NULL;

	///////////////////
	//read in arguments
	///////////////////
	//char charBuffer[2048] = {"testbeat11.wav"}; //works fine
	//char charBuffer[2048] = {"testbeat12.wav"}; //error in last segment last edge initial access, within fadeout, todo: fix
	char charBuffer[2048] = {"testbeat11.wav"}; //works fine
	double fSecondsPerSegment = 1.0; //0.0 for loopplay //non-zero for spliting sample into sub-segments
	int numSecondsPlay = 20;
	if(argc>1)
	{
		//first argument is the filename
		sprintf_s(charBuffer,2048-1,argv[1]);
	}
	if(argc>2)
	{
		//second argument is the segment length in seconds, 0 for no split
		fSecondsPerSegment = atof(argv[2]);
	}
	if(argc>3)
	{
		//third argument is the time it will play
		numSecondsPlay = atoi(argv[3]);
	}
	/////////////////////////////////////////////////////////////
	//log currently used parameters into file (to ease debugging)
	/////////////////////////////////////////////////////////////
	if(1)
	{
		FILE* pFILE = fopen("debug.txt", "w");
		fprintf(pFILE, "filename = %s\n", charBuffer);
		fprintf(pFILE, "fSecondsPerSegment = %f\n", fSecondsPerSegment);
		fprintf(pFILE, "numSecondsPlay = %d\n", numSecondsPlay);
		fclose(pFILE);
	}

	//////////////////////////////
	//read a WAV file using WavSet 
	//////////////////////////////
	pWavSet = new WavSet;
	pWavSet->ReadWavFile(charBuffer);


	///////////////////////////
	// split WavSet in segments 
	///////////////////////////
	pWavSet->SplitInSegments(fSecondsPerSegment);
	#ifdef _DEBUG
		printf("numSegments=%d\n",pWavSet->numSegments);
	#endif //_DEBUG

	/*
	//tested OK
	WavSet* pWavSet2 = new WavSet;
	pWavSet2->CreateSin(0.5);
	pWavSet2->WriteWavFile("sinus_0-5.wav");
	WavSet* pWavSet3 = new WavSet;
	pWavSet3->CreateSin(3.0);
	pWavSet3->WriteWavFile("sinus_3-0.wav");
	WavSet* pWavSet4 = new WavSet;
	pWavSet4->CreateSilence(0.5);
	pWavSet4->WriteWavFile("sinus_1-0.wav");
	//tested OK
	WavSet* pWavSet5 = new WavSet;
	pWavSet5->Concatenate(pWavSet2);
	pWavSet5->Concatenate(pWavSet4);
	pWavSet5->Concatenate(pWavSet2);
	pWavSet5->Concatenate(pWavSet4);
	pWavSet5->Concatenate(pWavSet2);
	pWavSet5->Concatenate(pWavSet4);
	pWavSet5->Concatenate(pWavSet3);
	pWavSet5->WriteWavFile("sinus_6-0.wav");
	delete pWavSet2;
	delete pWavSet3;
	delete pWavSet4;
	delete pWavSet5;
	*/

	/*
	//tested OK
	WavSet* pWavSet2 = new WavSet;
	pWavSet2->ReadWavFile("testbeat10.wav");
	WavSet* pWavSet3 = new WavSet;
	pWavSet3->ReadWavFile("testbeat15.wav");
	WavSet* pWavSet4 = new WavSet;
	pWavSet4->Mix(0.75, pWavSet2, 0.25, pWavSet3);
	pWavSet4->WriteWavFile("testbeat10-15.wav");
	delete pWavSet2;
	delete pWavSet3;
	delete pWavSet4;
	*/

	/*
	///////////////////////////
	// erase segments in WavSet 
	///////////////////////////
	for(int idSeg=0; idSeg<pWavSet->numSegments; idSeg++)
	{
		if(idSeg!=0) pWavSet->EraseSegment(idSeg);
	#ifdef _DEBUG
		printf("segment id %d has been erased\n", idSeg);
	#endif //_DEBUG
	}
	*/

	/*
	////////////////////
	// duplicate WavSet 
	////////////////////
	WavSet* pWavSet2 = NULL;
	pWavSet2 = new WavSet(pWavSet, -1);
	pWavSet2->WriteWavFile("duplicate.wav");
	delete pWavSet2;
	*/

	if(1)
	{
		////////////////////////////////////////
		// fade edges for all segments in WavSet 
		////////////////////////////////////////
		for(int idSeg=0; idSeg<pWavSet->numSegments; idSeg++)
		{
			pWavSet->FadeSegmentEdges(idSeg);
			#ifdef _DEBUG
				printf("edges for segment id %d have been faded\n", idSeg);
			#endif //_DEBUG
		}
	}
	//////////////////////////
	//initialize random number
	//////////////////////////
	srand((unsigned)time(0));

	/////////////////////////////////////
	// create 1 filename for each segment 
	/////////////////////////////////////
	char charBuffer2[2048]={""};
	char charBuffer3[2048]={""};
	char charBuffer4[2048]={""};
	char charBuffer5[10]={"/b"}; //release runs start with /b option which stands for background
	char charBuffer6[10]={"-q"}; //release runs sox.exe with -q option which stands for quiet
	sprintf_s(charBuffer4,2048-1,charBuffer);
	#ifdef _DEBUG
			sprintf_s(charBuffer5,10-1,""); //debug runs start without /b option which stands for background
			sprintf_s(charBuffer6,10-1,""); //debug runs sox.exe without -q option which stands for quiet
	#endif //_DEBUG
	char* pChar = strrchr(charBuffer4,'.');
	*pChar = '\0';
	int numRepeat = numSecondsPlay/fSecondsPerSegment;
	if(1)
	{
		///////////////////////////////
		//play dry input sample as well
		///////////////////////////////
		sprintf_s(charBuffer3,2048-1,"start %s C:\\sox-14-3-2\\sox %s \"%s\" -d repeat %d",charBuffer5,charBuffer6,charBuffer,numRepeat);
		system(charBuffer3);
	}
	int numSegmentsToSkip = pWavSet->numSegments-30; //max 30 for a maximum of 30 sox.exe simultaniously
	//int numSegmentsToSkip = pWavSet->numSegments-20; //max 20 for a maximum of 20 sox.exe simultaniously
	for(int idSeg=0; idSeg<pWavSet->numSegments; idSeg++)
	{
		if(numSegmentsToSkip>0 && (idSeg)%(pWavSet->numSegments/numSegmentsToSkip)==0)
		{
			continue;
		}
		WavSet* pWavSet3 = NULL;
		pWavSet3 = new WavSet(pWavSet, idSeg);
		sprintf_s(charBuffer2,2048-1,"%s_%d.wav",charBuffer4,idSeg);
		pWavSet3->WriteWavFile(charBuffer2);
		delete pWavSet3;
		//////////////////////////////////////////////////////////////////////////////////
		//execute cmd line to play each .wav filename along with a randomly chosen process
		//////////////////////////////////////////////////////////////////////////////////
		int random_integer;
		int lowest=1, highest=8;
		int range=(highest-lowest)+1;
		random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
	#ifdef _DEBUG
		printf("random integer = %d\n",random_integer);
	#endif //_DEBUG
		switch(random_integer)
		{
			case 1:
				sprintf_s(charBuffer3,2048-1,"start %s C:\\sox-14-3-2\\sox %s \"%s\" -d repeat %d speed 0.5",charBuffer5,charBuffer6,charBuffer2,numRepeat);
				break;		
			case 2:
				sprintf_s(charBuffer3,2048-1,"start %s C:\\sox-14-3-2\\sox %s \"%s\" -d repeat %d stretch 4",charBuffer5,charBuffer6,charBuffer2,numRepeat);
				break;
			case 3:
				sprintf_s(charBuffer3,2048-1,"start %s C:\\sox-14-3-2\\sox %s \"%s\" -d repeat %d tempo 0.5",charBuffer5,charBuffer6,charBuffer2,numRepeat);
				break;
			case 4:
				sprintf_s(charBuffer3,2048-1,"start %s C:\\sox-14-3-2\\sox %s \"%s\" -d reverse repeat %d",charBuffer5,charBuffer6,charBuffer2,numRepeat);
				break;
			case 5:
				sprintf_s(charBuffer3,2048-1,"start %s C:\\sox-14-3-2\\sox %s \"%s\" -d reverb -w repeat %d",charBuffer5,charBuffer6,charBuffer2,numRepeat);
				break;
			case 6:
				sprintf_s(charBuffer3,2048-1,"start %s C:\\sox-14-3-2\\sox %s \"%s\" -d pitch -300 repeat %d",charBuffer5,charBuffer6,charBuffer2,numRepeat);
				break;
			case 7:
				sprintf_s(charBuffer3,2048-1,"start %s C:\\sox-14-3-2\\sox %s \"%s\" -d echos 0.8 0.7 40 0.25 63 0.3 repeat %d",charBuffer5,charBuffer6,charBuffer2,numRepeat);
				break;
			case 8:
				sprintf_s(charBuffer3,2048-1,"start %s C:\\sox-14-3-2\\sox %s \"%s\" -d delay 1.5 0.5 repeat %d",charBuffer5,charBuffer6,charBuffer2,numRepeat);
				break;
		}
		system(charBuffer3);

	#ifdef _DEBUG
		printf("file for segment id %d has been created\n", idSeg);
	#endif //_DEBUG
	}




#ifdef _DEBUG //debug: playback the loaded sample file (as is)
	if(0)
	{
		////////////////////////////////////
		// play loaded data using port audio 
		////////////////////////////////////
		err = Pa_Initialize();
		if( err != paNoError ) goto error;

		outputParameters.device = Pa_GetDefaultOutputDevice(); // default output device 
		if (outputParameters.device == paNoDevice) 
		{
			fprintf(stderr,"Error: No default output device.\n");
			goto error;
		}
		outputParameters.channelCount = pWavSet->numChannels;
		outputParameters.sampleFormat =  PA_SAMPLE_TYPE;
		outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
		outputParameters.hostApiSpecificStreamInfo = NULL;

		printf("Begin playback.\n"); fflush(stdout);
		err = Pa_OpenStream(
					&stream,
					NULL, // no input
					&outputParameters,
					pWavSet->SampleRate,
					BUFF_SIZE/pWavSet->numChannels, //FRAMES_PER_BUFFER,
					paClipOff,      // we won't output out of range samples so don't bother clipping them 
					NULL, // no callback, use blocking API 
					NULL ); // no callback, so no callback userData 
		if( err != paNoError ) goto error;

		if( stream )
		{
			err = Pa_StartStream( stream );
			if( err != paNoError ) goto error;
			printf("Waiting for playback to finish.\n"); fflush(stdout);

			err = Pa_WriteStream( stream, pWavSet->pSamples, pWavSet->totalFrames );
			if( err != paNoError ) goto error;

			err = Pa_CloseStream( stream );
			if( err != paNoError ) goto error;
			printf("Done.\n"); fflush(stdout);
		}
	}
	Pa_Terminate();
#endif //_DEBUG

	/////////////////////////////////
	//wait for sox playback to finish
	/////////////////////////////////
	Sleep(numSecondsPlay*1000);

	//////////////////////////////
	//taskkill all running sox.exe
	//////////////////////////////
	sprintf_s(charBuffer3,2048-1,"g:\\windows\\system32\\taskkill /f /im sox.exe");
	system(charBuffer3);
	Sleep(1*1000);
	for(int idSeg=0; idSeg<pWavSet->numSegments; idSeg++)
	{
		/////////////////////////////////////////////
		//delete previously created segments wav file
		/////////////////////////////////////////////
		sprintf_s(charBuffer2,2048-1,"%s_%d.wav",charBuffer4,idSeg);
		sprintf_s(charBuffer3,2048-1,"del \"%s\"",charBuffer2);
		system(charBuffer3);
	}

	if(pWavSet) delete pWavSet;
	printf("Exiting!\n"); fflush(stdout);

	nShowCmd = false;
	ShellExecuteA(NULL, "open", "end.bat", "", NULL, nShowCmd);
	return 0;
	


error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return -1;

}

