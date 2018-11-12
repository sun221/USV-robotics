#include "../include/qtmOutput.h"

#define WRITE_ANALOG_HEADERS_TO_FILE

COutput::COutput()
    : mfDist(NULL),
      mbWriteLogFileHeader(true),
      time_start(T_BOOST_CLOCK::local_time()),
      mbOutputModeScrolling(false) {
  initializemovingaverage();
}

void COutput::HandleDataFrame(FILE* logfile, bool bLogMinimum,
                              CRTProtocol* poRTProtocol) {
  CRTPacket* poRTPacket;

  mnReceivedFrames++;

  if (poRTProtocol) {
    poRTPacket = poRTProtocol->GetRTPacket();
    if (poRTPacket->GetComponentCount() == 0 ||
        poRTPacket->GetType() != CRTPacket::PacketData) {
      return;
    }
  } else {
    return;
  }

  unsigned int nFrameNumber = poRTPacket->GetFrameNumber();
  unsigned long long nTimeStamp = poRTPacket->GetTimeStamp();

  if (nFrameNumber == 1 && mnLastFrameNumber != 0) {
    // Start from the beginning in case we are running rt from file
    ResetCounters();
    mnReceivedFrames = 1;
  }

  // Update packet receive time.
  boost::posix_time::ptime time_now(T_BOOST_CLOCK::local_time());
  boost::posix_time::time_duration t_d = time_now - time_start;

  mfCurrentRecvTime = (double)(t_d.total_milliseconds() / 1000.0);

  if (mnReceivedFrames > 1) {
    mnFrameNumberDiff = nFrameNumber - mnLastFrameNumber;

    if (mnFrameNumberDiff <= 0) {
      // Frame repeated (should never happen).
      ResetCounters();
      mnReceivedFrames = 1;
    } else {
      // New frame received.
      mfCameraFreq =
          (mnFrameNumberDiff * 1000000.0) / (nTimeStamp - mnLastTimeStamp);
      mnMaxFrameNumberDiff =
          std::max((unsigned int)mnFrameNumberDiff, mnMaxFrameNumberDiff);
      mfRecvTimeDiff = mfCurrentRecvTime - mfLastRecvTime;
      mfMaxRecvTimeDiff = std::max(mfRecvTimeDiff, mfMaxRecvTimeDiff);
      mfMinRecvTimeDiff = std::min(mfRecvTimeDiff, mfMinRecvTimeDiff);
      mfRecFreq = 1.0 / mfRecvTimeDiff;

      if (mnFrameNumberDiff > 1) {
        mnMissingFrames += mnFrameNumberDiff - 1;
        printf("Missing %d frame%s. Frame number: %d", mnFrameNumberDiff,
               (mnFrameNumberDiff == 1) ? "" : "s", mnLastFrameNumber + 1);
        printf(" to %d\n\n", nFrameNumber - 1);
      }
    }
  }
  mnLastTimeStamp = nTimeStamp;
  mnLastFrameNumber = nFrameNumber;
  mfLastRecvTime = mfCurrentRecvTime;

  if (mfLastScreenUpdateTime == 0.0 ||
      (mfCurrentRecvTime - mfLastScreenUpdateTime) > 0.1) {
    mfLastScreenUpdateTime = mfCurrentRecvTime;
  } else {
    return;
  }

  PrintData6D(logfile, poRTPacket, poRTProtocol);
  PrintData6DRes(logfile, poRTPacket, poRTProtocol);
  PrintData6DEuler(logfile, poRTPacket, poRTProtocol);
  PrintData6DEulerRes(logfile, poRTPacket, poRTProtocol);

}  // PrintData

void COutput::HandleDataFrame(FILE* logfile, CRTProtocol* poRTProtocol,
                              realtimevessel_first& _realtimevessel_first,
                              realtimevessel_second& _realtimevessel_second,
                              realtimevessel_third& _realtimevessel_third) {
  CRTPacket* poRTPacket;

  if (poRTProtocol) {
    poRTPacket = poRTProtocol->GetRTPacket();
    if (poRTPacket->GetComponentCount() == 0 ||
        poRTPacket->GetType() != CRTPacket::PacketData) {
      return;
    }
  } else {
    return;
  }

  if (MAXCONNECTION == 1)
    PrintData6DEuler(poRTPacket, poRTProtocol, _realtimevessel_first);
  else if (MAXCONNECTION == 2)
    PrintData6DEuler(poRTPacket, poRTProtocol, _realtimevessel_first,
                     _realtimevessel_second);
  else if (MAXCONNECTION == 3)
    PrintData6DEuler(poRTPacket, poRTProtocol, _realtimevessel_first,
                     _realtimevessel_second, _realtimevessel_third);
}  // OutputData

void COutput::PrintData6D(FILE* logfile, CRTPacket* poRTPacket,
                          CRTProtocol* poRTProtocol) {
  float fX, fY, fZ;
  float afRotMatrix[9];

  if (poRTPacket->GetComponentSize(CRTPacket::Component6d)) {
    unsigned int nCount = poRTPacket->Get6DOFBodyCount();

    if (nCount > 0) {
      fprintf(logfile, "------------------ 6 DOF -----------------\n");
      for (unsigned int i = 0; i < nCount; i++) {
        char* label = (char*)poRTProtocol->Get6DOFBodyName(i);
        char emptyString[] = "";
        if (label == NULL) {
          label = emptyString;
        }
        poRTPacket->Get6DOFBody(i, fX, fY, fZ, afRotMatrix);

        fprintf(
            logfile,
            "%15s : %f\t%f\t%f\t -    %f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t\n",
            label, fX, fY, fZ, afRotMatrix[0], afRotMatrix[1], afRotMatrix[2],
            afRotMatrix[3], afRotMatrix[4], afRotMatrix[5], afRotMatrix[6],
            afRotMatrix[7], afRotMatrix[8]);
      }
      fprintf(logfile, "\n");
    }
  }
}  // PrintData6D

void COutput::PrintData6DRes(FILE* logfile, CRTPacket* poRTPacket,
                             CRTProtocol* poRTProtocol) {
  float fX, fY, fZ, fResidual;
  float afRotMatrix[9];

  if (poRTPacket->GetComponentSize(CRTPacket::Component6dRes)) {
    unsigned int nCount = poRTPacket->Get6DOFResidualBodyCount();

    if (nCount > 0) {
      fprintf(logfile, "------------- 6 DOF Residual -------------\n");
      for (unsigned int i = 0; i < nCount; i++) {
        char* label = (char*)poRTProtocol->Get6DOFBodyName(i);
        char emptyString[] = "";
        if (label == NULL) {
          label = emptyString;
        }
        poRTPacket->Get6DOFResidualBody(i, fX, fY, fZ, afRotMatrix, fResidual);

        fprintf(logfile,
                "%15s : %f\t%f\t%f\t -    %f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t "
                "-   %f\n",
                label, fX, fY, fZ, afRotMatrix[0], afRotMatrix[1],
                afRotMatrix[2], afRotMatrix[3], afRotMatrix[4], afRotMatrix[5],
                afRotMatrix[6], afRotMatrix[7], afRotMatrix[8], fResidual);
      }
      fprintf(logfile, "\n");
    }
  }
}  // PrintData6DRes

void COutput::PrintData6DEuler(FILE* logfile, CRTPacket* poRTPacket,
                               CRTProtocol* poRTProtocol) {
  float fX, fY, fZ, fAng1, fAng2, fAng3;

  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEuler)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerBodyCount();

    if (nCount > 0) {
      fprintf(logfile, "--------------- 6 DOF Euler --------------\n");
      for (unsigned int i = 0; i < nCount; i++) {
        char* label = (char*)poRTProtocol->Get6DOFBodyName(i);
        char emptyString[] = "";
        if (label == NULL) {
          label = emptyString;
        }
        poRTPacket->Get6DOFEulerBody(i, fX, fY, fZ, fAng1, fAng2, fAng3);

        fprintf(logfile, "%15s : %f\t%f\t%f\t -   %f\t%f\t%f\n", label, fX, fY,
                fZ, fAng1, fAng2, fAng3);
      }
      fprintf(logfile, "\n");
    }
  }
}  // PrintData6DEuler

void COutput::PrintData6DEuler(CRTPacket* poRTPacket, CRTProtocol* poRTProtocol,
                               realtimevessel_first& _realtimevessel_first) {
  float fX, fY, fZ, fAng1, fAng2, fAng3;  // mm, mm, mm, deg, deg, deg (QTM)

  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEuler)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerBodyCount();
    // determine if we have rigid body
    if (nCount > 0) {
      poRTPacket->Get6DOFEulerBody(0, fX, fY, fZ, fAng1, fAng2, fAng3);
      // determine if the measured data is out of range or NaN
      updaterealtimevesseldata_first(_realtimevessel_first, fX, fY, fZ, fAng1,
                                     fAng2, fAng3);

    } else {
      resetmeasurement(_realtimevessel_first.Measurement,
                       _realtimevessel_first.Position);

      // logfile = fopen(logsavepath.c_str(), "a+");
      fprintf(stdout, "No rigid body found!\n");
      // fclose(logfile);
    }
  }
}  // PrintData6DEuler (the first vessel)

void COutput::PrintData6DEuler(CRTPacket* poRTPacket, CRTProtocol* poRTProtocol,
                               realtimevessel_first& _realtimevessel_first,
                               realtimevessel_second& _realtimevessel_second) {
  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEuler)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerBodyCount();

    if (nCount > 0) {
      float fX_first, fY_first, fZ_first, fAng1_first, fAng2_first,
          fAng3_first;  // mm, mm, mm, deg, deg, deg (QTM)
      float fX_second, fY_second, fZ_second, fAng1_second, fAng2_second,
          fAng3_second;  // mm, mm, mm, deg, deg, deg (QTM)
      // the first vessel
      poRTPacket->Get6DOFEulerBody(0, fX_first, fY_first, fZ_first, fAng1_first,
                                   fAng2_first, fAng3_first);
      // determine if the measured data is out of range or NaN
      updaterealtimevesseldata_first(_realtimevessel_first, fX_first, fY_first,
                                     fZ_first, fAng1_first, fAng2_first,
                                     fAng3_first);

      // the second vessel
      poRTPacket->Get6DOFEulerBody(1, fX_second, fY_second, fZ_second,
                                   fAng1_second, fAng2_second, fAng3_second);
      // determine if the measured data is out of range or NaN
      updaterealtimevesseldata_second(_realtimevessel_second, fX_second,
                                      fY_second, fZ_second, fAng1_second,
                                      fAng2_second, fAng3_second);

    } else {
      resetmeasurement(_realtimevessel_first.Measurement,
                       _realtimevessel_first.Position);
      resetmeasurement(_realtimevessel_second.Measurement,
                       _realtimevessel_second.Position);

      // logfile = fopen(logsavepath.c_str(), "a+");
      fprintf(stdout, "No rigid body found!\n");
      // fclose(logfile);
    }
  }
}  // PrintData6DEuler (the first and second vessel)

void COutput::PrintData6DEuler(CRTPacket* poRTPacket, CRTProtocol* poRTProtocol,
                               realtimevessel_first& _realtimevessel_first,
                               realtimevessel_second& _realtimevessel_second,
                               realtimevessel_third& _realtimevessel_third) {
  float fX, fY, fZ, fAng1, fAng2, fAng3;  // mm, mm, mm, deg, deg, deg (QTM)
  double m_fx, m_fy, rad_orientation, raw_u, raw_v, raw_r;
  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEuler)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerBodyCount();

    if (nCount > 0) {
      // the first vessel
      poRTPacket->Get6DOFEulerBody(0, fX, fY, fZ, fAng1, fAng2, fAng3);
      // determine if the measured data is out of range or NaN
      updaterealtimevesseldata_first(_realtimevessel_first, fX, fY, fZ, fAng1,
                                     fAng2, fAng3);

      // the second vessel
      poRTPacket->Get6DOFEulerBody(1, fX, fY, fZ, fAng1, fAng2, fAng3);
      // determine if the measured data is out of range or NaN
      updaterealtimevesseldata_second(_realtimevessel_second, fX, fY, fZ, fAng1,
                                      fAng2, fAng3);

      // the third vessel
      poRTPacket->Get6DOFEulerBody(2, fX, fY, fZ, fAng1, fAng2, fAng3);
      // determine if the measured data is out of range or NaN
      updaterealtimevesseldata_third(_realtimevessel_third, fX, fY, fZ, fAng1,
                                     fAng2, fAng3);
    } else {
      resetmeasurement(_realtimevessel_first.Measurement,
                       _realtimevessel_first.Position);
      resetmeasurement(_realtimevessel_second.Measurement,
                       _realtimevessel_second.Position);
      resetmeasurement(_realtimevessel_third.Measurement,
                       _realtimevessel_third.Position);

      // logfile = fopen(logsavepath.c_str(), "a+");
      fprintf(stdout, "No rigid body found!\n");
      // fclose(logfile);
    }
  }
}  // PrintData6DEuler (the first/second/third vessel)

void COutput::PrintData6DEulerRes(FILE* logfile, CRTPacket* poRTPacket,
                                  CRTProtocol* poRTProtocol) {
  float fX, fY, fZ, fAng1, fAng2, fAng3, fResidual;

  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEulerRes)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerResidualBodyCount();

    if (nCount > 0) {
      fprintf(logfile, "---------- 6 DOF Euler Residual ----------\n");
      for (unsigned int i = 0; i < nCount; i++) {
        char* label = (char*)poRTProtocol->Get6DOFBodyName(i);
        char emptyString[] = "";
        if (label == NULL) {
          label = emptyString;
        }
        poRTPacket->Get6DOFEulerResidualBody(i, fX, fY, fZ, fAng1, fAng2, fAng3,
                                             fResidual);

        fprintf(logfile, "%15s : %f\t%f\t%f\t -   %f\t%f\t%f\t -    %f\n",
                label, fX, fY, fZ, fAng1, fAng2, fAng3, fResidual);
      }
      fprintf(logfile, "\n");
    }
  }
}  // PrintData6DEulerRes

void COutput::PrintTimingData() {
  printf("\n\nReceived %d data frames in %f seconds\n\n", mnReceivedFrames,
         mfCurrentRecvTime);
  printf("Average receive frequency = %.1f\n",
         mnReceivedFrames / mfCurrentRecvTime);
  printf("Camera frequency = %.1f Hz\n", mfCameraFreq);
  printf("Missed frames = %d\n", mnMissingFrames);
  printf("Max frame receive time diff = %f ms\n", mfMaxRecvTimeDiff);
  printf("Min frame receive time diff = %f ms\n\n", mfMinRecvTimeDiff);
}  // PrintTimingData

void COutput::ResetCounters() {
  // Reset statistic counters
  mfRecvTimeDiff = 0.0;
  mfMaxRecvTimeDiff = 0.0;
  mfMinRecvTimeDiff = 100000.0;
  mfLastRecvTime = 0.0;
  mfRecFreq = 0.0;
  mfLastScreenUpdateTime = 0.0;
  mnLastFrameNumber = 0xffffffff;
  mnMaxFrameNumberDiff = 0;
  mnMissingFrames = 0;
  mnReceivedFrames = 0;
  mnFrameNumberDiff = 0;
  mnMaxFrameNumberDiff = 0;
  mnLastTimeStamp = 0;

}  // ResetCounters

void COutput::updaterealtimevesseldata_first(
    realtimevessel_first& _realtimevessel, float _fX, float _fY, float _fZ,
    float _fAng1, float _fAng2, float _fAng3) {
  // determine if the measured data is out of range or NaN
  if ((abs(_fX) < qtm_max_position) && (abs(_fY) < qtm_max_position)) {
    double m_fx = _fX / 1000;
    double m_fy = _fY / 1000;
    double rad_orientation = _fAng3 * M_PI / 180;
    _realtimevessel.Position(0) = m_fx;
    _realtimevessel.Position(1) = m_fy;
    _realtimevessel.Position(2) = _fZ / 1000;
    _realtimevessel.Position(3) = _fAng1;
    _realtimevessel.Position(4) = _fAng2;
    _realtimevessel.Position(5) = _fAng3;

    _realtimevessel.Measurement(0) = movingaverage_surge_first(m_fx);
    _realtimevessel.Measurement(1) = movingaverage_sway_first(m_fy);
    // _measurement(2) = rad_orientation;
    double average_orientation = movingaverage_yaw_first(rad_orientation);
    calculateCoordinateTransform(_realtimevessel.CTG2B, _realtimevessel.CTB2G,
                                 average_orientation,
                                 _realtimevessel.setPoints(2));
    _realtimevessel.Measurement(2) = average_orientation;
    _realtimevessel.Measurement.tail(3) =
        _realtimevessel.CTG2B *
        movingaverage_velocity_first(m_fx, m_fy, rad_orientation);
  }
}

void COutput::updaterealtimevesseldata_second(
    realtimevessel_second& _realtimevessel, float _fX, float _fY, float _fZ,
    float _fAng1, float _fAng2,
    float _fAng3) {  // determine if the measured data is out of range or NaN
  if ((abs(_fX) < qtm_max_position) && (abs(_fY) < qtm_max_position)) {
    double m_fx = _fX / 1000;
    double m_fy = _fY / 1000;
    double rad_orientation = _fAng3 * M_PI / 180;
    _realtimevessel.Position(0) = m_fx;
    _realtimevessel.Position(1) = m_fy;
    _realtimevessel.Position(2) = _fZ / 1000;
    _realtimevessel.Position(3) = _fAng1;
    _realtimevessel.Position(4) = _fAng2;
    _realtimevessel.Position(5) = _fAng3;

    _realtimevessel.Measurement(0) = movingaverage_surge_second(m_fx);
    _realtimevessel.Measurement(1) = movingaverage_sway_second(m_fy);
    // _measurement(2) = rad_orientation;
    double average_orientation = movingaverage_yaw_second(rad_orientation);
    calculateCoordinateTransform(_realtimevessel.CTG2B, _realtimevessel.CTB2G,
                                 average_orientation,
                                 _realtimevessel.setPoints(2));
    _realtimevessel.Measurement(2) = average_orientation;
    _realtimevessel.Measurement.tail(3) =
        _realtimevessel.CTG2B *
        movingaverage_velocity_second(m_fx, m_fy, rad_orientation);
  }
}

void COutput::updaterealtimevesseldata_third(
    realtimevessel_third& _realtimevessel, float _fX, float _fY, float _fZ,
    float _fAng1, float _fAng2,
    float _fAng3) {  // determine if the measured data is out of range or NaN
  if ((abs(_fX) < qtm_max_position) && (abs(_fY) < qtm_max_position)) {
    double m_fx = _fX / 1000;
    double m_fy = _fY / 1000;
    double rad_orientation = _fAng3 * M_PI / 180;
    _realtimevessel.Position(0) = m_fx;
    _realtimevessel.Position(1) = m_fy;
    _realtimevessel.Position(2) = _fZ / 1000;
    _realtimevessel.Position(3) = _fAng1;
    _realtimevessel.Position(4) = _fAng2;
    _realtimevessel.Position(5) = _fAng3;

    _realtimevessel.Measurement(0) = movingaverage_surge_third(m_fx);
    _realtimevessel.Measurement(1) = movingaverage_sway_third(m_fy);
    // _measurement(2) = rad_orientation;
    double average_orientation = movingaverage_yaw_third(rad_orientation);
    calculateCoordinateTransform(_realtimevessel.CTG2B, _realtimevessel.CTB2G,
                                 average_orientation,
                                 _realtimevessel.setPoints(2));
    _realtimevessel.Measurement(2) = average_orientation;
    _realtimevessel.Measurement.tail(3) =
        _realtimevessel.CTG2B *
        movingaverage_velocity_third(m_fx, m_fy, rad_orientation);
  }
}

void COutput::resetmeasurement(Vector6d& _measurement, Vector6d& _position) {
  _measurement.setZero();
  _position.setZero();
}

void COutput::initializemovingaverage() {
  if (MAXCONNECTION > 0) {
    Matrix_average_first.setZero();
    average_vector_first.setZero();
    average_yaw_first.setZero();
    average_surge_first.setZero();
    average_sway_first.setZero();
  }
  if (MAXCONNECTION > 1) {
    Matrix_average_second.setZero();
    average_vector_second.setZero();
    average_yaw_second.setZero();
    average_surge_second.setZero();
    average_sway_second.setZero();
  }
  if (MAXCONNECTION > 2) {
    Matrix_average_third.setZero();
    average_vector_third.setZero();
    average_yaw_third.setZero();
    average_surge_third.setZero();
    average_sway_third.setZero();
  }
}

Eigen::Vector3d COutput::movingaverage_velocity_first(double _dx, double _dy,
                                                      double _dtheta) {
  // copy the former average vector
  Eigen::Vector3d former_average_vector = average_vector_first;
  // pop_front
  Matrix3100d t_Matrix_average = Matrix3100d::Zero();
  int index = num_average_point_velocity - 1;
  t_Matrix_average.leftCols(index) = Matrix_average_first.rightCols(index);
  // push_back
  t_Matrix_average(0, index) = _dx;
  t_Matrix_average(1, index) = _dy;
  t_Matrix_average(2, index) = _dtheta;
  Matrix_average_first = t_Matrix_average;
  // calculate the mean value
  for (int i = 0; i != 3; ++i)
    average_vector_first(i) = Matrix_average_first.row(i).mean();
  // calculate the velocity
  Eigen::Vector3d average_velocity = Eigen::Vector3d::Zero();
  average_velocity =
      (average_vector_first - former_average_vector) / motion_sample_time;
  return average_velocity;  // in the global coordinate
}

// moving average lowpass to remove noise
double COutput::movingaverage_yaw_first(double _dtheta) {
  // pop_front
  VectorAYaw t_average_yaw = VectorAYaw::Zero();
  int index = num_average_point_yaw - 1;
  t_average_yaw.head(index) = average_yaw_first.tail(index);
  // push back
  t_average_yaw(index) = _dtheta;
  average_yaw_first = t_average_yaw;
  // calculate the mean value
  return average_yaw_first.mean();
}
// moving average lowpass to remove noise
double COutput::movingaverage_surge_first(double _dx) {
  // pop_front
  VectorASurge t_average_surge = VectorASurge::Zero();
  int index = num_average_point_surge - 1;
  t_average_surge.head(index) = average_surge_first.tail(index);
  // push back
  t_average_surge(index) = _dx;
  average_surge_first = t_average_surge;
  // calculate the mean value
  return average_surge_first.mean();
}
// moving average lowpass to remove noise
double COutput::movingaverage_sway_first(double _dy) {
  // pop_front
  VectorASway t_average_sway = VectorASway::Zero();
  int index = num_average_point_sway - 1;
  t_average_sway.head(index) = average_sway_first.tail(index);
  // push back
  t_average_sway(index) = _dy;
  average_sway_first = t_average_sway;
  // calculate the mean value
  return average_sway_first.mean();
}

Eigen::Vector3d COutput::movingaverage_velocity_second(double _dx, double _dy,
                                                       double _dtheta) {
  // copy the former average vector
  Eigen::Vector3d former_average_vector = average_vector_second;
  // pop_front
  Matrix3100d t_Matrix_average = Matrix3100d::Zero();
  int index = num_average_point_velocity - 1;
  t_Matrix_average.leftCols(index) = Matrix_average_second.rightCols(index);
  // push_back
  t_Matrix_average(0, index) = _dx;
  t_Matrix_average(1, index) = _dy;
  t_Matrix_average(2, index) = _dtheta;
  Matrix_average_second = t_Matrix_average;
  // calculate the mean value
  for (int i = 0; i != 3; ++i)
    average_vector_second(i) = Matrix_average_second.row(i).mean();
  // calculate the velocity
  Eigen::Vector3d average_velocity = Eigen::Vector3d::Zero();
  average_velocity =
      (average_vector_second - former_average_vector) / motion_sample_time;
  return average_velocity;  // in the global coordinate
}

// moving average lowpass to remove noise
double COutput::movingaverage_yaw_second(double _dtheta) {
  // pop_front
  VectorAYaw t_average_yaw = VectorAYaw::Zero();
  int index = num_average_point_yaw - 1;
  t_average_yaw.head(index) = average_yaw_second.tail(index);
  // push back
  t_average_yaw(index) = _dtheta;
  average_yaw_second = t_average_yaw;
  // calculate the mean value
  return average_yaw_second.mean();
}
// moving average lowpass to remove noise
double COutput::movingaverage_surge_second(double _dx) {
  // pop_front
  VectorASurge t_average_surge = VectorASurge::Zero();
  int index = num_average_point_surge - 1;
  t_average_surge.head(index) = average_surge_second.tail(index);
  // push back
  t_average_surge(index) = _dx;
  average_surge_second = t_average_surge;
  // calculate the mean value
  return average_surge_second.mean();
}
// moving average lowpass to remove noise
double COutput::movingaverage_sway_second(double _dy) {
  // pop_front
  VectorASway t_average_sway = VectorASway::Zero();
  int index = num_average_point_sway - 1;
  t_average_sway.head(index) = average_sway_second.tail(index);
  // push back
  t_average_sway(index) = _dy;
  average_sway_second = t_average_sway;
  // calculate the mean value
  return average_sway_second.mean();
}

Eigen::Vector3d COutput::movingaverage_velocity_third(double _dx, double _dy,
                                                      double _dtheta) {
  // copy the former average vector
  Eigen::Vector3d former_average_vector = average_vector_third;
  // pop_front
  Matrix3100d t_Matrix_average = Matrix3100d::Zero();
  int index = num_average_point_velocity - 1;
  t_Matrix_average.leftCols(index) = Matrix_average_third.rightCols(index);
  // push_back
  t_Matrix_average(0, index) = _dx;
  t_Matrix_average(1, index) = _dy;
  t_Matrix_average(2, index) = _dtheta;
  Matrix_average_third = t_Matrix_average;
  // calculate the mean value
  for (int i = 0; i != 3; ++i)
    average_vector_third(i) = Matrix_average_third.row(i).mean();
  // calculate the velocity
  Eigen::Vector3d average_velocity = Eigen::Vector3d::Zero();
  average_velocity =
      (average_vector_third - former_average_vector) / motion_sample_time;
  return average_velocity;  // in the global coordinate
}

// moving average lowpass to remove noise
double COutput::movingaverage_yaw_third(double _dtheta) {
  // pop_front
  VectorAYaw t_average_yaw = VectorAYaw::Zero();
  int index = num_average_point_yaw - 1;
  t_average_yaw.head(index) = average_yaw_third.tail(index);
  // push back
  t_average_yaw(index) = _dtheta;
  average_yaw_third = t_average_yaw;
  // calculate the mean value
  return average_yaw_third.mean();
}
// moving average lowpass to remove noise
double COutput::movingaverage_surge_third(double _dx) {
  // pop_front
  VectorASurge t_average_surge = VectorASurge::Zero();
  int index = num_average_point_surge - 1;
  t_average_surge.head(index) = average_surge_third.tail(index);
  // push back
  t_average_surge(index) = _dx;
  average_surge_third = t_average_surge;
  // calculate the mean value
  return average_surge_third.mean();
}
// moving average lowpass to remove noise
double COutput::movingaverage_sway_third(double _dy) {
  // pop_front
  VectorASway t_average_sway = VectorASway::Zero();
  int index = num_average_point_sway - 1;
  t_average_sway.head(index) = average_sway_third.tail(index);
  // push back
  t_average_sway(index) = _dy;
  average_sway_third = t_average_sway;
  // calculate the mean value
  return average_sway_third.mean();
}

// calculate the real time coordinate transform matrix
void COutput::calculateCoordinateTransform(Eigen::Matrix3d& _CTG2B,
                                           Eigen::Matrix3d& _CTB2G,
                                           double realtime_orientation,
                                           double desired_orientation) {
  double cvalue = 0.0;
  double svalue = 0.0;
  // if (abs(realtime_orientation - desired_orientation) < M_PI / 36) {
  //   // use the fixed setpoint orientation to prevent measurement noise
  //   cvalue = std::cos(desired_orientation);
  //   svalue = std::sin(desired_orientation);
  // } else {
  //   // if larger than 5 deg, we use the realtime orientation
  //   cvalue = std::cos(realtime_orientation);
  //   svalue = std::sin(realtime_orientation);
  // }
  cvalue = std::cos(desired_orientation);
  svalue = std::sin(desired_orientation);
  _CTG2B(0, 0) = cvalue;
  _CTG2B(1, 1) = cvalue;
  _CTG2B(0, 1) = svalue;
  _CTG2B(1, 0) = -svalue;
  _CTB2G(0, 0) = cvalue;
  _CTB2G(1, 1) = cvalue;
  _CTB2G(0, 1) = -svalue;
  _CTB2G(1, 0) = svalue;
}