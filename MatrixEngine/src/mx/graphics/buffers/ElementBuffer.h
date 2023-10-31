#pragma once

///@file ElementBuffer.h

#include "Common.h" 

namespace Matrix 
{
	namespace graphics 
	{
		class MATRIX_DLL ElementBuffer
		{
		public:

			/// Creates a buffer object.
			ElementBuffer();

			/// Deletes the vertex buffer.
			~ElementBuffer();

			/// Bind the VertexBuffer.
			void Bind() const;

			/// UnBind the VertexBuffer.
			void UnBind() const;

			/// Function to buffer data into the ElementBuffer.
			void BufferData(uint32_t size, const void* data);

			/// Function to get the ID of the ElementBuffer.
			uint32_t GetId();

		private:

			uint32_t m_ID; /// Private member variable to store the ID of the ElementBuffer.
		};
	} 
}
