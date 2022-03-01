#pragma warning( disable : 4703 )
#pragma warning( disable : 4996 )

#include <Windows.h>
#include <iostream>
#include <random>
#include <string>
#include <io.h>
#include <fcntl.h>

#include "hook.h"
#include "unity.h"


struct vec3 { 
	vec3() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	};

	vec3(float x_, float y_, float z_) {
		x = x_;
		y = y_;
		z = z_;
	}

	float x; 
	float y; 
	float z; 
};


// Function Definitions

typedef void(*vp_Camera_SetRotation_t)(void* this_, vec3 ang, bool stop, bool refresh);
vp_Camera_SetRotation_t vp_Camera_SetRotation;

typedef void(*vp_Controller_AddForce_t)(void* this_, vec3 force);
vp_Controller_AddForce_t vp_Controller_AddForce;

typedef void(*vp_Controller_SetPosition_t)(void* this_, vec3 pos);
vp_Controller_SetPosition_t vp_Controller_SetPosition;


// Hook definitions

typedef void(*RemotePlayersController_Update_t)(void* this_);
RemotePlayersController_Update_t RemotePlayersController_Update_original;

typedef void(*vp_Camera_Update_t)(void* this_);
vp_Camera_Update_t vp_Camera_Update_original;

bool aimbot = false;

void RemotePlayersController_Update(Il2CppObject* this_) {

	auto obj = IL2CPP::Object(this_);

//  auto current_player = IL2CPP::Object(obj.get_value<Il2CppObject*>("pgoLocalPlayer"));
	auto player_array = IL2CPP::Array(obj.get_value<Il2CppArray*>("RemotePlayersList"));

	for (size_t i = 0; i < player_array.max_length(); i++)
	{
		auto player      = IL2CPP::Object(player_array.get_index<Il2CppObject*>(i));
		auto player_name = IL2CPP::String(player.get_value<Il2CppString*>("Name"));

		if (!player_name.str) {
			continue;
		}

		// New values
		byte new_team   = 0;
		bool new_zombie = true;

		// Set them...
		player.set_value("Team",   &new_team);
		player.set_value("zombie", &new_zombie);
	}

	return RemotePlayersController_Update_original(this_);
}

// from stackoverflow
float RandomFloat(float min, float max)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float range = max - min;
	return (random * range) + min;
}

void vp_Camera_Update(Il2CppObject* this_) {

	// Get our objects
	auto camera = IL2CPP::Object(this_);
	auto controller = IL2CPP::Object(camera.get_value<Il2CppObject*>("FPController"));

	// New values
	float new_acceleration = 0.25f;
	float new_gravity      = 0.05f;

	// Set them..
	controller.set_value("MotorAcceleration",      &new_acceleration);
	controller.set_value("MotorJumpForce",         &new_acceleration);
	controller.set_value("PhysicsGravityModifier", &new_gravity);

	// Crouch Height
	//controller.set_value("m_CrouchHeight", &new_crouch);

	// Rotation
	//vec3 new_rotation = vec3(RandomFloat(0, 360), RandomFloat(0, 360), RandomFloat(0, 360));
	//vp_Camera_SetRotation(this_, new_rotation, true, true);
	

	// TP
	if (GetAsyncKeyState(VK_SHIFT) & 1) {
		auto pos = controller.get_value<vec3>("m_FixedPosition");

		vec3 new_pos = vec3(pos.x, pos.y + 2, pos.z);

		vp_Controller_SetPosition(controller.obj, new_pos);
	}

	return vp_Camera_Update_original(this_);
}


DWORD WINAPI mainThread(HMODULE hModule)
{
	if (hModule) {//while (true) if (GetAsyncKeyState(VK_DELETE) & 1) break;

		FILE* f;
		AllocConsole();
		freopen_s(&f, "CONOUT$", "w", stdout);
		//_setmode(_fileno(stdout), _O_U16TEXT);
		
		Memory memory;

		// Initialize IL2CPP API
		IL2CPP::Initialize(memory);
		IL2CPP::Attach();

		// Initialize Unity API
		Unity::Initialize(memory);

		// Getting Namespace: Assembly-CSharp
		auto assembly_csharp  = IL2CPP::Assembly("Assembly-CSharp");
		auto global_namespace = assembly_csharp.global_namespaze();


		// Grabbing functions

		vp_Controller_AddForce = global_namespace.klass("vp_FPController").method("AddForce", 1)
		.get_function<vp_Controller_AddForce_t>();

		vp_Controller_SetPosition = global_namespace.klass("vp_FPController").method("SetPosition", 1)
		.get_function<vp_Controller_SetPosition_t>();

		vp_Camera_SetRotation = global_namespace.klass("vp_FPCamera").method("SetRotation", 3)
		.get_function<vp_Camera_SetRotation_t>();


		// Hooks

		auto update_hook = global_namespace.klass("RemotePlayersController").method("Update", 0)
		.hook<RemotePlayersController_Update_t>(memory, RemotePlayersController_Update, &RemotePlayersController_Update_original);

		auto update2_hook = global_namespace.klass("vp_FPCamera").method("Update", 0)
		.hook<vp_Camera_Update_t>(memory, vp_Camera_Update, &vp_Camera_Update_original);

		while (true) {};
	}

	return true;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CloseHandle(CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)mainThread, hModule, NULL, nullptr));
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}