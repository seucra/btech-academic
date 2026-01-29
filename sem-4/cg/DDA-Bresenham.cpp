#include <GL/glut.h>
#include <iostream>
#include <cmath>

// Global variables to hold user input for the display callback
int g_day, g_month;

// ---------------------------------------------------------
// 1. DDA ALGORITHM (Floating Point)
// ---------------------------------------------------------
void drawDDA(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    // Calculate steps required for generating pixels
    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

    float xIncrement = (float)dx / (float)steps;
    float yIncrement = (float)dy / (float)steps;

    float x = x1;
    float y = y1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++) {
        glVertex2i(round(x), round(y)); // Plot pixel
        x += xIncrement;
        y += yIncrement;
    }
    glEnd();
}

// ---------------------------------------------------------
// 2. BRESENHAM ALGORITHM (Integer Only)
// ---------------------------------------------------------
void drawBresenham(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    
    int err = dx - dy;

    glBegin(GL_POINTS);
    while (true) {
        glVertex2i(x1, y1); // Plot pixel

        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
    glEnd();
}

// ---------------------------------------------------------
// OPENGL DISPLAY FUNCTION
// ---------------------------------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw a simple grid (Optional, helps visualize pixels)
    glColor3f(0.2, 0.2, 0.2); // Dark Grey
    glBegin(GL_LINES);
    for(int i=0; i<100; i++) {
        glVertex2i(i, 0); glVertex2i(i, 100);
        glVertex2i(0, i); glVertex2i(100, i);
    }
    glEnd();

    // Calculate Points based on your assignment logic
    int x1 = g_day;
    int y1 = g_month;
    int x2 = g_day + 10;
    int y2 = g_month + 5;

    // --- DRAW 1: DDA (RED) ---
    // We draw exactly at the calculated coordinates
    glColor3f(1.0, 0.0, 0.0); // Red
    glPointSize(5.0);         // Make points big so we can see them
    drawDDA(x1, y1, x2, y2);

    // --- DRAW 2: BRESENHAM (GREEN) ---
    // We shift this line UP by 2 units so it doesn't overlap the Red one.
    // This lets you compare them side-by-side.
    glColor3f(0.0, 1.0, 0.0); // Green
    drawBresenham(x1, y1 + 2, x2, y2 + 2);

    glFlush();
}

void init() {
    // Set background color to black
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    // Set up the coordinate system
    // We map the window (0 to 500px) to coordinate space (0 to 50)
    // This effectively "zooms in" so (12, 9) looks big on screen.
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 50.0, 0.0, 50.0);
}

int main(int argc, char** argv) {
    // Get User Input before creating the window
    std::cout << "Enter your birth Day (X start): ";
    std::cin >> g_day;
    std::cout << "Enter your birth Month (Y start): ";
    std::cin >> g_month;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    
    // Window size on your screen
    glutInitWindowSize(500, 500); 
    glutInitWindowPosition(100, 100);
    
    glutCreateWindow("DDA (Red) vs Bresenham (Green)");
    
    init();
    glutDisplayFunc(display);
    
    glutMainLoop();
    return 0;
}
