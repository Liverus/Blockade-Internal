#include "il2cpp.h"
#include <iostream>



namespace IL2CPP {

	Il2CppDomain* domain;
	const Il2CppAssembly** assemblies;
	size_t assemblies_size;

	namespace API {
#define DO_API(r, n, p) IL2CPP_DECLARATION(n)
#include IL2CPP_API_H
#undef DO_API
	}

	bool Initialize(Memory memory)
	{
		void* game_assembly = memory.find_module("GameAssembly.dll");

#define DO_API(r, n, p) IL2CPP_FIND_FUNCTION(n)
#include IL2CPP_API_H
#undef DO_API

		domain = API::il2cpp_domain_get();
		assemblies = API::il2cpp_domain_get_assemblies(domain, &assemblies_size);

		return true;
	}

	void Attach() {
		API::il2cpp_thread_attach(domain);
	}

	IL2CPP_STRING String(const char* name) {
		return IL2CPP_STRING(API::il2cpp_string_new(name));
	}

	IL2CPP_STRING String(Il2CppString* str) {
		return IL2CPP_STRING(str);
	}

	IL2CPP_ASSEMBLY Assembly(const char* assembly_name) {
		return IL2CPP_ASSEMBLY(assembly_name);
	}

	IL2CPP_OBJECT Object(Il2CppObject* obj) {
		return IL2CPP_OBJECT(obj);
	}

	IL2CPP_CLASS Klass(Il2CppClass* klass) {
		return IL2CPP_CLASS(klass);
	}

	IL2CPP_ARRAY Array(Il2CppArray* array) {
		return IL2CPP_ARRAY(array);
	}
}



// IL2CPP_ASSEMBLY

IL2CPP_ASSEMBLY::IL2CPP_ASSEMBLY(Il2CppAssembly* assembly_) {
	assembly = assembly_;
}

IL2CPP_ASSEMBLY::IL2CPP_ASSEMBLY(const char* assembly_name) {
	assembly = IL2CPP::API::il2cpp_domain_assembly_open(IL2CPP::domain, assembly_name);
}

Il2CppImage* IL2CPP_ASSEMBLY::image() {
	return assembly->image;
}

IL2CPP_NAMESPACE IL2CPP_ASSEMBLY::namespaze(const char* namespace_name) {
	return IL2CPP_NAMESPACE(this, namespace_name);
}

IL2CPP_NAMESPACE IL2CPP_ASSEMBLY::global_namespaze() {
	return IL2CPP_NAMESPACE(this, "");
}



// IL2CPP_NAMESPACE

IL2CPP_NAMESPACE::IL2CPP_NAMESPACE(IL2CPP_ASSEMBLY* namespace_assembly, const char* namespace_name) {
	name = namespace_name;
	assembly = namespace_assembly;
}

IL2CPP_CLASS IL2CPP_NAMESPACE::klass(const char* klass_name) {
	return IL2CPP::API::il2cpp_class_from_name(assembly->image(), name, klass_name);
}



// IL2CPP_CLASS

IL2CPP_CLASS::IL2CPP_CLASS() {};

IL2CPP_CLASS::IL2CPP_CLASS(Il2CppClass* klass_) {
	klass = klass_;
}

IL2CPP_METHOD IL2CPP_CLASS::method(const char* method_name, int args_count) {
	return IL2CPP_METHOD(IL2CPP::API::il2cpp_class_get_method_from_name(klass, method_name, args_count));
};

//const MethodInfo* IL2CPP_CLASS::methods(void** iter) {
//	return IL2CPP::API::il2cpp_class_get_methods(klass, iter);
//};

IL2CPP_FIELD IL2CPP_CLASS::field(const char* field_name) {
	return IL2CPP_FIELD(IL2CPP::API::il2cpp_class_get_field_from_name(klass, field_name));
}

//IL2CPP_FIELD* IL2CPP_CLASS::fields(void** iter) {
//	return IL2CPP::API::il2cpp_class_get_fields(klass, iter);
//};

const PropertyInfo* IL2CPP_CLASS::property(const char* property_name) {
	return IL2CPP::API::il2cpp_class_get_property_from_name(klass, property_name);
}

//const PropertyInfo* IL2CPP_CLASS::properties(void** iter) {
//	return IL2CPP::API::il2cpp_class_get_properties(klass, iter);
//};

const char* IL2CPP_CLASS::name() {
	return  IL2CPP::API::il2cpp_class_get_name(klass);
}

const char* IL2CPP_CLASS::namespaze() {
	return  IL2CPP::API::il2cpp_class_get_namespace(klass);
}

const char* IL2CPP_CLASS::assemblyname() {
	return  IL2CPP::API::il2cpp_class_get_assemblyname(klass);
}

const Il2CppImage* IL2CPP_CLASS::image() {
	return IL2CPP::API::il2cpp_class_get_image(klass);
}



// IL2CPP_FIELD

IL2CPP_FIELD::IL2CPP_FIELD(FieldInfo* fieldinfo) {
	field = fieldinfo;
}

const char* IL2CPP_FIELD::name() {
	return IL2CPP::API::il2cpp_field_get_name(field);
}

size_t IL2CPP_FIELD::offset() {
	return IL2CPP::API::il2cpp_field_get_offset(field);
}

void IL2CPP_FIELD::set_value(Il2CppObject* obj, void* value) {
	IL2CPP::API::il2cpp_field_set_value(obj, field, value);
}



// IL2CPP_METHOD

IL2CPP_METHOD::IL2CPP_METHOD(const MethodInfo* methodinfo) {
	method = methodinfo;
}

const char* IL2CPP_METHOD::name() {
	return IL2CPP::API::il2cpp_method_get_name(method);
}



// IL2CPP_TYPE

IL2CPP_TYPE::IL2CPP_TYPE(Il2CppType* type_) {
	type = type;
}

IL2CPP_CLASS IL2CPP_TYPE::klass() {
	return IL2CPP_CLASS(IL2CPP::API::il2cpp_type_get_class_or_element_class(type));
}

const char* IL2CPP_TYPE::name() {
	return IL2CPP::API::il2cpp_type_get_name(type);
}



// IL2CPP_OBJECT

IL2CPP_OBJECT::IL2CPP_OBJECT(Il2CppObject* obj_) {
	obj = obj_;
	obj_klass = IL2CPP::Klass(IL2CPP::API::il2cpp_object_get_class(obj));
}

IL2CPP_CLASS IL2CPP_OBJECT::klass() {
	return obj_klass;
}

void IL2CPP_OBJECT::set_value(const char* field_name, void* value) {
	obj_klass.field(field_name).set_value(obj, value);
}



// IL2CPP_ARRAY

IL2CPP_ARRAY::IL2CPP_ARRAY(Il2CppArray* aray_) {
	aray = aray_;
}

size_t IL2CPP_ARRAY::max_length() {
	return aray->max_length;
}



// IL2CPP_STRING

IL2CPP_STRING::IL2CPP_STRING(Il2CppString* str_) {
	str = str_;
}

const size_t IL2CPP_STRING::length() {
	return IL2CPP::API::il2cpp_string_length(str);
}

const wchar_t* IL2CPP_STRING::wchars() {
	return (wchar_t*)IL2CPP::API::il2cpp_string_chars(str);
}

bool IL2CPP_STRING::equal(IL2CPP_STRING str) {
	return !wcscmp(this->wchars(), str.wchars());
}

