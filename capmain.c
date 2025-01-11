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

#include "strmini.h"
#include "ksmedia.h"
#include "capmain.h"
#include "mediums.h"
#include "capstrm.h"
#include "capprop.h"
#include "capdebug.h"

// The only global used by this driver.  It is used to keep track of the instance count of
// the number of times the driver is loaded.  This is used to create unique Mediums so that
// the correct capture, crossbar, tuner, and tvaudio filters all get connected together.

UINT GlobalDriverMediumInstanceCount = 0;

// Debug Logging
// 0 = Errors only
// 1 = Info, stream state changes, stream open close
// 2 = Verbose trace
ULONG gDebugLevel = 0;

/*
** DriverEntry() - AGFX
**
**   This routine is called when the driver is first loaded by PnP.
**   It in turn, calls upon the stream class to perform registration services.
**
** Arguments:
**
**   DriverObject -
**          Driver object for this driver
**
**   RegistryPath -
**          Registry path string for this driver's key
**
** Returns:
**
**   Results of StreamClassRegisterAdapter()
**
** Side Effects:  none
*/

ULONG
DriverEntry (
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    HW_INITIALIZATION_DATA      HwInitData;
    ULONG                       ReturnValue;

    RtlZeroMemory(&HwInitData, sizeof(HwInitData));

    HwInitData.HwInitializationDataSize = sizeof(HwInitData);

    //
    // Set the Adapter entry points for the driver
    //

    HwInitData.HwInterrupt              = NULL; // HwInterrupt;

    HwInitData.HwReceivePacket          = AdapterReceivePacket;
    HwInitData.HwCancelPacket           = AdapterCancelPacket;
    HwInitData.HwRequestTimeoutHandler  = AdapterTimeoutPacket;

    HwInitData.DeviceExtensionSize      = sizeof(HW_DEVICE_EXTENSION);
    HwInitData.PerRequestExtensionSize  = sizeof(SRB_EXTENSION);
    HwInitData.FilterInstanceExtensionSize = 0;
    HwInitData.PerStreamExtensionSize   = sizeof(STREAMEX);
    HwInitData.BusMasterDMA             = FALSE;
    HwInitData.Dma24BitAddresses        = FALSE;
    HwInitData.BufferAlignment          = 3;
    HwInitData.DmaBufferSize            = 0;

    // Don't rely on the stream class using raised IRQL to synchronize
    // execution.  This single paramter most affects the overall structure
    // of the driver.

    HwInitData.TurnOffSynchronization   = TRUE;

    ReturnValue = StreamClassRegisterAdapter(DriverObject, RegistryPath, &HwInitData);

    return ReturnValue;
}

//==========================================================================;
//                   Adapter Based Request Handling Routines
//==========================================================================;

/*
** HwInitialize() - AGFX
**
**   This routine is called when an SRB_INITIALIZE_DEVICE request is received
**
** Arguments:
**
**   pSrb - pointer to stream request block for the Initialize command
**
** Returns:
**
** Side Effects:  none
*/

BOOLEAN
STREAMAPI
HwInitialize (
    IN OUT PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    STREAM_PHYSICAL_ADDRESS     adr;
    ULONG                       Size;
    PUCHAR                      pDmaBuf;
    int                         j;
	int							ix;

    PPORT_CONFIGURATION_INFORMATION ConfigInfo = pSrb->CommandData.ConfigInfo;

    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)ConfigInfo->HwDeviceExtension;

    if (ConfigInfo->NumberOfAccessRanges != 0) 
	{
        pSrb->Status = STATUS_NO_SUCH_DEVICE;
        return (FALSE);
    }

	pHwDevExt->m_tcustom = 30;
	pHwDevExt->m_bheader = FALSE;

	for ( ix = 0; ix < TOTAL_SUPPORTED_RESOLUTIONS; ix ++ )
	{
		pHwDevExt->iClientsConnectedByResolution[ ix ] = 0;
		pHwDevExt->iIndexOfResolutionInArrayOfVideoBuffers[ ix ] = 0;
	}

	pHwDevExt->m_SupportedResolutions[ DSR_ZERO ].m_width = 99;
	pHwDevExt->m_SupportedResolutions[ DSR_ZERO ].m_heigth = 99;

	pHwDevExt->m_SupportedResolutions[ DSR_160x120 ].m_width = 160;
	pHwDevExt->m_SupportedResolutions[ DSR_160x120 ].m_heigth = 120;

	pHwDevExt->m_SupportedResolutions[ DSR_320x240 ].m_width = 320;
	pHwDevExt->m_SupportedResolutions[ DSR_320x240 ].m_heigth = 240;

	pHwDevExt->m_SupportedResolutions[ DSR_640x240 ].m_width = 640;
	pHwDevExt->m_SupportedResolutions[ DSR_640x240 ].m_heigth = 240;

	pHwDevExt->m_SupportedResolutions[ DSR_640x480 ].m_width = 640;
	pHwDevExt->m_SupportedResolutions[ DSR_640x480 ].m_heigth = 480;

	pHwDevExt->m_SupportedResolutions[ DSR_720x240 ].m_width = 720;
	pHwDevExt->m_SupportedResolutions[ DSR_720x240 ].m_heigth = 240;

	pHwDevExt->m_SupportedResolutions[ DSR_720x480 ].m_width = 720;
	pHwDevExt->m_SupportedResolutions[ DSR_720x480 ].m_heigth = 480;

	pHwDevExt->m_SupportedResolutions[ DSR_800x600 ].m_width = 800;
	pHwDevExt->m_SupportedResolutions[ DSR_800x600 ].m_heigth = 600;

	pHwDevExt->m_SupportedResolutions[ DSR_1024x768 ].m_width = 1024;
	pHwDevExt->m_SupportedResolutions[ DSR_1024x768 ].m_heigth = 768;

	pHwDevExt->m_SupportedResolutions[ DSR_1088x612 ].m_width = 1088;
	pHwDevExt->m_SupportedResolutions[ DSR_1088x612 ].m_heigth = 612;

	pHwDevExt->m_SupportedResolutions[ DSR_1176x664 ].m_width = 1176;
	pHwDevExt->m_SupportedResolutions[ DSR_1176x664 ].m_heigth = 664;

	pHwDevExt->m_SupportedResolutions[ DSR_1152x864 ].m_width = 1152;
	pHwDevExt->m_SupportedResolutions[ DSR_1152x864 ].m_heigth = 864;

	pHwDevExt->m_SupportedResolutions[ DSR_1280x600 ].m_width = 1280;
	pHwDevExt->m_SupportedResolutions[ DSR_1280x600 ].m_heigth = 600;

	pHwDevExt->m_SupportedResolutions[ DSR_1280x720 ].m_width = 1280;
	pHwDevExt->m_SupportedResolutions[ DSR_1280x720 ].m_heigth = 720;

	pHwDevExt->m_SupportedResolutions[ DSR_1280x768 ].m_width = 1280;
	pHwDevExt->m_SupportedResolutions[ DSR_1280x768 ].m_heigth = 768;

	pHwDevExt->m_SupportedResolutions[ DSR_1280x800 ].m_width = 1280;
	pHwDevExt->m_SupportedResolutions[ DSR_1280x800 ].m_heigth = 800;

	pHwDevExt->m_SupportedResolutions[ DSR_1280x854 ].m_width = 1280;
	pHwDevExt->m_SupportedResolutions[ DSR_1280x854 ].m_heigth = 854;

	pHwDevExt->m_SupportedResolutions[ DSR_1280x960 ].m_width = 1280;
	pHwDevExt->m_SupportedResolutions[ DSR_1280x960 ].m_heigth = 960;

	pHwDevExt->m_SupportedResolutions[ DSR_1280x1024 ].m_width = 1280;
	pHwDevExt->m_SupportedResolutions[ DSR_1280x1024 ].m_heigth = 1024;

	pHwDevExt->m_SupportedResolutions[ DSR_1360x768 ].m_width = 1360;
	pHwDevExt->m_SupportedResolutions[ DSR_1360x768 ].m_heigth = 768;

	pHwDevExt->m_SupportedResolutions[ DSR_1366x768 ].m_width = 1366;
	pHwDevExt->m_SupportedResolutions[ DSR_1366x768 ].m_heigth = 768;

	pHwDevExt->m_SupportedResolutions[ DSR_1400x1050 ].m_width = 1400;
	pHwDevExt->m_SupportedResolutions[ DSR_1400x1050 ].m_heigth = 1050;

	pHwDevExt->m_SupportedResolutions[ DSR_1440x900 ].m_width = 1440;
	pHwDevExt->m_SupportedResolutions[ DSR_1440x900 ].m_heigth = 900;

	pHwDevExt->m_SupportedResolutions[ DSR_1440x960 ].m_width = 1440;
	pHwDevExt->m_SupportedResolutions[ DSR_1440x960 ].m_heigth = 960;

	pHwDevExt->m_SupportedResolutions[ DSR_1600x900 ].m_width = 1600;
	pHwDevExt->m_SupportedResolutions[ DSR_1600x900 ].m_heigth = 900;

	pHwDevExt->m_SupportedResolutions[ DSR_1600x1200 ].m_width = 1600;
	pHwDevExt->m_SupportedResolutions[ DSR_1600x1200 ].m_heigth = 1200;

	pHwDevExt->m_SupportedResolutions[ DSR_1680x1024 ].m_width = 1680;
	pHwDevExt->m_SupportedResolutions[ DSR_1680x1024 ].m_heigth = 1024;

	pHwDevExt->m_SupportedResolutions[ DSR_1680x1050 ].m_width = 1680;
	pHwDevExt->m_SupportedResolutions[ DSR_1680x1050 ].m_heigth = 1050;

	pHwDevExt->m_SupportedResolutions[ DSR_1768x992 ].m_width = 1768;
	pHwDevExt->m_SupportedResolutions[ DSR_1768x992 ].m_heigth = 992;

	pHwDevExt->m_SupportedResolutions[ DSR_1920x1080 ].m_width = 1920;
	pHwDevExt->m_SupportedResolutions[ DSR_1920x1080 ].m_heigth = 1080;

	pHwDevExt->m_SupportedResolutions[ DSR_1920x1200 ].m_width = 1920;
	pHwDevExt->m_SupportedResolutions[ DSR_1920x1200 ].m_heigth = 1200;

	// Zerar o array de TOTAL_VIDEO_BUFFERS Buffers de V�deo que 
	// atendem todas as resolu��es suportadas
	// Funciona assim:
	// Cliente com resolu��o WxY pede conex�o
	// o array � varrido, se houver slot vazio, a resolu��o ocupa o slot e o aplicativo
	// se comunica com o driver nesse buffer
	// Ao sair, o array �varrido e ao achar a resolu�ao diminui o numero de clientes
	// conectados em 1, se o numero passar a ser 0 o slot � desocupado
	// e fica vago para o pr�ximo pedido de cliente

	for( ix=0; ix < TOTAL_VIDEO_BUFFERS; ix++ )
	{
		pHwDevExt->m_VideoStream[ix].m_streamWidth = 0;
		pHwDevExt->m_VideoStream[ix].m_streamHeigth = 0;
		pHwDevExt->m_VideoStream[ix].m_nClientsOnThisResolutionStream = 0;
	}

    if (ConfigInfo->NumberOfAccessRanges != 0) 
	{
        pHwDevExt->ioBaseLocal = (PULONG)(ULONG_PTR)   (ConfigInfo->AccessRanges[0].RangeStart.LowPart);
    }

    pHwDevExt->Irq  = (USHORT)(ConfigInfo->BusInterruptLevel);

    ConfigInfo->StreamDescriptorSize = sizeof (HW_STREAM_HEADER) + DRIVER_STREAM_COUNT * sizeof (HW_STREAM_INFORMATION);

    pDmaBuf = StreamClassGetDmaBuffer(pHwDevExt);

    adr = StreamClassGetPhysicalAddress(pHwDevExt, NULL, pDmaBuf, DmaBuffer, &Size);

    // Init Crossbar properties
    pHwDevExt->VideoInputConnected = 0;     // TvTuner video is the default
    pHwDevExt->AudioInputConnected = 5;     // TvTuner audio is the default

    // Init VideoProcAmp properties
    pHwDevExt->Brightness = BrightnessDefault;
    pHwDevExt->BrightnessFlags = KSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    pHwDevExt->Contrast = ContrastDefault;
    pHwDevExt->ContrastFlags = KSPROPERTY_VIDEOPROCAMP_FLAGS_AUTO;
    pHwDevExt->ColorEnable = ColorEnableDefault;
    pHwDevExt->ColorEnableFlags = KSPROPERTY_VIDEOPROCAMP_FLAGS_MANUAL;

    // Init CameraControl properties
    pHwDevExt->Focus = FocusDefault;
    pHwDevExt->FocusFlags = KSPROPERTY_CAMERACONTROL_FLAGS_AUTO;
    pHwDevExt->Zoom = ZoomDefault;
    pHwDevExt->ZoomFlags = KSPROPERTY_CAMERACONTROL_FLAGS_AUTO;

    // Init TvTuner properties
    pHwDevExt->TunerMode = KSPROPERTY_TUNER_MODE_TV;
    pHwDevExt->Channel = 4;
    pHwDevExt->TunerInput = 0;
    pHwDevExt->Busy = 0;

    // Init TvAudio properties
    pHwDevExt->TVAudioMode = KS_TVAUDIO_MODE_MONO | KS_TVAUDIO_MODE_LANG_A ;

    // Init AnalogVideoDecoder properties
    pHwDevExt->VideoDecoderVideoStandard = KS_AnalogVideo_None;
    pHwDevExt->VideoDecoderOutputEnable = FALSE;
    pHwDevExt->VideoDecoderVCRTiming = FALSE;

    // Init VideoControl properties
    pHwDevExt->VideoControlMode = 0;

    // Init VideoCompression properties
    pHwDevExt->CompressionSettings.CompressionKeyFrameRate = 15;
    pHwDevExt->CompressionSettings.CompressionPFramesPerKeyFrame = 0;
    pHwDevExt->CompressionSettings.CompressionQuality = 5000;

    pHwDevExt->PDO = ConfigInfo->RealPhysicalDeviceObject;
    //DbgLogInfo(("TestCap: Physical Device Object = %lx\n", pHwDevExt->PDO));

    for (j = 0; j < MAX_SCRCAM_STREAMS; j++)
	{
        // For each stream, maintain a separate queue for data and control
        InitializeListHead (&pHwDevExt->StreamSRBList[j]);
        InitializeListHead (&pHwDevExt->StreamControlSRBList[j]);
        KeInitializeSpinLock (&pHwDevExt->StreamSRBSpinLock[j]);
        pHwDevExt->StreamSRBListSize[j] = 0;
    }

    // Init ProtectionStatus
    pHwDevExt->ProtectionStatus = 0;

    // The following allows multiple instance of identical hardware
    // to be installed.  GlobalDriverMediumInstanceCount is set in the Medium.Id field.
    pHwDevExt->DriverMediumInstanceCount = GlobalDriverMediumInstanceCount++;
    
	AdapterSetInstance (pSrb);

    pSrb->Status = STATUS_SUCCESS;

    return (TRUE);
}

/*
** HwUnInitialize()
**
**   This routine is called when an SRB_UNINITIALIZE_DEVICE request is received
**
** Arguments:
**
**   pSrb - pointer to stream request block for the UnInitialize command
**
** Returns:
**
** Side Effects:  none
*/

BOOLEAN
STREAMAPI
HwUnInitialize (
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    pSrb->Status = STATUS_SUCCESS;

    return TRUE;
}

/*
** AdapterPowerState()
**
**   This routine is called when an SRB_CHANGE_POWER_STATE request is received
**
** Arguments:
**
**   pSrb - pointer to stream request block for the Change Power state command
**
** Returns:
**
** Side Effects:  none
*/

BOOLEAN
STREAMAPI
AdapterPowerState (
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);

    pHwDevExt->DeviceState = pSrb->CommandData.DeviceState;

    return TRUE;
}

/*
** AdapterSetInstance()
**
**   This routine is called to set all of the Medium instance fields
**
** Arguments:
**
**   pSrb - pointer to stream request block
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
AdapterSetInstance (
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    int j;
    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);

    // Use our HwDevExt as the instance data on the Mediums
    // This allows multiple instances to be uniquely identified and
    // connected.  The value used in .Id is not important, only that
    // it is unique for each hardware connection

    for (j = 0; j < SIZEOF_ARRAY (TVTunerMediums); j++) 
	{
        TVTunerMediums[j].Id = pHwDevExt->DriverMediumInstanceCount;
    }
    for (j = 0; j < SIZEOF_ARRAY (TVAudioMediums); j++) 
	{
        TVAudioMediums[j].Id = pHwDevExt->DriverMediumInstanceCount;
    }
    for (j = 0; j < SIZEOF_ARRAY (CrossbarMediums); j++) 
	{
        CrossbarMediums[j].Id = pHwDevExt->DriverMediumInstanceCount;
    }
    for (j = 0; j < SIZEOF_ARRAY (CaptureMediums); j++) 
	{
        CaptureMediums[j].Id = pHwDevExt->DriverMediumInstanceCount;
    }

    pHwDevExt->AnalogVideoInputMedium = CaptureMediums[2];
}

/*
** AdapterCompleteInitialization()
**
**   This routine is called when an SRB_COMPLETE_INITIALIZATION request is received
**
** Arguments:
**
**   pSrb - pointer to stream request block
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
AdapterCompleteInitialization (
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    NTSTATUS                Status;
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    KIRQL                   KIrql;

    KIrql = KeGetCurrentIrql();

    // Create the Registry blobs that DShow uses to create
    // graphs via Mediums

    // Register the TVTuner
    Status = StreamClassRegisterFilterWithNoKSPins (
                    pHwDevExt->PDO,                 // IN PDEVICE_OBJECT   DeviceObject,
                    &KSCATEGORY_TVTUNER,            // IN GUID           * InterfaceClassGUID,
                    SIZEOF_ARRAY (TVTunerMediums),  // IN ULONG            PinCount,
                    TVTunerPinDirection,            // IN ULONG          * Flags,
                    TVTunerMediums,                 // IN KSPIN_MEDIUM   * MediumList,
                    NULL                            // IN GUID           * CategoryList
            );

    // Register the Crossbar
    Status = StreamClassRegisterFilterWithNoKSPins (
                    pHwDevExt->PDO,                 // IN PDEVICE_OBJECT   DeviceObject,
                    &KSCATEGORY_CROSSBAR,           // IN GUID           * InterfaceClassGUID,
                    SIZEOF_ARRAY (CrossbarMediums), // IN ULONG            PinCount,
                    CrossbarPinDirection,           // IN ULONG          * Flags,
                    CrossbarMediums,                // IN KSPIN_MEDIUM   * MediumList,
                    NULL                            // IN GUID           * CategoryList
            );

    // Register the TVAudio decoder
    Status = StreamClassRegisterFilterWithNoKSPins (
                    pHwDevExt->PDO,                 // IN PDEVICE_OBJECT   DeviceObject,
                    &KSCATEGORY_TVAUDIO,            // IN GUID           * InterfaceClassGUID,
                    SIZEOF_ARRAY (TVAudioMediums),  // IN ULONG            PinCount,
                    TVAudioPinDirection,            // IN ULONG          * Flags,
                    TVAudioMediums,                 // IN KSPIN_MEDIUM   * MediumList,
                    NULL                            // IN GUID           * CategoryList
            );

    // Register the Capture filter
    // Note:  This should be done automatically be MSKsSrv.sys,
    // when that component comes on line (if ever) ...
    Status = StreamClassRegisterFilterWithNoKSPins (
                    pHwDevExt->PDO,                 // IN PDEVICE_OBJECT   DeviceObject,
                    &KSCATEGORY_CAPTURE,            // IN GUID           * InterfaceClassGUID,
                    SIZEOF_ARRAY (CaptureMediums),  // IN ULONG            PinCount,
                    CapturePinDirection,            // IN ULONG          * Flags,
                    CaptureMediums,                 // IN KSPIN_MEDIUM   * MediumList,
                    NULL                            // IN GUID           * CategoryList
            );
}


/*
** AdapterOpenStream() - AGFX
**
**   This routine is called when an OpenStream SRB request is received.
**   A stream is identified by a stream number, which indexes an array
**   of KSDATARANGE structures.  The particular KSDATAFORMAT format to
**   be used is also passed in, which should be verified for validity.
**
** Arguments:
**
**   pSrb - pointer to stream request block for the Open command
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
AdapterOpenStream (
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    //
    // the stream extension structure is allocated by the stream class driver
    //

	// this is the hardware extension for this one new stream and it comes to be filled
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension; 

    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    int                     StreamNumber = pSrb->StreamObject->StreamNumber;
    PKSDATAFORMAT           pKSDataFormat = pSrb->CommandData.OpenFormat;
	int						StreamAux;
	int						ix;
	BOOL					bfoundslot;
	BOOL					balreadyhasallocatedbuffer;
	int						indexvb = 0;

    RtlZeroMemory(pStrmEx, sizeof(STREAMEX));

    //
    // check that the stream index requested isn't too high
    // or that the maximum number of instances hasn't been exceeded
    //

    if (StreamNumber >= DRIVER_STREAM_COUNT || StreamNumber < 0) 
	{
        pSrb->Status = STATUS_INVALID_PARAMETER;

        return;
    }

    //
    // Check that we haven't exceeded the instance count for this stream
    //

    if (pHwDevExt->ActualInstances[StreamNumber] >= Streams[StreamNumber].hwStreamInfo.NumberOfPossibleInstances) 
	{
        pSrb->Status = STATUS_INVALID_PARAMETER;

        return;
    }

    //
    // Check the validity of the format being requested
    //

    if (!AdapterVerifyFormat (pKSDataFormat, StreamNumber)) 
	{
        pSrb->Status = STATUS_INVALID_PARAMETER;

        return;
    }
	else
	{
        if (IsEqualGUID (&pKSDataFormat->Specifier, &KSDATAFORMAT_SPECIFIER_VIDEOINFO)) 
		{
            PKS_DATAFORMAT_VIDEOINFOHEADER  pDataFormatVideoInfoHeader = (PKS_DATAFORMAT_VIDEOINFOHEADER) pKSDataFormat;
            PKS_VIDEOINFOHEADER pVideoInfoHdrToVerify = (PKS_VIDEOINFOHEADER) &pDataFormatVideoInfoHeader->VideoInfoHeader;

			UINT biWidth = pVideoInfoHdrToVerify->bmiHeader.biWidth;
			UINT biHeight = pVideoInfoHdrToVerify->bmiHeader.biHeight;


			// Funciona assim:
			// Cliente com resolu��o WxY pede conex�o
			// o array � varrido, se houver slot vazio, a resolu��o ocupa o slot e o aplicativo
			// se comunica com o driver nesse buffer
			// Ao sair, o array �varrido e ao achar a resolu�ao diminui o numero de clientes
			// conectados em 1, se o numero passar a ser 0 o slot � desocupado
			// e fica vago para o pr�ximo pedido de cliente

			// Pedido de conex�o de Cliente com resolu��o WxY 

			balreadyhasallocatedbuffer = FALSE;
			for( ix=0; ix < TOTAL_VIDEO_BUFFERS; ix++ )
			{
				if (
					( pHwDevExt->m_VideoStream[ ix ].m_streamWidth == biWidth )
					&&
					( pHwDevExt->m_VideoStream[ ix ].m_streamHeigth == biHeight )
					)
				{
					// j� tem buffer alocado para essa resolu��o
					balreadyhasallocatedbuffer = TRUE;
					pHwDevExt->m_VideoStream[ ix ].m_nClientsOnThisResolutionStream++;

					// IMPORTANTE: 
					// passa para o ScreenCamera o index de video buffer + 1
					// dessa forma diferencia de 0 Zero para ser tratado pelo ScreenCamera
					indexvb = ix + 1;

					break;
				}
			}

			if ( ! balreadyhasallocatedbuffer )
			{
				// Novo Cliente Resolu��o WxH, alocar um slot de mem�oria de video
				// para esse Cliente Resolu��o WxH
				bfoundslot = FALSE;
				for( ix=0; ix < TOTAL_VIDEO_BUFFERS; ix++ )
				{
					if (
						( pHwDevExt->m_VideoStream[ ix ].m_streamWidth == 0 )
						&&
						( pHwDevExt->m_VideoStream[ ix ].m_streamHeigth == 0 )
						)
					{
						// slot vago no array de buffers
						bfoundslot = TRUE;

						pHwDevExt->m_VideoStream[ ix ].m_streamWidth = biWidth;
						pHwDevExt->m_VideoStream[ ix ].m_streamHeigth = biHeight;
						pHwDevExt->m_VideoStream[ ix ].m_nClientsOnThisResolutionStream = 1;

						// IMPORTANTE: 
						// passa para o ScreenCamera o index de video buffer + 1
						// dessa forma diferencia de 0 Zero para ser tratado pelo ScreenCamera
						indexvb = ix + 1;

						break;
					}
				}

				// se nao tiver slot vazio entao recusa a resolu��o sendo pedida
				// veja porem que para isso TOAL conexos com pelomenos um cliente cada
				// deve estar usando o ScreenCamera portanto � muito improv�vel de acontecer
				if ( bfoundslot == FALSE )
				{
					pSrb->Status = STATUS_INVALID_PARAMETER;

					return;
				}
			}

			for ( ix = 0; ix < TOTAL_SUPPORTED_RESOLUTIONS; ix ++ )
			{
				if (
					( pHwDevExt->m_SupportedResolutions[ ix ].m_width == biWidth )
					&&
					( pHwDevExt->m_SupportedResolutions[ ix ].m_heigth == biHeight )
				   )
				{
					pHwDevExt->iClientsConnectedByResolution[ ix ]++;
					pHwDevExt->iIndexOfResolutionInArrayOfVideoBuffers[ ix ] = indexvb;
					break;
				}
			}
		}
	}

    //
    // And set the format for the stream
    //
    if (!VideoSetFormat (pSrb)) 
	{
        return;
    }

    // Set up pointers to the handlers for the stream data and control handlers

    pSrb->StreamObject->ReceiveDataPacket = (PVOID) Streams[StreamNumber].hwStreamObject.ReceiveDataPacket;
    pSrb->StreamObject->ReceiveControlPacket = (PVOID) Streams[StreamNumber].hwStreamObject.ReceiveControlPacket;

    //
    // The DMA flag must be set when the device will be performing DMA directly
    // to the data buffer addresses passed in to the ReceiceDataPacket routines.
    //

    pSrb->StreamObject->Dma = Streams[StreamNumber].hwStreamObject.Dma;

    //
    // The PIO flag must be set when the mini driver will be accessing the data
    // buffers passed in using logical addressing
    //

    pSrb->StreamObject->Pio = Streams[StreamNumber].hwStreamObject.Pio;

    //
    // How many extra bytes will be passed up from the driver for each frame?
    //

    pSrb->StreamObject->StreamHeaderMediaSpecific = Streams[StreamNumber].hwStreamObject.StreamHeaderMediaSpecific;

    pSrb->StreamObject->StreamHeaderWorkspace = Streams[StreamNumber].hwStreamObject.StreamHeaderWorkspace;

    //
    // Indicate the clock support available on this stream
    //

    pSrb->StreamObject->HwClockObject = Streams[StreamNumber].hwStreamObject.HwClockObject;

    //
    // GET THE STREAM ID - FROM THE MAX_SCRCAM_STREAMS FIND THE NEXT EMPTY ENTRY IN A CIRCULAR FASSION
    //
	StreamAux = pHwDevExt->AttStreamID[StreamNumber];

	while (pHwDevExt->pStrmEx [StreamNumber][StreamAux] != NULL)
	{
		StreamAux++;

		if (StreamAux > MAX_SCRCAM_STREAMS)
		{
			StreamAux = 0;
		}
	}

	pHwDevExt->AttStreamID[StreamNumber] = StreamAux;

	pStrmEx->StreamID = pHwDevExt->AttStreamID[StreamNumber];

    pHwDevExt->ActualInstances[StreamNumber]++;

    // Maintain an array of all the StreamEx structures in the HwDevExt
    // so that we can cancel IRPs from any stream
    ASSERT (pHwDevExt->pStrmEx [StreamNumber][pStrmEx->StreamID] == NULL);
    pHwDevExt->pStrmEx [StreamNumber][pStrmEx->StreamID] = (PSTREAMX) pStrmEx;

    // Retain a private copy of the HwDevExt and StreamObject in the stream extension
    // so we can use a timer

    pStrmEx->pHwDevExt = pHwDevExt;                     // For timer use
    pStrmEx->pStreamObject = pSrb->StreamObject;        // For timer use

    // Initialize the compression settings
    // These may have been changed from the default values in the HwDevExt
    // before the stream was opened
    pStrmEx->CompressionSettings.CompressionKeyFrameRate = pHwDevExt->CompressionSettings.CompressionKeyFrameRate;
    pStrmEx->CompressionSettings.CompressionPFramesPerKeyFrame = pHwDevExt->CompressionSettings.CompressionPFramesPerKeyFrame;
    pStrmEx->CompressionSettings.CompressionQuality = pHwDevExt->CompressionSettings.CompressionQuality;

    // Init VideoControl properties
    pStrmEx->VideoControlMode = pHwDevExt->VideoControlMode;

    // Init VBI variables
    pStrmEx->SentVBIInfoHeader = 0;
}

/*
** AdapterCloseStream() - AGFX
**
**   Close the requested data stream.
**
**   Note that a stream could be closed arbitrarily in the midst of streaming
**   if a user mode app crashes.  Therefore, you must release all outstanding
**   resources, disable interrupts, complete all pending SRBs, and put the
**   stream back into a quiescent condition.
**
** Arguments:
**
**   pSrb the request block requesting to close the stream
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
AdapterCloseStream (
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    //this is the unique stream
	PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;

    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    int                     StreamNumber = pSrb->StreamObject->StreamNumber;
    PKSDATAFORMAT           pKSDataFormat = pSrb->CommandData.OpenFormat;
    KS_VIDEOINFOHEADER      *pVideoInfoHdr = pStrmEx->pVideoInfoHeader;
    UINT biWidth;
    UINT biHeight;
	int ix;
	int indexvb = 0;

    if (pHwDevExt->StreamSRBListSize[pStrmEx->StreamID] > 0) 
	{
        VideoQueueCancelAllSRBs (pStrmEx);
    }

    pHwDevExt->ActualInstances[StreamNumber]--;

	ASSERT (pHwDevExt->pStrmEx [StreamNumber][pStrmEx->StreamID] != 0);

    pHwDevExt->pStrmEx [StreamNumber][pStrmEx->StreamID] = 0;

    //
    // the minidriver should free any resources that were allocate at
    // open stream time etc.
    //

    // Free the variable length VIDEOINFOHEADER
	if (pVideoInfoHdr) 
	{
		// verificar se � 320x240, 640x480., ... e diminuir no contador de formatos
		biWidth = pVideoInfoHdr->bmiHeader.biWidth;
		biHeight = pVideoInfoHdr->bmiHeader.biHeight;

		// Funciona assim:
		// Cliente com resolu��o WxY pede conex�o
		// o array � varrido, se houver slot vazio, a resolu��o ocupa o slot e o aplicativo
		// se comunica com o driver nesse buffer
		// Ao sair, o array �varrido e ao achar a resolu�ao diminui o numero de clientes
		// conectados em 1, se o numero passar a ser 0 o slot � desocupado
		// e fica vago para o pr�ximo pedido de cliente

		// Desconex�o de Cliente com resolu��o WxY

		for( ix=0; ix < TOTAL_VIDEO_BUFFERS; ix++ )
		{
			if (
				( pHwDevExt->m_VideoStream[ ix ].m_streamWidth == biWidth )
				&&
				( pHwDevExt->m_VideoStream[ ix ].m_streamHeigth == biHeight )
				)
			{
				// slot vago no array de buffers
				pHwDevExt->m_VideoStream[ ix ].m_nClientsOnThisResolutionStream--;

				if ( pHwDevExt->m_VideoStream[ ix ].m_nClientsOnThisResolutionStream == 0 )
				{
					// zera para a resolu��o por exemplo 1024x760
					// chegou na ultima zera os parametros sinalizando a vaga para outra stream
					// de outra resolu��o
					pHwDevExt->m_VideoStream[ ix ].m_streamWidth = 0;
					pHwDevExt->m_VideoStream[ ix ].m_streamHeigth = 0;
				}

				break;
			}
		}

		for ( ix = 0; ix < TOTAL_SUPPORTED_RESOLUTIONS; ix ++ )
		{
			if (
				( pHwDevExt->m_SupportedResolutions[ ix ].m_width == biWidth )
				&&
				( pHwDevExt->m_SupportedResolutions[ ix ].m_heigth == biHeight )
			   )
			{
				pHwDevExt->iClientsConnectedByResolution[ ix ]--;
				pHwDevExt->iIndexOfResolutionInArrayOfVideoBuffers[ ix ] = 0;
				break;
			}
		}

		ExFreePool(pVideoInfoHdr);
		pStrmEx->pVideoInfoHeader = NULL;
	}

	// Make sure we no longer reference the clock
	pStrmEx->hMasterClock = NULL;

	// Make sure the state is reset to stopped,
	pStrmEx->KSState = KSSTATE_STOP;
}


/*
** AdapterStreamInfo()
**
**   Returns the information of all streams that are supported by the
**   mini-driver
**
** Arguments:
**
**   pSrb - Pointer to the STREAM_REQUEST_BLOCK
**        pSrb->HwDeviceExtension - will be the hardware device extension for
**                                  as initialised in HwInitialise
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
AdapterStreamInfo (
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    int j;

    PHW_DEVICE_EXTENSION pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);

    //
    // pick up the pointer to header which preceeds the stream info structs
    //

    PHW_STREAM_HEADER pstrhdr = (PHW_STREAM_HEADER)&(pSrb->CommandData.StreamBuffer->StreamHeader);

     //
     // pick up the pointer to the array of stream information data structures
     //

    PHW_STREAM_INFORMATION pstrinfo = (PHW_STREAM_INFORMATION)&(pSrb->CommandData.StreamBuffer->StreamInfo);


    //
    // verify that the buffer is large enough to hold our return data
    //
    DEBUG_ASSERT (pSrb->NumberOfBytesToTransfer >= sizeof (HW_STREAM_HEADER) + sizeof (HW_STREAM_INFORMATION) * DRIVER_STREAM_COUNT);

    // Ugliness.  To allow mulitple instances, modify the pointer to the
    // AnalogVideoMedium and save it in our device extension

	Streams[STREAM_AnalogVideoInput].hwStreamInfo.Mediums = &pHwDevExt->AnalogVideoInputMedium;

	//
     // Set the header
     //

     StreamHeader.NumDevPropArrayEntries = NUMBER_OF_ADAPTER_PROPERTY_SETS;
     StreamHeader.DevicePropertiesArray = (PKSPROPERTY_SET) AdapterPropertyTable;
     *pstrhdr = StreamHeader;

     //
     // stuff the contents of each HW_STREAM_INFORMATION struct
     //

     for (j = 0; j < DRIVER_STREAM_COUNT; j++) 
	 {
        *pstrinfo++ = Streams[j].hwStreamInfo;
     }
}


/*
** AdapterReceivePacket() - AGFX
**
**   Main entry point for receiving adapter based request SRBs.  This routine
**   will always be called at Passive level.
**
**   Note: This is an asyncronous entry point.  The request does not necessarily
**         complete on return from this function, the request only completes when a
**         StreamClassDeviceNotification on this request block, of type
**         DeviceRequestComplete, is issued.
**
** Arguments:
**
**   pSrb - Pointer to the STREAM_REQUEST_BLOCK
**        pSrb->HwDeviceExtension - will be the hardware device extension for
**                                  as initialised in HwInitialise
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
AdapterReceivePacket(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    BOOL                    Busy;

    DEBUG_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    // The very first time through, we need to initialize the adapter spinlock
    // and queue
    if (!pHwDevExt->AdapterQueueInitialized) 
	{
        InitializeListHead (&pHwDevExt->AdapterSRBList);
        KeInitializeSpinLock (&pHwDevExt->AdapterSpinLock);
        pHwDevExt->AdapterQueueInitialized = TRUE;
        pHwDevExt->ProcessingAdapterSRB = FALSE;
    }

    //
    // If we're already processing an SRB, add it to the queue
    //
    Busy = AddToListIfBusy (pSrb, &pHwDevExt->AdapterSpinLock, &pHwDevExt->ProcessingAdapterSRB, &pHwDevExt->AdapterSRBList);

    if (Busy) {
        return;
    }

    //
    // This will run until the queue is empty
    //
    while (TRUE) {
        //
        // Assume success
        //
        pSrb->Status = STATUS_SUCCESS;

        //
        // determine the type of packet.
        //

        switch (pSrb->Command)
        {

        case SRB_INITIALIZE_DEVICE:

            // open the device

            HwInitialize(pSrb);

            break;

        case SRB_UNINITIALIZE_DEVICE:

            // close the device.

            HwUnInitialize(pSrb);

            break;

        case SRB_OPEN_STREAM:

            // open a stream

            AdapterOpenStream(pSrb);

            break;

        case SRB_CLOSE_STREAM:

            // close a stream

            AdapterCloseStream(pSrb);

            break;

        case SRB_GET_STREAM_INFO:

            //
            // return a block describing all the streams
            //

            AdapterStreamInfo(pSrb);

            break;

        case SRB_GET_DATA_INTERSECTION:

            //
            // Return a format, given a range
            //

            AdapterFormatFromRange(pSrb);

            break;

        case SRB_OPEN_DEVICE_INSTANCE:
        case SRB_CLOSE_DEVICE_INSTANCE:

            //
            // We should never get these since this is a single instance device
            //

            TRAP;
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;

        case SRB_GET_DEVICE_PROPERTY:

            //
            // Get adapter wide properties
            //

            AdapterGetProperty (pSrb);
            break;

        case SRB_SET_DEVICE_PROPERTY:

            //
            // Set adapter wide properties
            //

            AdapterSetProperty (pSrb);
            break;

        case SRB_PAGING_OUT_DRIVER:

            //
            // The driver is being paged out
            // Disable Interrupts if you have them!
            //
            break;

        case SRB_CHANGE_POWER_STATE:

            //
            // Changing the device power state, D0 ... D3
            //
            AdapterPowerState(pSrb);
            break;

        case SRB_INITIALIZATION_COMPLETE:

            //
            // Stream class has finished initialization.
            // Now create DShow Medium interface BLOBs.
            // This needs to be done at low priority since it uses the registry
            //

            AdapterCompleteInitialization (pSrb);
            break;


        case SRB_UNKNOWN_DEVICE_COMMAND:
        default:

            //
            // this is a request that we do not understand.  Indicate invalid
            // command and complete the request
            //
            pSrb->Status = STATUS_NOT_IMPLEMENTED;

        }

        //
        // Indicate back to the Stream Class that we're done with this SRB
        //
        CompleteDeviceSRB (pSrb);

        //
        // See if there's anything else on the queue
        //
        Busy = RemoveFromListIfAvailable (
                &pSrb,
                &pHwDevExt->AdapterSpinLock,
                &pHwDevExt->ProcessingAdapterSRB,
                &pHwDevExt->AdapterSRBList);

        if (!Busy) {
            break;
        }
    } // end of while there's anything in the queue
}

/*
** AdapterCancelPacket () - AGFX
**
**   Request to cancel a packet that is currently in process in the minidriver
**
** Arguments:
**
**   pSrb - pointer to request packet to cancel
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
AdapterCancelPacket(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION        pHwDevExt = (PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension;
    PSTREAMEX                   pStrmEx;
    int                         StreamNumber;
    int                         StreamIDn;
    BOOL                        Found = FALSE;

    //
    // Run through all the streams the driver has available
    //

    for (StreamNumber = 0; !Found && (StreamNumber < DRIVER_STREAM_COUNT); StreamNumber++) 
	{
	    for (StreamIDn = 0; !Found && (StreamIDn < MAX_SCRCAM_STREAMS); StreamIDn++) 
		{
			// GO OVER ALL OPENED STREAMS AND
			// Check to see if the stream is in use
			//
			if (pStrmEx = (PSTREAMEX) pHwDevExt->pStrmEx[StreamNumber][StreamIDn]) 
			{
				Found = VideoQueueCancelOneSRB (
					pStrmEx,
					pSrb
					);
			}

        } // if the stream is open
    } // for all streams
}

/*
** AdapterTimeoutPacket()
**
**   This routine is called when a packet has been in the minidriver for
**   too long.  The adapter must decide what to do with the packet
**
** Arguments:
**
**   pSrb - pointer to the request packet that timed out
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
AdapterTimeoutPacket(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    //
    // Unlike most devices, we need to hold onto data SRBs indefinitely,
    // since the graph could be in a pause state indefinitely
    //

    pSrb->TimeoutCounter = pSrb->TimeoutOriginal;
}

/*
** CompleteDeviceSRB ()
**
**   This routine is called when a packet is being completed.
**   The optional second notification type is used to indicate ReadyForNext
**
** Arguments:
**
**   pSrb - pointer to the request packet that timed out
**
** Returns:
**
** Side Effects:
**
*/

VOID
STREAMAPI
CompleteDeviceSRB (
     IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    StreamClassDeviceNotification( DeviceRequestComplete, pSrb->HwDeviceExtension, pSrb);
}

/*
** IsEqualOrWildGUID()
**
**   Compares two GUIDS like IsEqualGUID(), except allows wildcard matches
**
** Arguments:
**
**         IN GUID *g1
**         IN GUID *g2
**
** Returns:
**
**   TRUE if both GUIDs match or only one is a wildcard
**   FALSE if GUIDs are different or both are wildcards
**
** Side Effects:  none
*/

BOOL
STREAMAPI
IsEqualOrWildGUID(IN GUID *g1, IN GUID *g2)
{
    return (IsEqualGUID(g1, g2) && !IsEqualGUID(g1, &KSDATAFORMAT_TYPE_WILDCARD)
            || ((IsEqualGUID(g1, &KSDATAFORMAT_TYPE_WILDCARD)
                 || IsEqualGUID(g2, &KSDATAFORMAT_TYPE_WILDCARD))
                && !IsEqualGUID(g1, g2))
            );
}

/*
** AdapterCompareGUIDsAndFormatSize()
**
**   Checks for a match on the three GUIDs and FormatSize
**
** Arguments:
**
**         IN DataRange1
**         IN DataRange2
**         BOOL fCompareFormatSize - TRUE when comparing ranges
**                                 - FALSE when comparing formats
**
** Returns:
**
**   TRUE if all elements match
**   FALSE if any are different
**
** Side Effects:  none
*/

BOOL
STREAMAPI
AdapterCompareGUIDsAndFormatSize(
    IN PKSDATARANGE DataRange1,
    IN PKSDATARANGE DataRange2,
    BOOL fCompareFormatSize
    )
{
    return (
        IsEqualOrWildGUID (
            &DataRange1->MajorFormat,
            &DataRange2->MajorFormat) &&
        IsEqualOrWildGUID (
            &DataRange1->SubFormat,
            &DataRange2->SubFormat) &&
        IsEqualOrWildGUID (
            &DataRange1->Specifier,
            &DataRange2->Specifier) &&
        (fCompareFormatSize ?
                (DataRange1->FormatSize == DataRange2->FormatSize) : TRUE ));
}


/*
** AdapterVerifyFormat() - AGFX
**
**   Checks the validity of a format request by walking through the
**       array of supported KSDATA_RANGEs for a given stream.
**
** Arguments:
**
**   pKSDataFormat - pointer of a KSDATAFORMAT structure.
**   StreamNumber - index of the stream being queried / opened.
**
** Returns:
**
**   TRUE if the format is supported
**   FALSE if the format cannot be suppored
**
** Side Effects:  none
*/

BOOL
STREAMAPI
AdapterVerifyFormat(
    PKSDATAFORMAT pKSDataFormatToVerify,
    int StreamNumber
    )
{
    BOOL                        fOK = FALSE;
    ULONG                       j;
    ULONG                       NumberOfFormatArrayEntries;
    PKSDATAFORMAT               *pAvailableFormats;

    //
    // Check that the stream number is valid
    //

    if (StreamNumber >= DRIVER_STREAM_COUNT) 
	{
        TRAP;
        return FALSE;
    }

    NumberOfFormatArrayEntries = Streams[StreamNumber].hwStreamInfo.NumberOfFormatArrayEntries;

    //
    // Get the pointer to the array of available formats
    //
    pAvailableFormats = Streams[StreamNumber].hwStreamInfo.StreamFormatsArray;

    //
    // Walk the formats supported by the stream
    //
    for (j = 0; j < NumberOfFormatArrayEntries; j++, pAvailableFormats++) 
	{
        // Check for a match on the three GUIDs and format size

        if (!AdapterCompareGUIDsAndFormatSize(pKSDataFormatToVerify, *pAvailableFormats, FALSE /* CompareFormatSize */ )) 
		{
            continue;
        }

        //
        // Now that the three GUIDs match, switch on the Specifier
        // to do a further type-specific check
        //

        // -------------------------------------------------------------------
        // Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
        // -------------------------------------------------------------------

        if (IsEqualGUID (&pKSDataFormatToVerify->Specifier, &KSDATAFORMAT_SPECIFIER_VIDEOINFO)) 
		{
            PKS_DATAFORMAT_VIDEOINFOHEADER  pDataFormatVideoInfoHeader = (PKS_DATAFORMAT_VIDEOINFOHEADER) pKSDataFormatToVerify;
            PKS_VIDEOINFOHEADER pVideoInfoHdrToVerify = (PKS_VIDEOINFOHEADER) &pDataFormatVideoInfoHeader->VideoInfoHeader;
            PKS_DATARANGE_VIDEO pKSDataRangeVideo = (PKS_DATARANGE_VIDEO) *pAvailableFormats;
            KS_VIDEO_STREAM_CONFIG_CAPS *pConfigCaps = &pKSDataRangeVideo->ConfigCaps;
            RECT rcImage;

            /*
            **  HOW BIG IS THE IMAGE REQUESTED (pseudocode follows)
            **
            **  if (IsRectEmpty (&rcTarget) {
            **      SetRect (&rcImage, 0, 0,
            **              BITMAPINFOHEADER.biWidth,
                            BITMAPINFOHEADER.biHeight);
            **  }
            **  else {
            **      // Probably rendering to a DirectDraw surface,
            **      // where biWidth is used to expressed the "stride"
            **      // in units of pixels (not bytes) of the destination surface.
            **      // Therefore, use rcTarget to get the actual image size
            **
            **      rcImage = rcTarget;
            **  }
            */

            if ((pVideoInfoHdrToVerify->rcTarget.right - pVideoInfoHdrToVerify->rcTarget.left <= 0) || (pVideoInfoHdrToVerify->rcTarget.bottom - pVideoInfoHdrToVerify->rcTarget.top <= 0)) 
			{
                 rcImage.left = rcImage.top = 0;
                 rcImage.right = pVideoInfoHdrToVerify->bmiHeader.biWidth;
                 rcImage.bottom = pVideoInfoHdrToVerify->bmiHeader.biHeight;
            }
            else 
			{
                 rcImage = pVideoInfoHdrToVerify->rcTarget;
            }

            //
            // Perform all other verification tests here!!!
            //

            //
            // HOORAY, the format passed all of the tests, so we support it
            //

            fOK = TRUE;
            break;

        } // End of VIDEOINFOHEADER specifier

        // -------------------------------------------------------------------
        // Specifier FORMAT_AnalogVideo for KS_ANALOGVIDEOINFO
        // -------------------------------------------------------------------

        else if (IsEqualGUID (&pKSDataFormatToVerify->Specifier, &KSDATAFORMAT_SPECIFIER_ANALOGVIDEO)) 
		{
            //
            // For analog video, the DataRange and DataFormat
            // are identical, so just copy the whole structure
            //

            PKS_DATARANGE_ANALOGVIDEO DataRangeVideo = (PKS_DATARANGE_ANALOGVIDEO) *pAvailableFormats;

            //
            // Perform all other verification tests here!!!
            //

            fOK = TRUE;
            break;

        } // End of KS_ANALOGVIDEOINFO specifier

        // -------------------------------------------------------------------
        // Specifier FORMAT_VBI for KS_VIDEO_VBI
        // -------------------------------------------------------------------

        else if (IsEqualGUID (&pKSDataFormatToVerify->Specifier, &KSDATAFORMAT_SPECIFIER_VBI))
        {
            //
            // Do some VBI-specific tests
            //
            PKS_DATAFORMAT_VBIINFOHEADER    pKSVBIDataFormat;

            //DbgLogInfo(("Testcap: This is a VBIINFOHEADER format pin.\n" ));

            pKSVBIDataFormat = (PKS_DATAFORMAT_VBIINFOHEADER)pKSDataFormatToVerify;

			if (KS_AnalogVideo_None == pKSVBIDataFormat->VBIInfoHeader.VideoStandard)
            {
                fOK = TRUE;
                break;
            }
            else
            {
                //DbgLogError(("Testcap: AdapterVerifyFormat : VideoStandard(%d) != NTSC_M\n",pKSVBIDataFormat->VBIInfoHeader.VideoStandard));
            }
        }

        // -------------------------------------------------------------------
        // Type FORMAT_NABTS for NABTS pin
        // -------------------------------------------------------------------

        else if (IsEqualGUID (&pKSDataFormatToVerify->SubFormat, &KSDATAFORMAT_SUBTYPE_NABTS))
        {
            fOK = TRUE;
            break;
        }

        // -------------------------------------------------------------------
        // for CC pin
        // -------------------------------------------------------------------

        else if (IsEqualGUID (&pKSDataFormatToVerify->SubFormat, &KSDATAFORMAT_SUBTYPE_CC))
        {
            fOK = TRUE;
            break;
        }

    } // End of loop on all formats for this stream

    return fOK;
}

/*
** AdapterFormatFromRange()
**
**   Produces a DATAFORMAT given a DATARANGE.
**
**   Think of a DATARANGE as a multidimensional space of all of the possible image
**       sizes, cropping, scaling, and framerate possibilities.  Here, the caller
**       is saying "Out of this set of possibilities, could you verify that my
**       request is acceptable?".  The resulting singular output is a DATAFORMAT.
**       Note that each different colorspace (YUV vs RGB8 vs RGB24)
**       must be represented as a separate DATARANGE.
**
**   Generally, the resulting DATAFORMAT will be immediately used to open a stream
**       in that format.
**
** Arguments:
**
**         IN PHW_STREAM_REQUEST_BLOCK pSrb
**
** Returns:
**
**   TRUE if the format is supported
**   FALSE if the format cannot be suppored
**
** Side Effects:  none
*/

BOOL
STREAMAPI
AdapterFormatFromRange(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAM_DATA_INTERSECT_INFO IntersectInfo;
    PKSDATARANGE                DataRange;
    BOOL                        OnlyWantsSize;
    BOOL                        MatchFound = FALSE;
    ULONG                       FormatSize;
    ULONG                       StreamNumber;
    ULONG                       j;
    ULONG                       NumberOfFormatArrayEntries;
    PKSDATAFORMAT               *pAvailableFormats;

    IntersectInfo = pSrb->CommandData.IntersectInfo;
    StreamNumber = IntersectInfo->StreamNumber;
    DataRange = IntersectInfo->DataRange;

    //
    // Check that the stream number is valid
    //

    if (StreamNumber >= DRIVER_STREAM_COUNT) {
        pSrb->Status = STATUS_NOT_IMPLEMENTED;
        TRAP;
        return FALSE;
    }

    NumberOfFormatArrayEntries =
            Streams[StreamNumber].hwStreamInfo.NumberOfFormatArrayEntries;

    //
    // Get the pointer to the array of available formats
    //

    pAvailableFormats = Streams[StreamNumber].hwStreamInfo.StreamFormatsArray;

    //
    // Is the caller trying to get the format, or the size of the format?
    //

    OnlyWantsSize = (IntersectInfo->SizeOfDataFormatBuffer == sizeof(ULONG));

    //
    // Walk the formats supported by the stream searching for a match
    // of the three GUIDs which together define a DATARANGE
    //

    for (j = 0; j < NumberOfFormatArrayEntries; j++, pAvailableFormats++) {

        if (!AdapterCompareGUIDsAndFormatSize(
                        DataRange,
                        *pAvailableFormats,
                        TRUE /* CompareFormatSize */)) {
            continue;
        }

        //
        // Now that the three GUIDs match, do a further type-specific check
        //

        // -------------------------------------------------------------------
        // Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
        // -------------------------------------------------------------------

        if (IsEqualGUID (&DataRange->Specifier,
                &KSDATAFORMAT_SPECIFIER_VIDEOINFO)) {

            PKS_DATARANGE_VIDEO DataRangeVideoToVerify =
                    (PKS_DATARANGE_VIDEO) DataRange;
            PKS_DATARANGE_VIDEO DataRangeVideo =
                    (PKS_DATARANGE_VIDEO) *pAvailableFormats;
            PKS_DATAFORMAT_VIDEOINFOHEADER DataFormatVideoInfoHeaderOut;

            //
            // Check that the other fields match
            //
            if ((DataRangeVideoToVerify->bFixedSizeSamples != DataRangeVideo->bFixedSizeSamples) ||
                (DataRangeVideoToVerify->bTemporalCompression != DataRangeVideo->bTemporalCompression) ||
                (DataRangeVideoToVerify->StreamDescriptionFlags != DataRangeVideo->StreamDescriptionFlags) ||
                (DataRangeVideoToVerify->MemoryAllocationFlags != DataRangeVideo->MemoryAllocationFlags) ||
                (RtlCompareMemory (&DataRangeVideoToVerify->ConfigCaps,
                        &DataRangeVideo->ConfigCaps,
                        sizeof (KS_VIDEO_STREAM_CONFIG_CAPS)) !=
                        sizeof (KS_VIDEO_STREAM_CONFIG_CAPS)))
            {
                continue;
            }

            // MATCH FOUND!
            MatchFound = TRUE;
            FormatSize = sizeof (KSDATAFORMAT) +
                KS_SIZE_VIDEOHEADER (&DataRangeVideoToVerify->VideoInfoHeader);

            if (OnlyWantsSize) {
                break;
            }

            // Caller wants the full data format
            if (IntersectInfo->SizeOfDataFormatBuffer < FormatSize) {
                pSrb->Status = STATUS_BUFFER_TOO_SMALL;
                return FALSE;
            }

            // Copy over the KSDATAFORMAT, followed by the
            // actual VideoInfoHeader

            DataFormatVideoInfoHeaderOut = (PKS_DATAFORMAT_VIDEOINFOHEADER) IntersectInfo->DataFormatBuffer;

            // Copy over the KSDATAFORMAT
            RtlCopyMemory(
                &DataFormatVideoInfoHeaderOut->DataFormat,
                &DataRangeVideoToVerify->DataRange,
                sizeof (KSDATARANGE));

            DataFormatVideoInfoHeaderOut->DataFormat.FormatSize = FormatSize;

            // Copy over the callers requested VIDEOINFOHEADER

            RtlCopyMemory(
                &DataFormatVideoInfoHeaderOut->VideoInfoHeader,
                &DataRangeVideoToVerify->VideoInfoHeader,
                KS_SIZE_VIDEOHEADER (&DataRangeVideoToVerify->VideoInfoHeader));

            // Calculate biSizeImage for this request, and put the result in both
            // the biSizeImage field of the bmiHeader AND in the SampleSize field
            // of the DataFormat.
            //
            // Note that for compressed sizes, this calculation will probably not
            // be just width * height * bitdepth

            DataFormatVideoInfoHeaderOut->VideoInfoHeader.bmiHeader.biSizeImage =
                DataFormatVideoInfoHeaderOut->DataFormat.SampleSize =
                KS_DIBSIZE(DataFormatVideoInfoHeaderOut->VideoInfoHeader.bmiHeader);

            //
            // Perform other validation such as cropping and scaling checks
            //

            break;

        } // End of VIDEOINFOHEADER specifier

        // -------------------------------------------------------------------
        // Specifier FORMAT_AnalogVideo for KS_ANALOGVIDEOINFO
        // -------------------------------------------------------------------

        else if (IsEqualGUID (&DataRange->Specifier,
                &KSDATAFORMAT_SPECIFIER_ANALOGVIDEO)) {

            //
            // For analog video, the DataRange and DataFormat
            // are identical, so just copy the whole structure
            //

            PKS_DATARANGE_ANALOGVIDEO DataRangeVideo =
                    (PKS_DATARANGE_ANALOGVIDEO) *pAvailableFormats;

            // MATCH FOUND!
            MatchFound = TRUE;
            FormatSize = sizeof (KS_DATARANGE_ANALOGVIDEO);

            if (OnlyWantsSize) {
                break;
            }

            // Caller wants the full data format
            if (IntersectInfo->SizeOfDataFormatBuffer < FormatSize) {
                pSrb->Status = STATUS_BUFFER_TOO_SMALL;
                return FALSE;
            }

            RtlCopyMemory(
                IntersectInfo->DataFormatBuffer,
                DataRangeVideo,
                sizeof (KS_DATARANGE_ANALOGVIDEO));

            ((PKSDATAFORMAT)IntersectInfo->DataFormatBuffer)->FormatSize = FormatSize;

            break;

        } // End of KS_ANALOGVIDEOINFO specifier

        // -------------------------------------------------------------------
        // Specifier FORMAT_VBI for KS_VIDEO_VBI
        // -------------------------------------------------------------------

        else if (IsEqualGUID (&DataRange->Specifier,
                &KSDATAFORMAT_SPECIFIER_VBI))
        {
            PKS_DATARANGE_VIDEO_VBI pDataRangeVBI =
                (PKS_DATARANGE_VIDEO_VBI)*pAvailableFormats;
            PKS_DATAFORMAT_VBIINFOHEADER InterVBIHdr =
                (PKS_DATAFORMAT_VBIINFOHEADER)IntersectInfo->DataFormatBuffer;

            // MATCH FOUND!
            MatchFound = TRUE;

            FormatSize = sizeof (KS_DATAFORMAT_VBIINFOHEADER);

            // Is the caller trying to get the format, or the size of it?
            if (OnlyWantsSize)
                break;

            // Verify that there is enough room in the supplied buffer
            //   for the whole thing
            if (IntersectInfo->SizeOfDataFormatBuffer < FormatSize)
            {
                if (IntersectInfo->SizeOfDataFormatBuffer > 0) {
                    DbgLogError(
                        ("Testcap::AdapterFormatFromRange: "
                         "Specifier==VBI, Buffer too small=%d vs. %d\n",
                         IntersectInfo->SizeOfDataFormatBuffer,
                         FormatSize));
                }
                pSrb->Status = STATUS_BUFFER_TOO_SMALL;
                return FALSE;
            }

            // If there is room, go ahead...

            RtlCopyMemory(&InterVBIHdr->DataFormat,
                          &pDataRangeVBI->DataRange,
                          sizeof (KSDATARANGE));

            ((PKSDATAFORMAT)IntersectInfo->DataFormatBuffer)->FormatSize = FormatSize;

            RtlCopyMemory(&InterVBIHdr->VBIInfoHeader,
                          &pDataRangeVBI->VBIInfoHeader,
                          sizeof(KS_VBIINFOHEADER));

            break;

        } // End of KS_VIDEO_VBI specifier

        // -------------------------------------------------------------------
        // Type FORMAT_NABTS for NABTS pin
        // -------------------------------------------------------------------

        else if (IsEqualGUID (&DataRange->SubFormat,
                &KSDATAFORMAT_SUBTYPE_NABTS))
        {
            PKSDATARANGE pDataRange = (PKSDATARANGE)*pAvailableFormats;

            // MATCH FOUND!
            MatchFound = TRUE;

            FormatSize = sizeof (KSDATAFORMAT);

            // Is the caller trying to get the format, or the size of it?
            if (OnlyWantsSize)
                break;

            // Verify that there is enough room in the supplied buffer
            //   for the whole thing
            if (IntersectInfo->SizeOfDataFormatBuffer >= FormatSize)
            {
                RtlCopyMemory(IntersectInfo->DataFormatBuffer,
                              pDataRange,
                              FormatSize);

                ((PKSDATAFORMAT)IntersectInfo->DataFormatBuffer)->FormatSize = FormatSize;
            }
            else
            {
                if (IntersectInfo->SizeOfDataFormatBuffer > 0) {
                    DbgLogError(
                        ("Testcap::AdapterFormatFromRange: "
                         "SubFormat==NABTS, Buffer too small=%d vs. %d\n",
                         IntersectInfo->SizeOfDataFormatBuffer,
                         FormatSize));
                }
                pSrb->Status = STATUS_BUFFER_TOO_SMALL;
                return FALSE;
            }

            break;

        } // End of KS_SUBTYPE_NABTS

        // -------------------------------------------------------------------
        // for CC pin
        // -------------------------------------------------------------------

        else if (IsEqualGUID (&DataRange->SubFormat,
                &KSDATAFORMAT_SUBTYPE_CC))
        {
            PKSDATARANGE pDataRange = (PKSDATARANGE)*pAvailableFormats;

            // MATCH FOUND!
            MatchFound = TRUE;

            FormatSize = sizeof (KSDATAFORMAT);

            // Is the caller trying to get the format, or the size of it?
            if (OnlyWantsSize)
                break;

            // Verify that there is enough room in the supplied buffer
            //   for the whole thing
            if (IntersectInfo->SizeOfDataFormatBuffer >= FormatSize)
            {
                RtlCopyMemory(IntersectInfo->DataFormatBuffer,
                              pDataRange,
                              FormatSize);

                ((PKSDATAFORMAT)IntersectInfo->DataFormatBuffer)->FormatSize = FormatSize;
            }
            else
            {
                if (IntersectInfo->SizeOfDataFormatBuffer > 0) {
                    DbgLogError(
                        ("Testcap::AdapterFormatFromRange: "
                         "SubFormat==CC, Buffer too small=%d vs. %d\n",
                         IntersectInfo->SizeOfDataFormatBuffer,
                         FormatSize));
                }
                pSrb->Status = STATUS_BUFFER_TOO_SMALL;
                return FALSE;
            }

            break;

        } // End of CC pin format check

        else {
            pSrb->Status = STATUS_NO_MATCH;
            return FALSE;
        }

    } // End of loop on all formats for this stream

    if (!MatchFound) {
        pSrb->Status = STATUS_NO_MATCH;
        return FALSE;
    }

    if (OnlyWantsSize) {
        *(PULONG) IntersectInfo->DataFormatBuffer = FormatSize;
        FormatSize = sizeof(ULONG);
    }
    pSrb->ActualBytesTransferred = FormatSize;
    return TRUE;
}

