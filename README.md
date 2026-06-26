# SmartHelm
The Smart Helmet for Accident Detection is a sensor-fusion system that continuously monitors the kinematic state of a motorcycle helmet. At startup, the system performs automatic calibration to establish a baseline acceleration level that accounts for the orientation in which the helmet is worn or mounted. During normal operation, the MPU6050 IMU samples linear acceleration along three axes every 5 milliseconds.
<br>
<br>
When a sudden impact occurs, the raw acceleration values spike sharply. The Arduino computes a scalar magnitude from the three-axis readings and calculates the rate of change (jerk) between successive samples. If this jerk exceeds a threshold of 170 raw sensor units for at least two consecutive samples, the system enters a "suspected impact" state. It then immediately reads the helmet's tilt angles (computed via arctangent of the accelerometer ratios). If either the X or Y tilt angle exceeds 45°, the system classifies the event as a confirmed accident and activates the dual-mode alert.
<br>
<br>
The key innovation is the two-stage logic: jerk alone (which could result from road bumps) is insufficient for confirmation; tilt beyond 45° — indicative of a rider having fallen — must also be present. This dual-stage approach dramatically reduces false alarms while maintaining high sensitivity to genuine crash events.
