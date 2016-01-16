//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Keyboard for eyetracking input.

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "Element.h"
#include "externals/GLM/glm/glm.hpp"

#include <vector>

namespace eyegui
{
    class Keyboard : public Element
    {
    public:

        // TODO
        // - ASCII drawing of hexagon ring
        // - Float? Integer? meh...

        // Constructor
        Keyboard(
            std::string id,
            std::string styleName,
            Element* pParent,
            Layout const * pLayout,
            Frame* pFrame,
            AssetManager* pAssetManager,
            NotificationQueue* pNotificationQueue,
            float relativeScale,
            float border,
            bool dimming,
            bool adaptiveScaling);

        // Destructor
        virtual ~Keyboard();

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Transformation filled by subclasses
        virtual void specialTransformAndSize();

        // Reset filled by subclasses
        virtual void specialReset();

        // Implemented by subclasses
        virtual bool mayConsumeInput();

    private:

        // Origin is top left
        typedef std::pair<float, float> KeyPosition;

        // Calculate positions of keys
        void calculateKeyPositions(int availableWidth, int availableHeight, std::vector<glm::vec2>& rPositions, float& rRadius) const;

        // Add positions of ring to available positions for keys
        void addAvailablePositionsOfRing(unsigned int ring, std::vector<KeyPosition>& rAvailablePositions) const;

        // Inlier test for circle in rectangle (origin is top left)
        bool circleInRectangle(float rectX, float rectY, float rectWidth, float rectHeight, float circleX, float circleY, float circleRadius) const;

        // Members
        RenderItem const * mpBackground;
        RenderItem const * mpKey;
        std::vector<glm::vec2> mKeyPositions;
        float mKeyRadius;

        // TODO: Testing
        unsigned int mKeyCount;
        const float KEY_HORIZONTAL_OFFSET = 0.1f;
    };
}

#endif // KEYBOARD_H_