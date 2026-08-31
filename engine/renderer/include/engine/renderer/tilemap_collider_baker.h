#pragma once

namespace engine::core
{
    class TilemapColliderComponent;
}

namespace engine::renderer
{

    class TilemapComponent;

    void BakeTilemapCollider(TilemapComponent &tilemap, engine::core::TilemapColliderComponent &outCollider);

} // namespace engine::renderer