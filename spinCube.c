// appropriate headers
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef _WIN32 // if we aren't on Windows, then include unistd.h
#include <unistd.h>
#else // we're on Windows then use the Windows API to simulate usleep
#include <windows.h> // required for HANDLE, LARGE_INTEGER, etc.

// a custom implementation of usleep() for Windows
void usleep(__int64 usec) {
  HANDLE timer; // timer object
  LARGE_INTEGER t; // 64-bit integer used to set time

  t.QuadPart = -(10 * usec); // convert microseconds to 100 nanosecond intervals (negative means relative)

  timer = CreateWaitableTimer(NULL, TRUE, NULL); // create a waitable timer object
  SetWaitableTimer(timer, &t, 0, NULL, NULL, 0); // set the timer with the target time (no repetition and no callback)
  WaitForSingleObject(timer, INFINITE); // wait until the timer expires
  CloseHandle(timer);
}
#endif

// rotation angles around the axes
float A;
float B;
float C;

float cubeWidth = 20;

// terminal dimensions
int width = 160;
int height = 44;

float zBuffer[160 * 44]; // stores the depth of each pixel for proper occlusion
char buffer[160 * 44]; // stores ASCII chars for each screen cell

int backgroundASCIICode = ' ';
int distanceFromCam = 100;
float horizontalOffset;

float K1 = 40; // projection scaling factor (bigger = more zoomed in)

float incrementSpeed = 0.6;

// transformed 3D coords.
float x;
float y;
float z;

float ooz; // for perspective projection

// final 2D coords.
int xp;
int yp;

int indx; // index in the buffer arrays


// 3D Rotation Functions using Euler Angles
// calculates the rotated X-coordinate from (i, j, k)
float calculateX(int i, int j, int k) {
  return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
         j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

// calculates the rotated Y-coordinate from (i, j, k)
float calculateY(int i, int j, int k) {
  return j * cos(A) * cos(C) + k * sin(A) * cos(C) -
         j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
         i * cos(B) * sin(C);
}

// calculates the rotated Z coordinate from (i, j, k)
float calculateZ(int i, int j, int k) {
  return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

// transforms a 3D point into a 2D screen point and updates the screen and depth buffer at that location if visible.
void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch) {

  // apply the 3D toation to the point
  x = calculateX(cubeX, cubeY, cubeZ);
  y = calculateY(cubeX, cubeY, cubeZ);
  z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam; // push the object into view space

  ooz = 1 / z; // perspective projection

  // project 3D point to 2D screen coordinates
  // the constants (2.5 and 1.1) help adjust aspect ratio distortion
  xp = (int)(width / 2 + horizontalOffset + K1 * ooz * x * 2.5);
  yp = (int)(height / 2 + K1 * ooz * y * 1.1);

  indx = xp + yp * width; // calculate 1D buffer index from 2D coordinates

  // if the index is valid and this point is closer than what's already drawn
  if (indx >= 0 && indx < width * height) {
    if (ooz > zBuffer[indx]) {
      zBuffer[indx] = ooz; // update the z-buffer with the new depth
      buffer[indx] = ch; // sets the char to be drawn
    }
  }
}

int main() {
  printf("\x1b[2J"); // clear the terminal screen

  while (1) {
    memset(buffer, backgroundASCIICode, width * height); // clear screen buffer with spaces
    memset(zBuffer, 0, width * height * 4); // reset depth buffer

    // first cube
    cubeWidth = 20;
    horizontalOffset = -2 * cubeWidth;
    for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
      for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {
        // draw each face of the cube using different chars.
        calculateForSurface(cubeX, cubeY, -cubeWidth, '@');
        calculateForSurface(cubeWidth, cubeY, cubeX, '$');
        calculateForSurface(-cubeWidth, cubeY, -cubeX, '~');
        calculateForSurface(-cubeX, cubeY, cubeWidth, '#');
        calculateForSurface(cubeX, -cubeWidth, -cubeY, ';');
        calculateForSurface(cubeX, cubeWidth, cubeY, '+');
      }
    }

    // second cube
    cubeWidth = 10;
    horizontalOffset = 2 * cubeWidth;
    for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
      for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {
        calculateForSurface(cubeX, cubeY, -cubeWidth, '@');
        calculateForSurface(cubeWidth, cubeY, cubeX, '$');
        calculateForSurface(-cubeWidth, cubeY, -cubeX, '~');
        calculateForSurface(-cubeX, cubeY, cubeWidth, '#');
        calculateForSurface(cubeX, -cubeWidth, -cubeY, ';');
        calculateForSurface(cubeX, cubeWidth, cubeY, '+');
           }
    }

    // third cube
    cubeWidth = 5;
    horizontalOffset = 11 * cubeWidth;
    for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
      for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {
        calculateForSurface(cubeX, cubeY, -cubeWidth, '@');
        calculateForSurface(cubeWidth, cubeY, cubeX, '$');
        calculateForSurface(-cubeWidth, cubeY, -cubeX, '~');
        calculateForSurface(-cubeX, cubeY, cubeWidth, '#');
        calculateForSurface(cubeX, -cubeWidth, -cubeY, ';');
        calculateForSurface(cubeX, cubeWidth, cubeY, '+');
           }
    }

    // you could add more cubes!!

    printf("\x1b[H"); // move the cursor to top-left of terminal before printing next frame

    // print each char in the screen buffer
    for (int k = 0; k < width * height; k++) {
      if (k % width != 0) {
        putchar(buffer[k]); // print char
      } else {
        putchar('\n'); // new line at the end of each row
      }
    }

    // update the rotation angles for next frame
    A += 0.05;
    B += 0.05;
    C += 0.01;

    // sleep for some time to control the frame delay (animation speed)
    usleep(8000 * 2);
  }

  return 0;

}