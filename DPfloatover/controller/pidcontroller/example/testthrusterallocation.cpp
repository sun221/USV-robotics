#include "../include/thrusterallocation.h"

void testonestep() {
  vessel_first myvessel_first{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      4,                                       // P_x
      1,                                       // P_y
      5,                                       // P_theta
      0.2,                                     // I_x
      0.1,                                     // I_y
      1,                                       // I_theta
      0.1,                                     // D_x
      0.1,                                     // D_y
      0.2,                                     // D_theta
      0.01,                                    // allowed_error_x
      0.01,                                    // allowed_error_y
      0.01,                                    // allowed_error_orientation
      26.0,                                    // maxpositive_x_thrust(N)
      25.0,                                    // maxnegative_x_thrust(N)
      6,                                       // maxpositive_y_thrust(N)
      4,                                       // maxnegative_y_thrust(N)
      11,                                      // maxpositive_Mz_thrust(N*m)
      7.6,                                     // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.6e-7,                                  // Kbar_positive
      2.2e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      4000,                                    // max_rotation_bow
      8.96,                                    // max_thrust_bow_positive
      3.52,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_bow
      1000,                                    // max_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI,                                    // max_alpha_azimuth_left
      0,                                       // min_alpha_azimuth_left
      0,                                       // max_alpha_azimuth_right
      -M_PI,                                   // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };
  realtimevessel_first myrealtimevessel;
  myrealtimevessel.tau.setZero();
  myrealtimevessel.alpha << -M_PI / 2, 0, 0;
  myrealtimevessel.u << 0.8, 10, 5;
  thrusterallocation_first mythrusterallocation(myvessel_first,
                                                myrealtimevessel);
  mythrusterallocation.onestepthrusterallocation(myrealtimevessel);
  outputmatrices(mythrusterallocation);
}

void testmultistep() {
  savefile mysavefile_first;
  savefile mysavefile_second;
  savefile mysavefile_third;

  // constant parameters of the first vessel
  vessel_first myvessel_first{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      20,                                      // P_x
      10,                                      // P_y
      50.0,                                    // P_theta
      0.02,                                    // I_x
      0.01,                                    // I_y
      0.0,                                     // I_theta
      200.0,                                   // D_x
      150.0,                                   // D_y
      300.0,                                   // D_theta
      0.01,                                    // allowed_error_x
      0.01,                                    // allowed_error_y;
      0.01,                                    // allowed_error_orientation;
      6.0,                                     // maxpositive_x_thrust(N)
      5.0,                                     // maxnegative_x_thrust(N)
      3,                                       // maxpositive_y_thrust(N)
      2,                                       // maxnegative_y_thrust(N)
      5,                                       // maxpositive_Mz_thrust(N*m)
      3,                                       // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.5e-7,                                  // Kbar_positive
      1.3e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      3000,                                    // max_rotation_bow
      4.95,                                    // max_thrust_bow_positive
      1.17,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_azimuth
      1000,                                    // max_rotation_azimuth
      50,                                      // min_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.05,                                    // min_thrust_azimuth_left
      0.05,                                    // min_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI * 175 / 180,                        // max_alpha_azimuth_left
      M_PI / 18,                               // min_alpha_azimuth_left
      -M_PI / 18,                              // max_alpha_azimuth_right
      -M_PI * 175 / 180,                       // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };
  // constant parameters of the second vessel
  vessel_second myvessel_second{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      20,                                      // P_x
      10,                                      // P_y
      50.0,                                    // P_theta
      0.02,                                    // I_x
      0.01,                                    // I_y
      0.0,                                     // I_theta
      200.0,                                   // D_x
      150.0,                                   // D_y
      300.0,                                   // D_theta
      0.01,                                    // allowed_error_x
      0.01,                                    // allowed_error_y;
      0.02,                                    // allowed_error_orientation;
      6.0,                                     // maxpositive_x_thrust(N)
      5.0,                                     // maxnegative_x_thrust(N)
      3,                                       // maxpositive_y_thrust(N)
      2,                                       // maxnegative_y_thrust(N)
      5,                                       // maxpositive_Mz_thrust(N*m)
      3,                                       // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      3.7e-7,                                  // Kbar_positive
      1.7e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      3000,                                    // max_rotation_bow
      3.33,                                    // max_thrust_bow_positive
      1.53,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_azimuth
      1000,                                    // max_rotation_azimuth
      50,                                      // min_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.05,                                    // min_thrust_azimuth_left
      0.05,                                    // min_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI * 175 / 180,                        // max_alpha_azimuth_left
      M_PI / 18,                               // min_alpha_azimuth_left
      -M_PI / 18,                              // max_alpha_azimuth_right
      -M_PI * 175 / 180,                       // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };
  // constant parameters of the third vessel
  vessel_third myvessel_third{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      20,                                      // P_x
      10,                                      // P_y
      50.0,                                    // P_theta
      0.02,                                    // I_x
      0.01,                                    // I_y
      0.0,                                     // I_theta
      200.0,                                   // D_x
      150.0,                                   // D_y
      300.0,                                   // D_theta
      0.01,                                    // allowed_error_x
      0.01,                                    // allowed_error_y;
      0.02,                                    // allowed_error_orientation;
      6.0,                                     // maxpositive_x_thrust(N)
      5.0,                                     // maxnegative_x_thrust(N)
      3,                                       // maxpositive_y_thrust(N)
      2,                                       // maxnegative_y_thrust(N)
      5,                                       // maxpositive_Mz_thrust(N*m)
      3,                                       // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      3.7e-7,                                  // Kbar_positive
      1.7e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      3000,                                    // max_rotation_bow
      3.33,                                    // max_thrust_bow_positive
      1.53,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_azimuth
      1000,                                    // max_rotation_azimuth
      50,                                      // min_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.05,                                    // min_thrust_azimuth_left
      0.05,                                    // min_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI * 175 / 180,                        // max_alpha_azimuth_left
      M_PI / 18,                               // min_alpha_azimuth_left
      -M_PI / 18,                              // max_alpha_azimuth_right
      -M_PI * 175 / 180,                       // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };

  // realtime parameters of the first vessel (K class-I)
  realtimevessel_first myrealtimevessel_first{
      Vector6d::Zero(),             // position
      Vector6d::Zero(),             // measurement
      Vector6d::Zero(),             // state
      Vector6d::Zero(),             // state4control
      Eigen::Vector3d::Zero(),      // setPoints
      0,                            // index for step point
      Eigen::Matrix3d::Identity(),  // CTG2B
      Eigen::Matrix3d::Identity(),  // CTB2G
      Eigen::Vector3d::Zero(),      // tau
      Eigen::Vector3d::Zero(),      // BalphaU
      (Eigen::Vector3d() << -M_PI / 2, M_PI / 10, -M_PI / 4)
          .finished(),                                   // alpha
      Eigen::Vector3i::Zero(),                           // alpha_deg
      (Eigen::Vector3d() << 0.01, 0.2, 0.2).finished(),  // u
      Eigen::Vector3i::Zero()                            // rotation
  };
  // realtime parameters of the second vessel (K class-II)
  realtimevessel_second myrealtimevessel_second{
      Vector6d::Zero(),             // position
      Vector6d::Zero(),             // measurement
      Vector6d::Zero(),             // state
      Vector6d::Zero(),             // state4control
      Eigen::Vector3d::Zero(),      // setPoints
      0,                            // index for step point
      Eigen::Matrix3d::Identity(),  // CTG2B
      Eigen::Matrix3d::Identity(),  // CTB2G
      Eigen::Vector3d::Zero(),      // tau
      Eigen::Vector3d::Zero(),      // BalphaU
      (Eigen::Vector3d() << M_PI / 2, M_PI / 10, -M_PI / 4)
          .finished(),                                   // alpha
      Eigen::Vector3i::Zero(),                           // alpha_deg
      (Eigen::Vector3d() << 0.01, 0.2, 0.2).finished(),  // u
      Eigen::Vector3i::Zero()                            // rotation
  };
  // realtime parameters of the third vessel (X class)
  realtimevessel_third myrealtimevessel_third{
      Vector6d::Zero(),             // position
      Vector6d::Zero(),             // measurement
      Vector6d::Zero(),             // state
      Vector6d::Zero(),             // state4control
      Eigen::Vector3d::Zero(),      // setPoints
      0,                            // index for step point
      Eigen::Matrix3d::Identity(),  // CTG2B
      Eigen::Matrix3d::Identity(),  // CTB2G
      Eigen::Vector3d::Zero(),      // tau
      Eigen::Vector3d::Zero(),      // BalphaU
      (Eigen::Vector3d() << -M_PI / 2, M_PI / 10, -M_PI / 4)
          .finished(),                                   // alpha
      Eigen::Vector3i::Zero(),                           // alpha_deg
      (Eigen::Vector3d() << 0.01, 0.2, 0.2).finished(),  // u
      Eigen::Vector3i::Zero()                            // rotation
  };

  // specify parameters of the first vessel

  thrusterallocation_first mythrusterallocation_first(myvessel_first,
                                                      myrealtimevessel_first);
  mythrusterallocation_first.test_multiplethrusterallocation(
      mysavefile_first, myvessel_first, myrealtimevessel_first);
  output2csv(mythrusterallocation_first, mysavefile_first, "First");
  // specify parameters of the second vessel

  thrusterallocation_second mythrusterallocation_second(
      myvessel_second, myrealtimevessel_second);
  mythrusterallocation_second.test_multiplethrusterallocation(
      mysavefile_second, myvessel_second, myrealtimevessel_second);
  output2csv(mythrusterallocation_second, mysavefile_second, "Second");
  // specify parameters of the third vessel
  thrusterallocation_third mythrusterallocation_third(myvessel_third,
                                                      myrealtimevessel_third);
  mythrusterallocation_third.test_multiplethrusterallocation(
      mysavefile_third, myvessel_third, myrealtimevessel_third);
  output2csv(mythrusterallocation_third, mysavefile_third, "Third");
}
int main() { testmultistep(); }