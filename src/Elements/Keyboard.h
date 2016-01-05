//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Keyboard for eyetracking input.

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "Element.h"

namespace eyegui
{
    class Keyboard : public Element
    {
    public:

        // TODO
        // - maybe build up vector of structs saving position of keys in original state
        // - redo whole algorithm? first, determining size etc and then set positions is strange
        // - sizeWithKeyDistance is float, is that ok?

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

        const float KEY_DISTANCE = 0.1; // Amount of size

        // Calculate size of keys, number of rows and count of keys in even row
        void fitKeys(unsigned int& rSize, unsigned int& rRowCount, unsigned int& rCountPerRow) const;

        // Members
        RenderItem const * mpBackground;
        RenderItem const * mpKey;

        // TODO: Testing
        unsigned int mKeyCount;
    };
}

#endif // KEYBOARD_H_
