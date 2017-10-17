//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Drift map. Is filled at interaction through dwell times activation of
// elements with the distance between center of the activated elements and the
// provided gaze coordinates. Offset is from center of activation to gaze.

// TODO: breaks for windows smaller than 1x1 (division through 0)

#ifndef DRIFT_MAP_H_
#define DRIFT_MAP_H_

#include <algorithm>

namespace eyegui
{
	// Forward declaration
	class GUI;

	class DriftMap
	{
	public:

		// Constructor
		DriftMap(GUI const * pGUI);

		// Takes reference to gaze, stores it and provides correction
		void update(int& rGazeX, int& rGazeY);

		// Tell about center of attention and time until activation
		void notifyInteraction(int centerX, int centerY);

		// Reset drift map
		void reset();

	private:

		// Calculate nearest grid vertices
		void calculateNearestGridVertex(int coordX, int coordY, int& rVertexX, int& rVertexY) const;

		// Calculate nearest grid vertices
		struct GridPosition
		{
			int lowerX, upperX = 0, lowerY, upperY = 0; // grid vertices
			float innerX, innerY; // relative position within vertices
		};
		GridPosition calculateNearestGridVertices(int coordX, int coordY) const;

		// Pointer to GUI
		GUI const * mpGUI;

		// Current gaze
		int mGazeX = 0;
		int mGazeY = 0;

		// Global drift
		float mGlobalDriftX = 0;
		float mGlobalDriftY = 0;

		// Grid
		static const int RESOLUTION_X = 16; // cell count
		static const int RESOLUTION_Y = 9; // cell count
		std::pair<float, float> mGrid[RESOLUTION_X+1][RESOLUTION_Y+1] = { std::make_pair(0.f, 0.f) };
	};
}

#endif // DRIFT_MAP_H_