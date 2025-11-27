# Course Project Part 2

### Improvements from assignment 1
Since the last assignment, small changes were made to the preexisting code base - mostly micro adjustments to work with other parts of the game this project belongs in. Some functions were renamed or deleted, and parameters moved or renamed. These small changes were made after re-importing the plugin from my GDW game ("updated the plugin" commit). The next few commits were for tweaking some of the code to work with this project. This is all 100% my work. The following screenshot is to show where this import happened in the history.

<img width="1676" height="902" alt="imported-from-gdw" src="https://github.com/user-attachments/assets/a4ae745d-0068-4965-afa3-ca14cdb3f8e8" />


Moving on, the micro-changes are small and scattered but one notable change was done on the task factory. What changed is the initialization function for an individual task. 

<img width="1498" height="739" alt="changing-task h" src="https://github.com/user-attachments/assets/c7343995-ee86-4ccb-90dc-175cdf1f8cdd" />


<img width="1482" height="767" alt="changing-task cpp" src="https://github.com/user-attachments/assets/06453ab4-3f30-420f-99a6-ddabc7cc87bc" />


This is a neat and much needed change because nearly every task that does something to its owner actor depends on the output of a previous task. So now, there's no more need to repeat this code:


<img width="1051" height="303" alt="old-init-bp" src="https://github.com/user-attachments/assets/afe96115-abcf-4d14-a987-88b28f17c70e" />


Now, the factory initializes like this:
```
NextTask->Initialize(GetOwner(), Callback, Plan.LastResult);
```
And primitive tasks can just access the last result if needed (by default). 


<img width="826" height="348" alt="new-init-bp" src="https://github.com/user-attachments/assets/421253c2-fe09-4503-ad68-da72cedae8b8" />


Again, this is just for ease of use of the code to help with faster prototyping. Another notable change was done in an attempt to optimize the code. When spawning a lot of NPCs, performance drops (for reasons shown later in the Optimization section). One reason is that every npc is trying to do its thing during the same frames. My attempt to solve this was to add an initial random frame delay to spread out the computation of every NPC's plans over the span of a dozen frames. Unfortunately, this doesn't seem to have made much of a difference and it proved difficult to prove the performace gain. Therefore, it is included in this section as an honourable mention. The code for this change works like this:

```
void BeginPlay()
{
	// random number
	InitialFrameDelay = RandomRange(0,10);
}

void Tick()
{
	// the first thing in the tick function
	if(!bTickReady)
	{
		if (++InitialTickCount > InitialFrameDelay)
		{
			bTickReady = true;
		}
		else return;
	}
	// tick code...
}
```



## Use of Programming Patterns


### Dirty Flag

The dirty flag is used to trigger events in the tick function. It's essentially functionality locked behind a condition. As seen in the previous section, dirty flags are used to start a new task. Below are some examples of dirty flag uses in the project:

```
// HTNComponent.cpp
void UHTNComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)  
{
	if(!bTickReady) // dirty flag
	{
		if (++InitialTickCount > InitialFrameDelay) // dirty flag
		{
			bTickReady = true;
		}
		else return;
	}

	//
	// tick stuff
	//

	TimeSinceLastPlan += DeltaTime;

	if (TimeSinceLastPlan >= PlanningInterval) // dirty flag
    {
	    TimeSinceLastPlan = 0
	    // do stuff
    }
    
    // bGetNextTask turns true when a task completes
	if (bGetNextTask) // dirty flag
    { 
		bGetNextTask = false
	    // do stuff
    }
}


// Task.cpp
void UTaskSubsystem::Tick(float DeltaTime)  
{  
    for (int i = 0; i < RunningPrimitiveTasks.Num(); ++i)  
    {    
       if (RunningPrimitiveTasks[i]->ShouldTick())  // dirty flag
       {  
          RunningPrimitiveTasks[i]->Tick(DeltaTime);  
       }  
    }  
} 
```

Using dirty flags in these sections is beneficial because it helps to keep the code simple, readable, and it allows me to use the tick functionality to accurately time events.



### State

*Types mentioned in this seciton: UPrimitiveTask , UTask, UHTNComponent, FWorldState*

My state implementation is the hierarchical task network. This system is a state because the actor that uses it is being controlled by a sequence of external functionality that is stored locally (cached) as a list of **UTask** objects. This functionality is driven by the actor's current state (a.k.a. world state).

This hierarchical task network functionality is provided when a **UHTNComponent** actor component is attached to an actor. Below are descriptions for context:

- **UHTNComponent** is the primary container for all hierarchical task network (HTN) needs. It knows what tasks it can perform and generates plans if needed.
- **UPrimitiveTask** is an object with an initializer and ticks every frame. It's a class made for performing specific actions on an actor or for an actor. For example, scanning for actors in the scene, moving towards an actor, or setting the actor's max movement speed. This is the state. Only of these can run at one time for a single **UHTNComponent** and notifies it when the task is finished.
- **UTask** is a container of **UPrimitiveTask**. It an object that represents a high level task. For example, "loot", "attack", or "wander". This is a pre-planned array of states that a character with an HTN component can apply to itself.
- **FWorldState** is a fact. It's a wrapper for a boolean that includes a string for comparison (and readability). For example, "is armed?" The HTN component stores an array of world states that it uses when generating plans. World states depend on sensors to dynamically change during runtime.

Only one primitive task can run at once, acting as state. The HTN component executes the current task, and uses a dirty flag to run the next. In other words, the actor is controlled by its states, and the states trigger the next state, until the plan is finished or fails.

```
void UHTNComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)  
{  
    // tick code
	// ...
  
    if (TimeSinceLastPlan >= PlanningInterval)  
    {      
	    TimeSinceLastPlan = 0;
		if (Planner.Get()->RequestPlan())  
        {  
	        bGetNextTask = false;  
	        // starting the new plan  
            // stop current task
          
            // get next task
            CurrentTask = GetNextTaskInitialized();  
            CurrentTask->Run();
	    }  
    }  
  
    if (bGetNextTask)  
    {  
        bGetNextTask = false;  
        if (LastTaskSucceeded)  
        {  
            // stop current task
          
		    // get next task  
            CurrentTask = GetNextTaskInitialized();  
            CurrentTask->Run();  
            else // no tasks, make new plan  
	        {  
			    bGetNextTask = false;  
		    }
        }  
		else
		{
            Plan.Reset();  
            bGetNextTask = false;  
        }   
    }  
}
```

The game that this is designed for benefits from this design pattern because it makes the AI modular. The game's gameplay heavily relies on non playable character, and going the HTN route allows me as the developer to easily make scripted sets of actions and assign them to the AI as available actions. The AI can then dynamically chose the best option depending on its current context in the game.




## Optimization

I optimized the item sensing component of my NPCs. This flowchart illustrates my process:


<img width="419" height="821" alt="how i optimized" src="https://github.com/user-attachments/assets/56b2dc86-7b13-4917-8ed5-17b5e9428ec7" />


Using an Unreal Engine macro (TRACE_CPUPROFILER_EVENT_SCOPE_STR), I was able to time my own code into the profiler which was super cool. I wanted to monitor things that happened during the HTN tick function and found that sensing takes a long time to do. The following screenshot shows the difference from before and after the optimization was made.

<img width="2559" height="1396" alt="Sidebyside-before-after" src="https://github.com/user-attachments/assets/a29e653e-9d1b-4025-8373-ff6f58adadd6" />

This next screenshot zooms into the frame graph. Notice how in the right snapshot, the ItemSensor event now takes a fraction of the time compared to the old version. This is the optimization I implemented.

<img width="2559" height="1382" alt="zooming in on the item sensor before after" src="https://github.com/user-attachments/assets/8c1ac6d4-a6cc-420f-871b-26de2bf82bd8" />

This was acheived using a singleton (for context, the original code used a sphere cast to find nearby actors). I added a new game instance subsystem class to act as a manager that caches a reference to all items in the level. When an item spawns, it adds itself to this list, and removes itself when picked up. Then, I made a simple getter function that returns the nearest item:

```
AActor* FindNearest(FVector FromLocation)
{
	// sort objects array 
	Items.Sort([FromLocation](const AActor& A, const AActor& B)
	{
		const float FastDistA = FVector::DistSquared(FromLocation, A.GetActorLocation());
		const float FastDistB = FVector::DistSquared(FromLocation, B.GetActorLocation());
		return (FastDistA > FastDistB);
	});

	// return the closest
	return Items[0];
}
```

When an NPC wants to know if there are any nearby items, it can now do so 20 times faster than before.
