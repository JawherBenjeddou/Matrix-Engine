#pragma once

///@file VertexBuffer.h

#include "Common.h"

namespace Matrix
{
	namespace graphics
	{
		class MATRIX_DLL VertexBuffer
		{
		public:

			/// Creates a buffer object.
			VertexBuffer();
			
			/// Deletes the vertex buffer.
			~VertexBuffer();

			/// Bind the VertexBuffer.
			void Bind() const;
			
			/// UnBind the VertexBuffer.
			void UnBind() const;

			/// Function to bufferSubData into the VertexBuffer.
			void BufferSubData(uint32_t& offset, uint32_t size, const void* data);

			/// Function to buffer data into the VertexBuffer
			void BufferData(uint32_t size, const void* data);

			/// Function to get the ID of the VertexBuffer.
			uint32_t GetId();
		private:
			/// Private member variable to store the ID of the VertexBuffer.
			uint32_t m_ID;
		};
	}
}

