#pragma once
#include <Core/engine_types.hpp>
#include <Core/etl/string.hpp>

namespace Engine
{
	struct ENGINE_EXPORT Constants {
		static const uint_t processor_count;
		static const Vector3D OX;
		static const Vector3D OY;
		static const Vector3D OZ;
		static const Quaternion zero_rotation;
		static const Vector3D right_vector;
		static const Vector3D up_vector;
		static const Vector3D forward_vector;
		static const float PI;
		static const float E;
		static const Matrix4f identity_matrix;
		static const Matrix4f zero_matrix;
		static const Vector4D identity_vector;
		static const Vector4D zero_vector;
		static const float min_positive_float;
		static const Vector3D min_positive_vector;
		static const ArrayIndex index_none;
		static const ArrayOffset offset_none;
		static const HashIndex invalid_hash;
		static const size_t max_size;
		static const IntVector4D int_zero_vector;
		static const IntVector4D int_identity_vector;
		static const IntVector4D uint_zero_vector;
		static const IntVector4D uint_identity_vector;
		static const String asset_extention;
		static const String shader_extention;
		static const String name_separator;
		static const PriorityIndex max_priority;
		static const String default_entry_point;
		static const String library_load_function_name;
		static const String script_extension;
		static const String script_byte_code_extension;
		static const String virtual_file_system_extension;
		static const String translation_config_extension;
		static const ByteColor3 splash_text_color;
		static const HashIndex script_userdata_id;
	};
}// namespace Engine
