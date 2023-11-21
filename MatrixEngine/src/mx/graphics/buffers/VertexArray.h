#pragma once
#include "Common.h"

///@file VertexArray.h



namespace Matrix
{
	namespace graphics
	{
		class MATRIX_DLL VertexArray
		{
		public:
			/// Creates a VertexArray object.
			VertexArray();

			/// Destructor for VertexArray (calls DeleteVertexArray).
			~VertexArray();

			void Init();
			/// Bind the VertexArray.
			void Bind() const;

			/// Unbind the VertexArray.
			void UnBind() const;

			/// Get the ID of the VertexArray.
			uint32_t GetId();

		private:
			/// Private member variable to store the ID of the VertexArray.
			uint32_t m_ID;
		};
	}
}
