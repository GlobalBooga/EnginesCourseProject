# EnginesCourseProject
 
#### Group Formation

I am working alone. However, that decision came after finding out that there wasn't a partner for me. My strategy is simple. First, I think about what project would benefit me the most. Since my GDW game is being made in Unreal Engine, I figured that working in Unreal Engine would serve me as good practice. With this constraint in mind, I talked to some people that I'd want to work with and probed them about their plans. It turned out people were either already in groups or were trying to avoid Unreal Engine. Therefore, I will be working alone. This is fine because it allows me to keep my scope small.

Roles:
Alex Anastasakis - 100892202 (contribution 100%)
 - Singleton
 - Factory
 - Obsersver
 - Command
 - Video report
 - Github management
 - Project management
 - Submission


#### Interactive Media Scenario Information

The interactive media scenario presented in this project is a snapshot of my GDW game. It is essentially a demo of the AI used by all NPCs in the game. The scenario shown off in the project is a live run of this AI working. The purpose is to pressure test it in different situations to make sure it doesn't break and works as intended. In this demo, the user's role is to watch and judge. It is intended to be "played" from the editor, where the user will place or move objects in the scene and watch how the AI responds or acts during play. However, the player can also move around and spawn things.


#### About the Project

As stated above, this project is a snapshot the AI system in my GDW game. It's a technical demo, not a game. 
This system is for all of the AI in an atmospheric, realtime strategy, tower defense game where you run and defend your town from nightly invasions by commanding your villagers. The AI solution implemented in this project is a hierarchical task network. Characters will be given a list of complex tasks based on their occupation, sorted by priority. They automatically perform tasks based on their world state - which is directly controlled by sensors.


## Use of Programming Patterns

The scope of the project is a little small because this hierarchical task network (HTN) system manages to use all patterns required for this assignment. Everything else is essentially padding to properly show off this HTN AI work.


### Singleton
_Types introduced in this section: UTaskSubsystem, UPrimitiveTask._

My singleton implementation is very simple. Its just a globally ticking object that runs in the background and makes primitive tasks run. The reasoning for using a singleton this way is to separate the HTN component and individual primitive tasks. The goal is to execute tasks like this:
```
task = get a task object
task->run()
```

The singleton class is **UTaskSubsystem** (Task.h). It derives from **UGameInstanceSubsystem** and **FTickableGameObject**. Deriving from these classes make it automatically spawn in the background and gives it the ability to tick - which is used to propagate tick events to instances of UPrimitiveTask.

UPrimitiveTask derives from UObject. On its own, it does nothing. It relies on an external object to call it's Tick() function.

Before a primitive task can be executed, it needs to be initialized. In its initializer, it will automatically register itself in the task subsystem (UTaskSubsystem). 

```
UPrimitiveTask::Initialize()
{
 // Get the subsystem instance and register this
 SubsystemId = TaskSubsystemInstance->RegisterPrimitiveTask(this)

 // Set other properties
}
```
In UTaskSubsystem::RegisterPrimitiveTask...
```
UTaskSubsystem::RegisterPrimitiveTask(UPrimitiveTask* TaskToRegister)
{
 if (TaskToRegister != nullptr)
 	//add it to registered tasks array
}
```

Once initialized, the task subsystem will make it tick..
```
UTaskSubsystem::Tick(float DeltaTime)
{
 Foreach registered primitive task:
 {
  if (RegisteredPrimitiveTask == nullptr)
 		remove RegisteredPrimitiveTask
 		continue

  // Once we know that RegisteredPrimitiveTask is valid
  RegisteredPrimitiveTask->Tick(DeltaTime)
 }
}
```

When primitive tasks are finished, they will automatically unregister themselves...
```
UPrimitiveTask::OnTaskCompleted()
{
 // Get the sybsystem instance and unregister this
 TaskSubsystemInstance->UnRegisterPrimitiveTask(this)

 // On task complete code..
}
```


### Factory
_Types introduced in this Section: UHTNComponent, USensor._

My implementation of a factory in this project is subtle but important. This pattern is used to instantiate instances of primitive task sub classes for use on a local object (the instantiator). It is implemented in the same way to instantiate sensors. 



