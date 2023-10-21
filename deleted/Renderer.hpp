// Copyright 2023 JohnCorn
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once
#include <vector>
#include <array>
#include <memory>
#include <cassert>
#include <cstddef>
#include "StreamProcessor.hpp"



namespace Framework
{

	using ObjectsTypeID = unsigned int;
	struct IRenderer;

	template<typename T>
	struct CPUBufferPtr
	{
		size_t elem_count = 0;
		const T* data = nullptr;

		explicit CPUBufferPtr() = default;
		explicit CPUBufferPtr(const T* data, size_t elem_count)
			: elem_count(elem_count), data(data)
		{}

		inline operator bool() const { return elem_count > 0 && data != nullptr; }

		template<typename P>
		CPUBufferPtr<P> ReinterpretCast() const
		{
			return CPUBufferPtr<P>(reinterpret_cast<const P*>(data), sizeof(T) * elem_count / sizeof(P));
		}

		void Reset() { data = nullptr; elem_count = 0; }
	};

	/// static buffer on gpu (for textures, buffer object, etc)
	struct IGPUStaticBuffer
	{
		explicit IGPUStaticBuffer(size_t elem_size, size_t count, const void* data = nullptr)
			: size(elem_size* count)
		{}
		virtual ~IGPUStaticBuffer() = default;
		virtual void Realloc(size_t elem_size, size_t count, const void* data = nullptr)
		{
			size = elem_size * count;
		}
		virtual void Upload(const void* data, size_t elem_size, size_t count, size_t offset = 0) = 0;

	protected:
		size_t size; ///< allocated size in bytes
	private:
		IGPUStaticBuffer(const IGPUStaticBuffer&) = delete;
		IGPUStaticBuffer& operator=(const IGPUStaticBuffer&) = delete;
	};

	/// @brief geometry which can be rendered.
	struct Geometry final
	{
		using VertexData = CPUBufferPtr<std::byte>;
		using IndexData = CPUBufferPtr<unsigned int>;

		explicit Geometry(size_t vertex_size, VertexData vertices, IndexData indices = IndexData())
			: vertex_size(vertex_size)
			, vertices(vertices)
			, indices(indices)
		{}
		inline size_t GetVertexSize() const { return vertex_size; }
		inline size_t GetVerticesCount() const { return vertices.elem_count / vertex_size; }
		inline bool UseIndices() const { return indices; }

	private:
		size_t vertex_size = 0;
		VertexData vertices;
		IndexData indices;
		Geometry(const Geometry&) = delete;
		Geometry& operator=(const Geometry&) = delete;
	};

	/// @brief shader program which can render objects of specific ObjectsTypeID
	struct IShaderProgram
	{
		virtual ~IShaderProgram() = default;
		virtual ObjectsTypeID GetObjectsTypeID() const = 0;
		virtual void Bind() const = 0;

	private:
		IShaderProgram(const IShaderProgram&) = delete;
		IShaderProgram& operator=(const IShaderProgram&) = delete;
	};

	struct IRenderable
	{
		virtual ~IRenderable() = default;
		virtual void Render(double timestamp) const = 0;
	};

	using ContextID = unsigned int; ///< id of graphic context
	ContextID RequestNewContextID();

	void InitRenderingSystem();

	/// own context data and begin rendering. thread-local rendering interface
	using FrameID = unsigned long long;

	struct FrameData
	{

	};

	struct IRenderer : public IStreamProcessor<FrameID, FrameData>
	{
		friend class OSWindow; // to OSWindow can access to RenderPass
		//-------------- API ---------------------
		IRenderer() : IStreamProcessor<FrameID, FrameData>(2) {}
		/// destructor
		virtual ~IRenderer() = default;
	protected:
		void ProcessTask(FrameID frame_id, FrameData&& frame_data)
		{
			RenderPass();
		}

		/// render frame function
		virtual void RenderPass() = 0;
	private:
		ContextID ctx_id = RequestNewContextID();
	};

}