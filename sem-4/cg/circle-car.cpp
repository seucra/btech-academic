#include <GL/glut.h>
#include <iostream>

// Global variable for Car Position
float carPosition = -50.0f; // Start off-screen to the left
float speed = 0.5f;

// ---------------------------------------------------------
// 1. CIRCLE ALGORITHM (Midpoint / Bresenham)
// ---------------------------------------------------------
// This function plots 8 symmetric points for every 1 calculated point
void plotCirclePoints(int cx, int cy, int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(cx + x, cy + y);
    glVertex2i(cx - x, cy + y);
    glVertex2i(cx + x, cy - y);
    glVertex2i(cx - x, cy - y);
    glVertex2i(cx + y, cy + x);
    glVertex2i(cx - y, cy + x);
    glVertex2i(cx + y, cy - x);
    glVertex2i(cx - y, cy - x);
    glEnd();
}

void drawCircle(int cx, int cy, int radius) {
    int x = 0;
    int y = radius;
    int p = 1 - radius; // Initial Decision Parameter

    // We calculate 1/8th of the circle here
    while (x <= y) {
        plotCirclePoints(cx, cy, x, y);
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
    }
}

// ---------------------------------------------------------
// 2. DRAWING THE CAR
// ---------------------------------------------------------
void drawCar() {
    // We use glPushMatrix/Pop to isolate the car's movement
    glPushMatrix();
    
    // Move the entire car to the current global position
    glTranslatef(carPosition, 0.0f, 0.0f);

    // --- A. Car Body (Rectangle) ---
    glColor3f(0.0, 0.5, 1.0); // Blue
    glRectf(10.0, 10.0, 50.0, 30.0); // (x1, y1) to (x2, y2)
    
    // --- B. Car Top (Smaller Rectangle) ---
    glColor3f(0.0, 0.3, 0.8); // Darker Blue
    glRectf(20.0, 30.0, 40.0, 45.0);

    // --- C. Wheels (Using our Circle Function) ---
    glColor3f(1.0, 0.0, 0.0); // Red Wheels
    
    // Rear Wheel (Center at 20, 10)
    drawCircle(20, 10, 8);
    
    // Front Wheel (Center at 40, 10)
    drawCircle(40, 10, 8);

    glPopMatrix();
}

// ---------------------------------------------------------
// 3. ANIMATION LOGIC
// ---------------------------------------------------------
void display() {
    // Clear the buffer (Black background)
    glClear(GL_COLOR_BUFFER_BIT);

    drawCar();

    // SWAP BUFFERS: This is crucial for smooth animation.
    // It swaps the "hidden" drawing buffer with the "visible" screen buffer.
    glutSwapBuffers();
}

// This function is called every 16ms (approx 60 FPS)
void timer(int val) {
    // Update Logic
    carPosition += speed;

    // Reset if it goes off screen (Looping animation)
    if (carPosition > 100.0) {
        carPosition = -60.0;
    }

    // Request a redraw (calls display() again)
    glutPostRedisplay();

    // Re-register the timer for the next frame
    glutTimerFunc(16, timer, 0); 
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    // Setup a 100x100 coordinate system
    gluOrtho2D(0.0, 100.0, 0.0, 100.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    
    // GLUT_DOUBLE is required for animation to prevent flickering!
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    
    glutInitWindowSize(600, 600);
    glutCreateWindow("Car Animation (Bresenham Circles)");

    init();

    glutDisplayFunc(display);
    
    // Start the timer
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
