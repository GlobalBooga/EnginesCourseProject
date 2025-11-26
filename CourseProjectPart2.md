# Course Project Part 2

### Improvements from assignment 1
Since the last assignment, small changes were made to the preexisting code base - mostly micro adjustments to work with other parts of the game. Some functions were renamed or deleted, and parameters moved or renamed. Changes are small and scattered but one notable change was done on the task factory. What changed is the initialization function for an individual task. 

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


This change helps reduce the amount of data that needs to be passed around each time which even gives a little extra boost to performance (more on that in the optimization section). So far, what was done is really to clean up the project for readability.


## Use of Programming Patterns


### Dirty Flag

The dirty flag is used to trigger events in the tick function. It's essentially functionality locked behind a condition. As seen in the previous section, dirty flags are used to start a new task. Below are some examples of dirty flag uses in the project:

```
// HTNComponent.cpp
void UHTNComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)  
{
	// tick stuff
	
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

The small code change that I made to the factory does in fact bring some extra performance. It's small but it exists.

On the left, is a screenshot of the profiler **before** the change, and on the right is **after**. 


<img width="2556" height="1279" alt="optimization-before-after" src="https://github.com/user-attachments/assets/7e3e32ec-8d3f-49ce-967a-337cc545a7f3" />


What's happening in this scenario:
1. Spawn 100 NPCs 
2. Let them do their thing for a few seconds
3. End play

In the timing insights graph, the red region is the time when the NPCs exist (the first snapshot was longer than the second). Looking closely, you can see that the timing on the right (post-change) is ever so slightly better than the one on the left (pre-change).

In the frame diagram underneath, I've zoomed into one of the frames where the planning happens (HTN). Investigating the timing values on each HTN call, the change I made to task initialization improves initialization time by about 100us. 



