# Internal BLOCKADE Cheat

This is an internal cheat for the game BLOCKADE Classic, a free unity-il2cpp game on Steam.
The only point of this project is to learn IL2CPP API and make my wrappers for it. This is why the features are kinda pointless.
You can implement your own features by using Il2CppDumper + DnSpy and inspecting the game's assemblies.


## Features
- ESP (Sets every player to the same team, we use the game's default ESP)
- Speedhack (We go from 0.18 to 0.25 speed)
- No Gravity
- 2-blocks instant jump when pressing SHIFT

## Disclaimer

This source code is provided for educational purpose only. I'm not responsible if anyone uses this to cheat in game. (Even tho the game is a cancer p2w)

## IL2CPP

So from what I understand you have to hook a class's function in order to get its instances. (Il2CppObject*)

Here are some examples of how to use my wrappers:

Initialize
```c++
Memory memory;

IL2CPP::Initialize(memory);
IL2CPP::Attach();
```

Access the global namespace of Assembly-CSharp
```c++
auto assembly_csharp  = IL2CPP::Assembly("Assembly-CSharp");
auto global_namespace = assembly_csharp.global_namespaze();
```

Access function pointer of a class method
```c++
vp_Controller_AddForce = global_namespace.klass("vp_FPController").method("AddForce", 1) // 1 is the method's arg count
.get_function<vp_Controller_AddForce_t>();
```

Hook a function of a class method
```c++
auto update_hook = global_namespace.klass("RemotePlayersController").method("Update", 0) // 0 is the method's arg count
.hook<RemotePlayersController_Update_t>(memory, RemotePlayersController_Update, &RemotePlayersController_Update_original);
```
