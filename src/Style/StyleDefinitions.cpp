//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "StyleDefinitions.h"

namespace eyegui
{
	StylePropertyStringMappingTuple StylePropertyStringMappingMaps::value
	{
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
		},
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
		},
		{
			{ "SoundButtonHit",										StylePropertyString::SoundButtonHit },
			{ "SoundButtonDown",									StylePropertyString::SoundButtonDown },
			{ "SoundKeyPress",										StylePropertyString::SoundKeyPress },
		}
	};

	std::map<StylePropertyFloat, StylePropertyValue<StylePropertyFloat>::type> StylePropertyDefaultValueMaps::floatDefaults =
	{
		{ StylePropertyFloat::AnimationDuration,								0.1f },
		{ StylePropertyFloat::SensorPenetrationIncreaseDuration,				3.0f },
		{ StylePropertyFloat::SensorPenetrationDecreaseDuration,				1.5f },
		{ StylePropertyFloat::ButtonThresholdIncreaseDuration,					1.0f },
		{ StylePropertyFloat::ButtonThresholdDecreaseDuration,					2.0f },
		{ StylePropertyFloat::ButtonPressingDuration,							0.3f },
		{ StylePropertyFloat::SensorInteractionPenetrationAmount,				0.5f },
		{ StylePropertyFloat::DimIncreaseDuration,								1.5f },
		{ StylePropertyFloat::DimDecreaseDuration,								0.25f },
		{ StylePropertyFloat::DimAlpha,											0.5f },
		{ StylePropertyFloat::FlashDuration,									2.0f },
		{ StylePropertyFloat::MaximalAdaptiveScaleIncrease,						0.5f },
		{ StylePropertyFloat::AdaptiveScaleIncreaseDuration,					1.0f },
		{ StylePropertyFloat::AdaptiveScaleDecreaseDuration,					1.0f },
		{ StylePropertyFloat::KeyboardZoomSpeedMultiplier,						1.0f },
		{ StylePropertyFloat::KeyboardKeySelectionDuration,						1.25f },
		{ StylePropertyFloat::FlowSpeedMultiplier,								1.0f },
		{ StylePropertyFloat::TextEditScrollSpeedMultiplier,					1.0f },

		// Experimental
		{ StylePropertyFloat::FutureKeyboardPressDuration,						0.5f },
		{ StylePropertyFloat::FutureKeyboardRetriggerDelay,						0.5f },
		{ StylePropertyFloat::FutureKeyboardThresholdDuration,					1.0f },
		{ StylePropertyFloat::FutureKeyboardRepeatKeyThresholdMultiplier,		1.0f },
		{ StylePropertyFloat::FutureKeyboardSpaceKeyThresholdMultiplier,		1.0f },
		{ StylePropertyFloat::FutureKeyboardBackspaceKeyThresholdMultiplier,	1.0f },
		{ StylePropertyFloat::FutureKeyboardSuggestionLineThresholdMultiplier,	1.0f }
	};

	std::map<StylePropertyVec4, StylePropertyValue<StylePropertyVec4>::type> StylePropertyDefaultValueMaps::vec4Defaults =
	{
		{ StylePropertyVec4::Color,												glm::vec4(0.6f, 0.6f, 0.6f, 1.0f) },
		{ StylePropertyVec4::BackgroundColor,									glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) },
		{ StylePropertyVec4::HighlightColor,									glm::vec4(1.0f, 1.0f, 0.0f, 0.5f) },
		{ StylePropertyVec4::SeparatorColor,									glm::vec4(0.2f, 0.2f, 0.2f, 1.0f) },
		{ StylePropertyVec4::SelectionColor,									glm::vec4(0.0f, 1.0f, 1.0f, 0.5f) },
		{ StylePropertyVec4::IconColor,											glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ StylePropertyVec4::FontColor,											glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ StylePropertyVec4::DimColor,											glm::vec4(0.1f, 0.1f, 0.1f, 0.75f) },
		{ StylePropertyVec4::FlashColor,										glm::vec4(1.0f, 0.5f, 0.0f, 0.75f) },
		{ StylePropertyVec4::MarkColor,											glm::vec4(0.0f, 0.5f, 1.0f, 0.5f) },
		{ StylePropertyVec4::PickColor,											glm::vec4(0.2f, 1.0f, 0.0f, 0.5f) },
		{ StylePropertyVec4::ThresholdColor,									glm::vec4(0.0f, 1.0f, 1.0f, 0.5f) }
	};

	std::map<StylePropertyString, StylePropertyValue<StylePropertyString>::type> StylePropertyDefaultValueMaps::stringDefaults =
	{
		{ StylePropertyString::SoundButtonHit,									std::string() },
		{ StylePropertyString::SoundButtonDown,									std::string() },
		{ StylePropertyString::SoundKeyPress,									std::string() }
	};

    template<>
    typename StylePropertyValue<StylePropertyFloat>::type StylePropertyDefault<StylePropertyFloat>(StylePropertyFloat t)
    {
        const auto& rMap = StylePropertyDefaultValueMaps::floatDefaults;
        const auto i = rMap.find(t); return i == rMap.end() ? 0.f : i->second;
    }
    template<>
    typename StylePropertyValue<StylePropertyVec4>::type StylePropertyDefault<StylePropertyVec4>(StylePropertyVec4 t)
    {
        const auto& rMap = StylePropertyDefaultValueMaps::vec4Defaults;
        const auto i = rMap.find(t); return i == rMap.end() ? glm::vec4(0.f) : i->second;
    }
    template<>
    typename StylePropertyValue<StylePropertyString>::type StylePropertyDefault<StylePropertyString>(StylePropertyString t)
    {
        const auto& rMap = StylePropertyDefaultValueMaps::stringDefaults;
        const auto i = rMap.find(t); return i == rMap.end() ? std::string() : i->second;
    }
}
