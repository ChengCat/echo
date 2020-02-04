#pragma once

#include "engine/core/scene/render_node.h"
#include "engine/core/render/base/mesh/Mesh.h"
#include "engine/core/render/base/Material.h"
#include "engine/core/render/base/Renderable.h"
#include "engine/core/render/base/image/Image.h"
#include "terrain_tile.h"

namespace Echo
{
    class Terrain : public Render
    {
        ECHO_CLASS(Terrain, Render)
        
    public:
        // Vertex Format
        struct VertexFormat
        {
            Vector3        m_position;
            Vector3        m_normal;
            Vector2        m_uv;
        };
        typedef vector<VertexFormat>::type  VertexArray;
        typedef vector<ui32>::type          IndiceArray;
        
    public:
        Terrain();
        virtual ~Terrain();
        
        // font res path
        void setHeightmap(const ResourcePath& path);
        const ResourcePath& getHeightmap() { return m_heightmap; }
        
        // width height
        i32 getColumns() const { return m_columns; }
        i32 getRows() const { return m_rows; }

		// height range
		float getHeightRange() const { return m_heightRange; }
		void setHeightRange(float range);

		// grid spacing
		i32 getGridSpacing() const { return m_gridSpacing; }
		void setGridSpacing(i32 gridSpacing);
        
        // material
        Material* getMaterial() const { return m_material; }
        void setMaterial( Object* material);
        
        // get height
        float getHeight(i32 x, i32 z);
        
        // get normal
        Vector3 getNormal(i32 x, i32 z);
        
    protected:
        // build drawable
        void buildRenderable();
        
        // update
        virtual void update_self() override;
        
        // update vertex buffer
        void updateMeshBuffer();
        
        // build mesh data by drawables data
        void buildMeshData(VertexArray& oVertices, IndiceArray& oIndices);
        
        // clear
        void clear();
        void clearRenderable();
        
    private:
        bool                    m_isRenderableDirty = true;
        ResourcePath            m_heightmap = ResourcePath("", ".png");
        Image*                  m_heightmapImage = nullptr;
		float					m_heightRange = 256.f;
		i32						m_gridSpacing = 1;
        Mesh*                   m_mesh = nullptr;
        ShaderProgramPtr        m_shader;
        MaterialPtr             m_material;
        MaterialPtr             m_materialDefault;
        Renderable*             m_renderable = nullptr;
        Matrix4                 m_matWVP;
        i32                     m_columns = 0;
        i32                     m_rows = 0;
		//TerrainTiles			m_tiles;
    };
}
