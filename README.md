# DART-Rover
Repository for the autonomous DART rover, ENGR 7B (Winter 2022)

Uses PixyCam for object detection, an array of three photoresistors for line following, and two sets of RGB LEDs for lighting and/or status indication.

TODO:
- ~~Calibrate line following~~
- ~~Change modes once target object is recognized~~
- ~~Recognize when end of course is reached~~
- ~~Asynchronous functions~~ -> made library

Light status indication list (WIP):
 - **Red -> yellow -> green**
      - Initializing

 - **Steady green**
      - Turning right

 - **Steady red**
      - Turning left

 - **Blinking blue**
      - All three PRs have a low signal

 - **Blinking yellow**
      - Off-course, all three PRs have high signal

 - **Blinking green**
      - Found signature, switched to target locating

 - **Green -> magenta -> green**
      - Target within reach, grabbing

 - **Off**
      - On-course or offline, not turning
