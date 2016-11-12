//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextEdit.h"

#include "src/Layout.h"
#include "src/Rendering/ScissorStack.h"
#include "src/Utilities/Helper.h"

// TODO: testing
#include <iostream>

namespace eyegui
{
	TextEdit::TextEdit(
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
		FontSize fontSize) : InteractiveElement(
			id,
			styleName,
			pParent,
			pLayout,
			pFrame,
			pAssetManager,
			pNotificationQueue,
			RenderingMask::BOX,
			relativeScale,
			border,
			dimming,
			adaptiveScaling)
	{
		mType = Type::TEXT_EDIT;

		// Fill members
		mFontSize = fontSize;
		mTextFlowYOffset.setValue(0);
		mCursorFlowPartIndex = 0;
		mCursorLetterIndex = 0;
		mActiveEntityFading = 0;

		// Fetch render item for background
		mpBackground = mpAssetManager->fetchRenderItem(
			shaders::Type::COLOR,
			meshes::Type::QUAD);

		// Fetch render item for cursor
		mpCursor = mpAssetManager->fetchRenderItem(
			shaders::Type::COLOR,
			meshes::Type::QUAD);

		// Fetch render item for background of active entity
		mpActiveEntityBackground = mpAssetManager->fetchRenderItem(
			shaders::Type::COLOR,
			meshes::Type::QUAD);

		// Create text flow
        mupTextFlow = std::move(mpAssetManager->createTextFlow(
			fontSize,
			TextFlowAlignment::JUSTIFY, // TODO pipe through
			TextFlowVerticalAlignment::TOP,
			1.0f,
			u"Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.Lorem ipsum dolor sit amet", // TODO: maybe later some content from xeyegui or localization for initial value
            true));
	}

    TextEdit::~TextEdit()
    {
        // Nothing to do
    }

	void TextEdit::moveCursorOverLetters(int letterCount)
	{
        if(letterCount > 0)
        {
            moveCursorOverLettersRightward(letterCount);
        }
        else
        {
            moveCursorOverLettersLeftward(-letterCount);
        }
	}

	void TextEdit::moveCursorOverWords(int wordCount)
	{
		// Decide direction
		bool rightward = wordCount > 0;
		int absWordCount = glm::abs(wordCount);

		// Implement movement
		if (auto spActiveEntity = mwpActiveEntity.lock())
		{
			// Move as often as requested
			for (int i = 0; i < absWordCount; i++)
			{
				if (rightward) // rightward
				{
					// Try for next entity
					int index = spActiveEntity->getIndex();
					index++;
					if (auto spEntity = mupTextFlow->getFlowEntity(index).lock())
					{
						// Search for next word
						while (spEntity->getType() != FlowEntity::Type::Word)
						{
							index++;
							auto spNextEntity = mupTextFlow->getFlowEntity(index).lock();
							if (!spNextEntity)
							{
								return; // no next word found
							}
							else
							{
								spEntity = spNextEntity;
							}
						}

						// Let everything do set by method
						setActiveEntity(spEntity, true);
					}
				}
				else // leftward
				{
					// Try for previous entity
					int index = spActiveEntity->getIndex();
					index--;
					if (auto spEntity = mupTextFlow->getFlowEntity(index).lock())
					{
						// Search for previous word
						while (spEntity->getType() != FlowEntity::Type::Word)
						{
							index--;
							auto spPreviousEntity = mupTextFlow->getFlowEntity(index).lock();
							if (!spPreviousEntity)
							{
								return; // no previous word found
							}
							else
							{
								spEntity = spPreviousEntity;
							}
						}

						// Let everything do set by method
						setActiveEntity(spEntity, true);
					}
				}
			}
		}
	}

    void TextEdit::moveCursorToStart()
    {
		auto wpEntity = mupTextFlow->getFlowEntity(0);
		if (!wpEntity.expired())
		{
			setActiveEntity(wpEntity, false);
		}
    }

    void TextEdit::moveCursorToEnd()
    {
		auto wpEntity = mupTextFlow->getFlowEntity(mupTextFlow->getFlowEntityCount() - 1);
		if (!wpEntity.expired())
		{
			setActiveEntity(wpEntity, true);
		}
    }

	void TextEdit::addContentAtCursor(std::u16string content)
	{
        /*
        // When there is already text and a active word, use that cursor position
        FlowWord flowWord;
        int subWordIndex = 0;
        int letterIndex = 0;
		if (mupActiveWord != NULL)
		{
			// Find out, where the cursor is in content
			int contentIndex = mupActiveWord->contentStartIndex;
			for (int i = 0; i < mSubWordIndex; i++)
			{
				contentIndex += (int)mupActiveWord->subWords.at(i)->upWord->lettersXOffsets.size();
			}
            contentIndex += mLetterIndex; // -1 is ok here, since even at start of text, it should be inserted after -1 at 0

            // Tell text flow insert add content there
            if(mupTextFlow->insertContent(contentIndex, content, flowWord, subWordIndex, letterIndex))
            {
                // Update active word and cursor indices stuff
                setActiveWord(flowWord, false);
                mSubWordIndex = subWordIndex;
                mLetterIndex = letterIndex;
            }
		}
        else
        {
            // No active word and therefore no cursor position
            if(mupTextFlow->insertContent(-1, content, flowWord, subWordIndex, letterIndex))
            {
                // Update active word and cursor indices stuff
                setActiveWord(flowWord, false);
                mSubWordIndex = subWordIndex;
                mLetterIndex = letterIndex;
            }
        }
        */
	}

	void TextEdit::setContent(std::u16string content)
	{
        mupTextFlow->setContent(content);
        moveCursorToEnd();
	}

	void TextEdit::deleteContentAtCursor(int letterCount)
	{
		if (auto spActiveEntity = mwpActiveEntity.lock())
		{
			int contentIndex = spActiveEntity->getContentIndex(mCursorFlowPartIndex, mCursorLetterIndex);
			mwpActiveEntity = mupTextFlow->eraseContent(
				contentIndex,
				-letterCount, // interface is defined differently from internal here
				mCursorFlowPartIndex,
				mCursorLetterIndex);
		}
	}

	std::u16string TextEdit::getActiveEntityContent() const
	{
        if (auto spActiveEntity = mwpActiveEntity.lock())
		{
			return mupTextFlow->getContent(spActiveEntity->getContentStartIndex(), spActiveEntity->getLetterCount());
		}
        else
		{
			return std::u16string();
		}
	}

    float TextEdit::specialUpdate(float tpf, Input* pInput)
    {
        // Super call
        float adaptiveScale = InteractiveElement::specialUpdate(tpf, pInput);

		// *** ANIMATIONS ***

		// Update pulsing of cursor
		float fullCircle = 2 * glm::pi<float>();
		mCursorPulse += (tpf * fullCircle) / TEXT_EDIT_CURSOR_PULSE_DURATION;
		while (mCursorPulse >= fullCircle) { mCursorPulse -= fullCircle; }

		// Update active entity's fading
		if (!mwpActiveEntity.expired())
		{
			mActiveEntityFading = glm::min(mActiveEntityFading + tpf, mpLayout->getConfig()->animationDuration);
		}

		// *** UPDATE OF TEXT FLOW ***

		// Scroll text flow
		if (penetratedByInput(pInput) && (mupTextFlow->getFlowHeight() > 0))
		{
			// *** SCROLLING ***

			// Coordinates in flow coordinates
			int flowX = pInput->gazeX - mX;
			int flowY = pInput->gazeY - mY;

			// (Old) y offset of text flow in pixels
			int oldTextFlowYOffset = calculateTextFlowYOffset();

			// How much is gazeY away from elements center used for speed
			float offsetSpeed = ((float)((flowY - (mHeight / 2))) / (float)mHeight); // [-0.5, 0.5]

			// Calculate text flow height
			float textFlowHeight = (mupTextFlow->getFlowHeight() + mupTextFlow->getLineHeight()); // add line height to avoid cutting letters like 'p'

			// Normalization of offset speed
			if (textFlowHeight > 0)
			{
				offsetSpeed *= ((float)mHeight / textFlowHeight);
			}

			// Update relative offset
			mTextFlowYOffset.update(offsetSpeed * tpf * mpLayout->getConfig()->textEditScrollSpeedMultiplier);

			// *** ACTIVE ENTITY ***
			auto wpEntity = mupTextFlow->getFlowEntity(flowX, flowY + oldTextFlowYOffset);

			// Decide whether active entity has changed
			if (!wpEntity.expired()) // found one is valid
			{
				// Check that entity is no space
				if (auto spEntity = wpEntity.lock())
				{
					if (spEntity->getType() != FlowEntity::Type::Space)
					{
						// Check whether currently active entity is valid
						if (!mwpActiveEntity.expired()) // current one is valid
						{
							// Compare by indices within text flow vector
							if (auto spActiveEntity = mwpActiveEntity.lock())
							{
								if (spActiveEntity->getIndex() != spEntity->getIndex())
								{
									// Flow entity index is different, use it!
									setActiveEntity(wpEntity, true);
								}
							}
						}
						else
						{
							// No active entity has been set, do it now
							setActiveEntity(wpEntity, true);
						}
					}
				}
			}
		}

        return adaptiveScale;
    }

    void TextEdit::specialDraw() const
    {
        // *** BACKGROUND ***
        if (getStyle()->backgroundColor.a > 0)
        {
            // Bind, fill and draw background
            mpBackground->bind();
            mpBackground->getShader()->fillValue("matrix", mFullDrawMatrix);
            mpBackground->getShader()->fillValue("color", getStyle()->backgroundColor);
            mpBackground->getShader()->fillValue("alpha", getMultipliedDimmedAlpha());
            mpBackground->draw();
        }

		// *** PREPARATION ***

		// Push scissor to render text only within element
		pushScissor(mX, mY, mWidth, mHeight);

		// Y offset of text flow in pixels
		int textFlowYOffset = calculateTextFlowYOffset();

		// Function for draw matrix of active entity background
		std::function<glm::mat4(int, int, int)> calculateActiveEnitiyBackgroundDrawMatrix = [&](int x, int y, int width)
		{
			// Calculate width and height for active entity background
            int activeBackgroundWidth = width + (int)mupTextFlow->getPixelWidthOfSpace();
            int activeBackgroundHeight = (int)(mupTextFlow->getLineHeight() + mupTextFlow->getPixelWidthOfSpace());

			// Calculate draw matrix
			return calculateDrawMatrix(
				mpLayout->getLayoutWidth(),
				mpLayout->getLayoutHeight(),
				mX + x - ((activeBackgroundWidth - width) / 2),
				mY + y - textFlowYOffset - ((activeBackgroundHeight - (int)mupTextFlow->getLineHeight()) / 2),
				activeBackgroundWidth,
				activeBackgroundHeight);
		};

		// *** ACTIVE ENTITY BACKGROUND ***

		// Draw background behind active entity
		mpActiveEntityBackground->bind();
		mpActiveEntityBackground->getShader()->fillValue("color", getStyle()->markColor); // TODO: marked color used. Maybe use some custom

		// Draw currently active one
		if (auto spActiveEntity = mwpActiveEntity.lock())
		{
			// Iterate over flow parts
			for (uint i = 0; i < spActiveEntity->getFlowPartCount(); i++)
			{
				// Lock flow part pointer
				if (auto spFlowPart = spActiveEntity->getFlowPart(i).lock())
				{
					// Calculate draw matrix
					glm::mat4 activeEntityBackgroundDrawMatrix = calculateActiveEnitiyBackgroundDrawMatrix(spFlowPart->getX(), spFlowPart->getY(), (int)spFlowPart->getPixelWidth());

					// Draw flow part background
					mpActiveEntityBackground->getShader()->fillValue("matrix", activeEntityBackgroundDrawMatrix);
					mpActiveEntityBackground->getShader()->fillValue("alpha", (mActiveEntityFading / mpLayout->getConfig()->animationDuration) * getMultipliedDimmedAlpha());

					mpActiveEntityBackground->draw();
				}
			}
		}

		// *** TEXT FLOW ***

		// Drawing of text flow
		mupTextFlow->draw(
			getStyle()->fontColor,
			mAlpha,
			false,
			0,
			textFlowYOffset);

		// *** CURSOR ***

		// Calculate x and y of cursor
        int cursorX = 0; // fallback when there is no active entity
        int cursorY = 0; // fallback when there is no active entity

		// If there is a active entity, get cursor position from datastructure
		if (auto spActiveEntity = mwpActiveEntity.lock())
		{
			// Offsets are saved from 0 to letterCount, so adapt given index
			int offsetIndex = mCursorLetterIndex + 1;

			// Recieve offset from flow part
			if (auto spFlowPart = spActiveEntity->getFlowPart(mCursorFlowPartIndex).lock())
			{
				cursorX = spFlowPart->getX() + spFlowPart->getXOffset(offsetIndex);
				cursorY = spFlowPart->getY();
			}
		}

		// Calculate matrix for cursor
		glm::mat4 cursorDrawMatrix = calculateDrawMatrix(
			mpLayout->getLayoutWidth(),
			mpLayout->getLayoutHeight(),
			mX + cursorX,
			mY + cursorY - textFlowYOffset,
			glm::max(1, (int)(TEXT_EDIT_CURSOR_RELATIVE_WIDTH * mupTextFlow->getPixelWidthOfSpace())),
			(int)mupTextFlow->getLineHeight());

		// Draw pulsing cursor over text
		mpCursor->bind();
		mpCursor->getShader()->fillValue("matrix", cursorDrawMatrix);
		mpCursor->getShader()->fillValue("color", getStyle()->fontColor);
		mpCursor->getShader()->fillValue("alpha", getMultipliedDimmedAlpha() * (glm::cos(mCursorPulse) * 0.5f) + 0.5f);
		mpCursor->draw();

		// Pop own scissor
		popScissor();

        // Draw stuff like highlighting
        InteractiveElement::specialDraw();
    }

    void TextEdit::specialTransformAndSize()
    {
		// Tell text flow about transformation
		mupTextFlow->transformAndSize(mX, mY, mWidth, mHeight);

		// Set cursor to start
		mwpActiveEntity = std::weak_ptr<FlowEntity>(); // reset active entity to be sure of valid state
		moveCursorToEnd();
    }

    void TextEdit::specialReset()
    {
        // Call super
		InteractiveElement::specialReset();

		// Class resets
		mCursorPulse = 1.f;
    }

    void TextEdit::specialInteract()
    {
		// TODO
    }

    void TextEdit::specialPipeNotification(NotificationType notification, Layout* pLayout)
    {
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
        default:
            throwWarning(
                OperationNotifier::Operation::BUG,
                "Text Edit got notification which is not thought for it.");
            break;
        }
    }

	int TextEdit::calculateTextFlowYOffset() const
	{
        return (int)(mTextFlowYOffset.getValue() * glm::max(0.f, (float)((mupTextFlow->getFlowHeight() + mupTextFlow->getLineHeight()) - mHeight)));
	}

	void TextEdit::setActiveEntity(std::weak_ptr<const FlowEntity> wpFlowEntity, bool setCursorToEnd)
	{
		// Save active entity in members
		mwpActiveEntity = wpFlowEntity;
		mActiveEntityFading = 0.f; // reset fading

		// Set cursor
		mCursorFlowPartIndex = 0;
		mCursorLetterIndex = -1;
		if (setCursorToEnd)
		{
			if (auto spActiveEntity = mwpActiveEntity.lock())
			{
				// Set cursor position behind active flow entity
				if (spActiveEntity->getFlowPartCount() >= 1)
				{
					mCursorFlowPartIndex = (uint)((int)spActiveEntity->getFlowPartCount() - 1);

					if (auto spFlowPart = spActiveEntity->getFlowPart(mCursorFlowPartIndex).lock())
					{
						mCursorLetterIndex = (int)(spFlowPart->getLetterCount()) - 1;
					}
				}
			}
		}

		// Reset cursor pulse to make it directly visible
		mCursorPulse = 0.f;
	}

    void TextEdit::moveCursorOverLettersRightward(int letterCount)
    {
        // Check for current position
        if (auto spActiveEntity = mwpActiveEntity.lock())
        {
            // Repeat it as indicated
            for (int i = 0; i < letterCount; i++)
            {
				uint flowPartIndex = mCursorFlowPartIndex;
				int letterIndex = mCursorLetterIndex;

                // Try to increment just letter index
				letterIndex++;

                // Check whether still in range of flow part
				if (auto spFlowPart = spActiveEntity->getFlowPart(flowPartIndex).lock())
				{
					if (letterIndex >= (int)spFlowPart->getLetterCount()) // exceeds letter count and has to move in front of next flow part or entity
					{
						// Try for next flow part
						if (flowPartIndex < (int)spActiveEntity->getFlowPartCount() - 1) // there is another flow part
						{
							mCursorFlowPartIndex = flowPartIndex + 1;
							mCursorLetterIndex = -1; // front of flow part
						}
						else // no further flow part
						{
							// Try for next entity
							int index = spActiveEntity->getIndex();
							index++;
							if (auto spEntity = mupTextFlow->getFlowEntity(index).lock())
							{
								// Check for non-space. Should overstep spaces
								while (spEntity->getType() == FlowEntity::Type::Space)
								{
									index++;
									auto spNextEntity = mupTextFlow->getFlowEntity(index).lock();
									if (!spNextEntity)
									{
										break; // ok, no further. Just use the found space
									}
									else
									{
										spEntity = spNextEntity;
									}
								}

								// Let everything do set by method
								setActiveEntity(spEntity, false);
							}
						}
					}
					else // ok just to move within flow part
					{
						mCursorLetterIndex = letterIndex;
					}
				}
            }
        }
    }

    void TextEdit::moveCursorOverLettersLeftward(int letterCount)
    {
		// Check for current position
		if (auto spActiveEntity = mwpActiveEntity.lock())
		{
			// Repeat it as indicated
			for (int i = 0; i < letterCount; i++)
			{
				uint flowPartIndex = mCursorFlowPartIndex;
				int letterIndex = mCursorLetterIndex;

				// Try to decrement just letter index
				letterIndex--;

				// Check whether still in range of flow part
				if (auto spFlowPart = spActiveEntity->getFlowPart(flowPartIndex).lock())
				{
					if (letterIndex < -1) // in front of front, so check for other flow part or flow entity before
					{
						// Try for previous flow part
						if (flowPartIndex > 0) // there is another flow part
						{
							mCursorFlowPartIndex = (uint)((int)flowPartIndex - 1);
							mCursorLetterIndex = spFlowPart->getLetterCount() - 1; // front of flow part
						}
						else // no previous flow part
						{
							// Try for previous entity
							int index = spActiveEntity->getIndex();
							index--;
							if (auto spEntity = mupTextFlow->getFlowEntity(index).lock())
							{
								// Check for non-space. Should overstep spaces
								while (spEntity->getType() == FlowEntity::Type::Space)
								{
									index--;
									auto spPreviousEntity = mupTextFlow->getFlowEntity(index).lock();
									if (!spPreviousEntity)
									{
										break; // ok, no further. Just use the found space
									}
									else
									{
										spEntity = spPreviousEntity;
									}
								}

								// Let everything do set by method
								setActiveEntity(spEntity, true);
							}
						}
					}
					else // ok just to move within flow part
					{
						mCursorLetterIndex = letterIndex;
					}
				}
			}
		}
    }
}
