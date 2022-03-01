#pragma once

#include "il2cpp_types.h"

#define IL2CPP_API_H                "il2cpp_api.h"
#define IL2CPP_TYPEDEF(r, n, p)      typedef r(__cdecl* n ## _t)p;
#define IL2CPP_EXTERN_DECLARATION(n) extern n ## _t n;
#define IL2CPP_DECLARATION(n)        n ## _t n;
#define IL2CPP_FIND_FUNCTION(n)      API:: ## n = memory.find_function<n ## _t>(game_assembly, #n);

#define DO_API(r, n, p) IL2CPP_TYPEDEF(r, n, p);
#include IL2CPP_API_H
#undef DO_API

#include "hook.h"


struct IL2CPP_ASSEMBLY;
struct IL2CPP_NAMESPACE;
struct IL2CPP_CLASS;
struct IL2CPP_FIELD;
struct IL2CPP_METHOD;
struct IL2CPP_OBJECT;
struct IL2CPP_ARRAY;
struct IL2CPP_STRING;


namespace IL2CPP
{
	extern Il2CppDomain* domain;
	extern const Il2CppAssembly** assemblies;
	extern size_t assemblies_size;

	namespace API {
		#define DO_API(r, n, p) IL2CPP_EXTERN_DECLARATION(n)
		#include IL2CPP_API_H
		#undef DO_API
	}

	bool Initialize(Memory memory);
	void Attach();

	IL2CPP_OBJECT Object(Il2CppObject* obj);
	IL2CPP_CLASS Klass(Il2CppClass* klass);
	IL2CPP_STRING String(const char* name);
	IL2CPP_STRING String(Il2CppString* str);
	IL2CPP_ASSEMBLY Assembly(const char* assembly_name);
	IL2CPP_ARRAY Array(Il2CppArray* arr);
};



// IL2CP_ASSEMBLY

struct IL2CPP_ASSEMBLY {
	IL2CPP_ASSEMBLY(Il2CppAssembly* assembly);
	IL2CPP_ASSEMBLY(const char* assembly_name);

	IL2CPP_NAMESPACE global_namespaze();
	IL2CPP_NAMESPACE namespaze(const char* assembly_name);

	Il2CppImage* image();

	const Il2CppAssembly* assembly;
};



// IL2CPP_NAMESPACE

struct IL2CPP_NAMESPACE {
	IL2CPP_NAMESPACE(IL2CPP_ASSEMBLY* assembly, const char* namespace_name);

	IL2CPP_CLASS klass(const char* class_name);

	IL2CPP_ASSEMBLY* assembly;
	const char* name;
};



// IL2CPP_CLASS

struct IL2CPP_CLASS {
	IL2CPP_CLASS();
	IL2CPP_CLASS(Il2CppClass* klass);

	IL2CPP_METHOD method(const char* method_name, int args_count);
	//const MethodInfo* methods(void** iter);

	IL2CPP_FIELD field(const char* field_name);
	//FieldInfo* fields(void** iter);

	const PropertyInfo* property(const char* property_name);
	//const PropertyInfo* properties(void** iter);

	const char* name();
	const char* namespaze();
	const char* assemblyname();
	const Il2CppImage* image();

	Il2CppClass* klass;
};



// IL2CPP_FIELD

struct IL2CPP_FIELD {
	IL2CPP_FIELD(FieldInfo* fieldinfo);

	const char* name();
	size_t offset();
	void set_value(Il2CppObject* obj, void* value);

	template <class T>
	T get_value(Il2CppObject* obj) {

		T ret;

		IL2CPP::API::il2cpp_field_get_value(obj, field, &ret);

		return ret;
	};

	FieldInfo* field;
};



// IL2CPP_METHOD

struct IL2CPP_METHOD {
	IL2CPP_METHOD(const MethodInfo* methodinfo);

	const char* name();

	template <class T>
	Hook* hook(Memory memory, void* new_method, T* original) {
		auto new_hook = hook<T>(memory, new_method);
		*original = new_hook->get_original<T>();

		return new_hook;
	}

	template <class T>
	Hook* hook(Memory memory, void* new_method) {
		auto new_hook = new Hook(memory, this->get_function<T>(), new_method);

		new_hook->load();

		return new_hook;
	}

	template <class T>
	T get_function() {
		return (T)method->methodPointer;
	}

	const MethodInfo* method;
};



// IL2CPP_TYPE

struct IL2CPP_TYPE {
	IL2CPP_TYPE(Il2CppType* type_);

	IL2CPP_CLASS klass();
	const char* name();

	Il2CppType* type;
};



// IL2CPP_OBJECT

struct IL2CPP_OBJECT {
	IL2CPP_OBJECT(Il2CppObject* obj_);

	template <class T>
	T get_value(const char* field_name) {
		return obj_klass.field(field_name).get_value<T>(obj);
	};

	void set_value(const  char* field_name, void* value);

	IL2CPP_CLASS klass();

	IL2CPP_CLASS obj_klass;
	Il2CppObject* obj;
};



// IL2CPP_ARRAY

struct IL2CPP_ARRAY {
	IL2CPP_ARRAY(Il2CppArray* aray_);

	size_t max_length();

	template <class T>
	T* loop() {
		return (T*)((ptr_t)aray + sizeof(Il2CppArray));
	}

	template <class T>
	T get_index(size_t id) {
		return loop<T>()[id];
		//return (T)((ptr_t)(loop<T>()) + id * sizeof(void*));
	}

	Il2CppArray* aray;
};



// IL2CPP_STRING

struct IL2CPP_STRING {
	IL2CPP_STRING(Il2CppString* str_);

	bool equal(IL2CPP_STRING);
	const size_t length();
	const wchar_t* wchars();

	Il2CppString* str;
};