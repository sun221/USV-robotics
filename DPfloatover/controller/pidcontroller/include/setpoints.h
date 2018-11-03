/*
***********************************************************************
* setpoints.h:
* function for real time setpoints for controller
* This header file can be read by C++ compilers
*
* by Hu.ZH(Mr.SJTU)
***********************************************************************
*/
#ifndef _SETPOINTS_H_
#define _SETPOINTS_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/timer.hpp>
#include <chrono>
#include <cmath>
#include <thread>
#include "constants.h"
#include "realtimedata.h"

// go to fixed point
struct fixedpointdata {
  double desired_finalx;
  double desired_finaly;
  double desired_theta;
};

// go straight line
struct strightlinedata {
  double desired_velocity;
  double desired_theta;
  double desired_finalx;
  double desired_finaly;
  double desired_initialx;
  double desired_initialy;

  int orientation_adjustment_time;  // elapsed time for orientation
                                    // adjustment(seconds)
};

// go rotation
struct rotationaroundpoint {
  double rotation_center_x;
  double rotation_center_y;
  double rotation_speed;
};

class setpoints {
 public:
  setpoints() {}
  ~setpoints() {}
  // Enable each vessel to reach a fixed point independently
  void gofixedpoint_first(realtimevessel_first &_realtimevessel,
                          const fixedpointdata &_fixedpointdata) {
    gofixedpoint(_realtimevessel.setPoints, _fixedpointdata);
  }
  void gofixedpoint_second(realtimevessel_second &_realtimevessel,
                           const fixedpointdata &_fixedpointdata) {
    gofixedpoint(_realtimevessel.setPoints, _fixedpointdata);
  }
  void gofixedpoint_third(realtimevessel_third &_realtimevessel,
                          const fixedpointdata &_fixedpointdata) {
    gofixedpoint(_realtimevessel.setPoints, _fixedpointdata);
  }
  // Enable each vessel to go with a stright line independently
  void gostraightline_first(realtimevessel_first &_realtimevessel,
                            const strightlinedata &_strightlinedata) {
    gostraightline(_realtimevessel.setPoints, _strightlinedata);
  }
  void gostraightline_second(realtimevessel_second &_realtimevessel,
                             const strightlinedata &_strightlinedata) {
    gostraightline(_realtimevessel.setPoints, _strightlinedata);
  }
  void gostraightline_third(realtimevessel_third &_realtimevessel,
                            const strightlinedata &_strightlinedata) {
    gostraightline(_realtimevessel.setPoints, _strightlinedata);
  }

 private:
  Eigen::Vector3d setpoint_first;
  Eigen::Vector3d setpoint_second;
  Eigen::Vector3d setpoint_third;

  void gofixedpoint(Eigen::Vector3d &_setpoints,
                    const fixedpointdata &_fixedpointdata) {
    _setpoints << _fixedpointdata.desired_finalx,
        _fixedpointdata.desired_finaly, _fixedpointdata.desired_theta;
  }

  // we keep the orientation and velocity to go a straight line, to a final
  // points
  void gostraightline(Eigen::Vector3d &_setpoints,
                      const strightlinedata &_strightlinedata) {
    // We reach the desired orientation first.
    _setpoints << _strightlinedata.desired_initialx,
        _strightlinedata.desired_initialy, _strightlinedata.desired_theta;
    std::this_thread::sleep_for(
        std::chrono::seconds(_strightlinedata.orientation_adjustment_time));

    // then we keep the straight line and reach the desired points
    double total_delta_x =
        _strightlinedata.desired_finalx - _strightlinedata.desired_initialx;
    double total_delta_y =
        _strightlinedata.desired_finaly - _strightlinedata.desired_initialy;
    double total_length = std::sqrt(total_delta_x * total_delta_x +
                                    total_delta_y * total_delta_y);

    // setup timer
    boost::posix_time::ptime t_start =
        boost::posix_time::second_clock::local_time();
    boost::posix_time::ptime t_end =
        boost::posix_time::second_clock::local_time();
    boost::posix_time::time_duration t_elapsed = t_end - t_start;
    long int mt_elapsed = 0;

    long int total_mt_elapsed =
        (long int)(1000 * total_length / _strightlinedata.desired_velocity);
    // update the desired position step by step
    do {
      t_end = boost::posix_time::second_clock::local_time();
      t_elapsed = t_end - t_start;
      mt_elapsed = t_elapsed.total_milliseconds();
      _setpoints(0) = total_delta_x * mt_elapsed / total_mt_elapsed +
                      _strightlinedata.desired_initialx;
      _setpoints(1) = total_delta_y * mt_elapsed / total_mt_elapsed +
                      _strightlinedata.desired_initialy;
      _setpoints(2) = _strightlinedata.desired_theta;
    } while (mt_elapsed < total_mt_elapsed);
  }
};
#endif /* _SETPOINTS_H_ */