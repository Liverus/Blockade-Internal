#pragma warning( disable : 4703 )
#pragma warning( disable : 4996 )

#include <Windows.h>
#include <iostream>
#include <random>
#include <string>
#include <io.h>
#include <cmath>
#include <fcntl.h>

#include "vec.h"
#include "hook.h"

#include "memeory.h"
#include "il2cpp.h"

struct Color {
	Color(float r_, float g_, float b_, float z_ = 1.0f) {
		r = r_;
		g = g_;
		b = b_;
		z = z_;
	}

	float r;
	float g;
	float b;
	float z;
};

using namespace IL2CPP;

// Function Definitions

typedef void(*vp_Camera_SetRotation_t)(OBJECT* this_, Vector2 ang, bool stop, bool refresh);
vp_Camera_SetRotation_t vp_Camera_SetRotation;

typedef void(*vp_Controller_AddForce_t)(OBJECT* this_, Vector3 force);
vp_Controller_AddForce_t vp_Controller_AddForce;

typedef void(*vp_Controller_SetPosition_t)(OBJECT* this_, Vector3 pos);
vp_Controller_SetPosition_t vp_Controller_SetPosition;

typedef OBJECT* (*gameobject_get_transform_t)(OBJECT* this_);
gameobject_get_transform_t gameobject_get_transform;

typedef Vector3 (*transform_get_position_t)(OBJECT* this_);
transform_get_position_t transform_get_position;

typedef OBJECT* (*get_main_camera_t)();
get_main_camera_t get_main_camera;

typedef Vector3(*world_to_screen_point_t)(OBJECT* this_, Vector3 pos);
world_to_screen_point_t world_to_screen_point;

typedef int(*camera_width_t)(OBJECT* this_);
camera_width_t camera_width;

typedef int(*camera_height_t)(OBJECT* this_);
camera_height_t camera_height;

// Hook definitions

typedef void(*RemotePlayersController_Update_t)(OBJECT* this_);
RemotePlayersController_Update_t RemotePlayersController_Update_original;

typedef void(*vp_Camera_Update_t)(OBJECT* this_);
vp_Camera_Update_t vp_Camera_Update_original;


// Hooks

bool aimbot = false;
OBJECT* target;

void RemotePlayersController_Update(OBJECT* obj) {

	auto player_data_array = obj->GetArray("RemotePlayersList");
	auto player_obj_array  = obj->GetArray("BotsGmObj");

	float closest_dist = 0;
	float max_dist = pow(800.0f, 2);

	target = NULL;

	for (size_t i = 0; i < player_obj_array->MaxLength(); i++)
	{
		auto player_obj        = player_obj_array->GetObject(i);
		auto player_data       = player_data_array->GetObject(i);

		auto player_active     = player_data->GetValue<bool>("Active");
		auto player_dead       = player_data->GetValue<byte>("Dead");

		if (!player_active || player_dead) {
			continue;
		}

		auto player_name = player_data->GetString("Name");

		// Enemy Pos
		auto body = player_data->GetObject("botPoser")->GetObject("BodyRoot");
		auto body_transform = gameobject_get_transform(body);
		auto target_pos = transform_get_position(body_transform) - Vector3(0, 0.75, 0);

		auto cam = get_main_camera();
		auto screen_pos = world_to_screen_point(cam, target_pos);

		auto width  = camera_width(cam);
		auto height = camera_height(cam);

		auto x = screen_pos.x - width / 2;
		auto y = screen_pos.y - height / 2;
		auto dist = pow(x, 2) + pow(y, 2);

		if (i == 0 || (dist <= closest_dist)) {
			closest_dist = dist;
			target = player_data;
		}

		// Team ESP
		//byte new_team   = 0;
		//player_data.set_value("Team",   &new_team);

		byte team = player_data->GetValue<byte>("Team");

		auto renderer = player_data->GetObject("botPoser")->GetObject("myRenderer");

		typedef OBJECT*(*get_material_t)(void* this_);
		auto get_material = renderer->Class()->Method("GetMaterial")->FindFunction<get_material_t>();

		auto mat = get_material(renderer);

		typedef OBJECT*(*shader_find_t)(STRING* name);

		auto shader_find = IL2CPP::Assembly("UnityEngine.CoreModule")->Namespace("UnityEngine")->Class("Shader")->Method("Find", 1)->FindFunction<shader_find_t>();

		auto shader = shader_find(IL2CPP::String("GUI/Text Shader"));

		typedef void(*set_shader_t)(OBJECT* this_, OBJECT* shader);
		auto set_shader = mat->Class()->Method("set_shader", 1)->FindFunction<set_shader_t>();

		set_shader(mat, shader);

		typedef void(*set_color_t)(OBJECT* this_, Color clr);
		auto set_color = mat->Class()->Method("set_color", 1)->FindFunction<set_color_t>();

		float a = 0.2f;
		Color clr = Color(1.0f, 1.0f, 1.0f, a);

		switch (team) {
			case 0:
				clr = Color(0, 0, 1.0f, a);
				break;
			case 1:
				clr = Color(1.0f, 0, 0, a);
				break;
			case 2:
				clr = Color(0, 1.0f, 0, a);
				break;
			case 3:
				clr = Color(1.0f, 1.0f, 0, a);
		}

		set_color(mat, clr);
	}

	return RemotePlayersController_Update_original(obj);
}

void vp_Camera_Update(OBJECT* this_) {

	// Get our objects
	auto camera     = this_;
	auto controller = camera->GetObject("FPController");

	// New values
	float new_acceleration = 0.22f;
	float new_gravity      = 0.05f;

	// Set them..
	controller->SetValue("MotorAcceleration",      &new_acceleration);
	controller->SetValue("MotorJumpForce",         &new_acceleration);
	controller->SetValue("PhysicsGravityModifier", &new_gravity);

	// TP
	if (GetAsyncKeyState(VK_SHIFT) & 1) {
		auto pos = controller->GetValue<Vector3>("m_FixedPosition");

		vp_Controller_SetPosition(controller, pos + Vector3(0, 2, 0));
	}

	// Aimbot
	if (target) {

		// Enemy Pos
		auto body = target->GetObject("botPoser")->GetObject("BodyRoot");
		auto body_transform = gameobject_get_transform(body);
		auto target_pos = transform_get_position(body_transform) - Vector3(0, 0.75, 0);

		// My pos
		auto my_pos = controller->GetValue<Vector3>("m_FixedPosition");

		auto dir = my_pos.DeltaAngle(target_pos);
		
		auto cam = get_main_camera();
		auto screen_pos = world_to_screen_point(cam, target_pos);
		

		if (GetKeyState(VK_RBUTTON) & 0x8000) {
			vp_Camera_SetRotation(camera, dir, 0, 0);
		}
	}

	return vp_Camera_Update_original(this_);
}


DWORD WINAPI mainThread(HMODULE hModule)
{
	if (hModule) {

		// Initialize Console
		FILE* f;
		AllocConsole();
		freopen_s(&f, "CONOUT$", "w", stdout);
		//_setmode(_fileno(stdout), _O_U16TEXT); // <-- for utf-16 printing i guess? (Il2CppStrings are 2 bytes chars)
		

		// Initialize Memory
		MEMORY memory;


		// Initialize IL2CPP API
		IL2CPP::Initialize(memory);
		IL2CPP::Attach();


		// Getting Namespace: Assembly-CSharp
		auto assembly_csharp  = IL2CPP::Assembly("Assembly-CSharp");
		auto assembly_csharp_namespace = assembly_csharp->Namespace("");

		auto unity_engine = IL2CPP::Assembly("UnityEngine.CoreModule");
		auto unity_engine_namespace = unity_engine->Namespace("UnityEngine");

		// Grabbing functions

		vp_Controller_AddForce    = assembly_csharp_namespace->Class("vp_FPController")->Method("AddForce", 1)->FindFunction<vp_Controller_AddForce_t>();
		vp_Controller_SetPosition = assembly_csharp_namespace->Class("vp_FPController")->Method("SetPosition", 1)->FindFunction<vp_Controller_SetPosition_t>();
		vp_Camera_SetRotation     = assembly_csharp_namespace->Class("vp_FPCamera")->Method("SetRotation", 3)->FindFunction<vp_Camera_SetRotation_t>();

		gameobject_get_transform  =    unity_engine_namespace->Class("GameObject")->Method("get_transform", 0)->FindFunction<gameobject_get_transform_t>();
		transform_get_position    =    unity_engine_namespace->Class("Transform")->Method("get_position", 0)->FindFunction<transform_get_position_t>();
		get_main_camera           =    unity_engine_namespace->Class("Camera")->Method("get_main", 0)->FindFunction<get_main_camera_t>();
		world_to_screen_point     =    unity_engine_namespace->Class("Camera")->Method("WorldToScreenPoint", 1)->FindFunction<world_to_screen_point_t>();
		camera_width              =    unity_engine_namespace->Class("Camera")->Method("get_pixelWidth", 0)->FindFunction<camera_width_t>();
		camera_height             =    unity_engine_namespace->Class("Camera")->Method("get_pixelHeight", 0)->FindFunction<camera_height_t>();

		// Hooks

		assembly_csharp_namespace->Class("RemotePlayersController")->Method("Update")->Hook<RemotePlayersController_Update_t>(memory, RemotePlayersController_Update, &RemotePlayersController_Update_original);
		assembly_csharp_namespace->Class("vp_FPCamera")->Method("Update")->Hook<vp_Camera_Update_t>(memory, vp_Camera_Update, &vp_Camera_Update_original);

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