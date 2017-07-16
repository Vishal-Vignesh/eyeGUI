//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Button.h"

#include "Layout.h"
#include "NotificationQueue.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/Helper.h"

namespace eyegui
{
    Button::Button(
        std::string id,
        std::vector<std::string> styles,
        Element* pParent,
        Layout const * pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        NotificationQueue* pNotificationQueue,
        RenderingMask renderingMask,
        bool useCircleThreshold,
        float relativeScale,
        float border,
        bool dimming,
        bool adaptiveScaling,
        std::string iconFilepath,
        std::u16string desc,
        std::string descKey,
        bool isSwitch) : IconElement(
            id,
            styles,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            renderingMask,
            relativeScale,
            border,
            dimming,
            adaptiveScaling,
            iconFilepath,
            desc,
            descKey)
    {
        mType = Type::BUTTON;

        // Fill members
        mIsSwitch = isSwitch;
        mUseCircleThreshold = useCircleThreshold;
        mIsDown = false;

        // Render items
        if(mUseCircleThreshold)
        {
            mpThresholdItem = mpAssetManager->fetchRenderItem(shaders::Type::CIRCLE_THRESHOLD, meshes::Type::QUAD);
        }
        else
        {
            mpThresholdItem = mpAssetManager->fetchRenderItem(shaders::Type::BOX_THRESHOLD, meshes::Type::QUAD);
        }
    }

    Button::~Button()
    {
        // Nothing to do so far
    }

    void Button::hit(bool immediately)
    {
        if (isActive())
        {
            // Inform listener after updating
            mpNotificationQueue->enqueue(getId(), NotificationType::BUTTON_HIT);

            // Call context correct method
            if (mIsDown)
            {
                up(immediately);
            }
            else
            {
                down(immediately);
            }

			// Play sound
			if (!immediately)
			{
				mpAssetManager->playSound(getStyleValue(property::AssetPath::ButtonHitSound));
			}
        }
    }

    void Button::down(bool immediately)
    {
        if (!mIsDown && isActive())
        {
            // Remove highlight
            highlight(false);

            // Save state
            mIsDown = true;

            // Inform listener after updating
            mpNotificationQueue->enqueue(getId(), NotificationType::BUTTON_DOWN);

            // Immediately
            if (immediately)
            {
                mPressing.setValue(1);
            }

            // Interaction notification
            notifyInteraction("DOWN");

			// Play sound
			if(!immediately)
			{
				mpAssetManager->playSound(getStyleValue(property::AssetPath::ButtonDownSound));
			}
        }
    }

    void Button::up(bool immediately)
    {
        if (mIsDown && (isActive() || !mIsSwitch))
        {
            // Remove highlight
            highlight(false);

            // Save state
            mIsDown = false;

            // Inform listener after updating
            mpNotificationQueue->enqueue(getId(), NotificationType::BUTTON_UP);

            // Immediately
            if (immediately)
            {
                mPressing.setValue(0);
            }

            // Interaction notification
            notifyInteraction("UP");
        }
    }

    bool Button::isSwitch() const
    {
        return mIsSwitch;
    }

    bool Button::isDown() const
    {
        return mIsDown;
    }

    float Button::specialUpdate(float tpf, Input* pInput)
    {
        // Super call
        float adaptiveScale = IconElement::specialUpdate(tpf, pInput);

        // Check for penetration by input
        bool penetrated = penetratedByInput(pInput);

        // Pressing animation
        if (mIsDown && mPressing.getValue() < 1)
        {
            mPressing.update(tpf / getStyleValue(property::Duration::ButtonPressingDuration));
        }
        else if (!mIsDown && mPressing.getValue() > 0)
        {
            mPressing.update(-tpf / getStyleValue(property::Duration::ButtonPressingDuration));
        }

        // If pressed and no switch, go back (do it each frame and not only at end of going down,
        // because otherwise a deactivation would prohibit going up because of the test for
        // activity in up()
        if (mIsDown && !mIsSwitch && mPressing.getValue() >= 1)
        {
            up();
        }

        // Threshold
        if (
            (mPressing.getValue() == 0 || mPressing.getValue() == 1) // only when completey up or down
            && penetrated // penetration
            && !(!mIsSwitch && mPressing.getValue() > 0)) // avoids to add threshold for none switch when at down position
        {
            mThreshold.update(tpf / getStyleValue(property::Duration::ButtonThresholdIncreaseDuration));

            if (mThreshold.getValue() >= 1)
            {
                hit();
                mThreshold.setValue(0);
            }
        }
        else
        {
            mThreshold.update(-tpf / getStyleValue(property::Duration::ButtonThresholdDecreaseDuration));
        }

        return adaptiveScale;
    }

    void Button::specialDraw() const
    {
        if(mThreshold.getValue() > 0)
        {
            mpThresholdItem->bind();
            mpThresholdItem->getShader()->fillValue("matrix", mFullDrawMatrix);
            mpThresholdItem->getShader()->fillValue("thresholdColor", getStyleValue(property::Color::ThresholdColor));
            mpThresholdItem->getShader()->fillValue("threshold", mThreshold.getValue());
            mpThresholdItem->getShader()->fillValue("alpha", mAlpha);
            if(!mUseCircleThreshold)
            {
                float orientation = 0;
                if (getParent() != NULL)
                {
                    if (getParent()->getOrientation() == Element::Orientation::VERTICAL)
                    {
                        orientation = 1;
                    }
                }
                mpThresholdItem->getShader()->fillValue("orientation", orientation);
            }
            mpThresholdItem->getShader()->fillValue("mask", 0); // mask is always in slot 0
            mpThresholdItem->draw();
        }

        // Super call
        IconElement::specialDraw();
    }

    void Button::specialReset()
    {
        IconElement::specialReset();

        mThreshold.setValue(0);

        // Handle pressing
        if(mIsSwitch)
        {
            // Set pressing directly to finale value when this is a switch
            if(mIsDown)
            {
                mPressing.setValue(1);
            }
            else
            {
                mPressing.setValue(0);
            }
        }
        else
        {
            // No switch, so reset to unpressed position
            mPressing.setValue(0);
            mIsDown = false;
        }

    }

    void Button::specialInteract()
    {
        // Hit button
        hit();

        // Reset threshold
        mThreshold.setValue(0);
    }

    void Button::specialPipeNotification(NotificationType notification, Layout* pLayout)
    {
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
        case NotificationType::BUTTON_HIT:
            notifyListener(&ButtonListener::hit, pLayout, getId());
            break;
        case NotificationType::BUTTON_DOWN:
            notifyListener(&ButtonListener::down, pLayout, getId());
            break;
        case NotificationType::BUTTON_UP:
            notifyListener(&ButtonListener::up, pLayout, getId());
            break;
        default:
            throwWarning(
                OperationNotifier::Operation::BUG,
                "Button got notification which is not thought for it.");
            break;
        }
    }

    float Button::getPressing() const
    {
        return mPressing.getValue();
    }
}
