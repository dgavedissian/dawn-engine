//-----------------------------------------------------------------------------
// COMPILE-TIME OPTIONS FOR DEAR IMGUI
// Runtime options (clipboard callbacks, enabling various features, etc.) can generally be set via the ImGuiIO structure.
// You can use ImGui::SetAllocatorFunctions() before calling ImGui::CreateContext() to rewire memory allocation functions.
//-----------------------------------------------------------------------------
// A) You may edit imconfig.h (and not overwrite it when updating imgui, or maintain a patch/branch with your modifications to imconfig.h)
// B) or add configuration directives in your own file and compile with #define IMGUI_USER_CONFIG "myfilename.h" 
// C) Many compile-time options have an effect on data structures. They need defined consistently _everywhere_ imgui.h is included, 
// not only for the imgui*.cpp compilation units. Defining those options in imconfig.h will ensure they correctly get used everywhere.
//-----------------------------------------------------------------------------

#pragma once

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
