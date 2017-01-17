//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "StyleParser.h"

#include "src/Defines.h"
#include "src/Utilities/Helper.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/PathBuilder.h"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <map>

namespace eyegui
{
	// TODO NOTE: ADD MAPPING OF PROPERTIES HERE! Also remember to add initial value and constraints in StyleClass.cpp

	// Maps from string to style property type
	static const std::map<std::string, StylePropertyFloat> FLOAT_TYPE_MAP =
	{ 
		{ "AnimationDuration",									StylePropertyFloat::AnimationDuration },
		{ "SensorPenetrationIncreaseDuration",					StylePropertyFloat::SensorPenetrationIncreaseDuration },
		{ "SensorPenetrationDecreaseDuration",					StylePropertyFloat::SensorPenetrationDecreaseDuration },
		{ "ButtonThresholdIncreaseDuration",					StylePropertyFloat::ButtonThresholdIncreaseDuration },
		{ "ButtonThresholdDecreaseDuration",					StylePropertyFloat::ButtonThresholdDecreaseDuration },
		{ "ButtonPressingDuration",								StylePropertyFloat::ButtonPressingDuration },
		{ "SensorInteractionPenetrationAmount",					StylePropertyFloat::SensorInteractionPenetrationAmount },
		{ "DimIncreaseDuration",								StylePropertyFloat::DimIncreaseDuration },
		{ "DimDecreaseDuration",								StylePropertyFloat::DimDecreaseDuration },
		{ "DimAlpha",											StylePropertyFloat::DimAlpha },
		{ "FlashDuration",										StylePropertyFloat::FlashDuration },
		{ "MaximalAdaptiveScaleIncrease",						StylePropertyFloat::MaximalAdaptiveScaleIncrease },
		{ "AdaptiveScaleIncreaseDuration",						StylePropertyFloat::AdaptiveScaleIncreaseDuration },
		{ "AdaptiveScaleDecreaseDuration",						StylePropertyFloat::AdaptiveScaleDecreaseDuration },
		{ "KeyboardZoomSpeedMultiplier",						StylePropertyFloat::KeyboardZoomSpeedMultiplier },
		{ "KeyboardKeySelectionDuration",						StylePropertyFloat::KeyboardKeySelectionDuration },
		{ "FlowSpeedMultiplier",								StylePropertyFloat::FlowSpeedMultiplier },
		{ "TextEditScrollSpeedMultiplier",						StylePropertyFloat::TextEditScrollSpeedMultiplier },

		// Experimental
		{ "FutureKeyboardPressDuration",						StylePropertyFloat::FutureKeyboardPressDuration },
		{ "FutureKeyboardRetriggerDelay",						StylePropertyFloat::FutureKeyboardRetriggerDelay },
		{ "FutureKeyboardThresholdDuration",					StylePropertyFloat::FutureKeyboardThresholdDuration },
		{ "FutureKeyboardRepeatKeyThresholdMultiplier",			StylePropertyFloat::FutureKeyboardRepeatKeyThresholdMultiplier },
		{ "FutureKeyboardSpaceKeyThresholdMultiplier",			StylePropertyFloat::FutureKeyboardSpaceKeyThresholdMultiplier },
		{ "FutureKeyboardBackspaceKeyThresholdMultiplier",		StylePropertyFloat::FutureKeyboardBackspaceKeyThresholdMultiplier },
		{ "FutureKeyboardSuggestionLineThresholdMultiplier",	StylePropertyFloat::FutureKeyboardSuggestionLineThresholdMultiplier }
	};
	static const std::map<std::string, StylePropertyVec4> VEC4_TYPE_MAP =
	{
		{ "Color",												StylePropertyVec4::Color },
		{ "BackgroundColor",									StylePropertyVec4::BackgroundColor },
		{ "HighlightColor",										StylePropertyVec4::HighlightColor },
		{ "SeparatorColor",										StylePropertyVec4::SeparatorColor },
		{ "SelectionColor",										StylePropertyVec4::SelectionColor },
		{ "IconColor",											StylePropertyVec4::IconColor },
		{ "FontColor",											StylePropertyVec4::FontColor },
		{ "DimColor",											StylePropertyVec4::DimColor },
		{ "FlashColor",											StylePropertyVec4::FlashColor },
		{ "MarkColor",											StylePropertyVec4::MarkColor },
		{ "PickColor",											StylePropertyVec4::PickColor },
		{ "ThresholdColor",										StylePropertyVec4::ThresholdColor }
	};

    namespace style_parser
    {
		// Helper function not exposed through header
		void parseProperty(std::shared_ptr<StyleTree> spStyleTree, std::string styleClass, std::string line, std::string filepath)
		{
			// Get left and right side
			std::string delimiter = "=";
			size_t pos = 0;

			if ((pos = line.find(delimiter)) == std::string::npos)
			{
				throwWarning(OperationNotifier::Operation::PARSING, "Following line could not be parsed: " + line, filepath);
				return;
			}

			std::string left = line.substr(0, pos);
			line.erase(0, pos + delimiter.length());
			std::string right = line;

			// Fill it into style property
			auto floatIterator = FLOAT_TYPE_MAP.find(left);
			if (floatIterator != FLOAT_TYPE_MAP.end()) { spStyleTree->setValue(styleClass, floatIterator->second, right); }
			auto vec4Iterator = VEC4_TYPE_MAP.find(left);
			if (vec4Iterator != VEC4_TYPE_MAP.end()) { spStyleTree->setValue(styleClass, vec4Iterator->second, right); }
		}

		void parse(std::shared_ptr<StyleTree> spStyleTree, std::string filepath)
		{
			// Parse only if there is a file
			if (filepath != EMPTY_STRING_ATTRIBUTE)
			{
				// Check file name
				if (!checkFileNameExtension(filepath, STYLESHEET_EXTENSION))
				{
					throwWarning(OperationNotifier::Operation::PARSING, "Extension of style sheet file not as expected", filepath);
					return;
				}

				// Read file
				std::ifstream in(buildPath(filepath).c_str(), std::ios::in);

				// Check whether file was found
				if (!in)
				{
					throwWarning(OperationNotifier::Operation::PARSING, "Stylesheet file not found", filepath);
					return;
				}

				// Convert input file to string
				std::stringstream strStream;
				strStream << in.rdbuf();
				std::string content = strStream.str();

				// Close file
				in.close();

				// Streamline line endings
				streamlineLineEnding(content);

				// Get rid of whitespaces
				std::string::iterator end_pos = std::remove(content.begin(), content.end(), ' ');
				content.erase(end_pos, content.end());

				// Get rid of tabs
				end_pos = std::remove(content.begin(), content.end(), '\t');
				content.erase(end_pos, content.end());

				// Add some new line at the end of content because while loop does not read last line
				content += "\n";

				// Make some state variables for parsing
				enum class ParseState { OUTER, NAME_READ, INNER };
				ParseState state = ParseState::OUTER;
				std::string name; // name of currently processed style class

				// Go through the lines
				std::string delimiter = "\n";
				size_t pos = 0;
				std::string line;
				while ((pos = content.find(delimiter)) != std::string::npos)
				{
					// Read line and erase it from content
					line = content.substr(0, pos);
					content.erase(0, pos + delimiter.length());

					// Decide how to proceed
					switch (state)
					{
					case ParseState::OUTER:
						if (line.empty())
						{
							continue;
						}
						else
						{
							// First guess, whole line is name
							name = line;

							// Try to get left and right side
							std::string delimiter = ":";
							size_t pos = 0;
							std::string copyline = line;
							std::string parent;

							if ((pos = copyline.find(delimiter)) != std::string::npos)
							{
								// There is both name and parent present in line
								name = copyline.substr(0, pos);
								copyline.erase(0, pos + delimiter.length());
								parent = copyline;
							}

							// Do not proceed if name is empty (no empty style class name allowed)
							if(name.empty()) { continue; }

							// Add style class to tree
							if (parent.empty())
							{
								spStyleTree->addStyleClass(name); // no parent given
							}
							else
							{
								spStyleTree->addStyleClass(name, parent);
							}
							
							// Change state
							state = ParseState::NAME_READ;
						}
						break;
					case ParseState::NAME_READ:
						if (line == "{")
						{
							state = ParseState::INNER;
						}
						else
						{
							throwWarning(OperationNotifier::Operation::PARSING, "'{' expected but not found", filepath);
							continue;
						}
						break;
					case ParseState::INNER:
						if (line == "")
						{
							continue;
						}
						else if (line == "}")
						{
							state = ParseState::OUTER;
						}
						else
						{
							// There seems to be a value for a property
							parseProperty(spStyleTree, name, line, filepath);
						}
						break;
					}
				}

				// State should be OUTER after parsing
				if (state != ParseState::OUTER)
				{
					throwWarning(OperationNotifier::Operation::PARSING, "'}' expected but not found", filepath);
					return;
				}
			}
		}		
    }
}