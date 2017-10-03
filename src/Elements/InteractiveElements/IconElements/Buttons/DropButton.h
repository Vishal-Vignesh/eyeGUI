//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Extending box button by taking a single inner element. It is displayed when
// the button is down. Inner element MUST BE added, otherwise there will be
// null pointer exceptions.

#ifndef DROP_BUTTON_H_
#define DROP_BUTTON_H_

#include "BoxButton.h"

namespace eyegui
{
    class DropButton : public BoxButton
    {
    public:

        // Deconstructor
        ~DropButton();

        // Up and down
        virtual void down(bool immediately = false);
        virtual void up(bool immediately = false);

        // Attach inner element
        void attachInnerElement(std::unique_ptr<Element> upElement);

        // Replace an attached element, returns NULL if not found
        virtual std::unique_ptr<Element> replaceAttachedElement(
            Element* pTarget,
            std::unique_ptr<Element> upReplacement);

        // Starts the getting of the next interactive element, returns NULL if no available
        virtual InteractiveElement* nextInteractiveElement();

        // Activate or deactivate
        virtual void setActivity(bool active, bool fade);

		// Get ids of all children, recursively. Or here: inner element rather than children
		virtual std::set<std::string> getAllChildrenIds() const;

    protected:

		// Factory is friend
		friend ElementFactory;

		// Protected constructor
		DropButton(
			std::string id,
			std::vector<std::string> styles,
			Element* pParent,
			Layout const * pLayout,
			Frame* pFrame,
			AssetManager* pAssetManager,
			NotificationQueue* pNotificationQueue,
			float relativeScale,
			float border,
			bool dimming,
			bool adaptiveScaling,
			std::string iconFilepath,
			std::u16string desc,
			std::string descKey,
			float space,
			bool instantPress);

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Transformation
        virtual void specialTransformAndSize();

        // Reset filld by subclasses
        virtual void specialReset();

    private:

        // Member
		std::unique_ptr<Element> mupInnerElement;
        float mSpace;
        LerpValue mInnerAlpha;
        bool mInnerElementVisible;
    };
}

#endif // DROP_BUTTON_H_
