#pragma once

struct SpriteBufferIndices;

void debugPoint(int x, int y);

void initDebugSprites();
void deinitDebugSprites();
void drawDebugSprites(SpriteBufferIndices& indices);
