// Read the famous donut.c, and tried to implement it completely on my own
// Math formula is obtained from https://www.a1k0n.net/2011/07/20/donut-math.html

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

const int R1 = 1;
const int R2 = 2;
const int K1 = 15;
const int K2 = 5;

float A = 0;
float B = 0;

// map from -sqrt(2) to sqrt(2) to 0 - 11, ignoring negative numbers.
int map_luminance(float x) {
    // Ignore all negative numbers
    if (x <= 0.0f) {
        return -1;
    }

    // We divide by sqrt 2 (normalise), and multiply by 11
    int index = (int)(x * (11.0f / (float)M_SQRT2));

    // clamp to 11
    if (index > 11) {
        index = 11;
    }

    return index;
}

int main() {
    int total_steps = 360;
    float two_pi = 2.0f * (float)M_PI;
    // clears the whole screen
    printf("\x1b[2J");

    while (1) {
        A += 0.08;
        B += 0.04;
        // we first create a blank canvas of 24x80 (24 = row, 80 = column)
        char output[24][80];
        float z_buffer[24][80] = {0};
        memset(output, ' ', sizeof(output)); // fill with empty char

        // This is just looping through theta (for circle) and phi (for the donut)
        for (int i = 0; i < total_steps; i++) {
            float theta = (float)i * (two_pi / (float)total_steps);
            for (int j = 0; j < total_steps; j ++) {
                float phi = (float)j * (two_pi / (float)total_steps);

                // make horizontal twice as long to compensate for rectangle terminal character
                float x = ((R2 + R1 * cos(theta)) * (cos(B) * cos(phi) + sin(A) * sin(B) * sin(phi)) - R1 * cos(A) * sin(B) * sin(theta)) * 2;
                float y = (R2 + R1 * cos(theta)) * (cos(phi) * sin(B) - cos(B) * sin(A) * sin(phi)) + R1 * cos(A) * cos(B) * sin(theta);
                float z = cos(A) * (R2 + R1 * cos(theta)) * sin(phi) + R1 * sin(A) * sin(theta);
                int xp = (K1 * x) / (K2 + z) + 40; // +40 to center the donut
                int yp = -(K1 * y) / (K2 + z) + 12; // same as above
                float L = cos(phi) * cos(theta) * sin(B) - cos(A) * cos(theta) * sin(phi) - sin(A) * sin(theta) + cos(B) * (cos(A) * sin(theta) - cos(theta) * sin(A) * sin(phi));

                // This should make the brightness an int from 0 - 11
                int brightness = map_luminance(L);

                if (yp >= 24 || yp < 0 || xp >= 80 || xp < 0) {
                    printf("Out of bound!");
                    printf("yp: %d", yp);
                    printf("xp: %d", xp);
                    return 0;
                }
                // We divide by 1/(K2 + z) so 0 means infinitely far away (bigger z = closer)
                if (z_buffer[yp][xp] < 1 / (K2 + z)) {
                    z_buffer[yp][xp] = 1 / (K2 + z);
                    if (brightness < 0) {
                        output[yp][xp] = ' ';
                    } else {
                        // Map the 0-11 to the string of increasing brightness
                        output[yp][xp] = ".,-~:;=!*#$@"[brightness];
                    }
                }
            }
        }

        // moves cursor to the top-left corner
        printf("\x1b[H");
        // Just prints out all the characters
        for (int i = 0; i < 24; i ++) {
            for (int j = 0; j < 80; j ++) {
                printf("%c", output[i][j]);
            }
            putchar('\n');
        }

        // sleep 30 miliseconds
        usleep(30000);
    }

    return 0;
}