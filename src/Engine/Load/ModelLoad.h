#pragma once

#include "RenderMesh.h"
#include "tiny_gltf.h"

#include "EngineDLL.h"

#include <optional>

std::optional<RenderMesh>  ENGINE_DLL_API LoadGLTFAsRenderMesh(std::string path);
