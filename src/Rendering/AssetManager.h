//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Managing all kind of assets like shaders, meshes and graphics. Ensures that
// all assets are unique and loaded only one time into memory. Initializes and
// terminates the FreeType library and manages fonts.

#ifndef ASSET_MANAGER_H_
#define ASSET_MANAGER_H_

#include "Shader.h"
#include "Mesh.h"
#include "Textures/Texture.h"
#include "RenderItem.h"
#include "Font/Font.h"
#include "Assets/Text/TextFlow.h"
#include "Assets/Text/TextSimple.h"
#include "Assets/Key.h"
#include "Assets/CharacterKey.h"
#include "Assets/Image.h"

#include "externals/FreeType2/include/ft2build.h"
#include FT_FREETYPE_H

#include <memory>
#include <map>

namespace eyegui
{
    // Available assets
    namespace shaders { enum class Type {
		COLOR,
		CIRCLE,
		DIM,
		FLASH,
		ACTIVITY,
		MARK,
		CIRCLE_THRESHOLD,
		BOX_THRESHOLD,
		HIGHLIGHT,
		SELECTION,
		CIRCLE_BUTTON,
		BOX_BUTTON,
		SENSOR,
		TEXT,
		IMAGE,
		KEY,
		CHARACTER_KEY};
	}
    namespace meshes { enum class Type { QUAD, LINE }; }
    namespace graphics { enum class Type { CIRCLE, BOX, NOT_FOUND }; }

    // Forward declaration
    class GUI;

    class AssetManager
    {
    public:

        // Constructor
        AssetManager(GUI const * pGUI);

        // Destructor
        virtual ~AssetManager();

        // Fetch render item
        RenderItem const * fetchRenderItem(shaders::Type shader, meshes::Type mesh);

        // Fetch texture
        Texture const * fetchTexture(std::string filepath);
        Texture const * fetchTexture(
			std::string name,
			int width,
			int height,
			ColorFormat format,
			unsigned char const * pData,
			bool flipY);

        // Fetch graphics
        Texture const * fetchTexture(graphics::Type graphic);

        // Fetch shader
        Shader const * fetchShader(shaders::Type shader);

        // Fetch mesh
        Mesh const * fetchMesh(meshes::Type mesh);

        // Fetch font
        Font const * fetchFont(std::string filepath);

        // Resize font atlases (should be called by GUI only)
        void resizeFontAtlases();

        // Create text flow and return it as unique pointer
        std::unique_ptr<TextFlow> createTextFlow(
            FontSize fontSize,
            TextFlowAlignment alignment,
            TextFlowVerticalAlignment verticalAlignment,
            float scale = 1.f,
            std::u16string content = u"",
			bool overflowHeight = false,
			bool collapseSpaces = true);

        // Create text simple and return it as unique pointer
        std::unique_ptr<TextSimple> createTextSimple(
            FontSize fontSize,
            float scale = 1.f,
            std::u16string content = u"");

        // Create key for keyboard and return it as unique pointer
        std::unique_ptr<Key> createKey(Layout const * pLayout, char16_t character);

        // Create image and return it as unique pointer
        std::unique_ptr<Image> createImage(Layout const * pLayout, std::string filepath, ImageAlignment alignment);

    private:

        // Members
        GUI const * mpGUI;
        FT_Library mFreeTypeLibrary;
        std::map<shaders::Type, std::unique_ptr<Shader> > mShaders;
        std::map<meshes::Type, std::unique_ptr<Mesh> > mMeshes;
        std::map<shaders::Type, std::map<meshes::Type, std::unique_ptr<RenderItem> > > mRenderItems;
        std::map<std::string, std::unique_ptr<Texture> > mTextures;
        std::map<graphics::Type, std::unique_ptr<Texture> > mGraphics;
        std::map<std::string, std::unique_ptr<Font> > mFonts;
    };
}

#endif // ASSET_MANAGER_H_
