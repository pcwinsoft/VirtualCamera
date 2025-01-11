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
#include "capdebug.h"
#include "capxfer.h"
#include "ntstatus.h"

//==========================================================================;
// General queue management routines
//==========================================================================;

/*
** AddToListIfBusy ()
**
**   Grabs a spinlock, checks the busy flag, and if set adds an SRB to a queue
**
** Arguments:
**
**   pSrb - Stream request block
**
**   SpinLock - The spinlock to use when checking the flag
**
**   BusyFlag - The flag to check
**
**   ListHead - The list onto which the Srb will be added if the busy flag is set
**
** Returns:
**
**   The state of the busy flag on entry.  This will be TRUE if we're already
**   processing an SRB, and FALSE if no SRB is already in progress.
**
** Side Effects:  none
*/

BOOL
STREAMAPI
AddToListIfBusy (
    IN PHW_STREAM_REQUEST_BLOCK pSrb,
    IN KSPIN_LOCK              *SpinLock,
    IN OUT BOOL                *BusyFlag,
    IN LIST_ENTRY              *ListHead
    )
{
    KIRQL                       Irql;
    PSRB_EXTENSION              pSrbExt = (PSRB_EXTENSION)pSrb->SRBExtension;

    KeAcquireSpinLock (SpinLock, &Irql);

    // If we're already processing another SRB, add this current request
    // to the queue and return TRUE

    if (*BusyFlag == TRUE) 
	{
        // Save the SRB pointer away in the SRB Extension
        pSrbExt->pSrb = pSrb;
        InsertTailList(ListHead, &pSrbExt->ListEntry);
        KeReleaseSpinLock(SpinLock, Irql);
        return TRUE;
    }

    // Otherwise, set the busy flag, release the spinlock, and return FALSE
    *BusyFlag = TRUE;
    KeReleaseSpinLock(SpinLock, Irql);

    return FALSE;
}

/*
** RemoveFromListIfAvailable ()
**
**   Grabs a spinlock, checks for an available SRB, and removes it from the list
**
** Arguments:
**
**   &pSrb - where to return the Stream request block if available
**
**   SpinLock - The spinlock to use
**
**   BusyFlag - The flag to clear if the list is empty
**
**   ListHead - The list from which an SRB will be removed if available
**
** Returns:
**
**   TRUE if an SRB was removed from the list
**   FALSE if the list is empty
**
** Side Effects:  none
*/

BOOL
STREAMAPI
RemoveFromListIfAvailable (
    IN OUT PHW_STREAM_REQUEST_BLOCK *pSrb,
    IN KSPIN_LOCK                   *SpinLock,
    IN OUT BOOL                     *BusyFlag,
    IN LIST_ENTRY                   *ListHead
    )
{
    KIRQL                       Irql;

    KeAcquireSpinLock (SpinLock, &Irql);

    //
    // If the queue is now empty, clear the busy flag, and return
    //
    if (IsListEmpty(ListHead)) 
	{
        *BusyFlag = FALSE;
        KeReleaseSpinLock(SpinLock, Irql);
        return FALSE;
    }
    //
    // otherwise extract the SRB
    //
    else 
	{
        PUCHAR          ptr;
        PSRB_EXTENSION  pSrbExt;

        ptr = (PUCHAR)RemoveHeadList(ListHead);
        *BusyFlag = TRUE;
        KeReleaseSpinLock(SpinLock, Irql);
        // Get the SRB out of the SRB extension and return it
        pSrbExt = (PSRB_EXTENSION) (((PUCHAR) ptr) - FIELDOFFSET(SRB_EXTENSION, ListEntry));
        *pSrb = pSrbExt->pSrb;
    }
    return TRUE;
}

//==========================================================================;
// Routines for managing the SRB queue on a per stream basis
//==========================================================================;

/*
** VideoQueueAddSRB () - AGFX
**
**   Adds a stream data SRB to a stream queue.  The queue is maintained in a
**   first in, first out order.
**
** Arguments:
**
**   pSrb - Stream request block for the Video stream
**
** Returns: nothing
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoQueueAddSRB (
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    int                     StreamNumber = pSrb->StreamObject->StreamNumber;
    KIRQL                   oldIrql;

    KeAcquireSpinLock (&pHwDevExt->StreamSRBSpinLock[pStrmEx->StreamID], &oldIrql);

    // Save the SRB pointer in the IRP so we can use the IRPs
    // ListEntry to maintain a doubly linked list of pending
    // requests

    pSrb->Irp->Tail.Overlay.DriverContext[0] = pSrb;

    InsertTailList (&pHwDevExt->StreamSRBList[pStrmEx->StreamID], &pSrb->Irp->Tail.Overlay.ListEntry);

    // Increment the count of outstanding SRBs in this queue
    pHwDevExt->StreamSRBListSize[pStrmEx->StreamID]++;

    KeReleaseSpinLock (&pHwDevExt->StreamSRBSpinLock[pStrmEx->StreamID], oldIrql);
}

/*
** VideoQueueRemoveSRB () - AGFX
**
**   Removes a stream data SRB from a stream queue
**
** Arguments:
**
**   pHwDevExt - Device Extension
**
**   StreamNumber - Index of the stream
**
** Returns: SRB or NULL
**
** Side Effects:  none
*/

PHW_STREAM_REQUEST_BLOCK
STREAMAPI
VideoQueueRemoveSRB (
    PHW_DEVICE_EXTENSION pHwDevExt,
	
	// this is the unique id for the stream - already comes passed on the parameter
    int StreamNumberID
    )
{
    PUCHAR ptr;
    PIRP pIrp;
    PHW_STREAM_REQUEST_BLOCK pSrb = NULL;
    KIRQL oldIrql;

    KeAcquireSpinLock (&pHwDevExt->StreamSRBSpinLock[StreamNumberID], &oldIrql);

    //
    // Get the SRB out of the IRP out of the pending list
    //
    if (!IsListEmpty (&pHwDevExt->StreamSRBList[StreamNumberID])) 
	{
        ptr = (PUCHAR) RemoveHeadList(&pHwDevExt->StreamSRBList[StreamNumberID]);

        pIrp = (PIRP) (((PUCHAR) ptr) - FIELDOFFSET(IRP, Tail.Overlay.ListEntry));

        pSrb = (PHW_STREAM_REQUEST_BLOCK) pIrp->Tail.Overlay.DriverContext[0];

        // Decrement the count of SRBs in this queue
        pHwDevExt->StreamSRBListSize[StreamNumberID]--;
    }

    KeReleaseSpinLock (&pHwDevExt->StreamSRBSpinLock[StreamNumberID], oldIrql);

    return pSrb;
}

/*
** VideoQueueCancelAllSRBs() - AGFX
**
**    In case of a client crash, this empties the stream queue when the stream closes
**
** Arguments:
**
**    pStrmEx - pointer to the stream extension
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoQueueCancelAllSRBs (
	//this is the unique stream
	PSTREAMEX pStrmEx
    )
{
    PHW_DEVICE_EXTENSION        pHwDevExt = (PHW_DEVICE_EXTENSION)pStrmEx->pHwDevExt;
    int                         StreamNumber = pStrmEx->pStreamObject->StreamNumber;
    PUCHAR                      ptr;
    PIRP                        pIrp;
    PHW_STREAM_REQUEST_BLOCK    pSrb;
    KIRQL                       oldIrql;

    if (pStrmEx->KSState != KSSTATE_STOP) 
	{
        // May need to force the device to a stopped state here
        // may need to disable interrupts here !
    }

    //
    // The stream class will cancel all outstanding IRPs for us
    // (but only if we've set TurnOffSynchronization = FALSE)
    //

    KeAcquireSpinLock (&pHwDevExt->StreamSRBSpinLock[pStrmEx->StreamID], &oldIrql);
	
    //
    // Get the SRB out of the IRP out of the pending list
    //
    while (!IsListEmpty (&pHwDevExt->StreamSRBList[pStrmEx->StreamID])) 
	{
        ptr = (PUCHAR) RemoveHeadList(&pHwDevExt->StreamSRBList[pStrmEx->StreamID]);

        pIrp = (PIRP) (((PUCHAR) ptr) - FIELDOFFSET(IRP, Tail.Overlay.ListEntry));

        pSrb = (PHW_STREAM_REQUEST_BLOCK) pIrp->Tail.Overlay.DriverContext[0];

        // Decrement the count of SRBs in this queue
        pHwDevExt->StreamSRBListSize[pStrmEx->StreamID]--;

        //
        // Make the length zero, and status cancelled
        //
        pSrb->CommandData.DataBufferArray->DataUsed = 0;
        pSrb->Status = STATUS_CANCELLED;

        CompleteStreamSRB (pSrb);
    }

    KeReleaseSpinLock (&pHwDevExt->StreamSRBSpinLock[pStrmEx->StreamID], oldIrql);
}

/*
** VideoQueueCancelOneSRB() - AGFX
**
**    Called when cancelling a particular SRB
**
** Arguments:
**
**    pStrmEx - pointer to the stream extension
**
**    pSRBToCancel - pointer to the SRB
**
** Returns:
**
**    TRUE if the SRB was found in this queue
**
** Side Effects:  none
*/

BOOL
STREAMAPI
VideoQueueCancelOneSRB (
    PSTREAMEX pStrmEx,
    PHW_STREAM_REQUEST_BLOCK pSrbToCancel
    )
{
    PHW_DEVICE_EXTENSION        pHwDevExt = (PHW_DEVICE_EXTENSION)pStrmEx->pHwDevExt;
    int                         StreamNumber = pStrmEx->pStreamObject->StreamNumber;
    KIRQL                       oldIrql;
    BOOL                        Found = FALSE;
    PIRP                        pIrp;
    PHW_STREAM_REQUEST_BLOCK    pSrb;
    PLIST_ENTRY                 Entry;

    KeAcquireSpinLock (&pHwDevExt->StreamSRBSpinLock[pStrmEx->StreamID], &oldIrql);

    Entry = pHwDevExt->StreamSRBList[pStrmEx->StreamID].Flink;

    //
    // Loop through the linked list from the beginning to end,
    // trying to find the SRB to cancel
    //

    while (Entry != &pHwDevExt->StreamSRBList[pStrmEx->StreamID]) 
	{
        pIrp = (PIRP) (((PUCHAR) Entry) - FIELDOFFSET(IRP, Tail.Overlay.ListEntry));

        pSrb = (PHW_STREAM_REQUEST_BLOCK) pIrp->Tail.Overlay.DriverContext[0];

        if (pSrb == pSrbToCancel) 
		{
            RemoveEntryList(Entry);
            Found = TRUE;
            break;
        }

        Entry = Entry->Flink;
    }

    KeReleaseSpinLock (&pHwDevExt->StreamSRBSpinLock[pStrmEx->StreamID], oldIrql);

    if (Found) {

        pHwDevExt->StreamSRBListSize[pStrmEx->StreamID]--;

        //
        // Make the length zero, and status cancelled
        //

        pSrbToCancel->CommandData.DataBufferArray->DataUsed = 0;
        pSrbToCancel->Status = STATUS_CANCELLED;

        CompleteStreamSRB (pSrbToCancel);
    }

    return Found;
}

/*
** VideoSetFormat() - AGFX
**
**   Sets the format for a video stream.  This happens both when the
**   stream is first opened, and also when dynamically switching formats
**   on the preview pin.
**
**   It is assumed that the format has been verified for correctness before
**   this call is made.
**
** Arguments:
**
**   pSrb - Stream request block for the Video stream
**
** Returns:
**
**   TRUE if the format could be set, else FALSE
**
** Side Effects:  none
*/

BOOL
STREAMAPI
VideoSetFormat(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    int                     StreamNumber = pSrb->StreamObject->StreamNumber;
    UINT                    nSize;
    PKSDATAFORMAT           pKSDataFormat = pSrb->CommandData.OpenFormat;

    // -------------------------------------------------------------------
    // Specifier FORMAT_VideoInfo for VIDEOINFOHEADER
    // -------------------------------------------------------------------

    if (IsEqualGUID (&pKSDataFormat->Specifier, &KSDATAFORMAT_SPECIFIER_VIDEOINFO)) 
	{
        PKS_DATAFORMAT_VIDEOINFOHEADER  pVideoInfoHeader = (PKS_DATAFORMAT_VIDEOINFOHEADER) pSrb->CommandData.OpenFormat;
        PKS_VIDEOINFOHEADER pVideoInfoHdrRequested = &pVideoInfoHeader->VideoInfoHeader;

        nSize = KS_SIZE_VIDEOHEADER (pVideoInfoHdrRequested);

        //
        // If a previous format was in use, release the memory
        //
        if (pStrmEx->pVideoInfoHeader) 
		{
            ExFreePool(pStrmEx->pVideoInfoHeader);
            pStrmEx->pVideoInfoHeader = NULL;
        }

        // Since the VIDEOINFOHEADER is of potentially variable size
        // allocate memory for it

        pStrmEx->pVideoInfoHeader = ExAllocatePool(NonPagedPool, nSize);

        if (pStrmEx->pVideoInfoHeader == NULL) 
		{
            pSrb->Status = STATUS_INSUFFICIENT_RESOURCES;
            return FALSE;
        }

        // Copy the VIDEOINFOHEADER requested to our storage
        RtlCopyMemory(pStrmEx->pVideoInfoHeader, pVideoInfoHdrRequested, nSize);

        // A renderer may be switching formats, and in this case, the AvgTimePerFrame
        // will be zero.  Don't overwrite a previously set framerate.
        if (pStrmEx->pVideoInfoHeader->AvgTimePerFrame) 
		{
            pStrmEx->AvgTimePerFrame = pStrmEx->pVideoInfoHeader->AvgTimePerFrame;
        }
    }

    // -------------------------------------------------------------------
    // Specifier FORMAT_AnalogVideo for KS_ANALOGVIDEOINFO
    // -------------------------------------------------------------------
    else if (IsEqualGUID (&pKSDataFormat->Specifier, &KSDATAFORMAT_SPECIFIER_ANALOGVIDEO)) 
	{
        //
        // AnalogVideo DataRange == DataFormat!
        //

        //
        // For now, don't even cache this
        //
        PKS_DATARANGE_ANALOGVIDEO pDataFormatAnalogVideo = (PKS_DATARANGE_ANALOGVIDEO) pSrb->CommandData.OpenFormat;
    }

    // -------------------------------------------------------------------
    // Specifier FORMAT_VBI for KS_VIDEO_VBI
    // -------------------------------------------------------------------
    else if (IsEqualGUID (&pKSDataFormat->Specifier, &KSDATAFORMAT_SPECIFIER_VBI))
    {
        // On a VBI stream, we save a pointer to StreamFormatVBI, which
        //  has the timing info we want to get at later.
        pStrmEx->pVBIStreamFormat = &StreamFormatVBI;
    }

    // -------------------------------------------------------------------
    // Type FORMAT_NABTS for NABTS pin
    // -------------------------------------------------------------------
    else if (IsEqualGUID (&pKSDataFormat->SubFormat, &KSDATAFORMAT_SUBTYPE_NABTS))
    {
        // On a VBI stream, we save a pointer to StreamFormatVBI, which
        //  has the timing info we want to get at later. (Even though
        //  this is really a StreamFormatNABTS pin)
        pStrmEx->pVBIStreamFormat = &StreamFormatVBI;
    }

    // -------------------------------------------------------------------
    // for CC pin
    // -------------------------------------------------------------------
    else if (IsEqualGUID (&pKSDataFormat->SubFormat, &KSDATAFORMAT_SUBTYPE_CC))
    {
        // On a VBI stream, we save a pointer to StreamFormatVBI, which
        //  has the timing info we want to get at later. (Even though
        //  this is really a StreamFormatCC pin)
        pStrmEx->pVBIStreamFormat = &StreamFormatVBI;
    }

    else 
	{
        // Unknown format
        pSrb->Status = STATUS_INVALID_PARAMETER;
        return FALSE;
    }

    return TRUE;
}

/*
** VideoReceiveDataPacket()
**
**   Receives Video data packet commands on the output streams
**
** Arguments:
**
**   pSrb - Stream request block for the Video stream
**
** Returns: nothing
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoReceiveDataPacket(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    int                     StreamNumber = pSrb->StreamObject->StreamNumber;

    //
    // make sure we have a device extension and are at passive level
    //

    DEBUG_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    DEBUG_ASSERT(pHwDevExt!=NULL);

    //
    // Default to success
    //

    pSrb->Status = STATUS_SUCCESS;

    //
    // determine the type of packet.
    //

    switch (pSrb->Command){

    case SRB_READ_DATA:

        // Rule:
        // Only accept read requests when in either the Pause or Run
        // States.  If Stopped, immediately return the SRB.

        if (pStrmEx->KSState == KSSTATE_STOP) {

            CompleteStreamSRB (pSrb);

            break;
        }

        //
        // Put this read request on the pending queue
        //

        VideoQueueAddSRB (pSrb);

        // Since another thread COULD HAVE MODIFIED THE STREAM STATE
        // in the midst of adding it to the queue, check the stream
        // state again, and cancel the SRB if necessary.  Note that
        // this race condition was NOT handled in the original DDK
        // release of testcap!

        if (pStrmEx->KSState == KSSTATE_STOP) {

            VideoQueueCancelOneSRB (
                pStrmEx,
                pSrb);
        }

        break;

    default:

        //
        // invalid / unsupported command. Fail it as such
        //

        TRAP;

        pSrb->Status = STATUS_NOT_IMPLEMENTED;

        CompleteStreamSRB (pSrb);

    }  // switch (pSrb->Command)
}


/*
** VideoReceiveCtrlPacket() - AGFX
**
**   Receives packet commands that control the Video output streams
**
** Arguments:
**
**   pSrb - The stream request block for the Video stream
**
** Returns: nothing
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoReceiveCtrlPacket(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    
	// pStrmEx points to the hwstreamextnsion desta stream particular e esta contem os dados particulares como o ID da stream unica
	PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    int                     StreamNumber = pStrmEx->pStreamObject->StreamNumber;
    BOOL                    Busy;

    //
    // make sure we have a device extension and are at passive level
    //

    DEBUG_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    DEBUG_ASSERT(pHwDevExt!=NULL);

    //
    // If we're already processing an SRB, add it to the queue
    //

    Busy = AddToListIfBusy (pSrb, &pHwDevExt->AdapterSpinLock, &pHwDevExt->ProcessingControlSRB [pStrmEx->StreamID], &pHwDevExt->StreamControlSRBList[pStrmEx->StreamID]);


    if (Busy) 
	{
        return;
    }

    while (TRUE) 
	{

        //
        // Default to success
        //

        pSrb->Status = STATUS_SUCCESS;

        //
        // determine the type of packet.
        //

        switch (pSrb->Command)
        {

        case SRB_PROPOSE_DATA_FORMAT:

            if (!(AdapterVerifyFormat (pSrb->CommandData.OpenFormat, pSrb->StreamObject->StreamNumber))) 
			{
                pSrb->Status = STATUS_NO_MATCH;
            }
            // KS support for dynamic format changes is BROKEN right now,
            //  so we prevent these from happening by saying they ALL fail.
            // If this is ever fixed, the next line must be removed.
            pSrb->Status = STATUS_NO_MATCH; // prevent dynamic format changes
            break;

        case SRB_SET_DATA_FORMAT:

            if (!(AdapterVerifyFormat (pSrb->CommandData.OpenFormat, pSrb->StreamObject->StreamNumber))) 
			{
                pSrb->Status = STATUS_NO_MATCH;
            } 
			else 
			{
                VideoSetFormat (pSrb);
            }

            break;

        case SRB_GET_DATA_FORMAT:
            
            pSrb->Status = STATUS_NOT_IMPLEMENTED;
            break;


        case SRB_SET_STREAM_STATE:

            //here the state goes from paused to running and that is where the timer to capture video is set
			VideoSetState(pSrb);
            break;

        case SRB_GET_STREAM_STATE:

            VideoGetState(pSrb);
            break;

        case SRB_GET_STREAM_PROPERTY:

            VideoGetProperty(pSrb);
            break;

        case SRB_SET_STREAM_PROPERTY:

            VideoSetProperty(pSrb);
            break;

        case SRB_INDICATE_MASTER_CLOCK:

            //
            // Assigns a clock to a stream
            //

            VideoIndicateMasterClock (pSrb);

            break;

        default:

            //
            // invalid / unsupported command. Fail it as such
            //

            TRAP;

            pSrb->Status = STATUS_NOT_IMPLEMENTED;
        }

        CompleteStreamSRB (pSrb);

        //
        // See if there's anything else on the queue
        //

        Busy = RemoveFromListIfAvailable (&pSrb, &pHwDevExt->AdapterSpinLock, &pHwDevExt->ProcessingControlSRB [pStrmEx->StreamID], &pHwDevExt->StreamControlSRBList[pStrmEx->StreamID]);

        if (!Busy) 
		{
            break;
        }
    }
}

/*
** AnalogVideoReceiveDataPacket()
**
**   Receives AnalogVideo data packet commands on the input stream
**
** Arguments:
**
**   pSrb - Stream request block for the Analog Video stream.
**          This stream receives tuner control packets.
**
** Returns: nothing
**
** Side Effects:  none
*/

VOID
STREAMAPI
AnalogVideoReceiveDataPacket(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PKSSTREAM_HEADER        pDataPacket = pSrb->CommandData.DataBufferArray;

    //
    // make sure we have a device extension and are at passive level
    //

    DEBUG_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    DEBUG_ASSERT(pHwDevExt!=NULL);

    //
    // Default to success
    //

    pSrb->Status = STATUS_SUCCESS;

    //
    // determine the type of packet.
    //

    switch (pSrb->Command){

    case SRB_WRITE_DATA:

        //
        // This data packet contains the channel change information
        // passed on the AnalogVideoIn stream.  Devices which support
        // VBI data streams need to pass this info on their output pins.
        //

        if (pDataPacket->FrameExtent == sizeof (KS_TVTUNER_CHANGE_INFO)) {

            RtlCopyMemory(
                &pHwDevExt->TVTunerChangeInfo,
                pDataPacket->Data,
                sizeof (KS_TVTUNER_CHANGE_INFO));
        }

        CompleteStreamSRB (pSrb);

        break;

    default:

        //
        // invalid / unsupported command. Fail it as such
        //

        TRAP;

        pSrb->Status = STATUS_NOT_IMPLEMENTED;

        CompleteStreamSRB (pSrb);

    }  // switch (pSrb->Command)
}


/*
** AnalogVideoReceiveCtrlPacket()
**
**   Receives packet commands that control the Analog Video stream
**
** Arguments:
**
**   pSrb - The stream request block for the Video stream
**
** Returns: nothing
**
** Side Effects:  none
*/

VOID
STREAMAPI
AnalogVideoReceiveCtrlPacket(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION    pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    int                     StreamNumber = pStrmEx->pStreamObject->StreamNumber;
    BOOL                    Busy;

    //
    // make sure we have a device extension and we are at passive level
    //

    DEBUG_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
    DEBUG_ASSERT(pHwDevExt!=NULL);

    DbgLogTrace(("TestCap: Receiving Analog Stream Control SRB %p, %x\n", pSrb, pSrb->Command));

    //
    // If we're already processing an SRB, add it to the queue
    //
    Busy = AddToListIfBusy (
                        pSrb,
                        &pHwDevExt->AdapterSpinLock,
                        //&pHwDevExt->ProcessingControlSRB [StreamNumber],
                        //&pHwDevExt->StreamControlSRBList[StreamNumber]);
                        &pHwDevExt->ProcessingControlSRB [pStrmEx->StreamID],
                        &pHwDevExt->StreamControlSRBList[pStrmEx->StreamID]);


    if (Busy) {
        return;
    }

    do {
        //
        // Default to success
        //

        pSrb->Status = STATUS_SUCCESS;

        //
        // determine the type of packet.
        //

        switch (pSrb->Command)
        {

        case SRB_PROPOSE_DATA_FORMAT:
            DbgLogInfo(("TestCap: Receiving SRB_PROPOSE_DATA_FORMAT  SRB %p, StreamNumber= %d\n", pSrb, StreamNumber));

            if (!(AdapterVerifyFormat (
                    pSrb->CommandData.OpenFormat,
                    pSrb->StreamObject->StreamNumber))) {
                pSrb->Status = STATUS_NO_MATCH;
            }
            break;

        case SRB_SET_STREAM_STATE:

            //
            // Don't use VideoSetState, since we don't want to start another
            // timer running
            //

            pStrmEx->KSState = pSrb->CommandData.StreamState;
            DbgLogInfo(("TestCap: STATE=%d, Stream=%d\n", pStrmEx->KSState, StreamNumber));
            break;

        case SRB_GET_STREAM_STATE:

            VideoGetState(pSrb);
            break;

        case SRB_GET_STREAM_PROPERTY:

            VideoGetProperty(pSrb);
            break;

        case SRB_INDICATE_MASTER_CLOCK:

            //
            // Assigns a clock to a stream
            //

            VideoIndicateMasterClock (pSrb);

            break;

        default:

            //
            // invalid / unsupported command. Fail it as such
            //

            TRAP;

            pSrb->Status = STATUS_NOT_IMPLEMENTED;
        }

        CompleteStreamSRB (pSrb);

        //
        // See if there's anything else on the queue
        //
        Busy = RemoveFromListIfAvailable (
                        &pSrb,
                        &pHwDevExt->AdapterSpinLock,
                        //&pHwDevExt->ProcessingControlSRB [StreamNumber],
                        //&pHwDevExt->StreamControlSRBList[StreamNumber]);
                        &pHwDevExt->ProcessingControlSRB [pStrmEx->StreamID],
                        &pHwDevExt->StreamControlSRBList[pStrmEx->StreamID]);


    } while ( Busy );
}


/*
** CompleteStreamSRB ()
**
**   This routine is called when a packet is being completed.
**
** Arguments:
**
**   pSrb - pointer to the request packet to be completed
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
CompleteStreamSRB (
     IN PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    StreamClassStreamNotification(
            StreamRequestComplete,
            pSrb->StreamObject,
            pSrb);
}


/*
** VideoGetProperty()
**
**    Routine to process video property requests
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoGetProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

    if (IsEqualGUID (&KSPROPSETID_Connection, &pSPD->Property->Set)) {
        VideoStreamGetConnectionProperty (pSrb);
    }
    else if (IsEqualGUID (&PROPSETID_VIDCAP_DROPPEDFRAMES, &pSPD->Property->Set)) {
        VideoStreamGetDroppedFramesProperty (pSrb);
    }
    else {
       pSrb->Status = STATUS_NOT_IMPLEMENTED;
    }
}

/*
** VideoSetProperty()
**
**    Routine to process video property requests
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoSetProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
//    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;

    pSrb->Status = STATUS_NOT_IMPLEMENTED;
}



/*
** VideoTimerRoutine() - AGFX
**
**    A timer has been created based on the requested capture interval.
**    This is the callback routine for this timer event.
**
**    Note:  Devices capable of using interrupts should always
**           trigger capture on a VSYNC interrupt, and not use a timer.
**
** Arguments:
**
**    Context - pointer to the stream extension
**
** Returns: nothing
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoTimerRoutine(
    PVOID Context
    )
{
	//this is the unique stream with id and everything else
    PSTREAMEX                   pStrmEx = ((PSTREAMEX)Context);

    PHW_DEVICE_EXTENSION        pHwDevExt = pStrmEx->pHwDevExt;
    int                         StreamNumber = pStrmEx->pStreamObject->StreamNumber;

    // If we're stopped and the timer is still running, just return.
    // This will stop the timer.

    if (pStrmEx->KSState == KSSTATE_STOP) 
	{
        return;
    }

    // Capture a frame if it's time and we have a buffer
    VideoCaptureRoutine(pStrmEx);

    // Schedule the next timer event
    // Make it run at 2x the requested capture rate (which is in 100nS units)

    StreamClassScheduleTimer (
            pStrmEx->pStreamObject,     // StreamObject
            pHwDevExt,                  // HwDeviceExtension
			// ERR - FRAME-RATE FICA SLOW!!
            //(ULONG) (pStrmEx->AvgTimePerFrame), // Microseconds
            // THIS IS WHAT ALLOWS 30 FRAMES PER SECOND
			(ULONG) (pStrmEx->AvgTimePerFrame/20), // Microseconds
            VideoTimerRoutine,          // TimerRoutine
            pStrmEx);                   // Context - this context is the unique stream which means every time the timer runs it receives the unique stream
}


/*
** VideoCaptureRoutine() - AGFX
**
**    Routine to capture video frames based on a timer.
**
**    Note:  Devices capable of using interrupts should always
**           trigger capture on a VSYNC interrupt, and not use a timer.
**
** Arguments:
**
** Returns: nothing
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoCaptureRoutine(
	//this is the unique stream with id and everything else
	IN PSTREAMEX pStrmEx
    )
{
    PHW_DEVICE_EXTENSION    pHwDevExt = pStrmEx->pHwDevExt;
    int                     StreamNumber = pStrmEx->pStreamObject->StreamNumber;
    PKSSTREAM_HEADER        pDataPacket;
    PKS_FRAME_INFO          pFrameInfo;

    // If we're stopped and the timer is still running, just return.
    // This will stop the timer.

    if (pStrmEx->KSState == KSSTATE_STOP) 
	{
        return;
    }

    // Find out what time it is, if we're using a clock

    if (pStrmEx->hMasterClock ) 
	{
        HW_TIME_CONTEXT TimeContext;

        TimeContext.HwDeviceExtension = pHwDevExt;
        TimeContext.HwStreamObject = pStrmEx->pStreamObject;
        TimeContext.Function = TIME_GET_STREAM_TIME;

        StreamClassQueryMasterClockSync ( pStrmEx->hMasterClock, &TimeContext);

        pStrmEx->QST_StreamTime = TimeContext.Time;
        pStrmEx->QST_Now = TimeContext.SystemTime;

        if (pStrmEx->QST_NextFrame == 0) 
		{
            pStrmEx->QST_NextFrame = pStrmEx->QST_StreamTime + pStrmEx->AvgTimePerFrame;
        }
    }

    // Only capture in the RUN state

    if (pStrmEx->KSState == KSSTATE_RUN) 
	{
        //
        // Determine if it is time to capture a frame based on
        // how much time has elapsed since capture started.
        // If there isn't a clock available, then capture immediately.
        //

        if ((!pStrmEx->hMasterClock) || (pStrmEx->QST_StreamTime >= pStrmEx->QST_NextFrame)) 
		{
            PHW_STREAM_REQUEST_BLOCK pSrb;

            // Increment the picture count (usually this is VSYNC count)

            pStrmEx->FrameInfo.PictureNumber++;

            //
            // Get the next queue SRB (if any)
            //

            pSrb = VideoQueueRemoveSRB (pHwDevExt, pStrmEx->StreamID);

            if (pSrb) 
			{
                pDataPacket = pSrb->CommandData.DataBufferArray;
                pFrameInfo = (PKS_FRAME_INFO) (pDataPacket + 1);

                //
                // Call the routine which synthesizes images
                //

                ImageSynth (pSrb, pHwDevExt->VideoInputConnected, pStrmEx->VideoControlMode & KS_VideoControlFlag_FlipHorizontal);

                // Set additional info fields about the data captured such as:
                //   Frames Captured
                //   Frames Dropped
                //   Field Polarity


                pStrmEx->FrameInfo.ExtendedHeaderSize = pFrameInfo->ExtendedHeaderSize;

                *pFrameInfo = pStrmEx->FrameInfo;

                // Init the flags to zero
                pDataPacket->OptionsFlags = 0;

                // Set the discontinuity flag if frames have been previously
                // dropped, and then reset our internal flag

                if (pStrmEx->fDiscontinuity) 
				{
                    pDataPacket->OptionsFlags |= KSSTREAM_HEADER_OPTIONSF_DATADISCONTINUITY;
                    pStrmEx->fDiscontinuity = FALSE;
                }

                //
                // Return the timestamp for the frame
                //
                pDataPacket->PresentationTime.Numerator = 1;
                pDataPacket->PresentationTime.Denominator = 1;
                pDataPacket->Duration = pStrmEx->AvgTimePerFrame;

                //
                // if we have a master clock AND this is the capture stream
                //
                if (pStrmEx->hMasterClock)
				{
                    pDataPacket->PresentationTime.Time = pStrmEx->QST_StreamTime;
                    pDataPacket->OptionsFlags |= KSSTREAM_HEADER_OPTIONSF_TIMEVALID | KSSTREAM_HEADER_OPTIONSF_DURATIONVALID;
                }
                else 
				{
                    //
                    // no clock or the preview stream, so just mark the time as unknown
                    //
                    pDataPacket->PresentationTime.Time = 0;
                    // clear the timestamp valid flags

                    pDataPacket->OptionsFlags &= ~(KSSTREAM_HEADER_OPTIONSF_TIMEVALID | KSSTREAM_HEADER_OPTIONSF_DURATIONVALID);
                }

				pDataPacket->OptionsFlags |= KSSTREAM_HEADER_OPTIONSF_SPLICEPOINT;

                CompleteStreamSRB (pSrb);

            } // if we have an SRB
            else 
			{
                //
                // No buffer was available when we should have captured one

                // Increment the counter which keeps track of
                // dropped frames

                pStrmEx->FrameInfo.DropCount++;

                // Set the (local) discontinuity flag
                // This will cause the next packet processed to have the
                //   KSSTREAM_HEADER_OPTIONSF_DATADISCONTINUITY flag set.

                pStrmEx->fDiscontinuity = TRUE;

            }

            // Figure out when to capture the next frame
            pStrmEx->QST_NextFrame += pStrmEx->AvgTimePerFrame;

        } // endif time to capture a frame
    } // endif we're running
}


/*
** VideoSetState()
**
**    Sets the current state for a given stream
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoSetState(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PHW_DEVICE_EXTENSION        pHwDevExt = ((PHW_DEVICE_EXTENSION)pSrb->HwDeviceExtension);
    
	//this is the particular unique stream
	PSTREAMEX                   pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;

    int                         StreamNumber = pStrmEx->pStreamObject->StreamNumber;
    KSSTATE                     PreviousState;

    //
    // For each stream, the following states are used:
    //
    // Stop:    Absolute minimum resources are used.  No outstanding IRPs.
    // Acquire: KS only state that has no DirectShow correpondence
    //          Acquire needed resources.
    // Pause:   Getting ready to run.  Allocate needed resources so that
    //          the eventual transition to Run is as fast as possible.
    //          Read SRBs will be queued at either the Stream class
    //          or in your driver (depending on when you send "ReadyForNext")
    //          and whether you're using the Stream class for synchronization
    // Run:     Streaming.
    //
    // Moving to Stop to Run always transitions through Pause.
    //
    // But since a client app could crash unexpectedly, drivers should handle
    // the situation of having outstanding IRPs cancelled and open streams
    // being closed WHILE THEY ARE STREAMING!
    //
    // Note that it is quite possible to transition repeatedly between states:
    // Stop -> Pause -> Stop -> Pause -> Run -> Pause -> Run -> Pause -> Stop
    //

    //
    // Remember the state we're transitioning away from
    //

    PreviousState = pStrmEx->KSState;

    //
    // Set the new state
    //

    pStrmEx->KSState = pSrb->CommandData.StreamState;

    switch (pSrb->CommandData.StreamState)

    {
    case KSSTATE_STOP:

        //
        // The stream class will cancel all outstanding IRPs for us
        // (but only if it is maintaining the queue ie. using Stream Class synchronization)
        // Since Testcap is not using Stream Class synchronization, we must clear the queue here

        VideoQueueCancelAllSRBs (pStrmEx);

        break;

    case KSSTATE_ACQUIRE:

        //
        // This is a KS only state, that has no correspondence in DirectShow
        //
        break;

    case KSSTATE_PAUSE:

        //
        // On a transition to pause from acquire or stop, start our timer running.
        //

        if (PreviousState == KSSTATE_ACQUIRE || PreviousState == KSSTATE_STOP) 
		{
            // Zero the frame counters
            pStrmEx->FrameInfo.PictureNumber = 0;
            pStrmEx->FrameInfo.DropCount = 0;
            pStrmEx->FrameInfo.dwFrameFlags = 0;

            // this routine will call the capture video from time to time and it is this routine that makes it all happen
			// it passes the unique stream with id and everything else
            VideoTimerRoutine(pStrmEx);
        }

        break;

    case KSSTATE_RUN:

        //
        // Begin Streaming.
        //

        // Reset the discontinuity flag

        pStrmEx->fDiscontinuity = FALSE;

        // Setting the NextFrame time to zero will cause the value to be
        // reset from the stream time

        pStrmEx->QST_NextFrame = 0;

        break;

    } // end switch (pSrb->CommandData.StreamState)
}

/*
** VideoGetState()
**
**    Gets the current state of the requested stream
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoGetState(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAMEX               pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;

    pSrb->CommandData.StreamState = pStrmEx->KSState;
    pSrb->ActualBytesTransferred = sizeof (KSSTATE);

    // A very odd rule:
    // When transitioning from stop to pause, DShow tries to preroll
    // the graph.  Capture sources can't preroll, and indicate this
    // by returning VFW_S_CANT_CUE in user mode.  To indicate this
    // condition from drivers, they must return STATUS_NO_DATA_DETECTED

    if (pStrmEx->KSState == KSSTATE_PAUSE) {
       pSrb->Status = STATUS_NO_DATA_DETECTED;
    }
}


/*
** VideoStreamGetConnectionProperty()
**
**    Gets the properties for a stream
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoStreamGetConnectionProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property
    int  streamNumber = (int)pSrb->StreamObject->StreamNumber;

    switch (Id) {
        // This property describes the allocator requirements for the stream
        case KSPROPERTY_CONNECTION_ALLOCATORFRAMING:
        {
            PKSALLOCATOR_FRAMING Framing =
                (PKSALLOCATOR_FRAMING) pSPD->PropertyInfo;
            Framing->RequirementsFlags =
                KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY |
                KSALLOCATOR_REQUIREMENTF_INPLACE_MODIFIER |
                KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY;
            Framing->PoolType = PagedPool;
            Framing->FileAlignment = 0; // FILE_LONG_ALIGNMENT???;
            Framing->Reserved = 0;
            pSrb->ActualBytesTransferred = sizeof (KSALLOCATOR_FRAMING);

            switch (streamNumber) {
				case STREAM_Capture:
                case STREAM_Preview:
                    Framing->Frames = 2;
                    Framing->FrameSize =
                        pStrmEx->pVideoInfoHeader->bmiHeader.biSizeImage;
                    break;

                case STREAM_VBI:
                    Framing->Frames = 8;
                    Framing->FrameSize = StreamFormatVBI.DataRange.SampleSize;
                    break;

                case STREAM_CC:
                    Framing->Frames = 100;
                    Framing->FrameSize = StreamFormatCC.SampleSize;
                    break;

                case STREAM_NABTS:
                    Framing->Frames = 20;
                    Framing->FrameSize = StreamFormatNABTS.SampleSize;
                    break;

                case STREAM_AnalogVideoInput:
                default:
                    pSrb->Status = STATUS_INVALID_PARAMETER;
                    break;
            }
            break;
        }

        default:
            TRAP;
            break;
    }
}

/*
** VideoStreamGetDroppedFramesProperty()
**
**    Gets dynamic information about the progress of the capture process.
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoStreamGetDroppedFramesProperty(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;
    PSTREAM_PROPERTY_DESCRIPTOR pSPD = pSrb->CommandData.PropertyInfo;
    ULONG Id = pSPD->Property->Id;              // index of the property

    switch (Id) {

    case KSPROPERTY_DROPPEDFRAMES_CURRENT:
        {
            PKSPROPERTY_DROPPEDFRAMES_CURRENT_S pDroppedFrames =
                (PKSPROPERTY_DROPPEDFRAMES_CURRENT_S) pSPD->PropertyInfo;

            pDroppedFrames->PictureNumber = pStrmEx->FrameInfo.PictureNumber;
            pDroppedFrames->DropCount = pStrmEx->FrameInfo.DropCount;
            pDroppedFrames->AverageFrameSize = pStrmEx->pVideoInfoHeader->bmiHeader.biSizeImage;

            pSrb->ActualBytesTransferred = sizeof (KSPROPERTY_DROPPEDFRAMES_CURRENT_S);
        }
        break;

    default:
        TRAP;
        break;
    }
}

//==========================================================================;
//                   Clock Handling Routines
//==========================================================================;


/*
** VideoIndicateMasterClock ()
**
**    If this stream is not being used as the master clock, this function
**      is used to provide us with a handle to the clock to use when
**      requesting the current stream time.
**
** Arguments:
**
**    pSrb - pointer to the stream request block for properties
**
** Returns:
**
** Side Effects:  none
*/

VOID
STREAMAPI
VideoIndicateMasterClock(
    PHW_STREAM_REQUEST_BLOCK pSrb
    )
{
    PSTREAMEX pStrmEx = (PSTREAMEX)pSrb->StreamObject->HwStreamExtension;

    pStrmEx->hMasterClock = pSrb->CommandData.MasterClockHandle;
}



