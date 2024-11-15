#pragma once
#include "Windows.h"
#include <list>
#include <locale>        
#include <codecvt>        
#include <cstdint>
#include <mono/metadata/threads.h>
#include <mono/metadata/object.h>

#include "libil2cpp/il2cpp-object-internals.h"

typedef struct JInfo {
	DWORD dMethod;
	DWORD n;
	DWORD codeStart;
	unsigned int unwind_info;
	int codeSize;
};

typedef Il2CppThread* (*t_mono_thread_attach)(MonoDomain*);
typedef void (*t_mono_thread_detach)(Il2CppThread*);
typedef MonoDomain* (*t_mono_get_root_domain)();
typedef MonoAssembly* (*t_mono_domain_assembly_open)(MonoDomain* domain, const char* name);
typedef MonoImage* (*t_mono_assembly_get_image)(MonoAssembly* assembly);
typedef MonoClass* (*t_mono_class_from_name)(MonoImage* image, const char* name_space, const char* name);
typedef MonoMethod* (*t_mono_class_get_method_from_name)(MonoClass* klass, const char* name, int param_count);
typedef FieldInfo* (*t_il2cpp_class_get_field_from_name)(Il2CppClass* klass, const char* name);
typedef Il2CppObject* (*t_il2cpp_field_get_value_object)(FieldInfo* field, Il2CppObject* obj);
typedef void (*t_il2cpp_field_set_value_object)(Il2CppObject* obj, FieldInfo* field, Il2CppObject* value);
typedef Il2CppObject* (*t_il2cpp_object_new)(Il2CppClass* klass);
typedef Il2CppString* (*t_il2cpp_string_new)(const char* str);
typedef Il2CppObject* (*t_mono_runtime_invoke)(MonoMethod* method, DWORD* thisArg, void** params, DWORD* exc);
typedef void* (*t_mono_object_unbox)(Il2CppObject* object);
typedef const char* (*t_mono_string_to_utf8_checked)(MonoString* method, MonoException* exc);
typedef Il2CppArray* (*t_il2cpp_array_new)(Il2CppClass* elementTypeInfo, il2cpp_array_size_t length);


inline auto base = (intptr_t*)GetModuleHandle(L"GameAssembly.dll");

class Mono
{
private:


	void Initialize() {
		hMono = GetModuleHandle(L"GameAssembly.dll");
		if (hMono == NULL) {
			return;
		}
		mono_domain_assembly_open = reinterpret_cast<t_mono_domain_assembly_open>(GetProcAddress(hMono, "il2cpp_domain_assembly_open"));
		mono_assembly_get_image = reinterpret_cast<t_mono_assembly_get_image>(GetProcAddress(hMono, "il2cpp_assembly_get_image"));
		mono_class_from_name = reinterpret_cast<t_mono_class_from_name>(GetProcAddress(hMono, "il2cpp_class_from_name"));
		mono_class_get_method_from_name = reinterpret_cast<t_mono_class_get_method_from_name>(GetProcAddress(hMono, "il2cpp_class_get_method_from_name"));
		il2cpp_class_get_field_from_name = reinterpret_cast<t_il2cpp_class_get_field_from_name>(GetProcAddress(hMono, "il2cpp_class_get_field_from_name"));
		il2cpp_field_get_value_object = reinterpret_cast<t_il2cpp_field_get_value_object>(GetProcAddress(hMono, "il2cpp_field_get_value_object"));
		il2cpp_field_set_value_object = reinterpret_cast<t_il2cpp_field_set_value_object>(GetProcAddress(hMono, "il2cpp_field_set_value_object"));
		il2cpp_object_unbox = reinterpret_cast<t_mono_object_unbox>(GetProcAddress(hMono, "il2cpp_object_unbox"));
		il2cpp_string_new = reinterpret_cast<t_il2cpp_string_new>(GetProcAddress(hMono, "il2cpp_string_new"));
		mono_thread_attach = reinterpret_cast<t_mono_thread_attach>(GetProcAddress(hMono, "il2cpp_thread_attach"));
		mono_thread_detach = reinterpret_cast<t_mono_thread_detach>(GetProcAddress(hMono, "il2cpp_thread_detach"));
		mono_get_root_domain = reinterpret_cast<t_mono_get_root_domain>(GetProcAddress(hMono, "mono_get_root_domain"));
		il2cpp_object_new = reinterpret_cast<t_il2cpp_object_new>(GetProcAddress(hMono, "il2cpp_object_new"));
		runtime_invoke = reinterpret_cast<t_mono_runtime_invoke>(GetProcAddress(hMono, "il2cpp_runtime_invoke"));
		il2cpp_array_new = reinterpret_cast<t_il2cpp_array_new>(GetProcAddress(hMono, "il2cpp_array_new"));
		mono_thread_attach(mono_get_root_domain());
		initialized = true;

	}
	bool initialized = false;
	HMODULE hMono;

public:
	Mono() {
	}
	t_il2cpp_object_new il2cpp_object_new;
	t_mono_thread_attach mono_thread_attach;
	t_mono_get_root_domain mono_get_root_domain;
	t_mono_domain_assembly_open mono_domain_assembly_open;
	t_mono_assembly_get_image mono_assembly_get_image;
	t_mono_class_from_name mono_class_from_name;
	t_mono_object_unbox il2cpp_object_unbox;
	t_mono_class_get_method_from_name mono_class_get_method_from_name;
	t_il2cpp_class_get_field_from_name il2cpp_class_get_field_from_name;
	t_il2cpp_field_get_value_object il2cpp_field_get_value_object;
	t_il2cpp_field_set_value_object il2cpp_field_set_value_object;
	t_mono_runtime_invoke runtime_invoke;
	t_il2cpp_string_new il2cpp_string_new;
	t_il2cpp_array_new il2cpp_array_new;
	t_mono_thread_detach mono_thread_detach;

	HMODULE getHandle() { return hMono; }
	static Mono& instance() {
		static Mono _instance;
		if (!_instance.initialized) {
			_instance.Initialize();
		}
		return _instance;
	}

	std::list<MonoClass*> GetAssemblyClassList(MonoImage* image)
	{
		std::list<MonoClass*> class_list;

		const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);

		int rows = mono_table_info_get_rows(table_info);

		/* For each row, get some of its values */
		for (int i = 0; i < rows; i++)
		{
			MonoClass* _class = nullptr;
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			_class = mono_class_from_name(image, name_space, name);
			class_list.push_back(_class);
		}
		return class_list;
	}

	void* GetMethod(const char* className, const char* methodName, int methodNumber = -1, const char* name_space = "", const char* assemblyName = "Assembly-CSharp") {
		auto pDomain = mono_get_root_domain();
		if (pDomain == nullptr) {
			return nullptr;
		}

		auto pAssembly = mono_domain_assembly_open(pDomain, assemblyName);
		if (pAssembly == nullptr) {
			return nullptr;
		}

		auto pImage = mono_assembly_get_image(pAssembly);
		if (pImage == nullptr) {
			return nullptr;
		}

		auto pClass = mono_class_from_name(pImage, name_space, className);
		if (pClass == nullptr) {
			return nullptr;
		}

		auto pMethod = (intptr_t*)mono_class_get_method_from_name(pClass, methodName, methodNumber);
		if (pMethod == nullptr) {
			return nullptr;
		}

		auto method = (intptr_t*)*pMethod;
		return (void*)((method - base) * sizeof(intptr_t*));
	}

	void* GetMethod(MonoClass* pClass, const char* methodName, int methodNumber = -1) {
		auto pMethod = (intptr_t*)mono_class_get_method_from_name(pClass, methodName, methodNumber);
		if (pMethod == nullptr) {
			return nullptr;
		}

		auto method = (intptr_t*)*pMethod;
		return (void*)((method - base) * sizeof(intptr_t*));
	}

	Il2CppClass* GetIl2CppClass(const char* className, const char* name_space = "", const char* assemblyName = "Assembly-CSharp") {
		auto pDomain = mono_get_root_domain();
		if (pDomain == nullptr) {
			return nullptr;
		}

		auto pAssembly = mono_domain_assembly_open(pDomain, assemblyName);
		if (pAssembly == nullptr) {
			return nullptr;
		}

		auto pImage = mono_assembly_get_image(pAssembly);
		if (pImage == nullptr) {
			return nullptr;
		}

		return (Il2CppClass*)mono_class_from_name(pImage, name_space, className);

	}

	MonoMethod* GetIl2CppMethod(const char* className, const char* methodName, int methodNumber = -1, const char* name_space = "", const char* assemblyName = "Assembly-CSharp") {
		auto pDomain = mono_get_root_domain();
		if (pDomain == nullptr) {
			return nullptr;
		}

		auto pAssembly = mono_domain_assembly_open(pDomain, assemblyName);
		if (pAssembly == nullptr) {
			return nullptr;
		}

		auto pImage = mono_assembly_get_image(pAssembly);
		if (pImage == nullptr) {
			return nullptr;
		}

		auto pClass = mono_class_from_name(pImage, name_space, className);
		if (pClass == nullptr) {
			return nullptr;
		}

		auto pMethod = (MonoMethod*)mono_class_get_method_from_name(pClass, methodName, methodNumber);
		if (pMethod == nullptr) {
			return nullptr;
		}

		return pMethod;
	}

	MonoMethod* GetIl2CppMethod(Il2CppClass* pClass, const char* methodName, int methodNumber = -1) {
		return mono_class_get_method_from_name((MonoClass*)pClass, methodName, methodNumber);
	}

	Il2CppObject* GetField(Il2CppObject* obj, const char* fieldName) {
		auto fieldInfo = il2cpp_class_get_field_from_name(obj->klass, fieldName);
		if (fieldInfo == nullptr) {
			return nullptr;
		}

		return il2cpp_field_get_value_object(fieldInfo, obj);
	}

	void SetField(Il2CppObject* obj, const char* fieldName, Il2CppObject* value) {
		auto fieldInfo = il2cpp_class_get_field_from_name(obj->klass, fieldName);
		if (fieldInfo == nullptr) {
			return;
		}

		il2cpp_field_set_value_object(obj, fieldInfo, value);
	}
};



// Helper function to convert Il2CppString to std::string
inline std::string il2cppi_to_string(Il2CppString* str) {
	std::u16string u16(reinterpret_cast<const char16_t*>(str->chars));
	return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16);
}

// Helper function to convert System.String to std::string
inline std::string ConvertString(DWORD* str) {
	return il2cppi_to_string(reinterpret_cast<Il2CppString*>(str));
}