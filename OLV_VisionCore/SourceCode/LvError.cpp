#include "../Include/LvError.h"

namespace LvError
{

    /**
    * @brief Sets the fields of a LabVIEW Error Cluster.
    */
    void setErrorCluster(ErrorClusterPtr errorCluster, LVBoolean status, int32 code, const std::string& message)
    {
        errorCluster->status = status;
        errorCluster->code = code;
        LvString::setLvStringHandle(errorCluster->source, message);
    }

    /**
    * @brief Converts a LabVIEW MgErr code to its symbolic string name.
    */
    std::string mgErrToString(int32 errorCode)
    {

        switch (errorCode)
        {
        case mgNoErr: return "mgNoErr";
        case mgArgErr: return "mgArgErr";
        case mFullErr: return "mFullErr";
        case mZoneErr: return "mZoneErr";
        case fEOF: return "fEOF";
        case fIsOpen: return "fIsOpen";
        case fIOErr: return "fIOErr";
        case fNotFound: return "fNotFound";
        case fNoPerm: return "fNoPerm";
        case fDiskFull: return "fDiskFull";
        case fDupPath: return "fDupPath";
        case fTMFOpen: return "fTMFOpen";
        case fNotEnabled: return "fNotEnabled";
        case rFNotFound: return "rFNotFound";
        case rAddFailed: return "rAddFailed";
        case rNotFound: return "rNotFound";
        case iNotFound: return "iNotFound";
        case iMemoryErr: return "iMemoryErr";
        case dPenNotExist: return "dPenNotExist";
        case cfgBadType: return "cfgBadType";
        case cfgTokenNotFound: return "cfgTokenNotFound";
        case cfgParseError: return "cfgParseError";
        case cfgAllocError: return "cfgAllocError";
        case ecLVSBFormatError: return "ecLVSBFormatError";
        case ecLVSBSubrError: return "ecLVSBSubrError";
        case ecLVSBNoCodeError: return "ecLVSBNoCodeError";
        case wNullWindow: return "wNullWindow";
        case wDestroyMixup: return "wDestroyMixup";
        case menuNullMenu: return "menuNullMenu";
        case pAbortJob: return "pAbortJob";
        case pBadPrintRecord: return "pBadPrintRecord";
        case pDriverError: return "pDriverError";
        case pWindowsError: return "pWindowsError";
        case pMemoryError: return "pMemoryError";
        case pDialogError: return "pDialogError";
        case pMiscError: return "pMiscError";
        case dvInvalidRefnum: return "dvInvalidRefnum";
        case dvDeviceNotFound: return "dvDeviceNotFound";
        case dvParamErr: return "dvParamErr";
        case dvUnitErr: return "dvUnitErr";
        case dvOpenErr: return "dvOpenErr";
        case dvAbortErr: return "dvAbortErr";
        case bogusError: return "bogusError";
        case cancelError: return "cancelError";
        case OMObjLowErr: return "OMObjLowErr";
        case OMObjHiErr: return "OMObjHiErr";
        case OMObjNotInHeapErr: return "OMObjNotInHeapErr";
        case OMOHeapNotKnownErr: return "OMOHeapNotKnownErr";
        case OMBadDPIdErr: return "OMBadDPIdErr";
        case OMNoDPinTabErr: return "OMNoDPinTabErr";
        case OMMsgOutOfRangeErr: return "OMMsgOutOfRangeErr";
        case OMMethodNullErr: return "OMMethodNullErr";
        case OMUnknownMsgErr: return "OMUnknownMsgErr";
        case mgNotSupported: return "mgNotSupported";
        case ncBadAddressErr: return "ncBadAddressErr";
        case ncInProgress: return "ncInProgress";
        case ncTimeOutErr: return "ncTimeOutErr";
        case ncBusyErr: return "ncBusyErr";
        case ncNotSupportedErr: return "ncNotSupportedErr";
        case ncNetErr: return "ncNetErr";
        case ncAddrInUseErr: return "ncAddrInUseErr";
        case ncSysOutOfMem: return "ncSysOutOfMem";
        case ncSysConnAbortedErr: return "ncSysConnAbortedErr";
        case ncConnRefusedErr: return "ncConnRefusedErr";
        case ncNotConnectedErr: return "ncNotConnectedErr";
        case ncAlreadyConnectedErr: return "ncAlreadyConnectedErr";
        case ncConnClosedErr: return "ncConnClosedErr";
        case amInitErr: return "amInitErr";
        case occBadOccurrenceErr: return "occBadOccurrenceErr";
        case occWaitOnUnBoundHdlrErr: return "occWaitOnUnBoundHdlrErr";
        case occFunnyQOverFlowErr: return "occFunnyQOverFlowErr";
        case fDataLogTypeConflict: return "fDataLogTypeConflict";
        case ecLVSBCannotBeCalledFromThread: return "ecLVSBCannotBeCalledFromThread";
        case amUnrecognizedType: return "amUnrecognizedType";
        case mCorruptErr: return "mCorruptErr";
        case ecLVSBErrorMakingTempDLL: return "ecLVSBErrorMakingTempDLL";
        case ecLVSBOldCIN: return "ecLVSBOldCIN";
        case dragSktNotFound: return "dragSktNotFound";
        case dropLoadErr: return "dropLoadErr";
        case oleRegisterErr: return "oleRegisterErr";
        case oleReleaseErr: return "oleReleaseErr";
        case fmtTypeMismatch: return "fmtTypeMismatch";
        case fmtUnknownConversion: return "fmtUnknownConversion";
        case fmtTooFew: return "fmtTooFew";
        case fmtTooMany: return "fmtTooMany";
        case fmtScanError: return "fmtScanError";
        case ecLVSBFutureCIN: return "ecLVSBFutureCIN";
        case lvOLEConvertErr: return "lvOLEConvertErr";
        case rtMenuErr: return "rtMenuErr";
        case pwdTampered: return "pwdTampered";
        case LvVariantAttrNotFound: return "LvVariantAttrNotFound";
        case LvVariantTypeMismatch: return "LvVariantTypeMismatch";
        case axEventDataNotAvailable: return "axEventDataNotAvailable";
        case axEventStoreNotPresent: return "axEventStoreNotPresent";
        case axOccurrenceNotFound: return "axOccurrenceNotFound";
        case axEventQueueNotCreated: return "axEventQueueNotCreated";
        case axEventInfoNotAvailable: return "axEventInfoNotAvailable";
        case oleNullRefnumPassed: return "oleNullRefnumPassed";
        case omidGetClassGUIDErr: return "omidGetClassGUIDErr";
        case omidGetCoClassGUIDErr: return "omidGetCoClassGUIDErr";
        case omidGetTypeLibGUIDErr: return "omidGetTypeLibGUIDErr";
        case appMagicBad: return "appMagicBad";
        case iviInvalidDowncast: return "iviInvalidDowncast";
        case iviInvalidClassSesn: return "iviInvalidClassSesn";
        case maxErr: return "maxErr";
        case maxConfigErr: return "maxConfigErr";
        case maxConfigLoadErr: return "maxConfigLoadErr";
        case maxGroupNotSupported: return "maxGroupNotSupported";
        case ncSockNotMulticast: return "ncSockNotMulticast";
        case ncSockNotSinglecast: return "ncSockNotSinglecast";
        case ncBadMulticastAddr: return "ncBadMulticastAddr";
        case ncMcastSockReadOnly: return "ncMcastSockReadOnly";
        case ncMcastSockWriteOnly: return "ncMcastSockWriteOnly";
        case ncDatagramMsgSzErr: return "ncDatagramMsgSzErr";
        case bufferEmpty: return "bufferEmpty";
        case bufferFull: return "bufferFull";
        case dataCorruptErr: return "dataCorruptErr";
        case requireFullPathErr: return "requireFullPathErr";
        case folderNotExistErr: return "folderNotExistErr";
        case ncBtInvalidModeErr: return "ncBtInvalidModeErr";
        case ncBtSetModeErr: return "ncBtSetModeErr";
        case mgBtInvalidGUIDStrErr: return "mgBtInvalidGUIDStrErr";
        case rVersInFuture: return "rVersInFuture";
        case mgErrTimeOut: return "mgErrTimeOut";
        case mgErrStringCannotContainNull: return "mgErrStringCannotContainNull";
        case mgErrStackOverflow: return "mgErrStackOverflow";
        case ncSocketQueryFailed: return "ncSocketQueryFailed";
        case ncNotInetSocket: return "ncNotInetSocket";
        case ncTooManySocketsErr: return "ncTooManySocketsErr";
        case mgPathTooLong: return "mgPathTooLong";
        case mgDupeEltInSet: return "mgDupeEltInSet";
        case mgDupeKeyInMap: return "mgDupeKeyInMap";
            // Add more specific codes here if needed
        case kAppLicenseErr: return "kAppLicenseErr";
        case kAppCharsetConvertErr: return "kAppCharsetConvertErr";
        case kAppInvalidUTF8ByteSequence: return "kAppInvalidUTF8ByteSequence";
        default:
            // Handle ranges or unknown codes
            if (errorCode >= mgPrivErrBase && errorCode <= mgPrivErrLast) {
                return "Private Error Code";
            }

            if (errorCode >= kAppErrorBase && errorCode <= kAppErrorLast) {
                return "Application Error Code";
            }
            return "Unknown Error Code";
        }
    }

    /**
     * @brief Wraps C++ calls, handles errors/exceptions, reports via Error Cluster.
     */
    void handleError(ErrorClusterPtr errorCluster, const std::function<MgErr()>& function)
    {

        MgErr functionErrorCode = mgNoErr;
        std::string errorMessage = "";
        LVBoolean errorStatus = LVBooleanFalse;

        //Labiew Manager Exeption
        try
        {
            functionErrorCode = function();

            if (functionErrorCode != mgNoErr)
            {
                errorStatus = LVBooleanTrue;
                errorMessage = "LabVIEW C API Error: " + mgErrToString(functionErrorCode) + " (Code " + std::to_string(functionErrorCode) + ")";
            }
        }
        //Standard C++ Exception
        catch (const std::exception& ex)
        {
            errorStatus = LVBooleanTrue;
            functionErrorCode = 5998;
            errorMessage = "C++ Exception: " + std::string(ex.what());
        }
        //Unkown Exception
        catch (...)
        {
            errorStatus = LVBooleanTrue;
            functionErrorCode = 5997;
            errorMessage = "Error: An unknown C++ exception occurred.";
        }

        // Update cluster only if an error occurred.
        if (errorStatus == LVBooleanTrue)
        {
            setErrorCluster(errorCluster, errorStatus, functionErrorCode, errorMessage);
        }
    }

}