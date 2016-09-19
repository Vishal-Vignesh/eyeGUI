//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Image.h"

#include "src/Layout.h"
#include "src/Rendering/AssetManager.h"

namespace eyegui
{
    Image::Image(
        Layout const * pLayout,
        AssetManager* pAssetManager,
        std::string filepath,
        ImageAlignment alignment)
    {
        // Initialize members
        mpLayout = pLayout;
        mpAssetManager = pAssetManager;
        mpQuad = mpAssetManager->fetchRenderItem(shaders::Type::IMAGE, meshes::Type::QUAD);
		mTexture = filepath;
        mAlignment = alignment;
        mX = 0;
        mY = 0;
        mWidth = 0;
        mHeight = 0;
    }

    Image::~Image()
    {
        // Nothing to do
    }

    void Image::setContent(
        std::string name,
        int width,
        int height,
		ColorFormat format,
        unsigned char const * pData,
		bool flipY)
    {
        mpAssetManager->fetchTexture(name, width, height, format, pData, flipY);
		mTexture = name;
    }

	void Image::setContent(std::string name)
	{
		mTexture = name;
	}

    void Image::evaluateSize(
        int availableWidth,
        int availableHeight,
        int& rWidth,
        int& rHeight) const
    {
        switch(mAlignment)
        {
        case ImageAlignment::ORIGINAL:
            {
				// Fetch current texture
				Texture const * pTexture = mpAssetManager->fetchTexture(mTexture);

                // Extra parenthesis somehow necessary because of variable initialization
                float availableAspectRatio = ((float)availableWidth) / ((float)availableHeight);
                float aspectRatio = pTexture->getAspectRatio();

                if (availableAspectRatio < aspectRatio)
                {
                    // Horizontal space less than necessary
                    rWidth = availableWidth;

                    // Adjust vertical size
                    rHeight = (int)((float)rWidth / aspectRatio);
                }
                else
                {
                    // Vertical space less than necessary
                    rHeight = availableHeight;

                    // Adjust horizontal size
                    rWidth = (int)((float)rHeight * aspectRatio);
                }
                break;
            }
        case ImageAlignment::STRETCHED:
            rWidth = availableWidth;
            rHeight = availableHeight;
            break;
        case ImageAlignment::ZOOMED:
            rWidth = availableWidth;
            rHeight = availableHeight;
            break;
        }
    }

    void Image::transformAndSize(int x, int y, int width, int height)
    {
        mX = x;
        mY = y;
        mWidth = width;
        mHeight = height;
    }

    void Image::draw(float alpha) const
    {
        // Bind render item before setting values and drawing
        mpQuad->bind();

        // Fill matrix
        mpQuad->getShader()->fillValue(
            "matrix",
            calculateDrawMatrix(
                mpLayout->getLayoutWidth(),
                mpLayout->getLayoutHeight(),
                mX,
                mY,
                mWidth,
                mHeight));

        // Fill values
        mpQuad->getShader()->fillValue("alpha", alpha);
        mpQuad->getShader()->fillValue("image", 1);

		// Fetch current texture
		Texture const * pTexture = mpAssetManager->fetchTexture(mTexture);

        // Fill scale
        glm::vec2 scale = glm::vec2(1.f,1.f);
        if(mAlignment == ImageAlignment::ZOOMED)
        {
            float availableAspectRatio = ((float)mWidth) / ((float)mHeight);
            float aspectRatio = pTexture->getAspectRatio();

            if (availableAspectRatio < aspectRatio)
            {
                // Horizontal space less than necessary
                scale.x = availableAspectRatio / aspectRatio;
            }
            else
            {
                // Vertical space less than necessary
                scale.y = aspectRatio / availableAspectRatio;
            }

        }
        mpQuad->getShader()->fillValue("scale", scale);

        // Bind texture to render
		pTexture->bind(1);
		
        // Draw the quad
        mpQuad->draw();
    }

    unsigned int Image::getTextureWidth() const
    {
        return (uint)(mpAssetManager->fetchTexture(mTexture)->getWidth());
    }

    unsigned int Image::getTextureHeight() const
    {
        return (uint)(mpAssetManager->fetchTexture(mTexture)->getHeight());
    }
}
