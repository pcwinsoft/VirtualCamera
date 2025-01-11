//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1999  Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

#ifndef __CAPSTRM_H__
#define __CAPSTRM_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


KSPIN_MEDIUM StandardMedium = {
    STATIC_KSMEDIUMSETID_Standard,
    0, 0
};

// ------------------------------------------------------------------------
// Property sets for all video capture streams
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_TABLE(VideoStreamConnectionProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_CONNECTION_ALLOCATORFRAMING,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY),                     // MinProperty
        sizeof(KSALLOCATOR_FRAMING),            // MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
};

DEFINE_KSPROPERTY_TABLE(VideoStreamDroppedFramesProperties)
{
    DEFINE_KSPROPERTY_ITEM
    (
        KSPROPERTY_DROPPEDFRAMES_CURRENT,
        TRUE,                                   // GetSupported or Handler
        sizeof(KSPROPERTY_DROPPEDFRAMES_CURRENT_S),// MinProperty
        sizeof(KSPROPERTY_DROPPEDFRAMES_CURRENT_S),// MinData
        FALSE,                                  // SetSupported or Handler
        NULL,                                   // Values
        0,                                      // RelationsCount
        NULL,                                   // Relations
        NULL,                                   // SupportHandler
        0                                       // SerializedSize
    ),
};


// ------------------------------------------------------------------------
// Array of all of the property sets supported by video streams
// ------------------------------------------------------------------------

DEFINE_KSPROPERTY_SET_TABLE(VideoStreamProperties)
{
    DEFINE_KSPROPERTY_SET
    ( 
        &KSPROPSETID_Connection,                        // Set
        SIZEOF_ARRAY(VideoStreamConnectionProperties),  // PropertiesCount
        VideoStreamConnectionProperties,                // PropertyItem
        0,                                              // FastIoCount
        NULL                                            // FastIoTable
    ),
    DEFINE_KSPROPERTY_SET
    ( 
        &PROPSETID_VIDCAP_DROPPEDFRAMES,                // Set
        SIZEOF_ARRAY(VideoStreamDroppedFramesProperties),  // PropertiesCount
        VideoStreamDroppedFramesProperties,             // PropertyItem
        0,                                              // FastIoCount
        NULL                                            // FastIoTable
    ),
};

#define NUMBER_VIDEO_STREAM_PROPERTIES (SIZEOF_ARRAY(VideoStreamProperties))

//---------------------------------------------------------------------------
// All of the video and vbi data formats we might use
//---------------------------------------------------------------------------

#define MAX_FRAME_RATE	333333 //30 FRAMES PER SECOND => 1/60 * 10000000
//#define MAX_FRAME_RATE	333667 //30 FRAMES PER SECOND => 1/60 * 10000000

#define FRAME_FACTOR	30


	#define D_X 320
	#define D_Y 240

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_NTSC_M |
			KS_AnalogVideo_PAL_B,                    // AnalogVideoStandard
			720,480,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			160,120,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			720,480,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			8,              // CropGranularityX, granularity of cropping size
			1,              // CropGranularityY
			8,              // CropAlignX, alignment of cropping rect 
			1,              // CropAlignY;
			160, 120,       // MinOutputSize, smallest bitmap stream can produce
			720, 480,       // MaxOutputSize, largest  bitmap stream can produce
			8,              // OutputGranularityX, granularity of output bitmap size
			1,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			333667,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * 30 * 160 * 120,  // MinBitsPerSecond;
			8 * 3 * 30 * 720 * 480   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * 30,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			333667,                             // REFERENCE_TIME  AvgTimePerFrame;   

			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y


	#define D_X 640
	#define D_Y 480

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture640x480All = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			720,480,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			160,120,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			720,480,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			80,              // CropGranularityX, granularity of cropping size
			120,              // CropGranularityY
			80,              // CropAlignX, alignment of cropping rect 
			120,              // CropAlignY;
			160, 120,       // MinOutputSize, smallest bitmap stream can produce
			720, 480,       // MaxOutputSize, largest  bitmap stream can produce
			80,              // OutputGranularityX, granularity of output bitmap size
			120,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * 160 * 120,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * 720 * 480   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y



	#define D_X 800
	#define D_Y 600

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture800x600 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y







	#define D_X 1024
	#define D_Y 768

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1024x768 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y





	#define D_X 1088
	#define D_Y 612

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1088x612 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1176
	#define D_Y 664

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1176x664 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y



	#define D_X 1152
	#define D_Y 864

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1152x864 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1280
	#define D_Y 600

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1280x600 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1280
	#define D_Y 720

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1280x720 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1280
	#define D_Y 768

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1280x768 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y



	#define D_X 1280
	#define D_Y 800

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1280x800 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1280
	#define D_Y 854

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1280x854 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y



	#define D_X 1280
	#define D_Y 960

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1280x960 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y



	#define D_X 1280
	#define D_Y 1024

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1280x1024 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1360
	#define D_Y 768

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1360x768 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y



	#define D_X 1366
	#define D_Y 768

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1366x768 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1400
	#define D_Y 1050

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1400x1050 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1440
	#define D_Y 900

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1440x900 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1440
	#define D_Y 960

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1440x960 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y



	#define D_X 1600
	#define D_Y 900

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1600x900 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1600
	#define D_Y 1024

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1600x1024 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1600
	#define D_Y 1200

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1600x1200 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y





	#define D_X 1680
	#define D_Y 1024

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1680x1024 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1768
	#define D_Y 992

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1768x992 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y



	#define D_X 1680
	#define D_Y 1050

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1680x1050 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y




	#define D_X 1920
	#define D_Y 1080

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1920x1080 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y



	#define D_X 1920
	#define D_Y 1200

	static  KS_DATARANGE_VIDEO StreamFormatRGB24Bpp_Capture1920x1200 = 
	{
		// KSDATARANGE
		{   
			sizeof (KS_DATARANGE_VIDEO),            // FormatSize
			0,                                      // Flags
			D_X * D_Y * 3,                          // SampleSize
			0,                                      // Reserved

			STATIC_KSDATAFORMAT_TYPE_VIDEO,         // aka. MEDIATYPE_Video
			0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70, //MEDIASUBTYPE_RGB24,
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO // aka. FORMAT_VideoInfo
		},

		TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
		TRUE,               // BOOL,  bTemporalCompression (all I frames?)
		0,                  // Reserved (was StreamDescriptionFlags)
		0,                  // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

		// _KS_VIDEO_STREAM_CONFIG_CAPS  
		{
			STATIC_KSDATAFORMAT_SPECIFIER_VIDEOINFO, // GUID
			KS_AnalogVideo_None,
			D_X,D_Y,        // InputSize, (the inherent size of the incoming signal
						//             with every digitized pixel unique)
			D_X,D_Y,        // MinCroppingSize, smallest rcSrc cropping rect allowed
			D_X,D_Y,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
			0,              // CropGranularityX, granularity of cropping size
			0,              // CropGranularityY
			0,              // CropAlignX, alignment of cropping rect 
			0,              // CropAlignY;
			D_X, D_Y,       // MinOutputSize, smallest bitmap stream can produce
			D_X, D_Y,       // MaxOutputSize, largest  bitmap stream can produce
			0,              // OutputGranularityX, granularity of output bitmap size
			0,              // OutputGranularityY;
			0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
			0,              // StretchTapsY
			0,              // ShrinkTapsX 
			0,              // ShrinkTapsY 
			MAX_FRAME_RATE,         // MinFrameInterval, 100 nS units
			640000000,      // MaxFrameInterval, 100 nS units
			8 * 3 * FRAME_FACTOR * D_X * D_Y,  // MinBitsPerSecond;
			8 * 3 * FRAME_FACTOR * D_X * D_Y   // MaxBitsPerSecond;
		}, 
        
		// KS_VIDEOINFOHEADER (default format)
		{
			0,0,0,0,                            // RECT  rcSource; 
			0,0,0,0,                            // RECT  rcTarget; 
			D_X * D_Y * 3 * FRAME_FACTOR,                 // DWORD dwBitRate;
			0L,                                 // DWORD dwBitErrorRate; 
			MAX_FRAME_RATE,                             // REFERENCE_TIME  AvgTimePerFrame;   
			sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
			D_X,                                // LONG  biWidth;
			D_Y,                                // LONG  biHeight;
			1,                                  // WORD  biPlanes;
			24,                                 // WORD  biBitCount;
			KS_BI_RGB,                          // DWORD biCompression;
			D_X * D_Y * 3,                      // DWORD biSizeImage;
			0,                                  // LONG  biXPelsPerMeter;
			0,                                  // LONG  biYPelsPerMeter;
			0,                                  // DWORD biClrUsed;
			0                                   // DWORD biClrImportant;
		}
	}; 

	#undef D_X
	#undef D_Y


#define VBIStride (768*2)
#define VBISamples (768*2)
#define VBIStart   10
#define VBIEnd     21
#define VBILines (((VBIEnd)-(VBIStart))+1)
KS_DATARANGE_VIDEO_VBI StreamFormatVBI =
{
   // KSDATARANGE
   {
      {
         sizeof( KS_DATARANGE_VIDEO_VBI ),
         0,
         VBIStride * VBILines,      // SampleSize
         0,                          // Reserved
         { STATIC_KSDATAFORMAT_TYPE_VBI },
         { STATIC_KSDATAFORMAT_SUBTYPE_RAW8 },
         { STATIC_KSDATAFORMAT_SPECIFIER_VBI }
      }
   },
   TRUE,    // BOOL,  bFixedSizeSamples (all samples same size?)
   TRUE,    // BOOL,  bTemporalCompression (all I frames?)

   0,       // Reserved (was StreamDescriptionFlags)
   0,       // Reserved (was MemoryAllocationFlags   (KS_VIDEO_ALLOC_*))

   // _KS_VIDEO_STREAM_CONFIG_CAPS
   {
      { STATIC_KSDATAFORMAT_SPECIFIER_VBI },
      KS_AnalogVideo_NTSC_M,                       // AnalogVideoStandard
      {
         VBIStride, 480 /*VBILines*/   // SIZE InputSize
      },
      {
         VBISamples, VBILines   // SIZE MinCroppingSize;       smallest rcSrc cropping rect allowed
      },
      {
         VBIStride, VBILines   // SIZE MaxCroppingSize;       largest rcSrc cropping rect allowed
      },
      1,           // int CropGranularityX;       // granularity of cropping size
      1,           // int CropGranularityY;
      1,           // int CropAlignX;             // alignment of cropping rect
      1,           // int CropAlignY;
      {
         VBISamples, VBILines   // SIZE MinOutputSize;         // smallest bitmap stream can produce
      },
      {
         VBIStride, VBILines   // SIZE MaxOutputSize;         // largest  bitmap stream can produce
      },
      1,          // int OutputGranularityX;     // granularity of output bitmap size
      2,          // int OutputGranularityY;
      0,          // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
      0,          // StretchTapsY
      0,          // ShrinkTapsX
      0,          // ShrinkTapsY
      166834,     // LONGLONG MinFrameInterval;  // 100 nS units
      166834,     // LONGLONG MaxFrameInterval;  // 16683.4uS == 1/60 sec
      VBIStride * VBILines * 8 * 30 * 2, // LONG MinBitsPerSecond;
      VBIStride * VBILines * 8 * 30 * 2  // LONG MaxBitsPerSecond;
   },

   // KS_VBIINFOHEADER (default format)
   {
      VBIStart,      // StartLine  -- inclusive
      VBIEnd,        // EndLine    -- inclusive
      KS_VBISAMPLINGRATE_5X_NABTS,   // SamplingFrequency;   Hz.
      732,           // MinLineStartTime;
      732,           // MaxLineStartTime;
      732,           // ActualLineStartTime
      0,             // ActualLineEndTime;
      KS_AnalogVideo_NTSC_M,      // VideoStandard;
      VBISamples,       // SamplesPerLine;
      VBIStride,       // StrideInBytes;
      VBIStride * VBILines   // BufferSize;
   }
};

// output is NABTS records
KSDATARANGE StreamFormatNABTS =
{
    sizeof (KSDATARANGE),
    0,
    sizeof (NABTS_BUFFER),
    0,                  // Reserved
    { STATIC_KSDATAFORMAT_TYPE_VBI },
    { STATIC_KSDATAFORMAT_SUBTYPE_NABTS },
    { STATIC_KSDATAFORMAT_SPECIFIER_NONE }
};

KSDATARANGE StreamFormatCC = 
{
    // Definition of the CC stream
    {   
        sizeof (KSDATARANGE),           // FormatSize
        0,                              // Flags
        sizeof (CC_HW_FIELD),           // SampleSize
        0,                              // Reserved
        { STATIC_KSDATAFORMAT_TYPE_VBI },
        { STATIC_KSDATAFORMAT_SUBTYPE_CC },
        { STATIC_KSDATAFORMAT_SPECIFIER_NONE }
    }
};


//---------------------------------------------------------------------------
//  STREAM_Capture Formats
//---------------------------------------------------------------------------

static  PKSDATAFORMAT StreamCaptureFormats[] = 
{
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture,

	// The last one on this last is set as the default resolution
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture640x480All,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture800x600,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1024x768,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1088x612,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1176x664,
	
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1152x864,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x600,
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x720,
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x768,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x800,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x854,
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x960,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x1024,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1360x768,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1366x768,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1400x1050,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1440x900,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1440x960,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1600x900,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1600x1024,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1600x1200,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1680x1050,


	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1680x1024,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1768x992,


	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1920x1080,

	//StreamFormatRGB24Bpp_Capture2040x1080
	
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1920x1200,

	// The last one on this last is set as the default resolution
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture640x480All,

};
#define NUM_STREAM_CAPTURE_FORMATS (SIZEOF_ARRAY(StreamCaptureFormats))

//---------------------------------------------------------------------------
//  STREAM_Preview Formats
//---------------------------------------------------------------------------

static  PKSDATAFORMAT StreamPreviewFormats[] = 
{
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture,

	// The last one on this last is set as the default resolution
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture640x480All,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture800x600,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1024x768,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1088x612,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1176x664,
	
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1152x864,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x600,
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x720,
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x768,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x800,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x854,
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x960,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1280x1024,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1360x768,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1366x768,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1400x1050,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1440x900,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1440x960,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1600x900,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1600x1024,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1600x1200,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1680x1050,


	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1680x1024,

	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1768x992,


	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1920x1080,

	//StreamFormatRGB24Bpp_Capture2040x1080
	
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture1920x1200,

	// The last one on this last is set as the default resolution
	(PKSDATAFORMAT) &StreamFormatRGB24Bpp_Capture640x480All,
};
#define NUM_STREAM_PREVIEW_FORMATS (SIZEOF_ARRAY (StreamPreviewFormats))

//---------------------------------------------------------------------------
//  STREAM_VBI Formats
//---------------------------------------------------------------------------

static PKSDATAFORMAT StreamVBIFormats[] = 
{
    (PKSDATAFORMAT) &StreamFormatVBI,
};
#define NUM_STREAM_VBI_FORMATS (SIZEOF_ARRAY(StreamVBIFormats))

//---------------------------------------------------------------------------
//  STREAM_NABTS Formats
//---------------------------------------------------------------------------

static PKSDATAFORMAT StreamNABTSFormats[] = 
{
    (PKSDATAFORMAT) &StreamFormatNABTS,
};
#define NUM_STREAM_NABTS_FORMATS (SIZEOF_ARRAY(StreamNABTSFormats))

static PKSDATAFORMAT StreamCCFormats[] = 
{
    (PKSDATAFORMAT) &StreamFormatCC,
};
#define NUM_STREAM_CC_FORMATS (SIZEOF_ARRAY (StreamCCFormats))

//---------------------------------------------------------------------------
// Create an array that holds the list of all of the streams supported
//---------------------------------------------------------------------------

typedef struct _ALL_STREAM_INFO {
    HW_STREAM_INFORMATION   hwStreamInfo;
    HW_STREAM_OBJECT        hwStreamObject;
} ALL_STREAM_INFO, *PALL_STREAM_INFO;

// Warning:  The StreamNumber element of the HW_STREAM_OBJECT below MUST be
//           the same as its position in the Streams[] array.
static  ALL_STREAM_INFO Streams [] = 
{
  // -----------------------------------------------------------------
  // STREAM_Capture
  // -----------------------------------------------------------------
  {
    // HW_STREAM_INFORMATION -------------------------------------------
    {
    MAX_SCRCAM_STREAMS,                     // NumberOfPossibleInstances
    KSPIN_DATAFLOW_OUT,                     // DataFlow
    TRUE,                                   // DataAccessible
    NUM_STREAM_CAPTURE_FORMATS,             // NumberOfFormatArrayEntries
    StreamCaptureFormats,                   // StreamFormatsArray
    0,                                      // ClassReserved[0]
    0,                                      // ClassReserved[1]
    0,                                      // ClassReserved[2]
    0,                                      // ClassReserved[3]
    NUMBER_VIDEO_STREAM_PROPERTIES,         // NumStreamPropArrayEntries
    (PKSPROPERTY_SET) VideoStreamProperties,// StreamPropertiesArray
    0,                                      // NumStreamEventArrayEntries;
    0,                                      // StreamEventsArray;
    (GUID *) &PINNAME_VIDEO_CAPTURE,        // Category
    (GUID *) &PINNAME_VIDEO_CAPTURE,        // Name
    1,                                      // MediumsCount
    &StandardMedium,                        // Mediums
    FALSE,                                  // BridgeStream
    },
           
    // HW_STREAM_OBJECT ------------------------------------------------
    {
    sizeof (HW_STREAM_OBJECT),              // SizeOfThisPacket
    STREAM_Capture,                         // StreamNumber
    0,                                      // HwStreamExtension
    VideoReceiveDataPacket,                 // HwReceiveDataPacket
    VideoReceiveCtrlPacket,                 // HwReceiveControlPacket
    { NULL, 0 },                            // HW_CLOCK_OBJECT
    FALSE,                                  // Dma
    TRUE,                                   // Pio
    NULL,                                   // HwDeviceExtension
    sizeof (KS_FRAME_INFO),                 // StreamHeaderMediaSpecific
    0,                                      // StreamHeaderWorkspace 
    FALSE,                                  // Allocator 
    NULL,                                   // HwEventRoutine
    { 0, 0 },                               // Reserved[2]
    },            
 },
};

#define DRIVER_STREAM_COUNT (SIZEOF_ARRAY (Streams))


//---------------------------------------------------------------------------
// Topology
//---------------------------------------------------------------------------

// Categories define what the device does.

static const GUID Categories[] = {
    STATIC_KSCATEGORY_VIDEO,
    STATIC_KSCATEGORY_CAPTURE,
};

#define NUMBER_OF_CATEGORIES  SIZEOF_ARRAY (Categories)


static KSTOPOLOGY Topology = {
    NUMBER_OF_CATEGORIES,               // CategoriesCount
    (GUID*) &Categories,                // Categories
    0,                                  // TopologyNodesCount
    NULL,                               // TopologyNodes
    0,                                  // TopologyConnectionsCount
    NULL,                               // TopologyConnections
    NULL,                               // TopologyNodesNames
    0,                                  // Reserved
};


//---------------------------------------------------------------------------
// The Main stream header
//---------------------------------------------------------------------------

static HW_STREAM_HEADER StreamHeader = 
{
    DRIVER_STREAM_COUNT,                // NumberOfStreams
    sizeof (HW_STREAM_INFORMATION),     // Future proofing
    0,                                  // NumDevPropArrayEntries set at init time
    NULL,                               // DevicePropertiesArray  set at init time
    0,                                  // NumDevEventArrayEntries;
    NULL,                               // DeviceEventsArray;
    &Topology                           // Pointer to Device Topology
};

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // __CAPSTRM_H__

