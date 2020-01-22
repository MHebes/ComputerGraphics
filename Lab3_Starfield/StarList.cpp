#include "StarList.h"
#include <iostream>

StarList::StarList(unsigned int numStars, float spread, float speed) : spread_(spread), speed_(speed)
{
    for (int i = 0; i < numStars; ++i) {
        stars_.push_back({ 0, 0, 0, 0, 0, QColor(255, 255, 255) });
        initStar(i);
    }
}

StarList::~StarList()
{}

void StarList::initStar(unsigned int idx)
{
	if (idx >= stars_.size()) {
		return;
	}

	// randomGen.generateDouble will generate a random number [0,1)
    // Generate positions: (-1, 1)
	stars_[idx].x = 2.0f * (randomGen_.generateDouble() - 0.5f) * spread_;
	stars_[idx].y = 2.0f * (randomGen_.generateDouble() - 0.5f) * spread_;
	stars_[idx].z = (randomGen_.generateDouble() + 0.0001f) * spread_;
	stars_[idx].speed = speed_;
    stars_[idx].lifetime = 0.0f;
	stars_[idx].color = QColor(255, 255, 255);
}

void StarList::updateAndRender(QImage& image, float delta, const QSize& windowSize)
{
    // We compute half width and half height, because we are
    // only working with half of the screen.     
    float halfWidth = 800 / 2.0f;
    float halfHeight = 600 / 2.0f;

    // Note the conversion to radians
    // PI / 180 * FOV / 2
    float tanHalfFOV = tan(3.141592654 * 70 / 360);

    // Iterate through all of your stars 
    for (int i = 0; i < stars_.size(); i++) {
        stars_[i].z -= delta * speed_;
        stars_[i].lifetime += delta;

        if (stars_[i].z <= 0) {
            initStar(i);
            continue;
        }

        float givePerspective = stars_[i].z * tanHalfFOV ;

        // Apply our perspective
        int x = (int)((stars_[i].x / (givePerspective)) * halfWidth + halfWidth);
        int y = (int)((stars_[i].y / (givePerspective)) * halfHeight + halfHeight);

        // Reinitialize a star
        if (x <0 || x >= windowSize.width()) {
            initStar(i);
            continue;
        }
        if (y <0 || y >= windowSize.height()) {
            initStar(i);
            continue;
        }

        // Change color based on distance and angle
        int h = 180 * atan2(stars_[i].y, stars_[i].x) / 3.1416f;
        if (h < 0) h += 360;
        int s = 255 * randomGen_.generateDouble();
        int v = 255 * 5 * stars_[i].lifetime;
        if (v > 255) v = 255;
        stars_[i].color.setHsv(h, s, v);

        // Draw a pixel to the renderer.
        image.setPixelColor(x, y, stars_[i].color);
    }
}
