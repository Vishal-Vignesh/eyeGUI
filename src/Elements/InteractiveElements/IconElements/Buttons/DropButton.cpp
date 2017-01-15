//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "DropButton.h"

#include "src/Layout.h"
#include "src/Utilities/Helper.h"
#include "src/Utilities/OperationNotifier.h"

namespace eyegui
{
    DropButton::DropButton(
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
        bool adaptiveScaling,
        std::string iconFilepath,
		std::u16string desc,
		std::string descKey,
        float space) : BoxButton(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            relativeScale,
            border,
            dimming,
            adaptiveScaling,
            iconFilepath,
			desc,
			descKey,
            true)
    {
        mType = Type::DROP_BUTTON;

        // Fill members
        mSpace = space;

        // Initial values
		mupInnerElement = NULL; // One has to call attach inner element method
    }

    DropButton::~DropButton()
    {
        // Nothing to do here
    }

    void DropButton::down(bool immediately)
    {
        // Super call
        BoxButton::down(immediately);

        mInnerElementVisible = true;
        mForceUndim = true;

        // Immediately
        if (immediately)
        {
            mInnerAlpha.setValue(1);
        }
    }

    void DropButton::up(bool immediately)
    {
        // Super call
        BoxButton::up(immediately);

        mInnerElementVisible = false;
        mForceUndim = false;

        // Immediately
        if (immediately)
        {
            mInnerAlpha.setValue(0);
        }
    }

    void DropButton::attachInnerElement(std::unique_ptr<Element> upElement)
    {
        // Condition to allow only one inner child
        if (mupInnerElement == NULL)
        {
            // Should be the only one
			mupInnerElement = std::move(upElement);

            // Register in frame for updating before everything else and drawing after everything else
            mpFrame->registerFrontElementForUpdateAndDraw(mupInnerElement.get(), isDown());
        }
		else
		{
			throwError(OperationNotifier::Operation::BUG, "Tried to set inner element a second time in drop button with following id:" + getId());
		}
    }

    std::unique_ptr<Element> DropButton::replaceAttachedElement(
        Element* pTarget,
        std::unique_ptr<Element> upReplacement)
    {
        // Replace the only inner element
        // (assumption is guaranteed by condition in attachInnerElement method)
        std::unique_ptr<Element> upElement = BoxButton::replaceAttachedElement(
            pTarget,
            std::move(upReplacement));

        // Only continue, if element was really replaced
        if (upElement.get() != NULL)
        {
            // Unregistering of old inner element in layout as front
            // element and all children was done in layout during replacement

            // Register new element
            mpFrame->registerFrontElementForUpdateAndDraw(mupInnerElement.get(), isDown());
        }

        return std::move(upElement);
    }

    InteractiveElement* DropButton::nextInteractiveElement()
    {
        // Go into inner element if visible
        if (mInnerElementVisible)
        {
            return mupInnerElement->internalNextInteractiveElement(NULL);
        }

        // Otherwise, do standard behaviour
        return BoxButton::nextInteractiveElement();
    }

    void DropButton::setActivity(bool active, bool fade)
    {
        // Member not yet set, so use parameter
        if (!active)
        {
            up(!fade);
        }

        // Super call after up(), because up() tests for active
        BoxButton::setActivity(active, fade);
    }

    float DropButton::specialUpdate(float tpf, Input * pInput)
    {
        // Super call
        float adaptiveScale = BoxButton::specialUpdate(tpf, pInput);

        // Update alpha of inner element
        mInnerAlpha.update(tpf / mpLayout->getConfig()->getValue(StylePropertyFloat::AnimationDuration)->get(), !mInnerElementVisible);
		mpFrame->setFrontElementAlpha(mupInnerElement.get(), mInnerAlpha.getValue() * mAlpha);

        return adaptiveScale;
    }

    void DropButton::specialTransformAndSize()
    {
        // Super call
        BoxButton::specialTransformAndSize();

        // Determine used space
        int usedX, usedY, usedWidth, usedHeight;

        // Reserve some values
        int elemWidth, elemHeight;

        // Calculate available space
        if (getParent() != NULL
            && getParent()->getOrientation() == Element::Orientation::HORIZONTAL)
        {
            // Horizontal origin, so search in upper or lower area of layout for space
            int upperSpace = mY;
            int lowerSpace = mpLayout->getLayoutHeight() - mY - mHeight;

            if (upperSpace > lowerSpace)
            {
                // Use upper space!
                elemHeight = (int)(upperSpace * mSpace);
				mupInnerElement->evaluateSize(mWidth, elemHeight, usedWidth, usedHeight);
                usedY = mY - usedHeight;
            }
            else
            {
                // Use lower space!
                elemHeight = (int)(lowerSpace * mSpace);
				mupInnerElement->evaluateSize(mWidth, elemHeight, usedWidth, usedHeight);
                usedY = mY + mHeight;
            }

            usedX = mX + (mWidth - usedWidth) / 2;
        }
        else
        {
            // Search left and right for space
            int leftSpace = mX;
            int rightSpace = mpLayout->getLayoutWidth() - mX - mWidth;

            if (leftSpace > rightSpace)
            {
                // Use left space!
                elemWidth = (int)(leftSpace * mSpace);
				mupInnerElement->evaluateSize(elemWidth, mHeight, usedWidth, usedHeight);
                usedX = mX - usedWidth;
            }
            else
            {
                // Use right space!
                elemWidth = (int)(rightSpace * mSpace);
				mupInnerElement->evaluateSize(elemWidth, mHeight, usedWidth, usedHeight);
                usedX = mX + mWidth;
            }

            usedY = mY + (mHeight - usedHeight) / 2;

        }

        // Tell inner element about it
		mupInnerElement->transformAndSize(usedX, usedY, usedWidth, usedHeight);
    }

    void DropButton::specialReset()
    {
        // Super call
        BoxButton::specialReset();

        mInnerAlpha.setValue(0);
        mInnerElementVisible = false;
    }
}
