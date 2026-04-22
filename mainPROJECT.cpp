

#include <windows.h>
#include <gl/GL.h>
#include <math.h>

// Globals Variable
HWND hwnd;
HDC g_hdc;
HGLRC g_hrc;

bool running = true;
float time_elapsed = 0.0f;
void drawLineBresenham(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    glBegin(GL_POINTS);
    while (true) {
        glVertex2f(x1 / 100.0f, y1 / 100.0f);
        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
    glEnd();
}

void drawLineDDA(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;

    int steps = (fabs(dx) > fabs(dy)) ? fabs(dx * 100) : fabs(dy * 100);

    float xInc = dx / steps;
    float yInc = dy / steps;

    float x = x1;
    float y = y1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++) {
        glVertex2f(x, y);
        x += xInc;
        y += yInc;
    }
    glEnd();
}

void drawCircle(float cx, float cy, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 50; i++) {
        float a = i * 2.0f * 3.14159f / 50;
        glVertex2f(cx + cos(a)*r, cy + sin(a)*r);
    }
    glEnd();
}

void drawCircleBorder(float cx, float cy, float r) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 48; i++) {
        float a = i * 2.0f * 3.14159f / 48;
        glVertex2f(cx + cos(a) * r, cy + sin(a) * r);
    }
    glEnd();
}

void drawSky() {
    glColor3f(0.53f, 0.81f, 0.92f);
    glBegin(GL_QUADS);
    glVertex2f(-1, 1);
    glVertex2f(1, 1);
    glVertex2f(1, 0);
    glVertex2f(-1, 0);
    glEnd();
}

void drawGround() {
    glColor3f(0.13f, 0.55f, 0.13f);
    glBegin(GL_QUADS);
    glVertex2f(-1, 0);
    glVertex2f(1, 0);
    glVertex2f(1, -1);
    glVertex2f(-1, -1);
    glEnd();
}

void drawEllipse(float cx, float cy, float rx, float ry) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 48; i++) {
        float a = i * 2.0f * 3.14159f / 48;
        glVertex2f(cx + cos(a) * rx, cy + sin(a) * ry);
    }
    glEnd();
}

void drawPath() {
    // Curved dirt path from foreground to house door
    glColor3f(0.58f, 0.42f, 0.24f);
    glBegin(GL_QUADS);
    glVertex2f(0.45f, -1.00f);
    glVertex2f(0.74f, -1.00f);
    glVertex2f(0.66f, -0.20f);
    glVertex2f(0.56f, -0.20f);
    glEnd();

    // Path highlight strip for depth
    glColor3f(0.66f, 0.50f, 0.30f);
    glBegin(GL_QUADS);
    glVertex2f(0.53f, -1.00f);
    glVertex2f(0.61f, -1.00f);
    glVertex2f(0.61f, -0.20f);
    glVertex2f(0.58f, -0.20f);
    glEnd();

    // Path border
    glColor3f(0.24f, 0.16f, 0.08f);
    glLineWidth(1.6f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.45f, -1.00f);
    glVertex2f(0.74f, -1.00f);
    glVertex2f(0.66f, -0.20f);
    glVertex2f(0.56f, -0.20f);
    glEnd();
    glLineWidth(1.0f);
}

void drawGrassDetails(float t) {
    // Denser, brighter and taller layered grass for better visibility
    glLineWidth(1.5f);
    for (int i = 0; i < 78; i++) {
        float x = -0.99f + i * 0.0255f;
        float sway = sin(t * 1.6f + i * 0.47f) * 0.006f;
        float h1 = 0.08f + 0.02f * sin(i * 0.33f);
        float h2 = 0.10f + 0.015f * cos(i * 0.29f);

        // Keep the path clean and visible
        bool onPathZone = (x > 0.44f && x < 0.74f);
        if (onPathZone) {
            continue;
        }

        glColor3f(0.10f, 0.46f, 0.10f);
       drawLineDDA(x, -0.95f, x + sway, -0.95f + h1);

        glColor3f(0.16f, 0.62f, 0.16f);
    drawLineDDA(x, -0.95f, x + sway, -0.95f + h1);

glColor3f(0.10f, 0.46f, 0.10f);
drawLineDDA(x, -0.95f, x + sway, -0.95f + h1);

glColor3f(0.16f, 0.62f, 0.16f);
drawLineDDA(x + 0.007f, -0.92f, x + 0.010f + sway, -0.92f + h2);

glColor3f(0.22f, 0.72f, 0.22f);
drawLineDDA(x - 0.005f, -0.90f, x - 0.002f + sway, -0.90f + h1 * 0.9f);
    }
    glLineWidth(1.0f);
}

void drawGroundShadows(float t) {
    // Soft contact shadows for key objects to reduce flat look
    glColor3f(0.08f, 0.33f, 0.08f);

    // House shadow
    drawEllipse(0.60f, -0.24f, 0.27f, 0.05f);

    // Windmill shadow
    drawEllipse(-0.30f, -0.39f, 0.12f, 0.03f);

    // Tree shadows
    drawEllipse(-0.90f, -0.23f, 0.10f, 0.024f);
    drawEllipse(-0.75f, -0.23f, 0.10f, 0.024f);
    drawEllipse(-0.70f, -0.23f, 0.10f, 0.024f);
    drawEllipse(-0.55f, -0.23f, 0.10f, 0.024f);
    drawEllipse(-0.40f, -0.23f, 0.10f, 0.024f);
    drawEllipse(-0.25f, -0.23f, 0.10f, 0.024f);
    drawEllipse(0.70f, -0.23f, 0.10f, 0.024f);
    drawEllipse(0.85f, -0.23f, 0.10f, 0.024f);

    // Player and ball shadows with slight motion
    float ballX = -0.28f + sin(t * 1.35f) * 0.17f;
    drawEllipse(-0.65f, -0.71f, 0.05f, 0.014f);
    drawEllipse(-0.45f + sin(t * 0.8f) * 0.05f, -0.74f, 0.05f, 0.014f);
    drawEllipse(-0.15f + sin(t * 1.2f) * 0.06f, -0.71f, 0.05f, 0.014f);
    drawEllipse(0.15f, -0.68f, 0.05f, 0.014f);
    drawEllipse(ballX, -0.70f, 0.025f, 0.010f);
}

void drawSun() {
    glColor3f(1.0f, 0.95f, 0.1f);
    drawCircle(0.6f, 0.7f, 0.095f);
}

void drawCloud(float x, float y) {
    glColor3f(1,1,1);
    drawCircle(x, y, 0.05f);
    drawCircle(x+0.05f, y+0.02f, 0.05f);
}

void drawClouds(float t) {
    float offsetNear = sin(t * 0.2f) * 0.2f;
    float offsetFar = sin(t * 0.13f + 1.2f) * 0.14f;

    // Near layer clouds
    drawCloud(-0.60f + offsetNear, 0.60f);
    drawCloud(0.20f + offsetNear, 0.65f);
    drawCloud(0.70f + offsetNear, 0.60f);
    drawCloud(0.05f + offsetNear, 0.58f);
    drawCloud(-0.25f + offsetNear, 0.67f);
    drawCloud(0.45f + offsetNear, 0.56f);

    // Far layer clouds
    drawCloud(-0.85f + offsetFar, 0.75f);
    drawCloud(0.90f + offsetFar, 0.72f);
    drawCloud(-0.05f + offsetFar, 0.79f);
    drawCloud(0.55f + offsetFar, 0.77f);
}

void drawMountains() {
    glColor3f(0.3f, 0.5f, 0.7f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1, 0);
    glVertex2f(-0.5f, 0.5f);
    glVertex2f(0, 0);
    glEnd();

    glColor3f(0.4f, 0.7f, 0.9f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 0);
    glVertex2f(0.5f, 0.6f);
    glVertex2f(1, 0);
    glEnd();
}

void drawTree(float x) {
    // Trunk
    glColor3f(0.5f, 0.3f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(x-0.025f, -0.2f);
    glVertex2f(x+0.025f, -0.2f);
    glVertex2f(x+0.025f, 0.05f);
    glVertex2f(x-0.025f, 0.05f);
    glEnd();

    // Trunk outline
    glColor3f(0.1f, 0.06f, 0.02f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x-0.025f, -0.2f);
    glVertex2f(x+0.025f, -0.2f);
    glVertex2f(x+0.025f, 0.05f);
    glVertex2f(x-0.025f, 0.05f);
    glEnd();

    // Foliage - bottom circle
    glColor3f(0.2f, 0.7f, 0.2f);
    drawCircle(x, 0.1f, 0.12f);
    glColor3f(0.08f, 0.34f, 0.08f);
    drawCircleBorder(x, 0.1f, 0.12f);

    // Foliage - middle circle (overlapping)
    glColor3f(0.15f, 0.65f, 0.15f);
    drawCircle(x, 0.18f, 0.11f);
    glColor3f(0.08f, 0.34f, 0.08f);
    drawCircleBorder(x, 0.18f, 0.11f);

    // Foliage - top circle
    glColor3f(0.25f, 0.75f, 0.25f);
    drawCircle(x, 0.25f, 0.09f);
    glColor3f(0.08f, 0.34f, 0.08f);
    drawCircleBorder(x, 0.25f, 0.09f);
    glLineWidth(1.0f);
}

void drawFenceSegment(float x, float y, float w, float h) {
    // Wooden fence post
    glColor3f(0.72f, 0.52f, 0.28f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    // Fence post cap
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + h);
    glVertex2f(x + w, y + h);
    glVertex2f(x + w * 0.5f, y + h + h * 0.35f);
    glEnd();

    // Outline for clarity
    glColor3f(0.2f, 0.12f, 0.06f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
    glLineWidth(1.0f);
}

void drawFences() {
    // Left fence line
    for (int i = 0; i < 9; i++) {
        drawFenceSegment(-0.96f + i * 0.055f, -0.36f, 0.022f, 0.11f);
    }

    // Horizontal rails left
    glColor3f(0.68f, 0.48f, 0.24f);
    glBegin(GL_QUADS);
    glVertex2f(-0.96f, -0.30f);
    glVertex2f(-0.49f, -0.30f);
    glVertex2f(-0.49f, -0.28f);
    glVertex2f(-0.96f, -0.28f);
    glVertex2f(-0.96f, -0.34f);
    glVertex2f(-0.49f, -0.34f);
    glVertex2f(-0.49f, -0.32f);
    glVertex2f(-0.96f, -0.32f);
    glEnd();
}

void drawGateNearHouse() {
    // Two posts + small gate near the house path
    drawFenceSegment(0.49f, -0.36f, 0.024f, 0.12f);
    drawFenceSegment(0.72f, -0.36f, 0.024f, 0.12f);

    glColor3f(0.70f, 0.50f, 0.26f);
    glBegin(GL_QUADS);
    glVertex2f(0.515f, -0.34f);
    glVertex2f(0.72f, -0.34f);
    glVertex2f(0.72f, -0.25f);
    glVertex2f(0.515f, -0.25f);
    glEnd();

    glColor3f(0.2f, 0.12f, 0.06f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.515f, -0.34f);
    glVertex2f(0.72f, -0.34f);
    glVertex2f(0.72f, -0.25f);
    glVertex2f(0.515f, -0.25f);
    glEnd();

    // Vertical slats for gate detail
    glBegin(GL_LINES);
    glVertex2f(0.545f, -0.34f); glVertex2f(0.545f, -0.25f);
    glVertex2f(0.575f, -0.34f); glVertex2f(0.575f, -0.25f);
    glVertex2f(0.605f, -0.34f); glVertex2f(0.605f, -0.25f);
    glVertex2f(0.635f, -0.34f); glVertex2f(0.635f, -0.25f);
    glVertex2f(0.665f, -0.34f); glVertex2f(0.665f, -0.25f);
    glVertex2f(0.695f, -0.34f); glVertex2f(0.695f, -0.25f);
    glEnd();
    glLineWidth(1.0f);
}

void drawFlower(float x, float y, float s, float phase, int style) {
    // Stem
    glColor3f(0.12f, 0.44f, 0.12f);
    glLineWidth(1.8f);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x + sin(phase) * s * 0.08f, y + s * 0.24f);
    glEnd();
    glLineWidth(1.0f);

    float cx = x + sin(phase) * s * 0.08f;
    float cy = y + s * 0.24f;

    // Petals (two palettes for variety)
    if (style == 0) {
        glColor3f(0.92f, 0.46f, 0.68f);
    } else {
        glColor3f(0.88f, 0.82f, 0.34f);
    }
    drawCircle(cx - s * 0.045f, cy, s * 0.03f);
    drawCircle(cx + s * 0.045f, cy, s * 0.03f);
    drawCircle(cx, cy - s * 0.045f, s * 0.03f);
    drawCircle(cx, cy + s * 0.045f, s * 0.03f);

    // Petal borders
    glColor3f(0.35f, 0.15f, 0.25f);
    glLineWidth(1.0f);
    for (int p = 0; p < 4; p++) {
        float ox = 0.0f;
        float oy = 0.0f;
        if (p == 0) ox = -s * 0.045f;
        if (p == 1) ox =  s * 0.045f;
        if (p == 2) oy = -s * 0.045f;
        if (p == 3) oy =  s * 0.045f;

        glBegin(GL_LINE_LOOP);
        for (int i = 0; i <= 24; i++) {
            float a = i * 2.0f * 3.14159f / 24;
            glVertex2f(cx + ox + cos(a) * s * 0.03f, cy + oy + sin(a) * s * 0.03f);
        }
        glEnd();
    }

    // Center
    if (style == 0) {
        glColor3f(0.92f, 0.78f, 0.12f);
    } else {
        glColor3f(0.95f, 0.52f, 0.18f);
    }
    drawCircle(cx, cy, s * 0.028f);

    // Center border
    glColor3f(0.35f, 0.25f, 0.05f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 24; i++) {
        float a = i * 2.0f * 3.14159f / 24;
        glVertex2f(cx + cos(a) * s * 0.028f, cy + sin(a) * s * 0.028f);
    }
    glEnd();
    glLineWidth(1.0f);
}

void drawFlowers(float t) {
    // Smaller flowers with irregular spacing for a natural look
    drawFlower(-0.92f, -0.62f, 0.46f, t * 0.8f + 0.2f, 0);
    drawFlower(-0.84f, -0.58f, 0.42f, t * 0.9f + 0.6f, 1);
    drawFlower(-0.77f, -0.64f, 0.50f, t * 0.7f + 1.1f, 0);
    drawFlower(-0.69f, -0.59f, 0.44f, t * 1.0f + 0.2f, 1);
    drawFlower(-0.60f, -0.66f, 0.47f, t * 0.85f + 0.4f, 0);
    drawFlower(-0.52f, -0.61f, 0.40f, t * 0.75f + 0.9f, 1);

    // Right-side flowers moved away from path (path x range is roughly 0.45 to 0.74)
    drawFlower(0.80f, -0.65f, 0.41f, t * 0.88f + 0.5f, 1);
    drawFlower(0.86f, -0.60f, 0.46f, t * 0.72f + 1.0f, 0);
    drawFlower(0.92f, -0.67f, 0.44f, t * 0.82f + 0.1f, 1);
    drawFlower(0.97f, -0.61f, 0.48f, t * 0.9f + 1.5f, 0);
}

void drawPond(float t) {
    float cx = 0.33f;
    float cy = -0.74f;

    // Main water body
    glColor3f(0.20f, 0.58f, 0.86f);
    drawEllipse(cx, cy, 0.15f, 0.09f);

    // Inner lighter water layer
    glColor3f(0.36f, 0.72f, 0.95f);
    drawEllipse(cx, cy, 0.11f, 0.06f);

    // Pond border
    glColor3f(0.10f, 0.26f, 0.38f);
    glLineWidth(1.8f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 56; i++) {
        float a = i * 2.0f * 3.14159f / 56;
        glVertex2f(cx + cos(a) * 0.15f, cy + sin(a) * 0.09f);
    }
    glEnd();

    // Ripples
    float ripple = 0.004f * sin(t * 2.2f);
    glColor3f(0.78f, 0.92f, 1.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 44; i++) {
        float a = i * 2.0f * 3.14159f / 44;
        glVertex2f(cx - 0.04f + cos(a) * (0.045f + ripple), cy + sin(a) * (0.020f + ripple * 0.6f));
    }
    glEnd();
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 44; i++) {
        float a = i * 2.0f * 3.14159f / 44;
        glVertex2f(cx + 0.04f + cos(a) * (0.038f - ripple), cy + 0.01f + sin(a) * (0.017f - ripple * 0.5f));
    }
    glEnd();
    glLineWidth(1.0f);
}

void drawBench() {
    // Bench legs
    glColor3f(0.24f, 0.14f, 0.07f);


    // Bench seat and backrest
    glColor3f(0.65f, 0.44f, 0.22f);
    glBegin(GL_QUADS);
    glVertex2f(0.22f, -0.45f); glVertex2f(0.44f, -0.45f); glVertex2f(0.44f, -0.41f); glVertex2f(0.22f, -0.41f);
    glVertex2f(0.22f, -0.39f); glVertex2f(0.44f, -0.39f); glVertex2f(0.44f, -0.35f); glVertex2f(0.22f, -0.35f);
    glEnd();

    // Bench outlines
    glColor3f(0.16f, 0.09f, 0.04f);
    glLineWidth(1.3f)
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.22f, -0.45f); glVertex2f(0.44f, -0.45f); glVertex2f(0.44f, -0.41f); glVertex2f(0.22f, -0.41f);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.22f, -0.39f); glVertex2f(0.44f, -0.39f); glVertex2f(0.44f, -0.35f); glVertex2f(0.22f, -0.35f);
    glEnd();
    glLineWidth(1.0f);
}

void drawBicycle() {
    float x = 0.72f;
    float y = -0.74f;

    // Wheels
    glColor3f(0.10f, 0.10f, 0.10f);
    drawCircle(x, y, 0.050f);
    drawCircle(x + 0.18f, y, 0.050f);

    glColor3f(0.72f, 0.72f, 0.72f);
    drawCircle(x, y, 0.032f);
    drawCircle(x + 0.18f, y, 0.032f);

    // Bicycle frame
    glColor3f(0.85f, 0.10f, 0.10f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y); glVertex2f(x + 0.09f, y + 0.06f);
    glVertex2f(x + 0.09f, y + 0.06f); glVertex2f(x + 0.18f, y);
    glVertex2f(x + 0.09f, y + 0.06f); glVertex2f(x + 0.05f, y);
    glVertex2f(x + 0.05f, y); glVertex2f(x + 0.18f, y);
    glVertex2f(x + 0.09f, y + 0.06f); glVertex2f(x + 0.09f, y + 0.10f);
    glVertex2f(x + 0.18f, y); glVertex2f(x + 0.205f, y + 0.07f);
    glEnd();

    // Handlebar and seat
    glColor3f(0.08f, 0.08f, 0.08f);
    glBegin(GL_LINES);
    glVertex2f(x + 0.205f, y + 0.07f); glVertex2f(x + 0.22f, y + 0.09f);
    glVertex2f(x + 0.08f, y + 0.10f); glVertex2f(x + 0.11f, y + 0.10f);
    glEnd();
    glLineWidth(1.0f);
}

void drawWindmill(float t) {
    float cx = -0.3f, cy = -0.1f;
    float angle = t * 3;
    float PI = 3.14159f;

    // Windmill tower/base (smaller)
    glColor3f(0.65f, 0.45f, 0.25f);
    glBegin(GL_QUADS);
    glVertex2f(cx-0.022f, cy-0.28f);
    glVertex2f(cx+0.022f, cy-0.28f);
    glVertex2f(cx+0.022f, cy+0.04f);
    glVertex2f(cx-0.022f, cy+0.04f);
    glEnd();

    // Base outline
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(cx-0.022f, cy-0.28f);
    glVertex2f(cx+0.022f, cy-0.28f);
    glVertex2f(cx+0.022f, cy+0.04f);
    glVertex2f(cx-0.022f, cy+0.04f);
    glEnd();
    glLineWidth(1.0f);

    // Windmill gear box (red cylinder head - smaller)
    glColor3f(0.9f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(cx-0.045f, cy+0.015f);
    glVertex2f(cx+0.045f, cy+0.015f);
    glVertex2f(cx+0.045f, cy+0.075f);
    glVertex2f(cx-0.045f, cy+0.075f);
    glEnd();

    // Gear box outline
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(cx-0.045f, cy+0.015f);
    glVertex2f(cx+0.045f, cy+0.015f);
    glVertex2f(cx+0.045f, cy+0.075f);
    glVertex2f(cx-0.045f, cy+0.075f);
    glEnd();
    glLineWidth(1.0f);

    // Windmill roof/cap (brown triangle - smaller)
    glColor3f(0.75f, 0.5f, 0.3f);
    glBegin(GL_TRIANGLES);
    glVertex2f(cx-0.06f, cy+0.075f);
    glVertex2f(cx+0.06f, cy+0.075f);
    glVertex2f(cx, cy+0.16f);
    glEnd();

    // Roof outline
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(cx-0.06f, cy+0.075f);
    glVertex2f(cx+0.06f, cy+0.075f);
    glVertex2f(cx, cy+0.16f);
    glEnd();

    // Windmill hub (dark red circle - smaller)
    glColor3f(0.7f, 0.1f, 0.1f);
    drawCircle(cx, cy+0.04f, 0.032f);

    // 4 rotating triangular blades - smaller
    for(int i=0; i<4; i++){
        float a = angle + i * PI/2;
        float x1 = cx + cos(a)*0.055f;
        float y1 = cy+0.04f + sin(a)*0.055f;
        float x2 = cx + cos(a)*0.22f;
        float y2 = cy+0.04f + sin(a)*0.22f;

        // Perpendicular direction for blade width
        float px = -sin(a)*0.025f;
        float py = cos(a)*0.025f;

        glColor3f(0.98f, 0.85f, 0.65f);
        glBegin(GL_TRIANGLES);
        glVertex2f(x2, y2);
        glVertex2f(x1-px, y1-py);
        glVertex2f(x1+px, y1+py);
        glEnd();

        // Blade outline
        glColor3f(0.6f, 0.4f, 0.2f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x2, y2);
        glVertex2f(x1-px, y1-py);
        glVertex2f(x1+px, y1+py);
        glEnd();
        glLineWidth(1.0f);
    }
}

void drawBird(float x, float y, float size) {
    // Bird body (main triangle)
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y);
    glVertex2f(x-size, y-size*0.5f);
    glVertex2f(x-size*0.3f, y+size*0.3f);
    glEnd();

    // Left wing
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x-size*0.2f, y);
    glVertex2f(x-size*0.6f, y-size*0.4f);
    glVertex2f(x-size*0.5f, y+size*0.1f);
    glEnd();

    // Eye
    glColor3f(1.0f, 1.0f, 0.0f);
    drawCircle(x+size*0.1f, y+size*0.1f, size*0.15f);
}

void drawBirds(float t) {
    // Bird 1 - moving left-right with vertical variation
    float b1x = sin(t * 0.8f) * 0.35f + 0.15f;
    float b1y = cos(t * 0.5f) * 0.15f + 0.5f;
    drawBird(b1x, b1y, 0.024f);

    // Bird 2 - moving in different pattern
    float b2x = cos(t * 1.1f) * 0.4f - 0.1f;
    float b2y = sin(t * 0.6f) * 0.2f + 0.45f;
    drawBird(b2x, b2y, 0.027f);

    // Bird 3 - circular motion
    float b3x = sin(t * 0.7f) * 0.3f + 0.3f;
    float b3y = cos(t * 0.9f) * 0.18f + 0.55f;
    drawBird(b3x, b3y, 0.021f);

    // Bird 4 - diagonally moving
    float b4x = sin(t * 1.2f + 2.0f) * 0.35f - 0.2f;
    float b4y = cos(t * 0.4f) * 0.12f + 0.52f;
    drawBird(b4x, b4y, 0.025f);

    // Bird 5 - random-like pattern
    float b5x = sin(t * 0.6f + 1.5f) * 0.32f + 0.25f;
    float b5y = cos(t * 1.3f) * 0.22f + 0.48f;
    drawBird(b5x, b5y, 0.023f);
}

void drawPerson(float x, float y, float size, bool leftArm) {
    // Head
    glColor3f(0.9f, 0.7f, 0.5f);
    drawCircle(x, y + size*0.35f, size*0.12f);

    // Body
    glColor3f(0.2f, 0.4f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(x - size*0.08f, y + size*0.2f);
    glVertex2f(x + size*0.08f, y + size*0.2f);
    glVertex2f(x + size*0.08f, y - size*0.1f);
    glVertex2f(x - size*0.08f, y - size*0.1f);
    glEnd();

    // Left arm
    glColor3f(0.9f, 0.7f, 0.5f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    if (leftArm) {
        glVertex2f(x - size*0.08f, y + size*0.15f);
        glVertex2f(x - size*0.25f, y + size*0.05f);
    } else {
        glVertex2f(x - size*0.08f, y + size*0.15f);
        glVertex2f(x - size*0.2f, y + size*0.2f);
    }
    glEnd();

    // Right arm
    glBegin(GL_LINES);
    if (leftArm) {
        glVertex2f(x + size*0.08f, y + size*0.15f);
        glVertex2f(x + size*0.25f, y + size*0.2f);
    } else {
        glVertex2f(x + size*0.08f, y + size*0.15f);
        glVertex2f(x + size*0.25f, y + size*0.05f);
    }
    glEnd();
    glLineWidth(1.0f);

    // Left leg
    glBegin(GL_LINES);
    glVertex2f(x - size*0.05f, y - size*0.1f);
    glVertex2f(x - size*0.1f, y - size*0.3f);
    glEnd();

    // Right leg
    glBegin(GL_LINES);
    glVertex2f(x + size*0.05f, y - size*0.1f);
    glVertex2f(x + size*0.1f, y - size*0.3f);
    glEnd();
    glLineWidth(1.0f);
}

void drawFootball(float x, float y) {
    // Main ball body - brown/leather
    glColor3f(0.8f, 0.4f, 0.1f);
    drawCircle(x, y, 0.018f);

    // White stripe/panel on the ball
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.015f, y);
    glVertex2f(x + 0.015f, y);
    glVertex2f(x, y + 0.014f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.015f, y);
    glVertex2f(x + 0.015f, y);
    glVertex2f(x, y - 0.014f);
    glEnd();

    // Ball outline - darker color for definition
    glColor3f(0.3f, 0.15f, 0.05f);
    glLineWidth(1.5f);
    drawCircle(x, y, 0.018f);
    glLineWidth(1.0f);

    // Laces detail - simple stitching
    glColor3f(0.2f, 0.1f, 0.05f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(x - 0.008f, y + 0.008f);
    glVertex2f(x - 0.008f, y - 0.008f);
    glVertex2f(x + 0.008f, y + 0.008f);
    glVertex2f(x + 0.008f, y - 0.008f);
    glEnd();
    glLineWidth(1.0f);
}

void drawFootballPlayers(float t) {
    // Player 1 - left, kicking motion
    float kick1 = sin(t * 1.5f) * 0.08f;
    drawPerson(-0.65f, -0.65f + kick1, 0.16f, true);

    // Player 2 - center left, defending
    float move2 = sin(t * 0.8f) * 0.05f;
    drawPerson(-0.45f + move2, -0.68f, 0.16f, false);

    // Player 3 - center right, running
    float move3 = sin(t * 1.2f) * 0.06f;
    drawPerson(-0.15f + move3, -0.65f, 0.16f, true);

    // Player 4 - right, jumping
    float jump4 = sin(t * 1.8f) * 0.12f;
    drawPerson(0.15f, -0.6f + jump4, 0.16f, false);

    // Football - moving around players
    float ballX = -0.28f + sin(t * 1.35f) * 0.17f;
    float ballY = -0.69f + fabs(sin(t * 1.6f)) * 0.055f;
    drawFootball(ballX, ballY);
}

void drawHouse() {
    // Foundation/base
    glColor3f(0.7f, 0.5f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(0.38f, -0.26f);
    glVertex2f(0.82f, -0.26f);
    glVertex2f(0.82f, -0.23f);
    glVertex2f(0.38f, -0.23f);
    glEnd();

    // Foundation outline
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.38f, -0.26f);
    glVertex2f(0.82f, -0.26f);
    glVertex2f(0.82f, -0.23f);
    glVertex2f(0.38f, -0.23f);
    glEnd();
    glLineWidth(1.0f);

    // Main house body
    glColor3f(0.95f, 0.88f, 0.78f);
    glBegin(GL_QUADS);
    glVertex2f(0.4f, -0.23f);
    glVertex2f(0.8f, -0.23f);
    glVertex2f(0.8f, 0.1f);
    glVertex2f(0.4f, 0.1f);
    glEnd();

    // House body outline - thick dark border
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.4f, -0.23f);
    glVertex2f(0.8f, -0.23f);
    glVertex2f(0.8f, 0.1f);
    glVertex2f(0.4f, 0.1f);
    glEnd();
    glLineWidth(1.0f);

    // Roof - large dark red triangle
    glColor3f(0.75f, 0.02f, 0.02f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.4f, 0.1f);
    glVertex2f(0.6f, 0.32f);
    glVertex2f(0.8f, 0.1f);
    glEnd();

    // Roof outline
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.4f, 0.1f);
    glVertex2f(0.6f, 0.32f);
    glVertex2f(0.8f, 0.1f);
    glEnd();
    glLineWidth(1.0f);

    // Roof trim/eave line
    glColor3f(0.65f, 0.0f, 0.0f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2f(0.4f, 0.1f);
    glVertex2f(0.8f, 0.1f);
    glEnd();
    glLineWidth(1.0f);

    // Chimney - dark brown on left side of roof
    glColor3f(0.5f, 0.25f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(0.46f, 0.12f);
    glVertex2f(0.52f, 0.12f);
    glVertex2f(0.52f, 0.28f);
    glVertex2f(0.46f, 0.28f);
    glEnd();

    // Chimney outline
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.46f, 0.12f);
    glVertex2f(0.52f, 0.12f);
    glVertex2f(0.52f, 0.28f);
    glVertex2f(0.46f, 0.28f);
    glEnd();
    glLineWidth(1.0f);

    // Chimney bricks pattern
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(0.46f, 0.18f);
    glVertex2f(0.52f, 0.18f);
    glVertex2f(0.46f, 0.23f);
    glVertex2f(0.52f, 0.23f);
    glEnd();
    glLineWidth(1.0f);

    // Front door (centered, properly sized)
    glColor3f(0.35f, 0.18f, 0.02f);
    glBegin(GL_QUADS);
    glVertex2f(0.565f, -0.18f);
    glVertex2f(0.635f, -0.18f);
    glVertex2f(0.635f, 0.04f);
    glVertex2f(0.565f, 0.04f);
    glEnd();

    // Door frame/outline - thick dark border
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.565f, -0.18f);
    glVertex2f(0.635f, -0.18f);
    glVertex2f(0.635f, 0.04f);
    glVertex2f(0.565f, 0.04f);
    glEnd();
    glLineWidth(1.0f);

    // Door panel line - divides door in half
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(0.6f, -0.18f);
    glVertex2f(0.6f, 0.04f);
    glEnd();
    glLineWidth(1.0f);

    // Door handle (left side)
    glColor3f(1.0f, 0.85f, 0.0f);
    drawCircle(0.625f, -0.07f, 0.012f);

    // Left lower window
    glColor3f(0.45f, 0.72f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.44f, -0.08f);
    glVertex2f(0.53f, -0.08f);
    glVertex2f(0.53f, 0.01f);
    glVertex2f(0.44f, 0.01f);
    glEnd();

    // Left window outline - thick border
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.44f, -0.08f);
    glVertex2f(0.53f, -0.08f);
    glVertex2f(0.53f, 0.01f);
    glVertex2f(0.44f, 0.01f);
    glEnd();
    glLineWidth(1.0f);

    // Left window cross panes
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(0.485f, -0.08f);
    glVertex2f(0.485f, 0.01f);
    glVertex2f(0.44f, -0.035f);
    glVertex2f(0.53f, -0.035f);
    glEnd();
    glLineWidth(1.0f);

    // Right lower window
    glColor3f(0.45f, 0.72f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.67f, -0.08f);
    glVertex2f(0.76f, -0.08f);
    glVertex2f(0.76f, 0.01f);
    glVertex2f(0.67f, 0.01f);
    glEnd();

    // Right window outline - thick border
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.67f, -0.08f);
    glVertex2f(0.76f, -0.08f);
    glVertex2f(0.76f, 0.01f);
    glVertex2f(0.67f, 0.01f);
    glEnd();
    glLineWidth(1.0f);

    // Right window cross panes
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(0.715f, -0.08f);
    glVertex2f(0.715f, 0.01f);
    glVertex2f(0.67f, -0.035f);
    glVertex2f(0.76f, -0.035f);
    glEnd();
    glLineWidth(1.0f);

    // Window panes (cross lines)
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    // Left window divider
    glVertex2f(0.485f, -0.08f);
    glVertex2f(0.485f, 0.01f);
    glVertex2f(0.44f, -0.035f);
    glVertex2f(0.53f, -0.035f);
    // Right window divider
    glVertex2f(0.715f, -0.08f);
    glVertex2f(0.715f, 0.01f);
    glVertex2f(0.67f, -0.035f);
    glVertex2f(0.76f, -0.035f);
    glEnd();
    glLineWidth(1.0f);
}

void drawSceneGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawSky();
    drawMountains();
    drawSun();
    drawClouds(time_elapsed);
    drawGround();
    drawPath();
    drawGrassDetails(time_elapsed);
    drawPond(time_elapsed);
    drawGroundShadows(time_elapsed);

    // Multiple trees scattered around (left side forest)
    drawTree(-0.9f);
    drawTree(-0.75f);
    drawTree(-0.7f);
    drawTree(-0.55f);
    drawTree(-0.4f);
    drawTree(-0.25f);

    // Right side trees
    drawTree(0.7f);
    drawTree(0.85f);

    // Foreground details
    drawFences();
    drawGateNearHouse();
    drawFlowers(time_elapsed);
    drawBench();
    drawBicycle();

    drawHouse();
    drawWindmill(time_elapsed);
    drawBirds(time_elapsed);
    drawFootballPlayers(time_elapsed);

    SwapBuffers(g_hdc);
}

// ----------- WINDOW ------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CLOSE) running = false;
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// ----------- MAIN ------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = "OpenGL";
    wc.hInstance = hInstance;

    RegisterClass(&wc);

    hwnd = CreateWindow("OpenGL","Village OpenGL",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100,100,1400,800,
        NULL,NULL,hInstance,NULL);

    g_hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = { sizeof(pfd),1 };
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    int pf = ChoosePixelFormat(g_hdc,&pfd);
    SetPixelFormat(g_hdc,pf,&pfd);

    g_hrc = wglCreateContext(g_hdc);
    wglMakeCurrent(g_hdc,g_hrc);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);

    MSG msg;
    DWORD last = GetTickCount();

    while(running){
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            DWORD now = GetTickCount();
            float dt = (now - last)*0.001f;
            last = now;

            time_elapsed += dt;

            drawSceneGL();
        }
    }

    wglMakeCurrent(NULL,NULL);
    wglDeleteContext(g_hrc);
    ReleaseDC(hwnd,g_hdc);

    return 0;
}
