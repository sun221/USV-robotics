#include "Output.h"
#include "RTProtocol.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <float.h>

void COutput::PrintGeneralSettings(CRTProtocol* poRTProtocol)
{
    CRTProtocol::SPoint       sPoint;
    unsigned int              nMarkerExposure, nMarkerExposureMin, nMarkerExposureMax;
    unsigned int              nMarkerThreshold, nMarkerThresholdMin, nMarkerThresholdMax;
    unsigned int              nVideoExposure, nVideoExposureMin, nVideoExposureMax;
    unsigned int              nVideoFlashTime, nVideoFlashTimeMin, nVideoFlashTimeMax;
    float                     fvRotationMatrix[3][3];
    int                       nOrientation;
    unsigned int              nMarkerResolutionWidth, nMarkerResolutionHeight, nVideoResolutionWidth, nVideoResolutionHeight;
    unsigned int              nMarkerFOVLeft, nMarkerFOVTop, nMarkerFOVRight, nMarkerFOVBottom;
    unsigned int              nVideoFOVLeft, nVideoFOVTop, nVideoFOVRight, nVideoFOVBottom;


    printf("================ General Settings ================\n\n");

    unsigned int nCaptureFrequency;
    float        fCaptureTime;
    bool         bStartOnExtTrig;
    bool         bStartOnTriggerNO;
    bool         bStartOnTriggerNC;
    bool         bStartOnTrigSoftware;
    CRTProtocol::EProcessingActions eProcessingActions[3];

    poRTProtocol->GetSystemSettings(
        nCaptureFrequency, fCaptureTime,
        bStartOnExtTrig, bStartOnTriggerNO, bStartOnTriggerNC, bStartOnTrigSoftware,
        eProcessingActions[0], eProcessingActions[1], eProcessingActions[2]);

    printf("Capture frequency: %d\n", nCaptureFrequency);
    printf("Capture time: %f\n",      fCaptureTime);
    
    // Start on external trigger is redundant when we inspect StartOnTrigger(NO/NC/Software) individually.
    //printf("Start on external trigger: %s\n", bStartOnExtTrig ? "True" : "False");
    printf("Start on trigger NO (Normally Open): %s\n", bStartOnTriggerNO ? "True" : "False");
    printf("Start on trigger NC (Normally Closed): %s\n", bStartOnTriggerNC ? "True" : "False");
    printf("Start on software trigger: %s\n\n", bStartOnTrigSoftware ? "True" : "False");

    _TCHAR* processings[3] = { "----- Processing Actions-----", "----- Real-Time Processing Actions-----", "----- Reprocessing Actions-----" };

    unsigned int majorVersion;
    unsigned int minorVersion;
    poRTProtocol->GetVersion(majorVersion, minorVersion);
    auto actionsCount = (majorVersion > 1 || minorVersion > 13) ? 3 : 1;

    for (auto i = 0; i < 3; i++)
    {
        printf("%s\n",processings[i]);

        if (majorVersion > 1 || minorVersion > 13)
        {
            if (eProcessingActions[i] & CRTProtocol::ProcessingPreProcess2D)
            {
                printf("2D pre-processing\n");
            }
        }
        if (eProcessingActions[i] & CRTProtocol::ProcessingTracking2D)
        {
            printf("2D Tracking\n");
        }
        else if (eProcessingActions[i] & CRTProtocol::ProcessingTracking3D)
        {
            printf("3D Tracking\n");
        }
        if (eProcessingActions[i] & CRTProtocol::ProcessingTwinSystemMerge)
        {
            printf("Twin system merge\n");
        }
        if (eProcessingActions[i] & CRTProtocol::ProcessingSplineFill)
        {
            printf("Spline Fill\n");
        }
        if (eProcessingActions[i] & CRTProtocol::ProcessingAIM)
        {
            printf("AIM\n");
        }
        if (eProcessingActions[i] & CRTProtocol::Processing6DOFTracking)
        {
            printf("6DOF Tracking\n");
        }
        if (eProcessingActions[i] & CRTProtocol::ProcessingForceData)
        {
            printf("Force Calculation\n");
        }
        if (eProcessingActions[i] & CRTProtocol::ProcessingGazeVector)
        {
            printf("Gaze Vector Calculation\n");
        }
        if (eProcessingActions[i] & CRTProtocol::ProcessingExportTSV)
        {
            printf("Export TSV\n");
        }
        if (eProcessingActions[i] & CRTProtocol::ProcessingExportC3D)
        {
            printf("Export C3D\n");
        }
        if (eProcessingActions[i] & CRTProtocol::ProcessingExportMatlabFile)
        {
            printf("Export MATLAB File\n");
        }

        if (eProcessingActions[i] & CRTProtocol::ProcessingExportAviFile)
        {
            printf("Export AVI File\n");
        }
    }

    bool                       bEnabled;
    CRTProtocol::ESignalSource eSignalSource;
    bool                       bSignalModePeriodic;
    unsigned int               nFreqMultiplier;
    unsigned int               nFreqDivisor;
    unsigned int               nFreqTolerance;
    float                      fNominalFrequency;
    bool                       bNegativeEdge;
    unsigned int               nSignalShutterDelay;
    float                      fNonPeriodicTimeout;

    poRTProtocol->GetExtTimeBaseSettings(bEnabled, eSignalSource, bSignalModePeriodic, nFreqMultiplier,
        nFreqDivisor, nFreqTolerance, fNominalFrequency, bNegativeEdge,
        nSignalShutterDelay, fNonPeriodicTimeout);

    printf("\n----- External Time Base-----\n\n");
    printf("External Time Base:   %s\n", bEnabled ? "Enabled" : "Disabled");
    if (bEnabled)
    {
        printf("Signal Source:        ");
        switch (eSignalSource)
        {
        case CRTProtocol::SourceControlPort :
            printf("Control Port\n");
            break;
        case CRTProtocol::SourceIRReceiver :
            printf("IR Receiver\n");
            break;
        case CRTProtocol::SourceSMPTE :
            printf("SMPTE\n");
            break;
        case CRTProtocol::SourceVideoSync :
            printf("Video Sync\n");
            break;
        }
        printf("Signal Mode Periodic: %s\n", bSignalModePeriodic ? "True" : "False");
        if (bSignalModePeriodic)
        {
            printf("Frequency Multiplier: %d\n", nFreqMultiplier);
            printf("Frequency Divisor:    %d\n", nFreqDivisor);
            if (eSignalSource != CRTProtocol::SourceSMPTE)
            {
                printf("Frequency Tolerance:  %d ppm\n", nFreqTolerance);
            }
            if (fNominalFrequency == -1)
            {
                printf("Nominal Frequency:    None\n");
            }
            else
            {
                printf("Nominal Frequency:    %f Hz\n", fNominalFrequency);
            }
        }
        if (eSignalSource == CRTProtocol::SourceControlPort || eSignalSource == CRTProtocol::SourceVideoSync)
        {
            printf("Signal Edge:          %s\n", bNegativeEdge ? "Negative" : "Positive"); 
        }
        printf("Signal Shutter Delay: %d us\n", nSignalShutterDelay);
        if ((eSignalSource == CRTProtocol::SourceControlPort || eSignalSource == CRTProtocol::SourceIRReceiver ||
            eSignalSource == CRTProtocol::SourceVideoSync) && !bSignalModePeriodic)
        {
            printf("Non Periodic Timeout: %f s\n\n", fNonPeriodicTimeout);
        }
    }

    unsigned int                  nID;
    CRTProtocol::ECameraModel     eModel;
    bool                          bUnderwater;
    bool                          bSupportsHwSync;
    unsigned int                  nSerial;
    CRTProtocol::ECameraMode      eMode;
    CRTProtocol::ECameraVideoMode eVideoMode;
    unsigned int                  nVideoFrequency;

    printf("\n----- Camera Settings-----\n\n");

    for (unsigned int iCamera = 0; iCamera < poRTProtocol->GetCameraCount(); iCamera++)
    {
        if (poRTProtocol->GetCameraSettings(iCamera, nID, eModel, bUnderwater, bSupportsHwSync, nSerial, eMode))
        {
            printf("Camera ID: %d\n", nID);
            switch (eModel)
            {
            case CRTProtocol::ModelMacReflex :
                printf("  Model: MacReflex  ");
                break;
            case CRTProtocol::ModelProReflex120 :
                printf("  Model: ProReflex 120  ");
                break;
            case CRTProtocol::ModelProReflex240 :
                printf("  Model: ProReflex 240  ");
                break;
            case CRTProtocol::ModelProReflex500 :
                printf("  Model: ProReflex 500  ");
                break;
            case CRTProtocol::ModelProReflex1000 :
                printf("  Model: ProReflex 1000  ");
                break;
            case CRTProtocol::ModelOqus100 :
                printf("  Model: Oqus 100  ");
                break;
            case CRTProtocol::ModelOqus200C :
                printf("  Model: Oqus 200 C  ");
                break;
            case CRTProtocol::ModelOqus300 :
                printf("  Model: Oqus 300  ");
                break;
            case CRTProtocol::ModelOqus300Plus :
                printf("  Model: Oqus 300 Plus  ");
                break;
            case CRTProtocol::ModelOqus400 :
                printf("  Model: Oqus 400  ");
                break;
            case CRTProtocol::ModelOqus500 :
                printf("  Model: Oqus 500  ");
                break;
            case CRTProtocol::ModelOqus500Plus :
                printf("  Model: Oqus 500 Plus  ");
                break;
            case CRTProtocol::ModelOqus700 :
                printf("  Model: Oqus 700  ");
                break;
            case CRTProtocol::ModelOqus700Plus :
                printf("  Model: Oqus 700 Plus  ");
                break;
            case CRTProtocol::ModelMiqusM1:
                printf("  Model: Miqus M1  ");
                break;
            case CRTProtocol::ModelMiqusM3:
                printf("  Model: Miqus M3  ");
                break;
            case CRTProtocol::ModelMiqusM5:
                printf("  Model: Miqus M5  ");
                break;
            case CRTProtocol::ModelMiqusSyncUnit:
                printf("  Model: Miqus Sync Unit  ");
                break;
            }
            printf("  %s\n", bUnderwater ? "Underwater" : "");
            printf("  Serial: %d\n", nSerial);
            printf("  Supports Hardware Sync: %s\n", bSupportsHwSync ? "True" : "False");

            if (eModel != CRTProtocol::ModelMiqusSyncUnit)
            {
                switch (eMode)
                {
                case CRTProtocol::ModeMarker:
                    printf("  Mode: Marker\n");
                    break;
                case CRTProtocol::ModeMarkerIntensity:
                    printf("  Mode: Marker Intensity\n");
                    break;
                case CRTProtocol::ModeVideo:
                    printf("  Mode: Video\n");
                    break;
                }
            }
        }

        if (eModel != CRTProtocol::ModelMiqusSyncUnit)
        {
            if (poRTProtocol->GetCameraMarkerSettings(iCamera, nMarkerExposure, nMarkerExposureMin, nMarkerExposureMax,
                nMarkerThreshold, nMarkerThresholdMin, nMarkerThresholdMax))
            {
                printf("  Marker Exposure:   ");
                printf("Current: %-7d", nMarkerExposure);
                printf("Min: %-7d", nMarkerExposureMin);
                printf("Max: %-7d\n", nMarkerExposureMax);

                printf("  Marker Threshold:  ");
                printf("Current: %-7d", nMarkerThreshold);
                printf("Min: %-7d", nMarkerThresholdMin);
                printf("Max: %-7d\n", nMarkerThresholdMax);
            }

            if (poRTProtocol->GetCameraVideoSettings(iCamera, eVideoMode, nVideoFrequency, nVideoExposure, nVideoExposureMin, nVideoExposureMax,
                nVideoFlashTime, nVideoFlashTimeMin, nVideoFlashTimeMax))
            {
                printf("  Video Mode: ");
                switch (eVideoMode)
                {
                case CRTProtocol::VideoModeCustom:
                    printf("Custom\n");
                    break;
                case CRTProtocol::VideoMode1080p_24hz:
                    printf("1024p 24 Hz\n");
                    break;
                case CRTProtocol::VideoMode720p_25hz:
                    printf("720p 25 Hz\n");
                    break;
                case CRTProtocol::VideoMode720p_50hz:
                    printf("720p 50 Hz\n");
                    break;
                case CRTProtocol::VideoMode540p_25hz:
                    printf("540p 25 Hz\n");
                    break;
                case CRTProtocol::VideoMode540p_50hz:
                    printf("540p 50 Hz\n");
                    break;
                case CRTProtocol::VideoMode540p_60hz:
                    printf("540p 60 Hz\n");
                    break;
                case CRTProtocol::VideoMode480p_25hz:
                    printf("480p 25 Hz\n");
                    break;
                case CRTProtocol::VideoMode480p_50hz:
                    printf("480p 50 Hz\n");
                    break;
                case CRTProtocol::VideoMode480p_60hz:
                    printf("480p 60 Hz\n");
                    break;
                case CRTProtocol::VideoMode480p_120hz:
                    printf("480p 120 Hz\n");
                    break;
                }
                printf("  Video Frequency: %d Hz\n", nVideoFrequency);

                printf("  Video Exposure:    ");
                printf("Current: %-7d", nVideoExposure);
                printf("Min: %-7d", nVideoExposureMin);
                printf("Max: %-7d\n", nVideoExposureMax);

                printf("  Video Flash Time:  ");
                printf("Current: %-7d", nVideoFlashTime);
                printf("Min: %-7d", nVideoFlashTimeMin);
                printf("Max: %-7d\n", nVideoFlashTimeMax);
            }

            if (poRTProtocol->GetCameraPosition(iCamera, sPoint, fvRotationMatrix))
            {
                printf("  Position:  X: %f  Y: %f  Z: %f\n", sPoint.fX, sPoint.fY, sPoint.fZ);

                printf("  Rotation Matrix:\n");
                printf("   %9f ", fvRotationMatrix[0][0]);
                printf("   %9f ", fvRotationMatrix[1][0]);
                printf("   %9f\n", fvRotationMatrix[2][0]);
                printf("   %9f ", fvRotationMatrix[0][1]);
                printf("   %9f ", fvRotationMatrix[1][1]);
                printf("   %9f\n", fvRotationMatrix[2][1]);
                printf("   %9f ", fvRotationMatrix[0][2]);
                printf("   %9f ", fvRotationMatrix[1][2]);
                printf("   %9f\n", fvRotationMatrix[2][2]);
            }

            if (poRTProtocol->GetCameraOrientation(iCamera, nOrientation))
            {
                printf("  Rotation: %d\n", nOrientation);
            }

            if (poRTProtocol->GetCameraResolution(iCamera, nMarkerResolutionWidth, nMarkerResolutionHeight,
                nVideoResolutionWidth, nVideoResolutionHeight))
            {
                printf("  Marker Resolution:  Width: %-6d Height: %d\n", nMarkerResolutionWidth, nMarkerResolutionHeight);

                printf("  Video Resolution:   Width: %-6d Height: %d\n", nVideoResolutionWidth, nVideoResolutionHeight);
            }

            if (poRTProtocol->GetCameraFOV(iCamera, nMarkerFOVLeft, nMarkerFOVTop, nMarkerFOVRight, nMarkerFOVBottom,
                nVideoFOVLeft, nVideoFOVTop, nVideoFOVRight, nVideoFOVBottom))
            {
                printf("  Marker FOV:  Left: %-5d Top: %-5d Right: %-5d Bottom: %d\n",
                    nMarkerFOVLeft, nMarkerFOVTop, nMarkerFOVRight, nMarkerFOVBottom);
                printf("  Video FOV:   Left: %-5d Top: %-5d Right: %-5d Bottom: %d\n\n",
                    nVideoFOVLeft, nVideoFOVTop, nVideoFOVRight, nVideoFOVBottom);
            }
        }

        if (bSupportsHwSync)

        {
            CRTProtocol::ESyncOutFreqMode eSyncOutMode;
            unsigned int                  nSyncOutValue;
            float                         fSyncOutDutyCycle;
            bool                          bSyncOutNegativePolarity;

            for (unsigned int portNumber = 1; portNumber < 4; portNumber++)
            {
                if (poRTProtocol->GetCameraSyncOutSettings(iCamera, portNumber, eSyncOutMode, nSyncOutValue, fSyncOutDutyCycle,
                    bSyncOutNegativePolarity))
                {
                    if (portNumber == 1 || portNumber == 2)
                    {
                        switch (eSyncOutMode)
                        {
                        case CRTProtocol::ModeShutterOut:
                            printf("  Sync Out%d Mode:   Shutter Out\n", portNumber);
                            printf("   Signal Polarity: %s\n", bSyncOutNegativePolarity ? "Negative" : "Positive");
                            break;
                        case CRTProtocol::ModeMultiplier:
                            printf("  Sync Out%d Mode:   Multiplier = %d\n", portNumber, nSyncOutValue);
                            printf("   Duty Cycle:      %f %%\n", fSyncOutDutyCycle);
                            printf("   Signal Polarity: %s\n", bSyncOutNegativePolarity ? "Negative" : "Positive");
                            break;
                        case CRTProtocol::ModeDivisor:
                            printf("  Sync Out%d Mode:   Divisor = %d\n", portNumber, nSyncOutValue);
                            printf("   Duty Cycle:      %f %%\n", fSyncOutDutyCycle);
                            printf("   Signal Polarity: %s\n", bSyncOutNegativePolarity ? "Negative" : "Positive");
                            break;
                        case CRTProtocol::ModeActualFreq:
                            printf("  Sync Out%d Mode:   Actual Frequency = %d\n", portNumber, nSyncOutValue);
                            printf("   Duty Cycle:      %f %%\n", fSyncOutDutyCycle);
                            printf("   Signal Polarity: %s\n", bSyncOutNegativePolarity ? "Negative" : "Positive");
                            break;
                        case CRTProtocol::ModeMeasurementTime:
                            printf("  Sync Out%d Mode:   Measurement Time\n", portNumber);
                            printf("   Signal Polarity: %s\n", bSyncOutNegativePolarity ? "Negative" : "Positive");
                            break;
                        case CRTProtocol::ModeFixed100Hz:
                            printf("  Sync Out%d Mode: Fixed 100 Hz\n", portNumber);
                            break;
                        }
                    }
                    if (portNumber == 3)
                    {
                        printf("  Sync Out MT Signal Polarity: %s\n", bSyncOutNegativePolarity ? "Negative" : "Positive");
                    }
                }
            }
        }
    }
}

void COutput::Print3DSettings(CRTProtocol* poRTProtocol)
{
    printf("================== 3D Settings ===================\n\n");

    switch (poRTProtocol->Get3DUpwardAxis())
    {
    case CRTProtocol::XPos :
        printf("Axis upwards: +X\n");
        break;
    case CRTProtocol::XNeg :
        printf("Axis upwards: -X\n");
        break;
    case CRTProtocol::YPos :
        printf("Axis upwards: +Y\n");
        break;
    case CRTProtocol::YNeg :
        printf("Axis upwards: -Y\n");
        break;
    case CRTProtocol::ZPos :
        printf("Axis upwards: +Z\n");
        break;
    case CRTProtocol::ZNeg :
        printf("Axis upwards: -Z\n");
        break;
    }

    const char* pTmpStr = poRTProtocol->Get3DCalibrated();
    if (pTmpStr[0] == 0)
    {
        printf("Calibration Time: Not Calibrated\n");
    }
    else
    {
        printf("Calibration Time: %s\n", pTmpStr);
    }

    unsigned int nCount = poRTProtocol->Get3DLabeledMarkerCount();
    printf("There are %d labeled markers\n", nCount);
    for (unsigned int iLabel = 0; iLabel < nCount; iLabel++)
    {
        printf("Marker %2d: %s", iLabel + 1, poRTProtocol->Get3DLabelName(iLabel));
        printf("   Color: %.6X\n", poRTProtocol->Get3DLabelColor(iLabel));
    }

    nCount = poRTProtocol->Get3DBoneCount();
    printf("There are %d bones\n", nCount);
    for (unsigned int iBone = 0; iBone < nCount; iBone++)
    {
        printf("Bone %2d: From: %s -> To: %s\n", iBone + 1, poRTProtocol->Get3DBoneFromName(iBone), poRTProtocol->Get3DBoneToName(iBone));
    }

    printf("\n");
}

void COutput::Print6DOFSettings(CRTProtocol* poRTProtocol)
{
    CRTProtocol::SPoint sPoint;

    int nBodies = poRTProtocol->Get6DOFBodyCount();

    if (nBodies > 0)
    {
        printf("================== 6DOF Settings =================\n\n");

        if (nBodies == 1)
        {
            printf("There is 1 6DOF body\n");
        }
        else
        {
            printf("There are %d 6DOF Bodies\n", nBodies);
        }
        for (int iBody = 0; iBody < nBodies; iBody++)
        {
            printf("Body #%d\n", iBody + 1);
            printf("  Name:  %s\n",   poRTProtocol->Get6DOFBodyName(iBody));
            printf("  Color: %.6X\n", poRTProtocol->Get6DOFBodyColor(iBody));
            for (unsigned int iPoint = 0; iPoint < poRTProtocol->Get6DOFBodyPointCount(iBody); iPoint++)
            {
                if (poRTProtocol->Get6DOFBodyPoint(iBody, iPoint, sPoint))
                {
                    printf("  Point: ");
                    printf("X = %9f   ", sPoint.fX);
                    printf("Y = %9f   ", sPoint.fY);
                    printf("Z = %9f",    sPoint.fZ);
                    printf("\n");
                }
            }
            printf("\n");
        }
    }
}

void COutput::PrintGazeVectorSettings(CRTProtocol* poRTProtocol)
{
    int nGazeVectorCount = poRTProtocol->GetGazeVectorCount();

    if (nGazeVectorCount > 0)
    {
        printf("============== Gaze Vector Settings ==============\n\n");

        if (nGazeVectorCount == 1)
        {
            printf("There is 1 gaze vector\n");
        }
        else
        {
            printf("There are %d gaze vectors\n", nGazeVectorCount);
        }
        for (int iVector = 0; iVector < nGazeVectorCount; iVector++)
        {
            printf("Gaze vector #%d\n", iVector + 1);
            printf("  Name:  %s\n",   poRTProtocol->GetGazeVectorName(iVector));
            printf("\n");
        }
    }
}

void COutput::PrintAnalogSettings(CRTProtocol* poRTProtocol)
{
    unsigned int nMajorVersion;
    unsigned int nMinorVersion;

    poRTProtocol->GetRTPacket()->GetVersion(nMajorVersion, nMinorVersion);

    unsigned int nCount = poRTProtocol->GetAnalogDeviceCount();

    if (nCount > 0)
    {
        unsigned int nDeviceID, nChannels, nFrequency;
        float        fMinRange, fMaxRange;
        char*        pName;
        char*        pUnit;

        printf("================ Analog Settings =================\n\n");

        if (nMajorVersion == 1 && nMinorVersion == 0)
        {
            if (nCount > 0)
            {
                poRTProtocol->GetAnalogDevice(0, nDeviceID, nChannels, pName, nFrequency, pUnit, fMinRange, fMaxRange);

                printf("Analog parameters\n");
                printf("  Channels:  %d\n",   nChannels);
                printf("  Frequency: %d\n",   nFrequency);
                printf("  Unit:      %s\n",   pUnit);
                printf("  Range Min: %f\n",   fMinRange);
                printf("  Range Max: %f\n\n", fMaxRange);
            }
        }
        else
        {
            printf("Analog parameters\n");
            for (unsigned int iDevice = 0; iDevice < nCount; iDevice++)
            {
                poRTProtocol->GetAnalogDevice(iDevice, nDeviceID, nChannels, pName,
                    nFrequency, pUnit, fMinRange, fMaxRange);

                printf("  Analog Device %d, %s\n", nDeviceID, pName);
                printf("    Channels:  %d\n", nChannels);
                printf("    Frequency: %d\n",   nFrequency);
                if (nMajorVersion == 1 && nMinorVersion < 11)
                {
                    printf("    Unit:      %s\n",   pUnit);
                }
                printf("    Range Min: %f\n",   fMinRange);
                printf("    Range Max: %f\n", fMaxRange);
                for (unsigned int iChannel = 0; iChannel < nChannels; iChannel++)
                {
                    printf("    Channel %d\n", iChannel + 1);
                    printf("      Label: %s\n", poRTProtocol->GetAnalogLabel(iDevice, iChannel));
                    if (nMajorVersion > 1 || nMinorVersion > 10)
                    {
                        printf("      Unit:  %s\n", poRTProtocol->GetAnalogUnit(iDevice, iChannel));
                    }
                }
            }
            printf("\n");
        }
    }
}

void COutput::PrintForceSettings(CRTProtocol* poRTProtocol)
{
    unsigned int nCount = poRTProtocol->GetForcePlateCount();

    if (nCount > 0)
    {
        unsigned int        nPlateID, nAnalogDeviceID, nFrequency;
        float               fLength, fWidth;
        char*               pType;
        char*               pName;
        CRTProtocol::SPoint sCorners[4];
        CRTProtocol::SPoint sOrigin;
        unsigned int        nChannelNo;
        float               fConversionFactor;

        printf("================ Force Settings ==================\n\n");

        for (unsigned int iPlate = 0; iPlate < nCount; iPlate++)
        {
            poRTProtocol->GetForcePlate(iPlate, nPlateID, nAnalogDeviceID, nFrequency, pType, pName, fLength, fWidth);

            printf("Force plate ID %d\n", nPlateID);
            printf("  Plate type: %s\n", pType);
            if (nAnalogDeviceID != 0)
            {
                printf("  Analog device id: %d\n", nAnalogDeviceID);
            }
            printf("  Frequency: %d\n", nFrequency);
            printf("  Length: %-8.2f Width: %-8.2f\n", fLength, fWidth);

            poRTProtocol->GetForcePlateLocation(iPlate, sCorners);

            printf("  Corner 1: ");
            printf("X = %-8.2f ",   sCorners[0].fX);
            printf("Y = %-8.2f ",   sCorners[0].fY);
            printf("Z = %-8.2f \n", sCorners[0].fZ);
            printf("  Corner 2: ");
            printf("X = %-8.2f ",   sCorners[1].fX);
            printf("Y = %-8.2f ",   sCorners[1].fY);
            printf("Z = %-8.2f \n", sCorners[1].fZ);
            printf("  Corner 3: ");
            printf("X = %-8.2f ",   sCorners[2].fX);
            printf("Y = %-8.2f ",   sCorners[2].fY);
            printf("Z = %-8.2f \n", sCorners[2].fZ);
            printf("  Corner 4: ");
            printf("X = %-8.2f ",   sCorners[3].fX);
            printf("Y = %-8.2f ",   sCorners[3].fY);
            printf("Z = %-8.2f \n", sCorners[3].fZ);

            poRTProtocol->GetForcePlateOrigin(iPlate, sOrigin);

            printf("  Origin:   ");
            printf("X = %-8.2f ",   sOrigin.fX);
            printf("Y = %-8.2f ",   sOrigin.fY);
            printf("Z = %-8.2f \n", sOrigin.fZ);

            if (poRTProtocol->GetForcePlateChannelCount(iPlate) > 0)
            {
                printf("  Channels\n");
            }

            for (unsigned int iChannel = 0; iChannel < poRTProtocol->GetForcePlateChannelCount(iPlate); iChannel++)
            {
                poRTProtocol->GetForcePlateChannel(iPlate, iChannel, nChannelNo, fConversionFactor);
                printf("    Channel number: %2d", nChannelNo);
                printf("    Conversion factor: %10f\n", fConversionFactor);
            }

            float fvCalMatrix[12][12];
            unsigned int nRows = 6;
            unsigned int nColumns = 6;
            if (poRTProtocol->GetForcePlateCalibrationMatrix(iPlate, fvCalMatrix, &nRows, &nColumns))
            {
                printf("  Calibration Matrix:\n");
                for (unsigned int iRow = 0; iRow < nRows; iRow++)
                {
                    printf("    ");
                    for (unsigned int iCol = 0; iCol < nColumns; iCol++)
                    {
                        printf("%11f ", fvCalMatrix[iRow][iCol]);
                    }
                    printf("\n");
                }
            }
        }
        printf("\n");
    }
}

void COutput::PrintImageSettings(CRTProtocol* poRTProtocol)
{
    unsigned int            nCameraID, nWidth, nHeight;
    float                   fCropLeft, fCropTop, fCropRight, fCropBottom;
    bool                    bEnabled;
    CRTPacket::EImageFormat eFormat;

    if (poRTProtocol->GetImageCameraCount() > 0)
    {
        printf("================ Image Settings ==================\n\n");

        for (unsigned int iCamera = 0; iCamera < poRTProtocol->GetImageCameraCount(); iCamera++)
        {
            poRTProtocol->GetImageCamera(iCamera, nCameraID, bEnabled, eFormat, nWidth, nHeight, fCropLeft, fCropTop,
                fCropRight, fCropBottom);
            printf("Camera ID %d\n", nCameraID);
            switch (eFormat)
            {
            case CRTPacket::FormatRawGrayscale :
                printf("  Image Format = RAWGrayscale\n");
                break;
            case CRTPacket::FormatRawBGR :
                printf("  Image Format = RAWBGR\n");
                break;
            case CRTPacket::FormatJPG :
                printf("  Image Format = JPG\n");
                break;
            case CRTPacket::FormatPNG :
                printf("  Image Format = PNG\n");
                break;
            }
            printf("  Width = %d  Height = %d\n", nWidth, nHeight);
            printf("  Crop:  Left = %d %%  Top = %d %%  Right = %d %%  Bottom = %d %%\n\n",
                (int)(fCropLeft * 100), (int)(fCropTop * 100), (int)(fCropRight * 100), (int)(fCropBottom * 100));
        }
    }
}
